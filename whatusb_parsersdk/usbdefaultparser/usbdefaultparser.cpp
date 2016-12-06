#include "stdafx.h"
#include "usbdefaultparser.h"

WCHAR * g_ConstFixedColumnHeader[COLUMNCOUNT] = { _T("���ڵ�#"), _T("�ð�"), _T("����/�Ϸ�"), _T("Item"), _T("EP����"), _T("����"), _T("Pipe�ڵ�"), _T("Endpoint"), _T("����"), _T("����"), _T("��ûũ��"), _T("����ũ��") };
SIZE_T g_ConstFixedColumnHeaderSize[COLUMNCOUNT] = { 120, 150, 70, 250, 50, 80, 100, 100, 150, 180, 70, 70 };
WCHAR * g_ConstEndpointAddressIn[16] = {
	_T("#0"), _T("#81"), _T("#82"), _T("#83"), _T("#84"), _T("#85"), _T("#86"), _T("#87"),
	_T("#88"), _T("#89"), _T("#8A"), _T("#8B"), _T("#8C"), _T("#8D"), _T("#8E"), _T("#8F")
};
WCHAR * g_ConstEndpointAddressOut[16] = {
	_T("#0"), _T("#1"), _T("#2"), _T("#3"), _T("#4"), _T("#5"), _T("#6"), _T("#7"),
	_T("#8"), _T("#9"), _T("#A"), _T("#B"), _T("#C"), _T("#D"), _T("#E"), _T("#F")
};
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

	parserHlpCmn_OutputStringToView(L"2016�� 10�� 19�� ��������Ʈ���� ���۵� �ļ��Դϴ�\n");
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
		g_ParserInfo.u.usb.SUPPORTED_CLASS[0].fFlag = fUSBDefault;

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

WCHAR * replaceItemForControlTransfer(U16 u16Function, U8 type = 0)
{
	WCHAR * pRet = NULL;
	switch (u16Function)
	{
	case URB_FUNCTION_SELECT_CONFIGURATION:
		pRet = _T("SET CONFIG");
		break;
	case URB_FUNCTION_SELECT_INTERFACE:
		pRet = _T("SET INTERFACE");
		break;
	case URB_FUNCTION_ABORT_PIPE:
		pRet = _T("ABORT PIPE");
		break;
	case URB_FUNCTION_RESET_PIPE:
		pRet = _T("RESET PIPE");
		break;
	case URB_FUNCTION_SYNC_RESET_PIPE:
		pRet = _T("SYNC RESET PIPE");
		break;
	case URB_FUNCTION_GET_CURRENT_FRAME_NUMBER:
		pRet = _T("GET CURRENT FRAME NUMBER");
		break;
	case URB_FUNCTION_CONTROL_TRANSFER:
	case URB_FUNCTION_CONTROL_TRANSFER_EX:
	case URB_FUNCTION_VENDOR_DEVICE:
		pRet = _T("CONTROL TRANSFER(VENDOR_DEVICE)");
		break;
	case URB_FUNCTION_VENDOR_INTERFACE:
		pRet = _T("CONTROL TRANSFER(VENDOR_INTERFACE)");
		break;
	case URB_FUNCTION_VENDOR_ENDPOINT:
		pRet = _T("CONTROL TRANSFER(VENDOR_ENDPOINT)");
		break;
	case URB_FUNCTION_VENDOR_OTHER:
		pRet = _T("CONTROL TRANSFER(VENDOR_OTHER)");
		break;
	case URB_FUNCTION_CLASS_DEVICE:
		pRet = _T("CONTROL TRANSFER(CLASS_DEVICE)");
		break;
	case URB_FUNCTION_CLASS_INTERFACE:
		pRet = _T("CONTROL TRANSFER(CLASS_INTERFACE)");
		break;
	case URB_FUNCTION_CLASS_ENDPOINT:
		pRet = _T("CONTROL TRANSFER(CLASS_ENDPOINT)");
		break;
	case URB_FUNCTION_CLASS_OTHER:
		pRet = _T("CONTROL TRANSFER(CLASS_OTHER)");
		break;
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT:
		if (type == USB_DEVICE_DESCRIPTOR_TYPE)
			pRet = _T("GET DESCRIPTOR(Device)");
		else if (type == USB_CONFIGURATION_DESCRIPTOR_TYPE)
			pRet = _T("GET DESCRIPTOR(Config)");
		else if (type == USB_STRING_DESCRIPTOR_TYPE)
			pRet = _T("GET DESCRIPTOR(String)");
		else
			pRet = _T("GET DESCRIPTOR(Other)");
		break;
	case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT:
		if (type == USB_DEVICE_DESCRIPTOR_TYPE)
			pRet = _T("SET DESCRIPTOR(Device)");
		else if (type == USB_CONFIGURATION_DESCRIPTOR_TYPE)
			pRet = _T("SET DESCRIPTOR(Config)");
		else if (type == USB_STRING_DESCRIPTOR_TYPE)
			pRet = _T("SET DESCRIPTOR(String)");
		else
			pRet = _T("SET DESCRIPTOR(Other)");
		break;
		break;
	case URB_FUNCTION_GET_CONFIGURATION:
		pRet = _T("GET CONFIG VALUE");
		break;
	case URB_FUNCTION_GET_INTERFACE:
		pRet = _T("GET ALTINTERFACE NUMBER");
		break;
	case URB_FUNCTION_GET_STATUS_FROM_DEVICE:
	case URB_FUNCTION_GET_STATUS_FROM_INTERFACE:
	case URB_FUNCTION_GET_STATUS_FROM_ENDPOINT:
	case URB_FUNCTION_GET_STATUS_FROM_OTHER:
		pRet = _T("GET STATUS");
		break;
	case URB_FUNCTION_SET_FEATURE_TO_DEVICE:
	case URB_FUNCTION_SET_FEATURE_TO_INTERFACE:
	case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT:
	case URB_FUNCTION_SET_FEATURE_TO_OTHER:
		pRet = _T("SET FEATURE");
		break;
	case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE:
	case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE:
	case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT:
	case URB_FUNCTION_CLEAR_FEATURE_TO_OTHER:
		pRet = _T("CLEAR FEATURE");
		break;
	}
	return pRet;
}

HJPARSERAPI BOOL fnHjDataParsing(PVOID handle, PVOID pSourceIrpUrb, SIZE_T SourceSize, PVOID *ppBuffer, SIZE_T *pResultSize)
{
	BOOL bRet = FALSE;
	PST_CF_IRP_URB pSource = NULL;
	PPARSER_CONTEXT pParser_Context = NULL;
	SIZE_T FixedDataSize = 0;
	PHJ_PARSING_FIXED_DATA	pBaseFixedData = NULL;
	PHJ_PARSING_FIXED_DATA_ENTRY pParentFixedData = NULL;
	UCHAR ep, eptype;

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
	switch (pSource->stUrbHeader.u16Function)
	{
	case URB_FUNCTION_SELECT_CONFIGURATION:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		pFixedData->ColumnData[3].pszData = _T("SET CONFIG");

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion);
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}
		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;

		// ��ûũ��[10]
		pFixedData->ColumnData[10].bIsString = TRUE;

		// ����ũ��[11]
		pFixedData->ColumnData[11].bIsString = TRUE;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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
			pFixedData->ColumnData[1].bIsString = TRUE;

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
			pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_SELECT_CONFIGURATION");

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;
			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;

			// ��ûũ��[10]
			pFixedData->ColumnData[10].bIsString = TRUE;

			// ����ũ��[11]
			pFixedData->ColumnData[11].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_SELECT_INTERFACE:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		pFixedData->ColumnData[3].pszData = _T("SET INTERFACE");

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion);
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}
		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;

		// ��ûũ��[10]
		pFixedData->ColumnData[10].bIsString = TRUE;

		// ����ũ��[11]
		pFixedData->ColumnData[11].bIsString = TRUE;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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
			pFixedData->ColumnData[1].bIsString = TRUE;

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
			pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_SELECT_INTERFACE");

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;
			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;

			// ��ûũ��[10]
			pFixedData->ColumnData[10].bIsString = TRUE;

			// ����ũ��[11]
			pFixedData->ColumnData[11].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_ABORT_PIPE:
	case URB_FUNCTION_RESET_PIPE:
	case URB_FUNCTION_SYNC_RESET_PIPE:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		switch (pSource->stUrbHeader.u16Function)
		{
		case URB_FUNCTION_ABORT_PIPE:
			pFixedData->ColumnData[3].pszData = _T("ABORT PIPE");
			break;
		case URB_FUNCTION_RESET_PIPE:
			pFixedData->ColumnData[3].pszData = _T("RESET PIPE");
			break;
		case URB_FUNCTION_SYNC_RESET_PIPE:
			pFixedData->ColumnData[3].pszData = _T("SYNC RESET PIPE");
			break;
		}

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		ep = parserHlpCmn_TranslatePipeHandleToEP(pParser_Context, pSource->stUrbPipeRequest.u64PipeHandle);

		// Pipe�ڵ�[6]
		if (ep == 0)
		{
			pFixedData->ColumnData[6].bIsString = TRUE;
		}
		else
		{
			pFixedData->ColumnData[6].Data = pSource->stUrbPipeRequest.u64PipeHandle;
			pFixedData->ColumnData[6].nRadix = 16;
		}

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = (U64)ep;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion);
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}
		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;

		// ��ûũ��[10]
		pFixedData->ColumnData[10].bIsString = TRUE;

		// ����ũ��[11]
		pFixedData->ColumnData[11].bIsString = TRUE;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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
			pFixedData->ColumnData[1].bIsString = TRUE;

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

			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_ABORT_PIPE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_ABORT_PIPE");
				break;
			case URB_FUNCTION_RESET_PIPE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_RESET_PIPE");
				break;
			case URB_FUNCTION_SYNC_RESET_PIPE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_SYNC_RESET_PIPE");
				break;
			}

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;

			eptype = parserHlpCmn_TranslatePipeHandleToEPType(pParser_Context, pSource->stUrbPipeRequest.u64PipeHandle);
			if (eptype == 0) // control
				pFixedData->ColumnData[4].pszData = _T("��Ʈ��");
			else if (eptype == 1) // iso
				pFixedData->ColumnData[4].pszData = _T("��ü�");
			else if (eptype == 2) // bulk
				pFixedData->ColumnData[4].pszData = _T("��ũ");
			else if (eptype == 3) // int
				pFixedData->ColumnData[4].pszData = _T("���ͷ�Ʈ");

			// ����[5]
			ep = parserHlpCmn_TranslatePipeHandleToEP(pParser_Context, pSource->stUrbPipeRequest.u64PipeHandle);

			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;

			if (ep == 0)
			{
				pFixedData->ColumnData[5].pszData = INOUTDIR;
				pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;
			}
			else if (ep & 0x80)
			{
				pFixedData->ColumnData[5].pszData = INDIR;
				pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;
			}
			else
			{
				pFixedData->ColumnData[5].pszData = OUTDIR;
				pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;
			}

			// Pipe�ڵ�[6]
			if (ep == 0)
			{
				pFixedData->ColumnData[6].bIsString = TRUE;
			}
			else
			{
				pFixedData->ColumnData[6].Data = pSource->stUrbPipeRequest.u64PipeHandle;
				pFixedData->ColumnData[6].nRadix = 16;
			}

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = (U64)ep;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;

			// ��ûũ��[10]
			pFixedData->ColumnData[10].bIsString = TRUE;

			// ����ũ��[11]
			pFixedData->ColumnData[11].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_GET_CURRENT_FRAME_NUMBER:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		pFixedData->ColumnData[3].pszData = _T("GET CURRENT FRAME NUMBER");

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion);
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}

		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"FrameNumber:", (U8 *)&pSource->stUrbGetCurrentFrameNumber.u32FrameNumber, sizeof(U32), sizeof(U32));
		}

		// ��ûũ��[10]
		pFixedData->ColumnData[10].bIsString = TRUE;

		// ����ũ��[11]
		pFixedData->ColumnData[11].bIsString = TRUE;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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
			pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_GET_CURRENT_FRAME_NUMBER");

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;
			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				if (pSource->stUrbHeader.u32Status == 0)
					pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"FrameNumber:", (U8 *)&pSource->stUrbGetCurrentFrameNumber.u32FrameNumber, sizeof(U32), sizeof(U32));
			}

			// ��ûũ��[10]
			pFixedData->ColumnData[10].bIsString = TRUE;

			// ����ũ��[11]
			pFixedData->ColumnData[11].bIsString = TRUE;

			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_CONTROL_TRANSFER:
	case URB_FUNCTION_CONTROL_TRANSFER_EX:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		pFixedData->ColumnData[3].pszData = _T("CONTROL TRANSFER");

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;

		switch (pSource->stUrbHeader.u16Function)
		{
		case URB_FUNCTION_CONTROL_TRANSFER:
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion, pSource->stUrbControlTransfer.u8SetupPacket[3]);
			}
			else
			{
				pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16Function, pSource->stUrbControlTransfer.u8SetupPacket[3]);
			}
			break;
		case URB_FUNCTION_CONTROL_TRANSFER_EX:
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion, pSource->stUrbControlTransferEx.u8SetupPacket[3]);
			}
			else
			{
				pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16Function, pSource->stUrbControlTransferEx.u8SetupPacket[3]);
			}
			break;
		}
		pFixedData->ColumnData[8].bIsConstant = TRUE;
		if (pSource->stUrbHeader.u32Status == 0)
			pFixedData->ColumnData[8].pszData = L"����";
		else
			pFixedData->ColumnData[8].pszData = L"����";

		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;

		switch (pSource->stUrbHeader.u16Function)
		{
		case URB_FUNCTION_CONTROL_TRANSFER:
			if (pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer)
			{
				if (pSource->stUrbControlTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
				{
					if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
					{
						if (pSource->stUrbHeader.u32Status == 0)
						{
							if (pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer > 8)
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer,
								(U64)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer, 8);
							else
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer,
								(U64)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer);
						}
						else
						{
							// No
						}
					}
					else
					{
						// No
					}
				}
				else
				{
					if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
					{
						if (pSource->stUrbHeader.u32Status == 0)
						{
							if (pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer > 8)
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer,
									(U64)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer, 8);
							else
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer,
								(U64)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer);
						}
						else
						{
							// No
						}
					}
					else
					{
						if (pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer > 8)
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer,
								(U64)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer, 8);
						else
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer,
								(U64)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer);
					}
				}
			}
			break;
		case URB_FUNCTION_CONTROL_TRANSFER_EX:
			if (pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer)
			{
				if (pSource->stUrbControlTransferEx.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
				{
					if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
					{
						if (pSource->stUrbHeader.u32Status == 0)
						{
							if (pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer > 8)
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransferEx.stTransferBuffer.u64OffsetBuffer,
								(U64)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer, 8);
							else
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransferEx.stTransferBuffer.u64OffsetBuffer,
								(U64)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer);
						}
						else
						{
							// No
						}
					}
					else
					{
						// No
					}
				}
				else
				{
					if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
					{
						if (pSource->stUrbHeader.u32Status == 0)
						{
							if (pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer > 8)
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransferEx.stTransferBuffer.u64OffsetBuffer,
								(U64)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer, 8);
							else
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransferEx.stTransferBuffer.u64OffsetBuffer,
								(U64)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer);
						}
						else
						{
							// No
						}
					}
					else
					{
						if (pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer > 8)
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransferEx.stTransferBuffer.u64OffsetBuffer,
							(U64)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer, 8);
						else
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlTransferEx.stTransferBuffer.u64OffsetBuffer,
							(U64)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer);
					}
				}
			}
			break;
		}

		// ��ûũ��[10]
		switch (pSource->stUrbHeader.u16Function)
		{
		case URB_FUNCTION_CONTROL_TRANSFER:
			pFixedData->ColumnData[10].Data = pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer;
			pFixedData->ColumnData[10].nRadix = 10;
			break;
		case URB_FUNCTION_CONTROL_TRANSFER_EX:
			pFixedData->ColumnData[10].Data = pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer;
			pFixedData->ColumnData[10].nRadix = 10;
			break;
		}

		// ����ũ��[11]
		switch (pSource->stUrbHeader.u16Function)
		{
		case URB_FUNCTION_CONTROL_TRANSFER:
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[11].Data = pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer;
				pFixedData->ColumnData[11].nRadix = 10;
			}
			else
				pFixedData->ColumnData[11].bIsString = TRUE;
			break;
		case URB_FUNCTION_CONTROL_TRANSFER_EX:
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[11].Data = pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer;
				pFixedData->ColumnData[11].nRadix = 10;
			}
			else
				pFixedData->ColumnData[11].bIsString = TRUE;
			break;
		}


		// �ð�[VIEWCOL_TIME]
		pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
		pFixedData->ColumnData[1].nRadix = 10;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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
			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_CONTROL_TRANSFER:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CONTROL_TRANSFER");
				break;
			case URB_FUNCTION_CONTROL_TRANSFER_EX:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CONTROL_TRANSFER_EX");
				break;
			}

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;

			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;

			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_CONTROL_TRANSFER:
				pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Setup:", (U8*)pSource->stUrbControlTransfer.u8SetupPacket, 8, 8);
				break;
			case URB_FUNCTION_CONTROL_TRANSFER_EX:
				pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Setup:", (U8*)pSource->stUrbControlTransferEx.u8SetupPacket, 8, 8);
				break;
			}


			// ��ûũ��[10]
			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_CONTROL_TRANSFER:
				pFixedData->ColumnData[10].Data = pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer;
				pFixedData->ColumnData[10].nRadix = 10;
				break;
			case URB_FUNCTION_CONTROL_TRANSFER_EX:
				pFixedData->ColumnData[10].Data = pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer;
				pFixedData->ColumnData[10].nRadix = 10;
				break;
			}

			// ����ũ��[11]
			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_CONTROL_TRANSFER:
				if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				{
					pFixedData->ColumnData[11].Data = pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer;
					pFixedData->ColumnData[11].nRadix = 10;
				}
				else
					pFixedData->ColumnData[11].bIsString = TRUE;
				break;
			case URB_FUNCTION_CONTROL_TRANSFER_EX:
				if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				{
					pFixedData->ColumnData[11].Data = pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer;
					pFixedData->ColumnData[11].nRadix = 10;
				}
				else
					pFixedData->ColumnData[11].bIsString = TRUE;
				break;
			}


			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_VENDOR_DEVICE:
	case URB_FUNCTION_VENDOR_INTERFACE:
	case URB_FUNCTION_VENDOR_ENDPOINT:
	case URB_FUNCTION_VENDOR_OTHER:
	case URB_FUNCTION_CLASS_DEVICE:
	case URB_FUNCTION_CLASS_INTERFACE:
	case URB_FUNCTION_CLASS_ENDPOINT:
	case URB_FUNCTION_CLASS_OTHER:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		pFixedData->ColumnData[3].pszData = _T("CONTROL TRANSFER");

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion);
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}

		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;
		if (pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer)
		{
			if (pSource->stUrbControlVendorOrClassRequest.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
				if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				{
					if (pSource->stUrbHeader.u32Status == 0)
					{
						if (pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer > 8)
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64OffsetBuffer,
							(U64)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer, 8);
						else
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64OffsetBuffer,
							(U64)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer);
					}
					else
					{
						// No
					}
				}
				else
				{
					// No
				}
			}
			else
			{
				if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				{
					if (pSource->stUrbHeader.u32Status == 0)
					{
						if (pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer > 8)
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64OffsetBuffer,
							(U64)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer, 8);
						else
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64OffsetBuffer,
							(U64)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer);
					}
					else
					{
						// No
					}
				}
				else
				{
					if (pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer > 8)
						pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64OffsetBuffer,
						(U64)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer, 8);
					else
						pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64OffsetBuffer,
						(U64)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		// ��ûũ��[10]
		pFixedData->ColumnData[10].Data = pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer;
		pFixedData->ColumnData[10].nRadix = 10;

		// ����ũ��[11]
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[11].Data = pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer;
			pFixedData->ColumnData[11].nRadix = 10;
		}
		else
			pFixedData->ColumnData[11].bIsString = TRUE;

		// �ð�[VIEWCOL_TIME]
		pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
		pFixedData->ColumnData[1].nRadix = 10;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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

			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_VENDOR_DEVICE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_VENDOR_DEVICE");
				break;
			case URB_FUNCTION_VENDOR_INTERFACE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_VENDOR_INTERFACE");
				break;
			case URB_FUNCTION_VENDOR_ENDPOINT:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_VENDOR_ENDPOINT");
				break;
			case URB_FUNCTION_VENDOR_OTHER:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_VENDOR_OTHER");
				break;
			case URB_FUNCTION_CLASS_DEVICE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CLASS_DEVICE");
				break;
			case URB_FUNCTION_CLASS_INTERFACE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CLASS_INTERFACE");
				break;
			case URB_FUNCTION_CLASS_ENDPOINT:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CLASS_ENDPOINT");
				break;
			case URB_FUNCTION_CLASS_OTHER:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CLASS_OTHER");
				break;
			}

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;
			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;
			pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Setup:", (U8*)pSource->stUrbControlTransfer.u8SetupPacket, 8, 8);

			// ��ûũ��[10]
			pFixedData->ColumnData[10].Data = pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer;
			pFixedData->ColumnData[10].nRadix = 10;

			// ����ũ��[11]
			pFixedData->ColumnData[11].nRadix = 10;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				pFixedData->ColumnData[11].Data = pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer;
			else
				pFixedData->ColumnData[11].bIsString = TRUE;

			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		pFixedData->ColumnData[3].pszData = _T("GET DESCRIPTOR");

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;

		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion, pSource->stUrbControlDescriptorRequest.u8DescriptorType);
		}
		else
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16Function, pSource->stUrbControlDescriptorRequest.u8DescriptorType);
		}

		pFixedData->ColumnData[8].bIsConstant = TRUE;
		if (pSource->stUrbHeader.u32Status == 0)
			pFixedData->ColumnData[8].pszData = L"����";
		else
			pFixedData->ColumnData[8].pszData = L"����";

		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			if (pSource->stUrbHeader.u32Status == 0)
			{
				if (pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer > 8)
					pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Descriptor:", (U8*)pSource + pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64OffsetBuffer,
					(U64)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer, 8);
				else
					pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Descriptor:", (U8*)pSource + pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64OffsetBuffer,
					(U64)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer);
			}
		}

		// ��ûũ��[10]
		pFixedData->ColumnData[10].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
		pFixedData->ColumnData[10].nRadix = 10;

		// ����ũ��[11]
		pFixedData->ColumnData[11].nRadix = 10;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			pFixedData->ColumnData[11].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
		else
			pFixedData->ColumnData[11].bIsString = TRUE;

		// �ð�[VIEWCOL_TIME]
		pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
		pFixedData->ColumnData[1].nRadix = 10;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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

			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE");
				break;
			case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE");
				break;
			case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT");
				break;
			}

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;
			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				if (pSource->stUrbHeader.u32Status == 0)
				{
					pFixedData->ColumnData[9].bIsConstant = TRUE;

					switch (pSource->stUrbControlDescriptorRequest.u8DescriptorType)
					{
					case USB_DEVICE_DESCRIPTOR_TYPE:
						pFixedData->ColumnData[9].pszData = L"Type: USB_DEVICE_DESCRIPTOR_TYPE";
						break;
					case USB_CONFIGURATION_DESCRIPTOR_TYPE:
						pFixedData->ColumnData[9].pszData = L"Type: USB_CONFIGURATION_DESCRIPTOR_TYPE";
						break;
					case USB_STRING_DESCRIPTOR_TYPE:
						pFixedData->ColumnData[9].pszData = L"Type: USB_STRING_DESCRIPTOR_TYPE";
						break;
					}
				}
			}

			// ��ûũ��[10]
			pFixedData->ColumnData[10].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
			pFixedData->ColumnData[10].nRadix = 10;

			// ����ũ��[11]
			pFixedData->ColumnData[11].nRadix = 10;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				pFixedData->ColumnData[11].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
			else
				pFixedData->ColumnData[11].bIsString = TRUE;

			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		pFixedData->ColumnData[3].pszData = _T("SET DESCRIPTOR");

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;


		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}

		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion, pSource->stUrbControlDescriptorRequest.u8DescriptorType);
		}
		else
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16Function, pSource->stUrbControlDescriptorRequest.u8DescriptorType);
		}

		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion);
			if (pSource->stUrbHeader.u32Status == 0)
			{
				if (pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer > 8)
					pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Descriptor:", (U8*)pSource + pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64OffsetBuffer,
					(U64)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer, 8);
				else
					pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Descriptor:", (U8*)pSource + pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64OffsetBuffer,
					(U64)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer, pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer);
			}
		}

		// ��ûũ��[10]
		pFixedData->ColumnData[10].nRadix = 10;
		pFixedData->ColumnData[10].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;

		// ����ũ��[11]
		pFixedData->ColumnData[11].nRadix = 10;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			pFixedData->ColumnData[11].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
		else
			pFixedData->ColumnData[11].bIsString = TRUE;

		// �ð�[VIEWCOL_TIME]
		pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
		pFixedData->ColumnData[1].nRadix = 10;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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

			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE");
				break;
			case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE");
				break;
			case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT");
				break;
			}

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;
			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				if (pSource->stUrbHeader.u32Status == 0)
				{
					pFixedData->ColumnData[9].bIsConstant = TRUE;

					switch (pSource->stUrbControlDescriptorRequest.u8DescriptorType)
					{
					case USB_DEVICE_DESCRIPTOR_TYPE:
						pFixedData->ColumnData[9].pszData = L"Type: USB_DEVICE_DESCRIPTOR_TYPE";
						break;
					case USB_CONFIGURATION_DESCRIPTOR_TYPE:
						pFixedData->ColumnData[9].pszData = L"Type: USB_CONFIGURATION_DESCRIPTOR_TYPE";
						break;
					case USB_STRING_DESCRIPTOR_TYPE:
						pFixedData->ColumnData[9].pszData = L"Type: USB_STRING_DESCRIPTOR_TYPE";
						break;
					}
				}
			}

			// ��ûũ��[10]
			pFixedData->ColumnData[10].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
			pFixedData->ColumnData[10].nRadix = 10;

			// ����ũ��[11]
			pFixedData->ColumnData[11].nRadix = 10;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				pFixedData->ColumnData[11].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
			else
				pFixedData->ColumnData[11].bIsString = TRUE;

			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_GET_CONFIGURATION:
	case URB_FUNCTION_GET_INTERFACE:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		switch (pSource->stUrbHeader.u16Function)
		{
		case URB_FUNCTION_GET_CONFIGURATION:
			pFixedData->ColumnData[3].pszData = _T("GET CONFIG VALUE");
			break;
		case URB_FUNCTION_GET_INTERFACE:
			pFixedData->ColumnData[3].pszData = _T("GET ALTINTERFACE NUMBER");
			break;
		}

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}

		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion);
			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_GET_CONFIGURATION:
				if (pSource->stUrbHeader.u32Status == 0)
					pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Value:", (U8*)pSource + pSource->stUrbControlGetConfigurationRequest.stTransferBuffer.u64OffsetBuffer,
						1, 1);
				break;
			case URB_FUNCTION_GET_INTERFACE:
				if (pSource->stUrbHeader.u32Status == 0)
					pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"AltInt:", (U8*)pSource + pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64OffsetBuffer,
						1, 1);
				break;
			}

		}

		// ��ûũ��[10]
		pFixedData->ColumnData[10].nRadix = 10;
		switch (pSource->stUrbHeader.u16Function)
		{
		case URB_FUNCTION_GET_CONFIGURATION:
			pFixedData->ColumnData[10].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
			break;
		case URB_FUNCTION_GET_INTERFACE:
			pFixedData->ColumnData[10].Data = pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64LengthBuffer;
			break;
		}

		// ����ũ��[11]
		pFixedData->ColumnData[11].nRadix = 10;
		switch (pSource->stUrbHeader.u16Function)
		{
		case URB_FUNCTION_GET_CONFIGURATION:
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				pFixedData->ColumnData[11].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
			else
				pFixedData->ColumnData[11].bIsString = TRUE;
			break;
		case URB_FUNCTION_GET_INTERFACE:
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				pFixedData->ColumnData[11].Data = pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64LengthBuffer;
			else
				pFixedData->ColumnData[11].bIsString = TRUE;
			break;
		}

		// �ð�[VIEWCOL_TIME]
		pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
		pFixedData->ColumnData[1].nRadix = 10;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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
			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_GET_CONFIGURATION:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_GET_CONFIGURATION");
				break;
			case URB_FUNCTION_GET_INTERFACE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_GET_INTERFACE");
				break;
			}


			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;
			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				switch (pSource->stUrbHeader.u16Function)
				{
				case URB_FUNCTION_GET_CONFIGURATION:
					if (pSource->stUrbHeader.u32Status == 0)
						pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Value:", (U8*)pSource + pSource->stUrbControlGetConfigurationRequest.stTransferBuffer.u64OffsetBuffer,
							1, 1);
					break;
				case URB_FUNCTION_GET_INTERFACE:
					if (pSource->stUrbHeader.u32Status == 0)
						pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"AltInt:", (U8*)pSource + pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64OffsetBuffer,
							1, 1);
					break;
				}
			}

			// ��ûũ��[10]
			pFixedData->ColumnData[10].nRadix = 10;
			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_GET_CONFIGURATION:
				pFixedData->ColumnData[10].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
				break;
			case URB_FUNCTION_GET_INTERFACE:
				pFixedData->ColumnData[10].Data = pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64LengthBuffer;
				break;
			}

			// ����ũ��[11]
			pFixedData->ColumnData[11].nRadix = 10;
			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_GET_CONFIGURATION:
				if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
					pFixedData->ColumnData[11].Data = pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
				else
					pFixedData->ColumnData[11].bIsString = TRUE;
				break;
			case URB_FUNCTION_GET_INTERFACE:
				if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
					pFixedData->ColumnData[11].Data = pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64LengthBuffer;
				else
					pFixedData->ColumnData[11].bIsString = TRUE;
				break;
			}

			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_GET_STATUS_FROM_DEVICE:
	case URB_FUNCTION_GET_STATUS_FROM_INTERFACE:
	case URB_FUNCTION_GET_STATUS_FROM_ENDPOINT:
	case URB_FUNCTION_GET_STATUS_FROM_OTHER:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		pFixedData->ColumnData[3].pszData = _T("GET STATUS");

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion);
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}

		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Status:", (U8*)pSource + pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64OffsetBuffer,
					2, 2);
		}

		// ��ûũ��[10]
		pFixedData->ColumnData[10].Data = pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64LengthBuffer;
		pFixedData->ColumnData[10].nRadix = 10;

		// ����ũ��[11]
		pFixedData->ColumnData[11].nRadix = 10;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			pFixedData->ColumnData[11].Data = pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64LengthBuffer;
		else
			pFixedData->ColumnData[11].bIsString = TRUE;

		// �ð�[VIEWCOL_TIME]
		pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
		pFixedData->ColumnData[1].nRadix = 10;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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

			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_GET_STATUS_FROM_DEVICE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_GET_STATUS_FROM_DEVICE");
				break;
			case URB_FUNCTION_GET_STATUS_FROM_INTERFACE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_GET_STATUS_FROM_INTERFACE");
				break;
			case URB_FUNCTION_GET_STATUS_FROM_ENDPOINT:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_GET_STATUS_FROM_ENDPOINT");
				break;
			case URB_FUNCTION_GET_STATUS_FROM_OTHER:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_GET_STATUS_FROM_OTHER");
				break;
			}

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;
			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				if (pSource->stUrbHeader.u32Status == 0)
					pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Status:", (U8*)pSource + pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64OffsetBuffer,
						2, 2);
			}

			// ��ûũ��[10]
			pFixedData->ColumnData[10].Data = pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64LengthBuffer;
			pFixedData->ColumnData[10].nRadix = 10;

			// ����ũ��[11]
			pFixedData->ColumnData[11].nRadix = 10;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				pFixedData->ColumnData[11].Data = pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64LengthBuffer;
			else
				pFixedData->ColumnData[11].bIsString = TRUE;

			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_SET_FEATURE_TO_DEVICE:
	case URB_FUNCTION_SET_FEATURE_TO_INTERFACE:
	case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT:
	case URB_FUNCTION_SET_FEATURE_TO_OTHER:
	case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE:
	case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE:
	case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT:
	case URB_FUNCTION_CLEAR_FEATURE_TO_OTHER:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		switch (pSource->stUrbHeader.u16Function)
		{
		case URB_FUNCTION_SET_FEATURE_TO_DEVICE:
		case URB_FUNCTION_SET_FEATURE_TO_INTERFACE:
		case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT:
		case URB_FUNCTION_SET_FEATURE_TO_OTHER:
			pFixedData->ColumnData[3].pszData = _T("SET FEATURE");
			break;
		case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE:
		case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE:
		case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT:
		case URB_FUNCTION_CLEAR_FEATURE_TO_OTHER:
			pFixedData->ColumnData[3].pszData = _T("CLEAR FEATURE");
			break;
		}

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[3].pszData = replaceItemForControlTransfer(pSource->stUrbHeader.u16FunctionForCompletion);
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}
		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;

		// ��ûũ��[10]
		pFixedData->ColumnData[10].bIsString = TRUE;

		// ����ũ��[11]
		pFixedData->ColumnData[11].bIsString = TRUE;

		// �ð�[VIEWCOL_TIME]
		pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
		pFixedData->ColumnData[1].nRadix = 10;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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

			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_SET_FEATURE_TO_DEVICE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_SET_FEATURE_TO_DEVICE");
				break;
			case URB_FUNCTION_SET_FEATURE_TO_INTERFACE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_SET_FEATURE_TO_INTERFACE");
				break;
			case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_SET_FEATURE_TO_ENDPOINT");
				break;
			case URB_FUNCTION_SET_FEATURE_TO_OTHER:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_SET_FEATURE_TO_OTHER");
				break;
			case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE");
				break;
			case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE");
				break;
			case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT");
				break;
			case URB_FUNCTION_CLEAR_FEATURE_TO_OTHER:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CLEAR_FEATURE_TO_OTHER");
				break;
			}

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;
			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Feature Selector:", (U8*)&pSource->stUrbControlFeatureRequest.u16FeatureSelector,
					2, 2);

			// ��ûũ��[10]
			pFixedData->ColumnData[10].bIsString = TRUE;

			// ����ũ��[11]
			pFixedData->ColumnData[11].bIsString = TRUE;

			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_OPEN_STATIC_STREAMS:
	case URB_FUNCTION_CLOSE_STATIC_STREAMS:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		switch (pSource->stUrbHeader.u16Function)
		{
		case URB_FUNCTION_OPEN_STATIC_STREAMS:
			pFixedData->ColumnData[3].pszData = _T("OPEN STREAM");
			break;
		case URB_FUNCTION_CLOSE_STATIC_STREAMS:
			pFixedData->ColumnData[3].pszData = _T("CLOSE STREAM");
			break;
		}

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;
		pFixedData->ColumnData[5].pszData = INOUTDIR;
		pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_INOUT;

		// Pipe�ڵ�[6]
		pFixedData->ColumnData[6].bIsString = TRUE;

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = 0;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}
		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;

		// ��ûũ��[10]
		pFixedData->ColumnData[10].bIsString = TRUE;

		// ����ũ��[11]
		pFixedData->ColumnData[10].bIsString = TRUE;

		// �ð�[VIEWCOL_TIME]
		pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
		pFixedData->ColumnData[1].nRadix = 10;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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
			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_OPEN_STATIC_STREAMS:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_OPEN_STATIC_STREAMS");
				break;
			case URB_FUNCTION_CLOSE_STATIC_STREAMS:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_CLOSE_STATIC_STREAMS");
				break;
			}

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��Ʈ��");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;
			pFixedData->ColumnData[5].pszData = INOUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;

			// Pipe�ڵ�[6]
			pFixedData->ColumnData[6].bIsString = TRUE;

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = 0;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;

			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_OPEN_STATIC_STREAMS:
				if (pSource->stUrbHeader.u32Status == 0)
					pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Streams:", (U8*)&pSource->stUrbOpenStaticStreams.u32NumberOfStreams,
						4, 4);
				break;
			}


			// ��ûũ��[10]
			pFixedData->ColumnData[10].bIsString = TRUE;

			// ����ũ��[11]
			pFixedData->ColumnData[11].bIsString = TRUE;

			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_ISOCH_TRANSFER:
	case URB_FUNCTION_ISOCH_TRANSFER_USING_CHAINED_MDL:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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
		pFixedData->ColumnData[3].pszData = _T("ISOCHRONOUS TRANSFER");

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;
		pFixedData->ColumnData[4].pszData = _T("��ü�");

		// ����[5]
		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;

		// Pipe�ڵ�[6]
		ep = parserHlpCmn_TranslatePipeHandleToEP(pParser_Context, pSource->stUrbIsochronousTransfer.u64PipeHandle);
		if (ep == 0)
		{
			pFixedData->ColumnData[6].bIsString = TRUE;
		}
		else
		{
			pFixedData->ColumnData[6].Data = pSource->stUrbIsochronousTransfer.u64PipeHandle;
			pFixedData->ColumnData[6].nRadix = 16;
		}

		// Endpoint Address[7]
		ep = parserHlpCmn_TranslatePipeHandleToEP(pParser_Context, pSource->stUrbIsochronousTransfer.u64PipeHandle);
		pFixedData->ColumnData[7].Data = (U64)ep;
		pFixedData->ColumnData[7].nRadix = 10;

		if (ep & 0x80)
		{
			pFixedData->ColumnData[5].pszData = INDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_IN;
		}
		else
		{
			pFixedData->ColumnData[5].pszData = OUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_OUT;
		}

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}

		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;
		if (pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer)
		{
			if (pSource->stUrbIsochronousTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
				if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				{
					if (pSource->stUrbHeader.u32Status == 0)
					{
						if (pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer > 8)
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbIsochronousTransfer.stTransferBuffer.u64OffsetBuffer,
								pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer, 8);
						else
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbIsochronousTransfer.stTransferBuffer.u64OffsetBuffer,
								pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer);
					}
					else
					{
						// No
					}
				}
				else
				{
					// No
				}
			}
			else
			{
				if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				{
					if (pSource->stUrbHeader.u32Status == 0)
					{
						if (pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer > 8)
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbIsochronousTransfer.stTransferBuffer.u64OffsetBuffer,
								pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer, 8);
						else
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbIsochronousTransfer.stTransferBuffer.u64OffsetBuffer,
								pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer);
					}
					else
					{
						// No
					}
				}
				else
				{
					if (pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer > 8)
						pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbIsochronousTransfer.stTransferBuffer.u64OffsetBuffer,
							pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer, 8);
					else
						pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbIsochronousTransfer.stTransferBuffer.u64OffsetBuffer,
							pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		// ��ûũ��[10]
		pFixedData->ColumnData[10].Data = pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer;
		pFixedData->ColumnData[10].nRadix = 10;

		// ����ũ��[11]
		pFixedData->ColumnData[11].nRadix = 10;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			pFixedData->ColumnData[11].Data = pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer;
		else
			pFixedData->ColumnData[11].bIsString = TRUE;

		// �ð�[VIEWCOL_TIME]
		pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
		pFixedData->ColumnData[1].nRadix = 10;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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

			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_ISOCH_TRANSFER:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_ISOCH_TRANSFER");
				break;
			case URB_FUNCTION_ISOCH_TRANSFER_USING_CHAINED_MDL:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_ISOCH_TRANSFER_USING_CHAINED_MDL");
				break;
			}

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;
			pFixedData->ColumnData[4].pszData = _T("��ü�");

			// ����[5]
			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;

			// Pipe�ڵ�[6]
			ep = parserHlpCmn_TranslatePipeHandleToEP(pParser_Context, pSource->stUrbIsochronousTransfer.u64PipeHandle);
			if (ep == 0)
			{
				pFixedData->ColumnData[6].bIsString = TRUE;
			}
			else
			{
				pFixedData->ColumnData[6].Data = pSource->stUrbIsochronousTransfer.u64PipeHandle;
				pFixedData->ColumnData[6].nRadix = 16;
			}

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = (U64)ep;
			pFixedData->ColumnData[7].nRadix = 10;

			if (ep & 0x80)
			{
				pFixedData->ColumnData[5].pszData = INDIR;
				pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;
			}
			else
			{
				pFixedData->ColumnData[5].pszData = OUTDIR;
				pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;
			}

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;
			pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"NumberOfPackets:", (U8*)&pSource->stUrbIsochronousTransfer.u32NumberOfPackets, 4, 4);

			// ��ûũ��[10]
			pFixedData->ColumnData[10].nRadix = 10;
			pFixedData->ColumnData[10].Data = pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer;

			// ����ũ��[11]
			pFixedData->ColumnData[11].nRadix = 10;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				pFixedData->ColumnData[11].Data = pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer;
			else
				pFixedData->ColumnData[11].bIsString = TRUE;

			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
	case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL:
	{
		PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

		////// level 1 //////
		////// level 1 //////
		////// level 1 //////
		FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
		memset(pFixedData, 0, FixedDataSize);
		InitializeListHead(&pFixedData->__Self__);
		pFixedData->pDetailsPage = NULL;
		pFixedData->pDataPage = NULL;
		pFixedData->ColCount = COLUMNCOUNT;
		pFixedData->bIsLast = FALSE;
		pFixedData->Level = 0;

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

		// EP����[4]
		pFixedData->ColumnData[4].bIsString = TRUE;
		pFixedData->ColumnData[4].bIsConstant = TRUE;

		eptype = parserHlpCmn_TranslatePipeHandleToEPType(pParser_Context, pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle);
		if (eptype == 2) // bulk
		{
			pFixedData->ColumnData[4].pszData = _T("��ũ");
			pFixedData->ColumnData[3].pszData = _T("BULK TRANSFER");
		}
		else if (eptype == 3) // int
		{
			pFixedData->ColumnData[4].pszData = _T("���ͷ�Ʈ");
			pFixedData->ColumnData[3].pszData = _T("INTERRUPT TRANSFER");
		}

		// ����[5]
		ep = parserHlpCmn_TranslatePipeHandleToEP(pParser_Context, pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle);

		pFixedData->ColumnData[5].bIsString = TRUE;
		pFixedData->ColumnData[5].bIsConstant = TRUE;

		if (ep & 0x80)
		{
			pFixedData->ColumnData[5].pszData = INDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_IN;
		}
		else
		{
			pFixedData->ColumnData[5].pszData = OUTDIR;
			pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_OUT;
		}

		// Pipe�ڵ�[6]
		ep = parserHlpCmn_TranslatePipeHandleToEP(pParser_Context, pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle);
		if (ep == 0)
		{
			pFixedData->ColumnData[6].bIsString = TRUE;
		}
		else
		{
			pFixedData->ColumnData[6].Data = pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle;
			pFixedData->ColumnData[6].nRadix = 16;
		}

		// Endpoint Address[7]
		pFixedData->ColumnData[7].Data = (U64)ep;
		pFixedData->ColumnData[7].nRadix = 10;

		// ����[8]
		pFixedData->ColumnData[8].bIsString = TRUE;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
		{
			pFixedData->ColumnData[8].bIsConstant = TRUE;
			if (pSource->stUrbHeader.u32Status == 0)
				pFixedData->ColumnData[8].pszData = L"����";
			else
				pFixedData->ColumnData[8].pszData = L"����";
		}

		// ����[9]
		pFixedData->ColumnData[9].bIsString = TRUE;
		if (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer)
		{
			if (pSource->stUrbBulkOrInterruptTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
				if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				{
					if (pSource->stUrbHeader.u32Status == 0)
					{
						if (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer > 8)
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
								pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, 8);
						else
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
								pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);
					}
					else
					{
						// No
					}
				}
				else
				{
					// No
				}
			}
			else
			{
				if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				{
					if (pSource->stUrbHeader.u32Status == 0)
					{
						if (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer > 8)
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
								pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, 8);
						else
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
								pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);
					}
					else
					{
						// No
					}
				}
				else
				{
					if (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer > 8)
						pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
							pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, 8);
					else
						pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
							pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		// ��ûũ��[10]
		pFixedData->ColumnData[10].nRadix = 10;
		pFixedData->ColumnData[10].Data = pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer;

		// ����ũ��[11]
		pFixedData->ColumnData[11].nRadix = 10;
		if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			pFixedData->ColumnData[11].Data = pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer;
		else
			pFixedData->ColumnData[11].bIsString = TRUE;

		// �ð�[VIEWCOL_TIME]
		pFixedData->ColumnData[1].Data = pSource->stUrbHeader.stHeader.u64Time;
		pFixedData->ColumnData[1].nRadix = 10;

		// appendix
		pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
		pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);

		{
			PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;

			////// level 2 //////
			////// level 2 //////
			////// level 2 //////
			FixedDataSize = sizeof(HJ_PARSING_FIXED_DATA_ENTRY) + (sizeof(HJ_COLUMNDATA)*(COLUMNCOUNT - 1));
			pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)malloc(FixedDataSize);
			memset(pFixedData, 0, FixedDataSize);
			InitializeListHead(&pFixedData->__Self__);
			pFixedData->pDetailsPage = NULL;
			pFixedData->pDataPage = NULL;
			pFixedData->ColCount = COLUMNCOUNT;
			pFixedData->bIsLast = TRUE;
			pFixedData->Level = 1;

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

			switch (pSource->stUrbHeader.u16Function)
			{
			case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER");
				break;
			case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL:
				pFixedData->ColumnData[3].pszData = _T("URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER_USING_CHAINED_MDL");
				break;
			}

			// EP����[4]
			pFixedData->ColumnData[4].bIsString = TRUE;
			pFixedData->ColumnData[4].bIsConstant = TRUE;

			eptype = parserHlpCmn_TranslatePipeHandleToEPType(pParser_Context, pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle);
			if (eptype == 2) // bulk
			{
				pFixedData->ColumnData[4].pszData = _T("��ũ");
			}
			else if (eptype == 3) // int
			{
				pFixedData->ColumnData[4].pszData = _T("���ͷ�Ʈ");
			}

			// ����[5]
			ep = parserHlpCmn_TranslatePipeHandleToEP(pParser_Context, pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle);

			pFixedData->ColumnData[5].bIsString = TRUE;
			pFixedData->ColumnData[5].bIsConstant = TRUE;

			if (ep & 0x80)
			{
				pFixedData->ColumnData[5].pszData = INDIR;
				pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;
			}
			else
			{
				pFixedData->ColumnData[5].pszData = OUTDIR;
				pFixedData->IndexForViewerImage = VIEWIMAGEINDEX_DEFAULT;
			}

			// Pipe�ڵ�[6]
			ep = parserHlpCmn_TranslatePipeHandleToEP(pParser_Context, pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle);
			if (ep == 0)
			{
				pFixedData->ColumnData[6].bIsString = TRUE;
			}
			else
			{
				pFixedData->ColumnData[6].Data = pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle;
				pFixedData->ColumnData[6].nRadix = 16;
			}

			// Endpoint Address[7]
			pFixedData->ColumnData[7].Data = (U64)ep;
			pFixedData->ColumnData[7].nRadix = 10;

			// ����[8]
			pFixedData->ColumnData[8].bIsString = TRUE;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
			{
				pFixedData->ColumnData[8].bIsConstant = TRUE;
				pFixedData->ColumnData[8].pszData = parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbHeader.u32Status);
			}

			// ����[9]
			pFixedData->ColumnData[9].bIsString = TRUE;
			if (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer)
			{
				if (pSource->stUrbBulkOrInterruptTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
				{
					if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
					{
						if (pSource->stUrbHeader.u32Status == 0)
						{
							if (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer > 8)
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
									pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, 8);
							else
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
									pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);
						}
						else
						{
							// No
						}
					}
					else
					{
						// No
					}
				}
				else
				{
					if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
					{
						if (pSource->stUrbHeader.u32Status == 0)
						{
							if (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer > 8)
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
									pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, 8);
							else
								pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
									pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);
						}
						else
						{
							// No
						}
					}
					else
					{
						if (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer > 8)
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
								pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, 8);
						else
							pFixedData->ColumnData[9].pszData = parserHlpCmn_translateString(L"Data:", (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer,
								pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer, pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);
					}
				}
			}

			// ��ûũ��[10]
			pFixedData->ColumnData[10].Data = pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer;
			pFixedData->ColumnData[10].nRadix = 10;

			// ����ũ��[11]
			pFixedData->ColumnData[11].nRadix = 10;
			if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
				pFixedData->ColumnData[11].Data = pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer;
			else
				pFixedData->ColumnData[11].bIsString = TRUE;

			// �ð�[VIEWCOL_TIME]
			pFixedData->ColumnData[1].bIsString = TRUE;

			// appendix
			pFixedData->pDetailsPage = parserHlpCmn_FillDetailsPage(pParser_Context, pSource, pFixedData->Level);
			pFixedData->pDataPage = parserHlpCmn_FillDataPages(pParser_Context, pSource, pFixedData->Level);
		}
	}
	break;

	default:
		break;
	}

	*ppBuffer = (void *)pBaseFixedData;
	*pResultSize = sizeof(PHJ_PARSING_FIXED_DATA);

	bRet = TRUE;
	return bRet;
}
