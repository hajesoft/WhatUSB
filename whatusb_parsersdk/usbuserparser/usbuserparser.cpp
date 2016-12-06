#include "stdafx.h"
#include "usbuserparser.h"

WCHAR * g_ConstFixedColumnHeader[COLUMNCOUNT] = { _T("레코드#"), _T("시간"), _T("시작/완료"), _T("Item") };
SIZE_T g_ConstFixedColumnHeaderSize[COLUMNCOUNT] = { 120, 150, 70, 250 };
HJ_CUSTOM_PARSING_FIXED_HEADER	g_FixedHeader;
HJ_PARSER_INFO g_ParserInfo;
PPARSER_CONTEXT g_pParser_Context_for_Output = NULL;

// ------------------------------------------------------------------------------------------------------------

HJPARSERAPI PVOID fnHjInit(IN PVOID MgrHandle, IN PVOID phlp_function, WCHAR *pRegistryPath, WCHAR *FilePath)
{
	WCHAR TempString[MAX_PATH];
	PPARSER_CONTEXT pParser_Context = NULL;
	PHJ_EXPORT_HLP_FUNCTION_LIST phlp_function_list = (PHJ_EXPORT_HLP_FUNCTION_LIST)phlp_function;
	pParser_Context = (PPARSER_CONTEXT)malloc(sizeof(PARSER_CONTEXT));

	pParser_Context->MgrHandle = MgrHandle;
	memcpy(&pParser_Context->hlp_function_list, phlp_function_list, sizeof(HJ_EXPORT_HLP_FUNCTION_LIST));

	g_pParser_Context_for_Output = pParser_Context;

	parserHlpCmn_OutputStringToView(L"개인이 작성하는 샘플 파서입니다\n");
	wsprintf(TempString, L"[Title] %ws", (WCHAR *)g_ParserInfo.COMMON.Title);
	parserHlpCmn_OutputStringToView(TempString);
	wsprintf(TempString, L"[Desc] %ws", (WCHAR *)g_ParserInfo.COMMON.Desc);
	parserHlpCmn_OutputStringToView(TempString);
	wsprintf(TempString, L"[MajorVersion] %d", g_ParserInfo.COMMON.MajorVersion);
	parserHlpCmn_OutputStringToView(TempString);

	return (PVOID)pParser_Context;
}

HJPARSERAPI BOOL fnHjDeinit(PVOID handle)
{
	PPARSER_CONTEXT pParser_Context = NULL;

	pParser_Context = (PPARSER_CONTEXT)handle;

	free(pParser_Context);

	return TRUE;
}

HJPARSERAPI BOOL fnHjFree(PVOID handle, PVOID Buffer, SIZE_T Size)
{
	BOOL bRet = FALSE;

	// 개인적으로 해제해야하는 내용을 다룹니다

	bRet = parserHlpCmn_free(handle, Buffer, Size);
	if (bRet == FALSE)
		goto exit;

exit:
	return bRet;
}

HJPARSERAPI BOOL fnHjInfo(PHJ_PARSER_INFO pInfo)
{
	U32		u32Size;

	if (pInfo == NULL)
		return FALSE;

	u32Size = (pInfo->COMMON.u32Size > g_ParserInfo.COMMON.u32Size) ? g_ParserInfo.COMMON.u32Size : pInfo->COMMON.u32Size;
	if (u32Size)
		memcpy(pInfo, &g_ParserInfo, u32Size);

	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpReserved)
{
	int count;

	switch (fdwReason)
	{
	case DLL_PROCESS_DETACH:
		break;

	case DLL_PROCESS_ATTACH:
		g_ParserInfo.COMMON.u32Size = sizeof(HJ_PARSER_INFO);
		g_ParserInfo.COMMON.Type = USB_PARSER;
		wcscpy_s((WCHAR *)g_ParserInfo.COMMON.Title, 1024, PARSER_TITLE);
		g_ParserInfo.COMMON.MajorVersion = 1;
		g_ParserInfo.COMMON.MinorVersion = 0;
		wcscpy_s((WCHAR *)g_ParserInfo.COMMON.Desc, 1024, PARSER_DESC);
		g_ParserInfo.EXTRA.bIsSupported = 1;
		g_ParserInfo.EXTRA.ParserInterface_MajorVersion = 1;
		g_ParserInfo.EXTRA.ParserInterface_MinorVersion = 0;
		g_ParserInfo.u.usb.NumSupportedClass = 1;

#if 0
		// Default 파서형태를 원하면 아래와 같이 사용해야 합니다
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].fFlag = fUSBDefault;
#endif
		// Massstorage Class를 위한 파서를 만드려면 다음과 같이 정의해야 합니다
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].fFlag = fInterfaceClass;
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].u16InterfaceClass = 8; // Massstorage Class를 위한 클래스 코드입니다

#if 0
		// Hid Class를 위한 파서를 만드려면 다음과 같이 정의해야 합니다
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].fFlag = fInterfaceClass;
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].u16InterfaceClass = 3; // Hid Class를 위한 클래스 코드입니다
#endif

		g_ParserInfo.EXTRA.pFixedHeaderInfo = (PHJ_PARSING_FIXED_HEADER)&g_FixedHeader;

		g_FixedHeader.ItemCount = COLUMNCOUNT;
		for (count = 0; count < COLUMNCOUNT; count++)
		{
			g_FixedHeader.ColumnHeader[count].bIsConstant = TRUE;
			g_FixedHeader.ColumnHeader[count].MaxListCtrlSize = g_ConstFixedColumnHeaderSize[count];
			g_FixedHeader.ColumnHeader[count].pszData = g_ConstFixedColumnHeader[count];
		}
		break;
	}
	return TRUE;
}

HJPARSERAPI BOOL fnHjDataParsing(PVOID handle, PVOID pSourceIrpUrb, SIZE_T SourceSize, PVOID *ppBuffer, SIZE_T *pResultSize)
{
	BOOL bRet = FALSE;
	PST_CF_IRP_URB pSource = NULL;
	PPARSER_CONTEXT pParser_Context = NULL;
	SIZE_T FixedDataSize = 0;
	PHJ_PARSING_FIXED_DATA	pBaseFixedData = NULL;
	PHJ_PARSING_FIXED_DATA_ENTRY pParentFixedData = NULL;

	// 핸들을 준비합니다
	pParser_Context = (PPARSER_CONTEXT)handle;

	// 원시레코드를 준비합니다
	pSource = (PST_CF_IRP_URB)pSourceIrpUrb;

	//////////////////////////////////////////////////////////////////////////////////////
	// 먼저 Fixed Header에 대한 데이타를 준비한다(문자열형태)

	// 1. 메인 화면에 나타날 레코드(HJ_PARSING_FIXED_DATA)를 위한 메모리를 할당합니다
	FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA);
	pBaseFixedData = (PHJ_PARSING_FIXED_DATA)malloc(FixedDataSize);
	memset(pBaseFixedData, 0, FixedDataSize);

	// 2. 링크드리스트를 위한 초기화
	InitializeListHead(&pBaseFixedData->__AllDataEntryHeader__);

	// 3. 하나하나씩 원시데이타를 해석하기 시작합니다.
	/*
	순서는 다음과 같아야 합니다.
	[첫번째 레벨의 Main레코드] -> [두번째 레벨의 Main레코드] -> .... 이와 같이 열거되어야 합니다.

	각각의 레벨의 레코드는 다음과 같습니다.
	[필드1의 서브레코드] -> [필드2의 서브레코드] .... -> [마지막 필드의 서브레코드] -> 상세페이지레코드(선택) -> 데이터페이지레코드(선택)
	필드1, 2는 현재 머하니에서 예약된 내용을 사용해야 합니다(1=레코드, 2=시간)


	각각의 Main레코드는 다음과 같은 사전정의가 필요합니다.
	pFixedData->pDetailsPage = NULL; // 별도의 속성페이지레코드가 있다면 이곳에 그 주소를 입력해야 합니다
	pFixedData->pDataPage = NULL; // 별도의 데이터페이지레코드가 있다면 이곳에 그 주소를 입력해야 합니다
	pFixedData->ColCount = COLUMNCOUNT; // 메인 화면에 출력할 컬럼수를 의미합니다.
	pFixedData->bIsLast = FALSE; // 현재 Main레코드가 파싱의 마지막인지를 의미합니다.
	pFixedData->Level = 0; // 레벨을 의미합니다. 반드시 순서대로 정의되어져야 합니다.
	pFixedData->IndexForViewerImage 머하니에서 보여진 레코드의 이미지아이콘인덱스입니다.

	pFixedData->ColumnData[X] 의 내용을 채우는 것이 서브레코드를 채우는 것입니다. 서브레코드를 채울때는 다음사항을 유의해야 합니다
	모든 기본 값은 0 또는 FALSE값을 채워져 있습니다.

	데이타가 8바이트이내의 경우,
	pFixedData->ColumnData[X].Data 에 데이타를 기록합니다.
	pFixedData->ColumnData[X].bIsString = FALSE (기본값)
	pFixedData->ColumnData[X].nRadix 진법을 기록합니다. 10(10진수), 16(16진수)

	데이타가 8바이트를 초과하는 경우, 메인컬럼에 출력하는 내용은 문자열로 다루고, 별도의 데이터페이지레코드을 위해서만 실제 데이타를 보관합니다
	pFixedData->ColumnData[X].pszData 에 문자열을 기록합니다. 예를 들어, (54 44 EE 23 11 ..)이런식의 문자열이 보관되도록합니다. 문자열을 데이타 일부만 표현하십시오.
	pFixedData->ColumnData[X].bIsString = TRUE

	데이타가 문자열의 경우, 해당하는 문자열을 위한 메모리영역이 상수영역인 경우,
	pFixedData->ColumnData[X].pszData = L"HELLO MR MONKEY";
	pFixedData->ColumnData[X].bIsString = TRUE
	pFixedData->ColumnData[X].bIsConstant = TRUE


	##################### 속성페이지레코드와 데이터페이지레코드작성법 #########################
	두가지 레코드는 동일한 방법으로 만듭니다. 다만, 속성페이지레코드는 여러개의 테이블이 존재할 수 있지만, 데이터페이지레코드는 단 하나의 레코드만 허용합니다.
	1) parserHlpCmn_CreateParsingData 함수를 사용해서 속성페이지레코드 또는 데이터페이지레코드 데이터를 만듭니다. 이후, 내용을 채우기 위해서 다음 함수를 사용합니다.
	반드시 한번만 사용합니다
	2) parserHlpCmn_CreateAndLinkParsingDataToParsingValue 함수 : Parsing Value를 만듭니다(Parsing Data의 자식 노드)
	생성할 테이블의 수만큼 사용하게 됩니다
	3) parserHlpCmn_CreateAndLinkParsingValueToParsingField 함수 : Parsing Field를 만듭니다(Parsing Value의 자식 노드)
	생성된 테이블의 항목수만큼 사용하게 됩니다

	ParsingData--------- ParsingValue (Table)
	|
	---- ParsingValue (Table) ------- Parsing Field (테이블내의 항목)
	|
	----- Parsing Field (테이블내의 항목)
	|
	----- Parsing Field (테이블내의 항목)

	속성페이지와 데이터페이지는 모두 기본적으로 디폴트 파싱라이브러리가 지원하는 내용입니다.
	따라서, 구현하려는 파서가 이 부분을 직접 구현하지 않고 디폴트 파싱라이브러리를 사용하려면, 다음과 같은 함수를 호출하면 됩니다.

	PHJ_PARSING_DATA parserHlpCmn_FillDetailsPage(PPARSER_CONTEXT pParset_Context, PST_CF_IRP_URB pSource, SIZE_T	Level); // 속성페이지레코드
	PHJ_PARSING_DATA_FOR_DATA parserHlpCmn_FillDataPages(PPARSER_CONTEXT pParset_Context, PST_CF_IRP_URB pSource, SIZE_T	Level); // 데이터페이지레코드

	이와 같은 함수를 사용해서 얻은 메모리주소를 pFixedData->pDetailsPage, pFixedData->pDataPage 에 담으면 됩니다.

	######################## Parsing Field 에 사용하는 필드 유형(Type) ########################
	Parsing Field에는 다음과 같은 유형이 정의되어 있습니다

	#define HJ_VALUE_ENDIAN_IGN (U32)(0x00000000)
	문자열출력을 지시합니다

	#define HJ_VALUE_ENDIAN_LSB (U32)(0x00000001)
	숫자를 LSB형태로 출력할것을 지시합니다

	#define HJ_VALUE_ENDIAN_MSB (U32)(0x00000002)
	숫자를 MSB형태로 출력할것을 지시합니다

	#define HJ_VALUE_EXTRA_BUFF (U32)(0x00000004) // Data Byte를 전달하기 위해서 사용하는 상수
	pFieldValue->nRowFieldBuffer 필드를 사용해서 데이타가 전달됩니다. 새롭게 할당되는 메모리여야 합니다

	#define HJ_VALUE_EXTRA_PRIVATEBUFF (U32)(0x00000008) // Private 용도로 사용하는 메모리할당을 예약하는 상수
	pFieldValue->nRowFieldPrivateBuffer 필드를 사용해서 문자열정보가 전달됩니다. 새롭게 할당되는 메모리여야 합니다

	#define HJ_STRUCT_TYPE__SPLIT_DESCRIPTOR (U32)(0x00000010)
	예약되어 있습니다. 지금 버젼에서는 사용되지 않습니다

	#define HJ_STRUCT_TYPE__SPLIT_ARRAY_BEGIN (U32)(0x00000020)
	배열을 정의합니다. 모든 배열의 시작에 사용됩니다. 파라미터로 사용될 배열의 항목수가 전달됩니다. 각각의 배열마다 매번 정의해주어야 합니다.

	#define HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN (U32)(0x00000040)
	배열의 첫번째 데이타가 시작됨을 알려줍니다.

	#define HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END (U32)(0x00000080)
	배열의 마지막 데이타가 기록되어 있음을 알려줍니다.

	#define HJ_STRUCT_TYPE__SPLIT_ARRAY_END (U32)(0x00000100)
	배열을 끝냅니다. 각각의 배열마다 매번 정의해주어야 합니다.

	#define HJ_STRUCT_TYPE__SPLIT_COMMON (U32)(0x00000200)
	예약되어 있습니다. 지금 버젼에서는 사용되지 않습니다

	다음과 같은 예시를 통해서 배열을 사용하는 방법을 알 수 있습니다

	[예시]

	NewArray[0].Data1
	NewArray[0].Data2
	NewArray[0].Data3
	NewArray[1].Data1
	NewArray[1].Data2
	NewArray[1].Data3

	HJ_STRUCT_TYPE__SPLIT_ARRAY_BEGIN (2) : 배열의 항목수가 2개이므로

	HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN (0) : 배열의 첫번째 첨자 시작
	..... Data1, 2, 3
	HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END (0) : 배열의 첫번째 첨자 끝

	HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN (1) : 배열의 두번째 첨자 시작
	..... Data1, 2, 3
	HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END (1) : 배열의 두번째 첨자 끝

	HJ_STRUCT_TYPE__SPLIT_ARRAY_END : 배열의 끝

	*/

	// example

	PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

	FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
	pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
	memset(pFixedData, 0, FixedDataSize);
	InitializeListHead(&pFixedData->__Self__);

	pFixedData->pDetailsPage = NULL;
	pFixedData->pDataPage = NULL;
	pFixedData->ColCount = COLUMNCOUNT;
	pFixedData->bIsLast = TRUE;
	pFixedData->Level = 0;
	pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

	InsertTailList(&pBaseFixedData->__AllDataEntryHeader__, &pFixedData->__Self__);
	pBaseFixedData->nCount++;

	// 레코드#[VIEWCOL_RECORD]
	if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
	{
		pFixedData->ColumnData[0].Data = pSource->stUrbHeader.stHeader.u64NumRecordOrg;
	}
	else
	{
		pFixedData->ColumnData[0].Data = pSource->stUrbHeader.stHeader.u64NumRecord;
	}
	pFixedData->ColumnData[0].nRadix = 10;

	// 시간[VIEWCOL_TIME]
	pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
	pFixedData->ColumnData[1].nRadix = 10;

	// 시작/완료[2]
	pFixedData->ColumnData[2].bIsString = TRUE;
	pFixedData->ColumnData[2].bIsConstant = TRUE;
	if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		pFixedData->ColumnData[2].pszData = L"완료";
	else
		pFixedData->ColumnData[2].pszData = L"시작";

	// Item[3]
	pFixedData->ColumnData[3].bIsString = TRUE;
	pFixedData->ColumnData[3].bIsConstant = TRUE;
	pFixedData->ColumnData[3].pszData = _T("TEST아이템입니다");

	// appendix
	pFixedData->pDetailsPage = NULL;
	pFixedData->pDataPage = NULL;
	pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
	pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

	*ppBuffer = (void *)pBaseFixedData;
	*pResultSize = sizeof(PHJ_PARSING_FIXED_DATA);

	bRet = TRUE;
	return bRet;
}
