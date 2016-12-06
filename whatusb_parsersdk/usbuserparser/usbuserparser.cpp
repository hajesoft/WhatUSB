#include "stdafx.h"
#include "usbuserparser.h"

WCHAR * g_ConstFixedColumnHeader[COLUMNCOUNT] = { _T("���ڵ�#"), _T("�ð�"), _T("����/�Ϸ�"), _T("Item") };
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

	parserHlpCmn_OutputStringToView(L"������ �ۼ��ϴ� ���� �ļ��Դϴ�\n");
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

	// ���������� �����ؾ��ϴ� ������ �ٷ�ϴ�

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
		// Default �ļ����¸� ���ϸ� �Ʒ��� ���� ����ؾ� �մϴ�
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].fFlag = fUSBDefault;
#endif
		// Massstorage Class�� ���� �ļ��� ������� ������ ���� �����ؾ� �մϴ�
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].fFlag = fInterfaceClass;
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].u16InterfaceClass = 8; // Massstorage Class�� ���� Ŭ���� �ڵ��Դϴ�

#if 0
		// Hid Class�� ���� �ļ��� ������� ������ ���� �����ؾ� �մϴ�
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].fFlag = fInterfaceClass;
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].u16InterfaceClass = 3; // Hid Class�� ���� Ŭ���� �ڵ��Դϴ�
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

	// �ڵ��� �غ��մϴ�
	pParser_Context = (PPARSER_CONTEXT)handle;

	// ���÷��ڵ带 �غ��մϴ�
	pSource = (PST_CF_IRP_URB)pSourceIrpUrb;

	//////////////////////////////////////////////////////////////////////////////////////
	// ���� Fixed Header�� ���� ����Ÿ�� �غ��Ѵ�(���ڿ�����)

	// 1. ���� ȭ�鿡 ��Ÿ�� ���ڵ�(HJ_PARSING_FIXED_DATA)�� ���� �޸𸮸� �Ҵ��մϴ�
	FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA);
	pBaseFixedData = (PHJ_PARSING_FIXED_DATA)malloc(FixedDataSize);
	memset(pBaseFixedData, 0, FixedDataSize);

	// 2. ��ũ�帮��Ʈ�� ���� �ʱ�ȭ
	InitializeListHead(&pBaseFixedData->__AllDataEntryHeader__);

	// 3. �ϳ��ϳ��� ���õ���Ÿ�� �ؼ��ϱ� �����մϴ�.
	/*
	������ ������ ���ƾ� �մϴ�.
	[ù��° ������ Main���ڵ�] -> [�ι�° ������ Main���ڵ�] -> .... �̿� ���� ���ŵǾ�� �մϴ�.

	������ ������ ���ڵ�� ������ �����ϴ�.
	[�ʵ�1�� ���극�ڵ�] -> [�ʵ�2�� ���극�ڵ�] .... -> [������ �ʵ��� ���극�ڵ�] -> �����������ڵ�(����) -> ���������������ڵ�(����)
	�ʵ�1, 2�� ���� ���ϴϿ��� ����� ������ ����ؾ� �մϴ�(1=���ڵ�, 2=�ð�)


	������ Main���ڵ�� ������ ���� �������ǰ� �ʿ��մϴ�.
	pFixedData->pDetailsPage = NULL; // ������ �Ӽ����������ڵ尡 �ִٸ� �̰��� �� �ּҸ� �Է��ؾ� �մϴ�
	pFixedData->pDataPage = NULL; // ������ ���������������ڵ尡 �ִٸ� �̰��� �� �ּҸ� �Է��ؾ� �մϴ�
	pFixedData->ColCount = COLUMNCOUNT; // ���� ȭ�鿡 ����� �÷����� �ǹ��մϴ�.
	pFixedData->bIsLast = FALSE; // ���� Main���ڵ尡 �Ľ��� ������������ �ǹ��մϴ�.
	pFixedData->Level = 0; // ������ �ǹ��մϴ�. �ݵ�� ������� ���ǵǾ����� �մϴ�.
	pFixedData->IndexForViewerImage ���ϴϿ��� ������ ���ڵ��� �̹����������ε����Դϴ�.

	pFixedData->ColumnData[X] �� ������ ä��� ���� ���극�ڵ带 ä��� ���Դϴ�. ���극�ڵ带 ä�ﶧ�� ���������� �����ؾ� �մϴ�
	��� �⺻ ���� 0 �Ǵ� FALSE���� ä���� �ֽ��ϴ�.

	����Ÿ�� 8����Ʈ�̳��� ���,
	pFixedData->ColumnData[X].Data �� ����Ÿ�� ����մϴ�.
	pFixedData->ColumnData[X].bIsString = FALSE (�⺻��)
	pFixedData->ColumnData[X].nRadix ������ ����մϴ�. 10(10����), 16(16����)

	����Ÿ�� 8����Ʈ�� �ʰ��ϴ� ���, �����÷��� ����ϴ� ������ ���ڿ��� �ٷ��, ������ ���������������ڵ��� ���ؼ��� ���� ����Ÿ�� �����մϴ�
	pFixedData->ColumnData[X].pszData �� ���ڿ��� ����մϴ�. ���� ���, (54 44 EE 23 11 ..)�̷����� ���ڿ��� �����ǵ����մϴ�. ���ڿ��� ����Ÿ �Ϻθ� ǥ���Ͻʽÿ�.
	pFixedData->ColumnData[X].bIsString = TRUE

	����Ÿ�� ���ڿ��� ���, �ش��ϴ� ���ڿ��� ���� �޸𸮿����� ��������� ���,
	pFixedData->ColumnData[X].pszData = L"HELLO MR MONKEY";
	pFixedData->ColumnData[X].bIsString = TRUE
	pFixedData->ColumnData[X].bIsConstant = TRUE


	##################### �Ӽ����������ڵ�� ���������������ڵ��ۼ��� #########################
	�ΰ��� ���ڵ�� ������ ������� ����ϴ�. �ٸ�, �Ӽ����������ڵ�� �������� ���̺��� ������ �� ������, ���������������ڵ�� �� �ϳ��� ���ڵ常 ����մϴ�.
	1) parserHlpCmn_CreateParsingData �Լ��� ����ؼ� �Ӽ����������ڵ� �Ǵ� ���������������ڵ� �����͸� ����ϴ�. ����, ������ ä��� ���ؼ� ���� �Լ��� ����մϴ�.
	�ݵ�� �ѹ��� ����մϴ�
	2) parserHlpCmn_CreateAndLinkParsingDataToParsingValue �Լ� : Parsing Value�� ����ϴ�(Parsing Data�� �ڽ� ���)
	������ ���̺��� ����ŭ ����ϰ� �˴ϴ�
	3) parserHlpCmn_CreateAndLinkParsingValueToParsingField �Լ� : Parsing Field�� ����ϴ�(Parsing Value�� �ڽ� ���)
	������ ���̺��� �׸����ŭ ����ϰ� �˴ϴ�

	ParsingData--------- ParsingValue (Table)
	|
	---- ParsingValue (Table) ------- Parsing Field (���̺��� �׸�)
	|
	----- Parsing Field (���̺��� �׸�)
	|
	----- Parsing Field (���̺��� �׸�)

	�Ӽ��������� �������������� ��� �⺻������ ����Ʈ �Ľ̶��̺귯���� �����ϴ� �����Դϴ�.
	����, �����Ϸ��� �ļ��� �� �κ��� ���� �������� �ʰ� ����Ʈ �Ľ̶��̺귯���� ����Ϸ���, ������ ���� �Լ��� ȣ���ϸ� �˴ϴ�.

	PHJ_PARSING_DATA parserHlpCmn_FillDetailsPage(PPARSER_CONTEXT pParset_Context, PST_CF_IRP_URB pSource, SIZE_T	Level); // �Ӽ����������ڵ�
	PHJ_PARSING_DATA_FOR_DATA parserHlpCmn_FillDataPages(PPARSER_CONTEXT pParset_Context, PST_CF_IRP_URB pSource, SIZE_T	Level); // ���������������ڵ�

	�̿� ���� �Լ��� ����ؼ� ���� �޸��ּҸ� pFixedData->pDetailsPage, pFixedData->pDataPage �� ������ �˴ϴ�.

	######################## Parsing Field �� ����ϴ� �ʵ� ����(Type) ########################
	Parsing Field���� ������ ���� ������ ���ǵǾ� �ֽ��ϴ�

	#define HJ_VALUE_ENDIAN_IGN (U32)(0x00000000)
	���ڿ������ �����մϴ�

	#define HJ_VALUE_ENDIAN_LSB (U32)(0x00000001)
	���ڸ� LSB���·� ����Ұ��� �����մϴ�

	#define HJ_VALUE_ENDIAN_MSB (U32)(0x00000002)
	���ڸ� MSB���·� ����Ұ��� �����մϴ�

	#define HJ_VALUE_EXTRA_BUFF (U32)(0x00000004) // Data Byte�� �����ϱ� ���ؼ� ����ϴ� ���
	pFieldValue->nRowFieldBuffer �ʵ带 ����ؼ� ����Ÿ�� ���޵˴ϴ�. ���Ӱ� �Ҵ�Ǵ� �޸𸮿��� �մϴ�

	#define HJ_VALUE_EXTRA_PRIVATEBUFF (U32)(0x00000008) // Private �뵵�� ����ϴ� �޸��Ҵ��� �����ϴ� ���
	pFieldValue->nRowFieldPrivateBuffer �ʵ带 ����ؼ� ���ڿ������� ���޵˴ϴ�. ���Ӱ� �Ҵ�Ǵ� �޸𸮿��� �մϴ�

	#define HJ_STRUCT_TYPE__SPLIT_DESCRIPTOR (U32)(0x00000010)
	����Ǿ� �ֽ��ϴ�. ���� ���������� ������ �ʽ��ϴ�

	#define HJ_STRUCT_TYPE__SPLIT_ARRAY_BEGIN (U32)(0x00000020)
	�迭�� �����մϴ�. ��� �迭�� ���ۿ� ���˴ϴ�. �Ķ���ͷ� ���� �迭�� �׸���� ���޵˴ϴ�. ������ �迭���� �Ź� �������־�� �մϴ�.

	#define HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN (U32)(0x00000040)
	�迭�� ù��° ����Ÿ�� ���۵��� �˷��ݴϴ�.

	#define HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END (U32)(0x00000080)
	�迭�� ������ ����Ÿ�� ��ϵǾ� ������ �˷��ݴϴ�.

	#define HJ_STRUCT_TYPE__SPLIT_ARRAY_END (U32)(0x00000100)
	�迭�� �����ϴ�. ������ �迭���� �Ź� �������־�� �մϴ�.

	#define HJ_STRUCT_TYPE__SPLIT_COMMON (U32)(0x00000200)
	����Ǿ� �ֽ��ϴ�. ���� ���������� ������ �ʽ��ϴ�

	������ ���� ���ø� ���ؼ� �迭�� ����ϴ� ����� �� �� �ֽ��ϴ�

	[����]

	NewArray[0].Data1
	NewArray[0].Data2
	NewArray[0].Data3
	NewArray[1].Data1
	NewArray[1].Data2
	NewArray[1].Data3

	HJ_STRUCT_TYPE__SPLIT_ARRAY_BEGIN (2) : �迭�� �׸���� 2���̹Ƿ�

	HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN (0) : �迭�� ù��° ÷�� ����
	..... Data1, 2, 3
	HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END (0) : �迭�� ù��° ÷�� ��

	HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN (1) : �迭�� �ι�° ÷�� ����
	..... Data1, 2, 3
	HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END (1) : �迭�� �ι�° ÷�� ��

	HJ_STRUCT_TYPE__SPLIT_ARRAY_END : �迭�� ��

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

	// ���ڵ�#[VIEWCOL_RECORD]
	if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
	{
		pFixedData->ColumnData[0].Data = pSource->stUrbHeader.stHeader.u64NumRecordOrg;
	}
	else
	{
		pFixedData->ColumnData[0].Data = pSource->stUrbHeader.stHeader.u64NumRecord;
	}
	pFixedData->ColumnData[0].nRadix = 10;

	// �ð�[VIEWCOL_TIME]
	pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
	pFixedData->ColumnData[1].nRadix = 10;

	// ����/�Ϸ�[2]
	pFixedData->ColumnData[2].bIsString = TRUE;
	pFixedData->ColumnData[2].bIsConstant = TRUE;
	if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		pFixedData->ColumnData[2].pszData = L"�Ϸ�";
	else
		pFixedData->ColumnData[2].pszData = L"����";

	// Item[3]
	pFixedData->ColumnData[3].bIsString = TRUE;
	pFixedData->ColumnData[3].bIsConstant = TRUE;
	pFixedData->ColumnData[3].pszData = _T("TEST�������Դϴ�");

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
