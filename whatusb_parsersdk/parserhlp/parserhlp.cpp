#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include ".\\..\\whatusb_common_header\\parserhlp.h"

extern PPARSER_CONTEXT g_pParser_Context_for_Output;

// View로 문자열을 출력하는 함수
BOOL parserHlpCmn_OutputStringToView(WCHAR * String)
{
	BOOL bRet = FALSE;
	WCHAR * TempString = NULL;

	if (g_pParser_Context_for_Output == NULL)
		return bRet;

	TempString = (WCHAR *)malloc((wcslen(String) + 1) * sizeof(WCHAR));
	wcscpy(TempString, String);
	if (g_pParser_Context_for_Output->hlp_function_list.pfn_ViewOutput)
		bRet = g_pParser_Context_for_Output->hlp_function_list.pfn_ViewOutput(g_pParser_Context_for_Output->MgrHandle, TempString);
	free(TempString);
	return bRet;
}

// MessageBox를 출력하는 함수
int parserHlpCmn_MessageBoxToView(WCHAR * pTitle, WCHAR * pMessage, int Type, U32 dwmsecTimeout)
{
	int wRet = IDABORT;

	if (g_pParser_Context_for_Output == NULL)
		return wRet;

	if (g_pParser_Context_for_Output->hlp_function_list.pfn_MessageBox)
		wRet = g_pParser_Context_for_Output->hlp_function_list.pfn_MessageBox(g_pParser_Context_for_Output->MgrHandle, pTitle, pMessage, Type, dwmsecTimeout);

	return wRet;
}

void freeFieldFromValue(PHJ_PARSING_VALUE pValue)
{
	SIZE_T i;
	SIZE_T count;
	PHJ_PARSING_FIELD_VALUE pFieldValue = NULL;

	count = pValue->nRowFieldNameCount;
	for (i = 0; i < count; i++)
	{
		pFieldValue = (PHJ_PARSING_FIELD_VALUE)RemoveHeadList(&pValue->__FieldHead__);
		if (pFieldValue->bRowFieldValueType & HJ_VALUE_EXTRA_BUFF)
		{
			free(pFieldValue->nRowFieldBuffer);
		}
		if (pFieldValue->bRowFieldValueType & HJ_VALUE_EXTRA_PRIVATEBUFF)
		{
			free(pFieldValue->nRowFieldPrivateBuffer);
		}
		free(pFieldValue);
	}
}

void freePartialData(PHJ_PARSING_DATA pData)
{
	PHJ_PARSING_DATA pSubData = NULL;
	PHJ_PARSING_VALUE pValue = NULL;
	PHJ_PARSING_FIELD_VALUE pFieldValue = NULL;
	SIZE_T count = 0;

	pValue = NULL;
	pFieldValue = NULL;

	while (IsListEmpty(&pData->__Child__) != TRUE)
	{
		pSubData = (PHJ_PARSING_DATA)RemoveHeadList(&pData->__Child__);
		for (count = 0; count < pSubData->nParsingValueCount; count++)
		{
			pValue = (PHJ_PARSING_VALUE)RemoveHeadList(&pSubData->__ValueHead__);
			freeFieldFromValue(pValue);
			free(pValue);
		}
		free(pSubData);
	}

	// 원래 구조체를 메모리에서 해재한다
	for (count = 0; count < pData->nParsingValueCount; count++)
	{
		pValue = (PHJ_PARSING_VALUE)RemoveHeadList(&pData->__ValueHead__);
		freeFieldFromValue(pValue);
		free(pValue);
	}
	free(pData);
}

WCHAR * translateFeatureSelector(U16 FeatureSelector, BOOL bDevice, BOOL bInterface, BOOL bEndpoint)
{
	WCHAR * bRet = NULL;

	if (bDevice == TRUE)
	{
		switch (FeatureSelector)
		{
		case 1:
			bRet = L"DEVICE_REMOTE_WAKEUP";
			break;
		case 2:
			bRet = L"TEST_MODE";
			break;
		case 3:
			bRet = L"b_HNP_ENABLE";
			break;
		case 4:
			bRet = L"a_HNP_SUPPORT";
			break;
		case 5:
			bRet = L"a_alt_HNP_SUPPORT";
			break;
		case 6:
			bRet = L"WDEVICE";
			break;
		case 48:
			bRet = L"U1_ENABLE";
			break;
		case 49:
			bRet = L"U2_ENABLE";
			break;
		case 50:
			bRet = L"LTM_ENABLE";
			break;
		case 51:
			bRet = L"B3_NTF_HOST_REL";
			break;
		case 52:
			bRet = L"B3_RSP_ENABLE";
			break;
		case 53:
			bRet = L"LDM_ENABLE";
			break;
		default:
			bRet = L"UNKNOWN FEATURE SELECTOR";
			break;
		}
	}
	else if (bInterface == TRUE)
	{
		switch (FeatureSelector)
		{
		case 0:
			bRet = L"FUNCTION_SUSPEND";
			break;
		default:
			bRet = L"UNKNOWN FEATURE SELECTOR";
			break;
		}
	}
	else if (bEndpoint == TRUE)
	{
		switch (FeatureSelector)
		{
		case 0:
			bRet = L"ENDPOINT_HALT";
			break;
		default:
			bRet = L"UNKNOWN FEATURE SELECTOR";
			break;
		}
	}
	else
	{
		bRet = L"UNKNOWN FEATURE SELECTOR";
	}
	return bRet;
}

WCHAR * translateIRPStatus(U32 u32NtStatus)
{
	WCHAR * bRet = NULL;

	switch (u32NtStatus)
	{
	case 0:
		bRet = L"STATUS_SUCCESS";
		break;
	case STATUS_TIMEOUT:
		bRet = L"STATUS_TIMEOUT";
		break;
	case STATUS_PENDING:
		bRet = L"STATUS_PENDING";
		break;
	case STATUS_BUFFER_OVERFLOW:
		bRet = L"STATUS_BUFFER_OVERFLOW";
		break;
	case STATUS_DEVICE_POWERED_OFF:
		bRet = L"STATUS_DEVICE_POWERED_OFF";
		break;
	case STATUS_DEVICE_BUSY:
		bRet = L"STATUS_DEVICE_BUSY";
		break;
	case STATUS_BUS_RESET:
		bRet = L"STATUS_BUS_RESET";
		break;
	case STATUS_UNSUCCESSFUL:
		bRet = L"STATUS_UNSUCCESSFUL";
		break;
	case STATUS_ACCESS_VIOLATION:
		bRet = L"STATUS_ACCESS_VIOLATION";
		break;
	case STATUS_INVALID_HANDLE:
		bRet = L"STATUS_INVALID_HANDLE";
		break;
	case STATUS_INVALID_PARAMETER:
		bRet = L"STATUS_INVALID_PARAMETER";
		break;
	case STATUS_NO_SUCH_DEVICE:
		bRet = L"STATUS_NO_SUCH_DEVICE";
		break;
	case STATUS_INVALID_DEVICE_REQUEST:
		bRet = L"STATUS_INVALID_DEVICE_REQUEST";
		break;
	case STATUS_MORE_PROCESSING_REQUIRED:
		bRet = L"STATUS_MORE_PROCESSING_REQUIRED";
		break;
	case STATUS_ACCESS_DENIED:
		bRet = L"STATUS_ACCESS_DENIED";
		break;
	case STATUS_BUFFER_TOO_SMALL:
		bRet = L"STATUS_BUFFER_TOO_SMALL";
		break;
	case STATUS_PORT_DISCONNECTED:
		bRet = L"STATUS_PORT_DISCONNECTED";
		break;
	case STATUS_DATA_OVERRUN:
		bRet = L"STATUS_DATA_OVERRUN";
		break;
	case STATUS_DATA_ERROR:
		bRet = L"STATUS_DATA_ERROR";
		break;
	case STATUS_CRC_ERROR:
		bRet = L"STATUS_CRC_ERROR";
		break;
	case STATUS_SHARING_VIOLATION:
		bRet = L"STATUS_SHARING_VIOLATION";
		break;
	case STATUS_DELETE_PENDING:
		bRet = L"STATUS_DELETE_PENDING";
		break;
	case STATUS_CANCELLED:
		bRet = L"STATUS_CANCELLED";
		break;
	default:
		bRet = L"STATUS_UNKNOWN";
		break;
	}

	return bRet;
}


TCHAR *GetMajorFunction(U8 u8Function)
{
	switch (u8Function)
	{
	case IRP_MJ_CREATE:
		return _T("IRP_MJ_CREATE");
	case IRP_MJ_CREATE_NAMED_PIPE:
		return _T("IRP_MJ_CREATE_NAMED_PIPE");
	case IRP_MJ_CLOSE:
		return _T("IRP_MJ_CLOSE");
	case IRP_MJ_READ:
		return _T("IRP_MJ_READ");
	case IRP_MJ_WRITE:
		return _T("IRP_MJ_WRITE");
	case IRP_MJ_QUERY_INFORMATION:
		return _T("IRP_MJ_QUERY_INFORMATION");
	case IRP_MJ_SET_INFORMATION:
		return _T("IRP_MJ_SET_INFORMATION");
	case IRP_MJ_QUERY_EA:
		return _T("IRP_MJ_QUERY_EA");
	case IRP_MJ_SET_EA:
		return _T("IRP_MJ_SET_EA");
	case IRP_MJ_FLUSH_BUFFERS:
		return _T("IRP_MJ_FLUSH_BUFFERS");
	case IRP_MJ_QUERY_VOLUME_INFORMATION:
		return _T("IRP_MJ_QUERY_VOLUME_INFORMATION");
	case IRP_MJ_SET_VOLUME_INFORMATION:
		return _T("IRP_MJ_SET_VOLUME_INFORMATION");
	case IRP_MJ_DIRECTORY_CONTROL:
		return _T("IRP_MJ_DIRECTORY_CONTROL");
	case IRP_MJ_FILE_SYSTEM_CONTROL:
		return _T("IRP_MJ_FILE_SYSTEM_CONTROL");
	case IRP_MJ_DEVICE_CONTROL:
		return _T("IRP_MJ_DEVICE_CONTROL");
	case IRP_MJ_INTERNAL_DEVICE_CONTROL:
		return _T("IRP_MJ_INTERNAL_DEVICE_CONTROL");
	case IRP_MJ_SHUTDOWN:
		return _T("IRP_MJ_SHUTDOWN");
	case IRP_MJ_LOCK_CONTROL:
		return _T("IRP_MJ_LOCK_CONTROL");
	case IRP_MJ_CLEANUP:
		return _T("IRP_MJ_CLEANUP");
	case IRP_MJ_CREATE_MAILSLOT:
		return _T("IRP_MJ_CREATE_MAILSLOT");
	case IRP_MJ_QUERY_SECURITY:
		return _T("IRP_MJ_QUERY_SECURITY");
	case IRP_MJ_SET_SECURITY:
		return _T("IRP_MJ_SET_SECURITY");
	case IRP_MJ_POWER:
		return _T("IRP_MJ_POWER");
	case IRP_MJ_SYSTEM_CONTROL:
		return _T("IRP_MJ_SYSTEM_CONTROL");
	case IRP_MJ_DEVICE_CHANGE:
		return _T("IRP_MJ_DEVICE_CHANGE");
	case IRP_MJ_QUERY_QUOTA:
		return _T("IRP_MJ_QUERY_QUOTA");
	case IRP_MJ_SET_QUOTA:
		return _T("IRP_MJ_SET_QUOTA");
	case IRP_MJ_PNP:
		return _T("IRP_MJ_PNP");
	default:
		return _T("UNKNOWN");
	}
	return NULL;
}

TCHAR *GetMinorFunction(U8 u8MajorFunction, U8 u8Function)
{
	switch (u8MajorFunction)
	{
	case IRP_MJ_INTERNAL_DEVICE_CONTROL:
		return _T("");
	case IRP_MJ_POWER:
		switch (u8Function)
		{
		case IRP_MN_WAIT_WAKE:
			return _T("IRP_MN_WAIT_WAKE");
		case IRP_MN_POWER_SEQUENCE:
			return _T("IRP_MN_POWER_SEQUENCE");
		case IRP_MN_SET_POWER:
			return _T("IRP_MN_SET_POWER");
		case IRP_MN_QUERY_POWER:
			return _T("IRP_MN_QUERY_POWER");
		default:
			return _T("UNKNOWN");
		}
	case IRP_MJ_SYSTEM_CONTROL:
		switch (u8Function)
		{
		case IRP_MN_QUERY_ALL_DATA:
			return _T("IRP_MN_QUERY_ALL_DATA");
		case IRP_MN_QUERY_SINGLE_INSTANCE:
			return _T("IRP_MN_QUERY_SINGLE_INSTANCE");
		case IRP_MN_CHANGE_SINGLE_INSTANCE:
			return _T("IRP_MN_CHANGE_SINGLE_INSTANCE");
		case IRP_MN_CHANGE_SINGLE_ITEM:
			return _T("IRP_MN_CHANGE_SINGLE_ITEM");
		case IRP_MN_ENABLE_EVENTS:
			return _T("IRP_MN_ENABLE_EVENTS");
		case IRP_MN_DISABLE_EVENTS:
			return _T("IRP_MN_DISABLE_EVENTS");
		case IRP_MN_ENABLE_COLLECTION:
			return _T("IRP_MN_ENABLE_COLLECTION");
		case IRP_MN_DISABLE_COLLECTION:
			return _T("IRP_MN_DISABLE_COLLECTION");
		case IRP_MN_REGINFO:
			return _T("IRP_MN_REGINFO");
		case IRP_MN_EXECUTE_METHOD:
			return _T("IRP_MN_EXECUTE_METHOD");
		case IRP_MN_REGINFO_EX:
			return _T("IRP_MN_REGINFO_EX");
		default:
			return _T("");
		}
	case IRP_MJ_PNP:
		switch (u8Function)
		{
		case IRP_MN_START_DEVICE:
			return _T("IRP_MN_START_DEVICE");
		case IRP_MN_QUERY_REMOVE_DEVICE:
			return _T("IRP_MN_QUERY_REMOVE_DEVICE");
		case IRP_MN_REMOVE_DEVICE:
			return _T("IRP_MN_REMOVE_DEVICE");
		case IRP_MN_CANCEL_REMOVE_DEVICE:
			return _T("IRP_MN_CANCEL_REMOVE_DEVICE");
		case IRP_MN_STOP_DEVICE:
			return _T("IRP_MN_STOP_DEVICE");
		case IRP_MN_QUERY_STOP_DEVICE:
			return _T("IRP_MN_QUERY_STOP_DEVICE");
		case IRP_MN_CANCEL_STOP_DEVICE:
			return _T("IRP_MN_CANCEL_STOP_DEVICE");
		case IRP_MN_QUERY_DEVICE_RELATIONS:
			return _T("IRP_MN_QUERY_DEVICE_RELATIONS");
		case IRP_MN_QUERY_INTERFACE:
			return _T("IRP_MN_QUERY_INTERFACE");
		case IRP_MN_QUERY_CAPABILITIES:
			return _T("IRP_MN_QUERY_CAPABILITIES");
		case IRP_MN_QUERY_RESOURCES:
			return _T("IRP_MN_QUERY_RESOURCES");
		case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
			return _T("IRP_MN_QUERY_RESOURCE_REQUIREMENTS");
		case IRP_MN_QUERY_DEVICE_TEXT:
			return _T("IRP_MN_QUERY_DEVICE_TEXT");
		case IRP_MN_FILTER_RESOURCE_REQUIREMENTS:
			return _T("IRP_MN_FILTER_RESOURCE_REQUIREMENTS");
		case IRP_MN_READ_CONFIG:
			return _T("IRP_MN_READ_CONFIG");
		case IRP_MN_WRITE_CONFIG:
			return _T("IRP_MN_WRITE_CONFIG");
		case IRP_MN_EJECT:
			return _T("IRP_MN_EJECT");
		case IRP_MN_SET_LOCK:
			return _T("IRP_MN_SET_LOCK");
		case IRP_MN_QUERY_ID:
			return _T("IRP_MN_QUERY_ID");
		case IRP_MN_QUERY_PNP_DEVICE_STATE:
			return _T("IRP_MN_QUERY_PNP_DEVICE_STATE");
		case IRP_MN_QUERY_BUS_INFORMATION:
			return _T("IRP_MN_QUERY_BUS_INFORMATION");
		case IRP_MN_DEVICE_USAGE_NOTIFICATION:
			return _T("IRP_MN_DEVICE_USAGE_NOTIFICATION");
		case IRP_MN_SURPRISE_REMOVAL:
			return _T("IRP_MN_SURPRISE_REMOVAL");
		default:
			return _T("");
		}
	default:
		return _T("");
	}
	return NULL;
}

TCHAR *GetTitleNameFromUrb(U16 u16Function)
{
	switch (u16Function)
	{
	case URB_FUNCTION_SELECT_INTERFACE:
		return _T("URB_FUNCTION_SELECT_INTERFACE");
	case URB_FUNCTION_SELECT_CONFIGURATION:
		return _T("URB_FUNCTION_SELECT_CONFIGURATION");
	case URB_FUNCTION_ABORT_PIPE:
		return _T("URB_FUNCTION_ABORT_PIPE");
	case URB_FUNCTION_RESET_PIPE:
		return _T("URB_FUNCTION_RESET_PIPE");
	case URB_FUNCTION_GET_CURRENT_FRAME_NUMBER:
		return _T("URB_FUNCTION_GET_CURRENT_FRAME_NUMBER");
	case URB_FUNCTION_CONTROL_TRANSFER:
		return _T("URB_FUNCTION_CONTROL_TRANSFER");
	case URB_FUNCTION_CONTROL_TRANSFER_EX:
		return _T("URB_FUNCTION_CONTROL_TRANSFER_EX");
	case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
		return _T("URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER");
	case URB_FUNCTION_ISOCH_TRANSFER:
		return _T("URB_FUNCTION_ISOCH_TRANSFER");
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
		return _T("URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE");
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE:
		return _T("URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE");
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT:
		return _T("URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT");
	case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE:
		return _T("URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE");
	case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE:
		return _T("URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE");
	case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT:
		return _T("URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT");
	case URB_FUNCTION_GET_STATUS_FROM_DEVICE:
		return _T("URB_FUNCTION_GET_STATUS_FROM_DEVICE");
	case URB_FUNCTION_GET_STATUS_FROM_INTERFACE:
		return _T("URB_FUNCTION_GET_STATUS_FROM_INTERFACE");
	case URB_FUNCTION_GET_STATUS_FROM_ENDPOINT:
		return _T("URB_FUNCTION_GET_STATUS_FROM_ENDPOINT");
	case URB_FUNCTION_SET_FEATURE_TO_DEVICE:
		return _T("URB_FUNCTION_SET_FEATURE_TO_DEVICE");
	case URB_FUNCTION_SET_FEATURE_TO_INTERFACE:
		return _T("URB_FUNCTION_SET_FEATURE_TO_INTERFACE");
	case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT:
		return _T("URB_FUNCTION_SET_FEATURE_TO_ENDPOINT");
	case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE:
		return _T("URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE");
	case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE:
		return _T("URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE");
	case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT:
		return _T("URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT");
	case URB_FUNCTION_VENDOR_DEVICE:
		return _T("URB_FUNCTION_VENDOR_DEVICE");
	case URB_FUNCTION_VENDOR_INTERFACE:
		return _T("URB_FUNCTION_VENDOR_INTERFACE");
	case URB_FUNCTION_VENDOR_ENDPOINT:
		return _T("URB_FUNCTION_VENDOR_ENDPOINT");
	case URB_FUNCTION_VENDOR_OTHER:
		return _T("URB_FUNCTION_VENDOR_OTHER");
	case URB_FUNCTION_CLASS_DEVICE:
		return _T("URB_FUNCTION_CLASS_DEVICE");
	case URB_FUNCTION_CLASS_INTERFACE:
		return _T("URB_FUNCTION_CLASS_INTERFACE");
	case URB_FUNCTION_CLASS_ENDPOINT:
		return _T("URB_FUNCTION_CLASS_ENDPOINT");
	case URB_FUNCTION_CLASS_OTHER:
		return _T("URB_FUNCTION_CLASS_OTHER");
	case URB_FUNCTION_GET_INTERFACE:
		return _T("URB_FUNCTION_GET_INTERFACE");
	case URB_FUNCTION_GET_CONFIGURATION:
		return _T("URB_FUNCTION_GET_CONFIGURATION");
	case URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR:
		return _T("URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR");
	case URB_FUNCTION_OPEN_STATIC_STREAMS:
		return _T("URB_FUNCTION_OPEN_STATIC_STREAMS");
	default:
		return _T("UNKNOWN URB");
	}
	return NULL;
}


void translateUrbFlagString(U32 u32TransferFlags, TCHAR * pOutputString)
{
	TCHAR tempString[1024] = { 0, };
	SIZE_T len = 0;

	pOutputString[0] = 0;

	if (u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
	{
		wsprintf(tempString, L"%wsUSBD_TRANSFER_DIRECTION_IN ", tempString);
	}
	else
	{
		wsprintf(tempString, L"%USBD_TRANSFER_DIRECTION_OUT ", tempString);
	}

	if (u32TransferFlags & USBD_SHORT_TRANSFER_OK)
	{
		wsprintf(tempString, L"%USBD_SHORT_TRANSFER_OK ", tempString);
	}
	if (u32TransferFlags & USBD_DEFAULT_PIPE_TRANSFER)
	{
		wsprintf(tempString, L"%USBD_DEFAULT_PIPE_TRANSFER ", tempString);
	}
	if (u32TransferFlags & USBD_START_ISO_TRANSFER_ASAP)
	{
		wsprintf(tempString, L"%USBD_START_ISO_TRANSFER_ASAP ", tempString);
	}

	len = _tcslen(tempString);
	if (len)
	{
		tempString[len - 1] = 0;
	}
	else
	{
		wsprintf(tempString, _T("URB Status = 0x%8X"), u32TransferFlags);
	}

	_tcscpy(pOutputString, tempString);
}

void translateConfigDescBmAttr(U8 u8BmAttr, TCHAR * pOutputString)
{
	TCHAR tempString[1024] = { 0, };
	SIZE_T len = 0;

	pOutputString[0] = 0;

	if (u8BmAttr & 0x40) // bit 6
	{
		wsprintf(tempString, L"%wsSELF-POWERED ", tempString);
	}
	else
	{
		wsprintf(tempString, L"%wsBUS-POWERED ", tempString);
	}

	if (u8BmAttr & 0x20) // bit 5
	{
		wsprintf(tempString, L"%wsREMOTE_WAKEUP ", tempString);
	}

	len = _tcslen(tempString);
	if (len)
	{
		tempString[len - 1] = 0;
	}
	else
	{
	}

	_tcscpy(pOutputString, tempString);
}

void translateEndpointDescBmAttr(U8 u8BmAttr, TCHAR * pOutputString)
{
	TCHAR tempString[1024] = { 0, };
	SIZE_T len = 0;
	BOOLEAN bIsIso = FALSE;

	pOutputString[0] = 0;

	switch (u8BmAttr & 0x03)
	{
	case 0:
		wsprintf(tempString, L"%wsCONTROL ", tempString);
		break;
	case 1:
		wsprintf(tempString, L"%wsISOCH ", tempString);
		bIsIso = TRUE;
		break;
	case 2:
		wsprintf(tempString, L"%wsBULK ", tempString);
		break;
	case 3:
		wsprintf(tempString, L"%wsINTERRUPT ", tempString);
		break;
	}

	if (bIsIso == TRUE)
	{
		switch ((u8BmAttr >> 2) & 0x03)
		{
		case 0:
			wsprintf(tempString, L"%wsNO SYNC ", tempString);
			break;
		case 1:
			wsprintf(tempString, L"%wsASYNC ", tempString);
			break;
		case 2:
			wsprintf(tempString, L"%wsADAPTIVE ", tempString);
			break;
		case 3:
			wsprintf(tempString, L"%wsSYNC ", tempString);
			break;
		}

		switch ((u8BmAttr >> 4) & 0x03)
		{
		case 0:
			wsprintf(tempString, L"%wsDATA ", tempString);
			break;
		case 1:
			wsprintf(tempString, L"%wsFEEDBACK ", tempString);
			break;
		case 2:
			wsprintf(tempString, L"%wsIMPLICIT FEEDBACK", tempString);
			break;
		}
	}

	len = _tcslen(tempString);
	if (len)
	{
		tempString[len - 1] = 0;
	}
	else
	{
		//		wsprintf(tempString, _T("URB Status = 0x%8X"), u32TransferFlags);
	}

	_tcscpy(pOutputString, tempString);
}

SIZE_T CountOfDescriptor(USB_COMMON_DESCRIPTOR * pUSBCommonDesc, SIZE_T Size)
{
	USB_COMMON_DESCRIPTOR * pLastDesc = NULL;
	USB_COMMON_DESCRIPTOR * pDesc = NULL;
	SIZE_T nDesc = 0;
	SIZE_T nDescSize = 0;

	pDesc = pUSBCommonDesc;
	pLastDesc = (USB_COMMON_DESCRIPTOR *)(((U8 *)pUSBCommonDesc) + Size);

	while (pDesc < pLastDesc)
	{
		nDescSize = pDesc->bLength;
		pDesc = (USB_COMMON_DESCRIPTOR *)(((U8 *)pDesc) + nDescSize);
		nDesc++;
	}
	return nDesc;
}

USB_COMMON_DESCRIPTOR * NextDescriptor(USB_COMMON_DESCRIPTOR *pUSBCommonDesc)
{
	SIZE_T nDescSize = 0;
	USB_COMMON_DESCRIPTOR * pDesc = NULL;

	pDesc = pUSBCommonDesc;
	nDescSize = pDesc->bLength;
	pDesc = (USB_COMMON_DESCRIPTOR *)(((U8 *)pDesc) + nDescSize);
	return pDesc;
}

USB_ENDPOINT_DESCRIPTOR * GetEndpointDescPtr(USB_PIPE_TO_ENDPOINT * pUsbPipeToEndpoint, U64 PipeHandle)
{
	USB_ENDPOINT_DESCRIPTOR * pEndDesc = NULL;
	SIZE_T count;

	for (count = 0; count < pUsbPipeToEndpoint->NumberOfPipes; count++)
	{
		if (PipeHandle == pUsbPipeToEndpoint->PipeToEndpoint[count].PipeHandle)
		{
			pEndDesc = &pUsbPipeToEndpoint->PipeToEndpoint[count].EndpointDesc;
			break;
		}
	}

	return pEndDesc;
}

IDINFO g_ClassInfo[] = {
	{ 1, L"Audio" },
	{ 2, L"Communications" },
	{ 3, L"Human Interface Device" },
	{ 5, L"Physical Interface Device" },
	{ 6, L"Imaging" },
	{ 7, L"Printer" },
	{ 8, L"Mass Storage" },
	{ 9, L"Hub" },
	{ 10, L"CDC Data" },
	{ 11, L"Chip/SmartCard" },
	{ 13, L"Content Security" },
	{ 14, L"Video" },
	{ 0x58, L"Xbox" },
	{ 0xdc, L"Diagnostic" },
	{ 0xe0, L"Wireless" },
	{ 0xef, L"Miscellaneous Device" },
	{ 0xfe, L"Application Specific Interface" },
	{ 0xff, L"Vendor Specific Class" },
	{ 0, 0 }
};

IDINFO g_IdInfo[] = {
	{ 1,L"Fry's Electronics" },
	{ 2,L"Ingram" },
	{ 3,L"Club Mac" },
	{ 4,L"Nebraska Furniture Mart" },
	{ 17,L"Unknown" },
	{ 83,L"Planex" },
	{ 121,L"DragonRise Inc." },
	{ 261,L"Trust International B.V." },
	{ 295,L"IBP" },
	{ 325,L"Unknown" },
	{ 380,L"MLK" },
	{ 512,L"TP-Link" },
	{ 516,L"Chipsbank Microelectronics Co., Ltd" },
	{ 536,L"Hangzhou Worlde" },
	{ 685,L"HUMAX Co., Ltd." },
	{ 768,L"MM300 eBook Reader" },
	{ 804,L"OCZ Technology Inc" },
	{ 805,L"OCZ Technology Inc" },
	{ 902,L"LTS" },
	{ 985,L"Shenzhen Sinote Tech-Electron Co., Ltd" },
	{ 986,L"Bernd Walter Computer Technology" },
	{ 1000,L"EndPoints, Inc." },
	{ 1001,L"Thesys Microelectronics" },
	{ 1002,L"Data Broadcasting Corp." },
	{ 1003,L"Atmel Corp." },
	{ 1004,L"Iwatsu America, Inc." },
	{ 1005,L"Mitel Corp." },
	{ 1006,L"Mitsumi" },
	{ 1008,L"Hewlett-Packard" },
	{ 1009,L"Genoa Technology" },
	{ 1010,L"Oak Technology, Inc." },
	{ 1011,L"Adaptec, Inc." },
	{ 1012,L"Diebold, Inc." },
	{ 1013,L"Siemens Electromechanical" },
	{ 1016,L"Epson Imaging Technology Center" },
	{ 1017,L"KeyTronic Corp." },
	{ 1019,L"OPTi, Inc." },
	{ 1020,L"Elitegroup Computer Systems" },
	{ 1021,L"Xilinx, Inc." },
	{ 1022,L"Farallon Comunications" },
	{ 1024,L"National Semiconductor Corp." },
	{ 1025,L"National Registry, Inc." },
	{ 1026,L"ALi Corp." },
	{ 1027,L"Future Technology Devices International, Ltd" },
	{ 1028,L"NCR Corp." },
	{ 1029,L"Synopsys, Inc." },
	{ 1030,L"Fujitsu-ICL Computers" },
	{ 1031,L"Fujitsu Personal Systems, Inc." },
	{ 1032,L"Quanta Computer, Inc." },
	{ 1033,L"NEC Corp." },
	{ 1034,L"Kodak Co." },
	{ 1035,L"Weltrend Semiconductor" },
	{ 1036,L"VTech Computers, Ltd" },
	{ 1037,L"VIA Technologies, Inc." },
	{ 1038,L"MCCI" },
	{ 1039,L"Echo Speech Corp." },
	{ 1041,L"BUFFALO INC. (formerly MelCo., Inc.)" },
	{ 1042,L"Award Software International" },
	{ 1043,L"Leadtek Research, Inc." },
	{ 1044,L"Giga-Byte Technology Co., Ltd" },
	{ 1046,L"Winbond Electronics Corp." },
	{ 1047,L"Symbios Logic" },
	{ 1048,L"AST Research" },
	{ 1049,L"Samsung Info. Systems America, Inc." },
	{ 1050,L"Phoenix Technologies, Ltd" },
	{ 1051,L"d'TV" },
	{ 1053,L"S3, Inc." },
	{ 1054,L"Creative Technology, Ltd" },
	{ 1055,L"LCS Telegraphics" },
	{ 1056,L"Chips and Technologies" },
	{ 1057,L"Nokia Mobile Phones" },
	{ 1058,L"ADI Systems, Inc." },
	{ 1059,L"Computer Access Technology Corp." },
	{ 1060,L"Standard Microsystems Corp." },
	{ 1061,L"Motorola Semiconductors HK, Ltd" },
	{ 1062,L"Integrated Device Technology, Inc." },
	{ 1063,L"Motorola Electronics Taiwan, Ltd" },
	{ 1064,L"Advanced Gravis Computer Tech, Ltd" },
	{ 1065,L"Cirrus Logic" },
	{ 1066,L"Ericsson Austrian, AG" },
	{ 1067,L"Intel Corp." },
	{ 1068,L"Innovative Semiconductors, Inc." },
	{ 1069,L"Micronics" },
	{ 1070,L"Acer, Inc." },
	{ 1071,L"Molex, Inc." },
	{ 1072,L"Sun Microsystems, Inc." },
	{ 1073,L"Itac Systems, Inc." },
	{ 1074,L"Unisys Corp." },
	{ 1075,L"Alps Electric, Inc." },
	{ 1076,L"Samsung Info. Systems America, Inc." },
	{ 1077,L"Hyundai Electronics America" },
	{ 1078,L"Taugagreining HF" },
	{ 1079,L"Framatome Connectors USA" },
	{ 1080,L"Advanced Micro Devices, Inc." },
	{ 1081,L"Voice Technologies Group" },
	{ 1085,L"Lexmark International, Inc." },
	{ 1086,L"LG Electronics USA, Inc." },
	{ 1087,L"RadiSys Corp." },
	{ 1088,L"Eizo Nanao Corp." },
	{ 1089,L"Winbond Systems Lab." },
	{ 1090,L"Ericsson, Inc." },
	{ 1091,L"Gateway, Inc." },
	{ 1093,L"Lucent Technologies, Inc." },
	{ 1094,L"NMB Technologies Corp." },
	{ 1095,L"Momentum Microsystems" },
	{ 1097,L"Duta Multi Robotik" },
	{ 1098,L"Shamrock Tech. Co., Ltd" },
	{ 1099,L"WSI" },
	{ 1100,L"CCL/ITRI" },
	{ 1101,L"Siemens Nixdorf AG" },
	{ 1102,L"Alps Electric Co., Ltd" },
	{ 1103,L"ThrustMaster, Inc." },
	{ 1104,L"DFI, Inc." },
	{ 1105,L"Texas Instruments, Inc." },
	{ 1106,L"Mitsubishi Electronics America, Inc." },
	{ 1107,L"CMD Technology" },
	{ 1108,L"Vobis Microcomputer AG" },
	{ 1109,L"Telematics International, Inc." },
	{ 1110,L"Analog Devices, Inc." },
	{ 1111,L"Silicon Integrated Systems Corp." },
	{ 1112,L"KYE Systems Corp. (Mouse Systems)" },
	{ 1113,L"Adobe Systems, Inc." },
	{ 1114,L"SONICblue, Inc." },
	{ 1115,L"Hitachi, Ltd" },
	{ 1117,L"Nortel Networks, Ltd" },
	{ 1118,L"Microsoft Corp." },
	{ 1120,L"Ace Cad Enterprise Co., Ltd" },
	{ 1121,L"Primax Electronics, Ltd" },
	{ 1123,L"MGE UPS Systems" },
	{ 1124,L"AMP/Tycoelectronics Corp." },
	{ 1127,L"AT&T Paradyne" },
	{ 1128,L"Wieson Technologies Co., Ltd" },
	{ 1130,L"Cherry GmbH" },
	{ 1131,L"American Megatrends, Inc." },
	{ 1132,L"Toshiba Corp., Digital Media Equipment" },
	{ 1133,L"Logitech, Inc." },
	{ 1134,L"Behavior Tech. Computer Corp." },
	{ 1135,L"Crystal Semiconductor" },
	{ 1137,L"Philips (or NXP)" },
	{ 1138,L"Chicony Electronics Co., Ltd" },
	{ 1139,L"Sanyo Information Business Co., Ltd" },
	{ 1140,L"Sanyo Electric Co., Ltd" },
	{ 1141,L"Relisys/Teco Information System" },
	{ 1142,L"AESP" },
	{ 1143,L"Seagate Technology, Inc." },
	{ 1144,L"Connectix Corp." },
	{ 1145,L"Advanced Peripheral Laboratories" },
	{ 1146,L"Semtech Corp." },
	{ 1147,L"Silitek Corp." },
	{ 1148,L"Dell Computer Corp." },
	{ 1149,L"Kensington" },
	{ 1150,L"Agere Systems, Inc. (Lucent)" },
	{ 1151,L"Plantronics, Inc." },
	{ 1152,L"Toshiba America Inc" },
	{ 1153,L"Zenith Data Systems" },
	{ 1154,L"Kyocera Corp." },
	{ 1155,L"STMicroelectronics" },
	{ 1156,L"Specialix" },
	{ 1157,L"Nokia Monitors" },
	{ 1158,L"ASUS Computers, Inc." },
	{ 1159,L"Stewart Connector" },
	{ 1160,L"Cirque Corp." },
	{ 1161,L"Foxconn / Hon Hai" },
	{ 1162,L"S-MOS Systems, Inc." },
	{ 1164,L"Alps Electric Ireland, Ltd" },
	{ 1165,L"Integrated Technology Express, Inc." },
	{ 1167,L"Eicon Tech." },
	{ 1168,L"United Microelectronics Corp." },
	{ 1169,L"Capetronic" },
	{ 1170,L"Samsung SemiConductor, Inc." },
	{ 1171,L"MAG Technology Co., Ltd" },
	{ 1173,L"ESS Technology, Inc." },
	{ 1174,L"Micron Electronics" },
	{ 1175,L"Smile International" },
	{ 1176,L"Capetronic (Kaohsiung) Corp." },
	{ 1177,L"Yamaha Corp." },
	{ 1178,L"Gandalf Technologies, Ltd" },
	{ 1179,L"Curtis Computer Products" },
	{ 1180,L"Acer Advanced Labs, Inc." },
	{ 1181,L"VLSI Technology" },
	{ 1183,L"Compaq Computer Corp." },
	{ 1184,L"Digital Equipment Corp." },
	{ 1185,L"SystemSoft Corp." },
	{ 1186,L"FirePower Systems" },
	{ 1187,L"Trident Microsystems, Inc." },
	{ 1188,L"Hitachi, Ltd" },
	{ 1189,L"Acer Peripherals Inc. (now BenQ Corp.)" },
	{ 1190,L"Nokia Display Products" },
	{ 1191,L"Visioneer" },
	{ 1192,L"Multivideo Labs, Inc." },
	{ 1193,L"Canon, Inc." },
	{ 1194,L"DaeWoo Telecom, Ltd" },
	{ 1195,L"Chromatic Research" },
	{ 1196,L"Micro Audiometrics Corp." },
	{ 1197,L"Dooin Electronics" },
	{ 1199,L"Winnov L.P." },
	{ 1200,L"Nikon Corp." },
	{ 1201,L"Pan International" },
	{ 1203,L"IBM Corp." },
	{ 1204,L"Cypress Semiconductor Corp." },
	{ 1205,L"ROHM LSI Systems USA, LLC" },
	{ 1206,L"Hint Corp." },
	{ 1207,L"Compal Electronics, Inc." },
	{ 1208,L"Seiko Epson Corp." },
	{ 1209,L"Rainbow Technologies, Inc." },
	{ 1210,L"Toucan Systems, Ltd" },
	{ 1211,L"I-O Data Device, Inc." },
	{ 1213,L"Toshiba Electronics Taiwan Corp." },
	{ 1214,L"Telia Research AB" },
	{ 1215,L"TDK Corp." },
	{ 1217,L"U.S. Robotics (3Com)" },
	{ 1218,L"Methode Electronics Far East PTE, Ltd" },
	{ 1219,L"Maxi Switch, Inc." },
	{ 1220,L"Lockheed Martin Energy Research" },
	{ 1221,L"Fujitsu, Ltd" },
	{ 1222,L"Toshiba America Electronic Components" },
	{ 1223,L"Micro Macro Technologies" },
	{ 1224,L"Konica Corp." },
	{ 1226,L"Lite-On Technology Corp." },
	{ 1227,L"Fuji Photo Film Co., Ltd" },
	{ 1228,L"ST-Ericsson" },
	{ 1229,L"Tatung Co. Of America" },
	{ 1230,L"ScanLogic Corp." },
	{ 1231,L"Myson Century, Inc." },
	{ 1232,L"Digi International" },
	{ 1233,L"ITT Canon" },
	{ 1234,L"Altec Lansing Technologies" },
	{ 1235,L"VidUS, Inc." },
	{ 1236,L"LSI Logic, Inc." },
	{ 1237,L"Forte Technologies, Inc." },
	{ 1238,L"Mentor Graphics" },
	{ 1239,L"Oki Semiconductor" },
	{ 1240,L"Microchip Technology, Inc." },
	{ 1241,L"Holtek Semiconductor, Inc." },
	{ 1242,L"Panasonic (Matsushita)" },
	{ 1243,L"Hypertec Pty, Ltd" },
	{ 1244,L"Huan Hsin Holdings, Ltd" },
	{ 1245,L"Sharp Corp." },
	{ 1246,L"MindShare, Inc." },
	{ 1247,L"Interlink Electronics" },
	{ 1249,L"Iiyama North America, Inc." },
	{ 1250,L"Exar Corp." },
	{ 1251,L"Zilog, Inc." },
	{ 1252,L"ACC Microelectronics" },
	{ 1253,L"Promise Technology" },
	{ 1254,L"SCM Microsystems, Inc." },
	{ 1255,L"Elo TouchSystems" },
	{ 1256,L"Samsung Electronics Co., Ltd" },
	{ 1257,L"PC-Tel, Inc." },
	{ 1258,L"Brooktree Corp." },
	{ 1259,L"Northstar Systems, Inc." },
	{ 1260,L"Tokyo Electron Device, Ltd" },
	{ 1261,L"Annabooks" },
	{ 1263,L"Pacific Electronic International, Inc." },
	{ 1264,L"Daewoo Electronics Co., Ltd" },
	{ 1265,L"Victor Company of Japan, Ltd" },
	{ 1266,L"Chicony Electronics Co., Ltd" },
	{ 1267,L"Elan Microelectronics Corp." },
	{ 1268,L"Harting Elektronik, Inc." },
	{ 1269,L"Fujitsu-ICL Systems, Inc." },
	{ 1270,L"Norand Corp." },
	{ 1271,L"Newnex Technology Corp." },
	{ 1272,L"FuturePlus Systems" },
	{ 1273,L"Brother Industries, Ltd" },
	{ 1274,L"Dallas Semiconductor" },
	{ 1275,L"Biostar Microtech International Corp." },
	{ 1276,L"Sunplus Technology Co., Ltd" },
	{ 1277,L"Soliton Systems, K.K." },
	{ 1278,L"PFU, Ltd" },
	{ 1279,L"E-CMOS Corp." },
	{ 1280,L"Siam United Hi-Tech" },
	{ 1281,L"Fujikura DDK, Ltd" },
	{ 1282,L"Acer, Inc." },
	{ 1283,L"Hitachi America, Ltd" },
	{ 1284,L"Hayes Microcomputer Products" },
	{ 1286,L"3Com Corp." },
	{ 1287,L"Hosiden Corp." },
	{ 1288,L"Clarion Co., Ltd" },
	{ 1289,L"Aztech Systems, Ltd" },
	{ 1290,L"Cinch Connectors" },
	{ 1291,L"Cable System International" },
	{ 1292,L"InnoMedia, Inc." },
	{ 1293,L"Belkin Components" },
	{ 1294,L"Neon Technology, Inc." },
	{ 1295,L"KC Technology, Inc." },
	{ 1296,L"Sejin Electron, Inc." },
	{ 1297,L"N'Able (DataBook) Technologies, Inc." },
	{ 1298,L"Hualon Microelectronics Corp." },
	{ 1299,L"digital-X, Inc." },
	{ 1300,L"FCI Electronics" },
	{ 1301,L"ACTC" },
	{ 1302,L"Longwell Electronics" },
	{ 1303,L"Butterfly Communications" },
	{ 1304,L"EzKEY Corp." },
	{ 1305,L"Star Micronics Co., Ltd" },
	{ 1306,L"WYSE Technology" },
	{ 1307,L"Silicon Graphics" },
	{ 1308,L"Shuttle, Inc." },
	{ 1309,L"American Power Conversion" },
	{ 1310,L"Scientific Atlanta, Inc." },
	{ 1311,L"IO Systems (Elite Electronics), Inc." },
	{ 1312,L"Taiwan Semiconductor Manufacturing Co." },
	{ 1313,L"Airborn Connectors" },
	{ 1314,L"Advanced Connectek, Inc." },
	{ 1315,L"ATEN GmbH" },
	{ 1316,L"Sola Electronics" },
	{ 1317,L"Netchip Technology, Inc." },
	{ 1318,L"Temic MHS S.A." },
	{ 1319,L"ALTRA" },
	{ 1320,L"ATI Technologies, Inc." },
	{ 1321,L"Aladdin Knowledge Systems" },
	{ 1322,L"Crescent Heart Software" },
	{ 1323,L"Tekom Technologies, Inc." },
	{ 1324,L"Canon Information Systems, Inc." },
	{ 1325,L"Avid Electronics Corp." },
	{ 1326,L"Standard Microsystems Corp." },
	{ 1327,L"Unicore Software, Inc." },
	{ 1328,L"American Microsystems, Inc." },
	{ 1329,L"Wacom Technology Corp." },
	{ 1330,L"Systech Corp." },
	{ 1331,L"Alcatel Mobile Phones" },
	{ 1332,L"Motorola, Inc." },
	{ 1333,L"LIH TZU Electric Co., Ltd" },
	{ 1334,L"Hand Held Products (Welch Allyn, Inc.)" },
	{ 1335,L"Inventec Corp." },
	{ 1336,L"Caldera International, Inc. (SCO)" },
	{ 1337,L"Shyh Shiun Terminals Co., Ltd" },
	{ 1338,L"PrehKeyTec GmbH" },
	{ 1339,L"Global Village Communication" },
	{ 1340,L"Institut of Microelectronic & Mechatronic Systems" },
	{ 1341,L"Silicon Architect" },
	{ 1342,L"Mobility Electronics" },
	{ 1343,L"Synopsys, Inc." },
	{ 1344,L"UniAccess AB" },
	{ 1345,L"Sirf Technology, Inc." },
	{ 1347,L"ViewSonic Corp." },
	{ 1348,L"Cristie Electronics, Ltd" },
	{ 1349,L"Xirlink, Inc." },
	{ 1350,L"Polaroid Corp." },
	{ 1351,L"Anchor Chips, Inc." },
	{ 1352,L"Tyan Computer Corp." },
	{ 1353,L"Pixera Corp." },
	{ 1354,L"Fujitsu Microelectronics, Inc." },
	{ 1355,L"New Media Corp." },
	{ 1356,L"Sony Corp." },
	{ 1357,L"Try Corp." },
	{ 1358,L"Proside Corp." },
	{ 1359,L"WYSE Technology Taiwan" },
	{ 1360,L"Fuji Xerox Co., Ltd" },
	{ 1361,L"CompuTrend Systems, Inc." },
	{ 1362,L"Philips Monitors" },
	{ 1363,L"STMicroelectronics Imaging Division (VLSI Vision)" },
	{ 1364,L"Dictaphone Corp." },
	{ 1365,L"ANAM S&T Co., Ltd" },
	{ 1366,L"Asahi Kasei Microsystems Co., Ltd" },
	{ 1367,L"ATEN International Co., Ltd" },
	{ 1368,L"Truevision, Inc." },
	{ 1369,L"Cadence Design Systems, Inc." },
	{ 1370,L"Kenwood USA" },
	{ 1371,L"KnowledgeTek, Inc." },
	{ 1372,L"Proton Electronic Ind." },
	{ 1373,L"Samsung Electro-Mechanics Co." },
	{ 1374,L"CTX Opto-Electronics Corp." },
	{ 1375,L"Mustek Systems, Inc." },
	{ 1376,L"Interface Corp." },
	{ 1377,L"Oasis Design, Inc." },
	{ 1378,L"Telex Communications, Inc." },
	{ 1379,L"Immersion Corp." },
	{ 1380,L"Kodak Digital Product Center, Japan Ltd. (formerly Chinon Industries Inc.)" },
	{ 1381,L"Peracom Networks, Inc." },
	{ 1382,L"Monterey International Corp." },
	{ 1383,L"Xyratex International, Ltd" },
	{ 1384,L"Quartz Ingenierie" },
	{ 1385,L"SegaSoft" },
	{ 1386,L"Wacom Co., Ltd" },
	{ 1387,L"Decicon, Inc." },
	{ 1388,L"eTEK Labs" },
	{ 1389,L"EIZO Corp." },
	{ 1390,L"Elecom Co., Ltd" },
	{ 1391,L"Korea Data Systems Co., Ltd" },
	{ 1392,L"Epson America" },
	{ 1393,L"Interex, Inc." },
	{ 1394,L"Conexant Systems (Rockwell), Inc." },
	{ 1395,L"Zoran Co. Personal Media Division (Nogatech)" },
	{ 1396,L"City University of Hong Kong" },
	{ 1397,L"Philips Creative Display Solutions" },
	{ 1398,L"BAFO/Quality Computer Accessories" },
	{ 1399,L"ELSA" },
	{ 1400,L"Intrinsix Corp." },
	{ 1401,L"GVC Corp." },
	{ 1402,L"Samsung Electronics America" },
	{ 1403,L"Y-E Data, Inc." },
	{ 1404,L"AVM GmbH" },
	{ 1405,L"Shark Multimedia, Inc." },
	{ 1406,L"Nintendo Co., Ltd" },
	{ 1407,L"QuickShot, Ltd" },
	{ 1408,L"Denron, Inc." },
	{ 1409,L"Racal Data Group" },
	{ 1410,L"Roland Corp." },
	{ 1411,L"Padix Co., Ltd (Rockfire)" },
	{ 1412,L"RATOC System, Inc." },
	{ 1413,L"FlashPoint Technology, Inc." },
	{ 1414,L"ZyXEL Communications Corp." },
	{ 1415,L"America Kotobuki Electronics Industries, Inc." },
	{ 1416,L"Sapien Design" },
	{ 1417,L"Victron" },
	{ 1418,L"Nohau Corp." },
	{ 1419,L"Infineon Technologies" },
	{ 1420,L"In Focus Systems" },
	{ 1421,L"Micrel Semiconductor" },
	{ 1422,L"Tripath Technology, Inc." },
	{ 1423,L"Alcor Micro Corp." },
	{ 1424,L"Omron Corp." },
	{ 1425,L"Questra Consulting" },
	{ 1426,L"Powerware Corp." },
	{ 1427,L"Incite" },
	{ 1428,L"Princeton Graphic Systems" },
	{ 1429,L"Zoran Microelectronics, Ltd" },
	{ 1430,L"MicroTouch Systems, Inc." },
	{ 1431,L"Trisignal Communications" },
	{ 1432,L"Niigata Canotec Co., Inc." },
	{ 1433,L"Brilliance Semiconductor, Inc." },
	{ 1434,L"Spectrum Signal Processing, Inc." },
	{ 1435,L"Iomega Corp." },
	{ 1436,L"A-Trend Technology Co., Ltd" },
	{ 1437,L"Advanced Input Devices" },
	{ 1438,L"Intelligent Instrumentation" },
	{ 1439,L"LaCie, Ltd" },
	{ 1440,L"Vetronix Corp." },
	{ 1441,L"USC Corp." },
	{ 1442,L"Fuji Film Microdevices Co., Ltd" },
	{ 1443,L"ARC International" },
	{ 1444,L"Ortek Technology, Inc." },
	{ 1445,L"Sampo Technology Corp." },
	{ 1446,L"Cisco Systems, Inc." },
	{ 1447,L"Bose Corp." },
	{ 1448,L"Spacetec IMC Corp." },
	{ 1449,L"OmniVision Technologies, Inc." },
	{ 1450,L"Utilux South China, Ltd" },
	{ 1451,L"In-System Design" },
	{ 1452,L"Apple, Inc." },
	{ 1453,L"Y.C. Cable U.S.A., Inc." },
	{ 1454,L"Synopsys, Inc." },
	{ 1455,L"Jing-Mold Enterprise Co., Ltd" },
	{ 1456,L"Fountain Technologies, Inc." },
	{ 1457,L"First International Computer, Inc." },
	{ 1460,L"LG Semicon Co., Ltd" },
	{ 1461,L"Dialogic Corp." },
	{ 1462,L"Proxima Corp." },
	{ 1463,L"Medianix Semiconductor, Inc." },
	{ 1464,L"Agiler, Inc." },
	{ 1465,L"Philips Research Laboratories" },
	{ 1466,L"DigitalPersona, Inc." },
	{ 1467,L"Grey Cell Systems" },
	{ 1468,L"3G Green Green Globe Co., Ltd" },
	{ 1469,L"RAFI GmbH & Co. KG" },
	{ 1470,L"Tyco Electronics (Raychem)" },
	{ 1471,L"S & S Research" },
	{ 1472,L"Keil Software" },
	{ 1473,L"Kawasaki Microelectronics, Inc." },
	{ 1474,L"Media Phonics (Suisse) S.A." },
	{ 1477,L"Digi International, Inc." },
	{ 1478,L"Qualcomm, Inc." },
	{ 1479,L"Qtronix Corp." },
	{ 1480,L"Cheng Uei Precision Industry Co., Ltd (Foxlink)" },
	{ 1481,L"Semtech Corp." },
	{ 1482,L"Ricoh Co., Ltd" },
	{ 1483,L"PowerVision Technologies, Inc." },
	{ 1484,L"ELSA AG" },
	{ 1485,L"Silicom, Ltd" },
	{ 1486,L"sci-worx GmbH" },
	{ 1487,L"Sung Forn Co., Ltd" },
	{ 1488,L"GE Medical Systems Lunar" },
	{ 1489,L"Brainboxes, Ltd" },
	{ 1490,L"Wave Systems Corp." },
	{ 1491,L"Tohoku Ricoh Co., Ltd" },
	{ 1493,L"Super Gate Technology Co., Ltd" },
	{ 1494,L"Philips Semiconductors, CICT" },
	{ 1495,L"Thomas & Betts Corp." },
	{ 1496,L"Ultima Electronics Corp." },
	{ 1497,L"Axiohm Transaction Solutions" },
	{ 1498,L"Microtek International, Inc." },
	{ 1499,L"Sun Corp. (Suntac?)" },
	{ 1500,L"Lexar Media, Inc." },
	{ 1501,L"Delta Electronics, Inc." },
	{ 1503,L"Silicon Vision, Inc." },
	{ 1504,L"Symbol Technologies" },
	{ 1505,L"Syntek Semiconductor Co., Ltd" },
	{ 1506,L"ElecVision, Inc." },
	{ 1507,L"Genesys Logic, Inc." },
	{ 1508,L"Red Wing Corp." },
	{ 1509,L"Fuji Electric Co., Ltd" },
	{ 1510,L"Keithley Instruments" },
	{ 1512,L"ICC, Inc." },
	{ 1513,L"Kawasaki LSI" },
	{ 1515,L"FFC, Ltd" },
	{ 1516,L"COM21, Inc." },
	{ 1518,L"Cytechinfo Inc." },
	{ 1519,L"AVB, Inc. [anko?]" },
	{ 1520,L"Canopus Co., Ltd" },
	{ 1521,L"Compass Communications" },
	{ 1522,L"Dexin Corp., Ltd" },
	{ 1523,L"PI Engineering, Inc." },
	{ 1525,L"Unixtar Technology, Inc." },
	{ 1526,L"AOC International" },
	{ 1527,L"RFC Distribution(s) PTE, Ltd" },
	{ 1529,L"PSC Scanning, Inc." },
	{ 1530,L"Siemens Telecommunications Systems, Ltd" },
	{ 1532,L"Harman" },
	{ 1533,L"InterAct, Inc." },
	{ 1534,L"Chic Technology Corp." },
	{ 1535,L"LeCroy Corp." },
	{ 1536,L"Barco Display Systems" },
	{ 1537,L"Jazz Hipster Corp." },
	{ 1538,L"Vista Imaging, Inc." },
	{ 1539,L"Novatek Microelectronics Corp." },
	{ 1540,L"Jean Co., Ltd" },
	{ 1541,L"Anchor C&C Co., Ltd" },
	{ 1542,L"Royal Information Electronics Co., Ltd" },
	{ 1543,L"Bridge Information Co., Ltd" },
	{ 1544,L"Genrad Ads" },
	{ 1545,L"SMK Manufacturing, Inc." },
	{ 1546,L"Worthington Data Solutions, Inc." },
	{ 1547,L"Solid Year" },
	{ 1548,L"EEH Datalink GmbH" },
	{ 1549,L"Auctor Corp." },
	{ 1550,L"Transmonde Technologies, Inc." },
	{ 1551,L"Joinsoon Electronics Mfg. Co., Ltd" },
	{ 1552,L"Costar Electronics, Inc." },
	{ 1553,L"Totoku Electric Co., Ltd" },
	{ 1555,L"TransAct Technologies, Inc." },
	{ 1556,L"Bio-Rad Laboratories" },
	{ 1557,L"Quabbin Wire & Cable Co., Inc." },
	{ 1558,L"Future Techno Designs PVT, Ltd" },
	{ 1559,L"Swiss Federal Insitute of Technology" },
	{ 1560,L"MacAlly" },
	{ 1561,L"Seiko Instruments, Inc." },
	{ 1562,L"Veridicom International, Inc." },
	{ 1563,L"Promptus Communications, Inc." },
	{ 1564,L"Act Labs, Ltd" },
	{ 1565,L"Quatech, Inc." },
	{ 1566,L"Nissei Electric Co." },
	{ 1568,L"Alaris, Inc." },
	{ 1569,L"ODU-Steckverbindungssysteme GmbH & Co. KG" },
	{ 1570,L"Iotech, Inc." },
	{ 1571,L"Littelfuse, Inc." },
	{ 1572,L"Avocent Corp." },
	{ 1573,L"TiMedia Technology Co., Ltd" },
	{ 1574,L"Nippon Systems Development Co., Ltd" },
	{ 1575,L"Adomax Technology Co., Ltd" },
	{ 1576,L"Tasking Software, Inc." },
	{ 1577,L"Zida Technologies, Ltd" },
	{ 1578,L"Creative Labs" },
	{ 1579,L"Greatlink Electronics Taiwan, Ltd" },
	{ 1580,L"Institute for Information Industry" },
	{ 1581,L"Taiwan Tai-Hao Enterprises Co., Ltd" },
	{ 1582,L"Mainsuper Enterprises Co., Ltd" },
	{ 1583,L"Sin Sheng Terminal & Machine, Inc." },
	{ 1585,L"JUJO Electronics Corp." },
	{ 1587,L"Cyrix Corp." },
	{ 1588,L"Micron Technology, Inc." },
	{ 1589,L"Methode Electronics, Inc." },
	{ 1590,L"Sierra Imaging, Inc." },
	{ 1592,L"Avision, Inc." },
	{ 1593,L"Chrontel, Inc." },
	{ 1594,L"Techwin Corp." },
	{ 1595,L"Taugagreining HF" },
	{ 1596,L"Yamaichi Electronics Co., Ltd (Sakura)" },
	{ 1597,L"Fong Kai Industrial Co., Ltd" },
	{ 1598,L"RealMedia Technology, Inc." },
	{ 1599,L"New Technology Cable, Ltd" },
	{ 1600,L"Hitex Development Tools" },
	{ 1601,L"Woods Industries, Inc." },
	{ 1602,L"VIA Medical Corp." },
	{ 1604,L"TEAC Corp." },
	{ 1605,L"Who? Vision Systems, Inc." },
	{ 1606,L"UMAX" },
	{ 1607,L"Acton Research Corp." },
	{ 1608,L"Inside Out Networks" },
	{ 1609,L"Weli Science Co., Ltd" },
	{ 1611,L"Analog Devices, Inc. (White Mountain DSP)" },
	{ 1612,L"Ji-Haw Industrial Co., Ltd" },
	{ 1613,L"TriTech Microelectronics, Ltd" },
	{ 1614,L"Suyin Corp." },
	{ 1615,L"WIBU-Systems AG" },
	{ 1616,L"Dynapro Systems" },
	{ 1617,L"Likom Technology Sdn. Bhd." },
	{ 1618,L"Stargate Solutions, Inc." },
	{ 1619,L"CNF, Inc." },
	{ 1620,L"Granite Microsystems, Inc." },
	{ 1621,L"Space Shuttle Hi-Tech Co., Ltd" },
	{ 1622,L"Glory Mark Electronic, Ltd" },
	{ 1623,L"Tekcon Electronics Corp." },
	{ 1624,L"Sigma Designs, Inc." },
	{ 1625,L"Aethra" },
	{ 1626,L"Optoelectronics Co., Ltd" },
	{ 1627,L"Tracewell Systems" },
	{ 1630,L"Silicon Graphics" },
	{ 1631,L"Good Way Technology Co., Ltd & GWC technology Inc." },
	{ 1632,L"TSAY-E (BVI) International, Inc." },
	{ 1633,L"Hamamatsu Photonics K.K." },
	{ 1634,L"Kansai Electric Co., Ltd" },
	{ 1635,L"Topmax Electronic Co., Ltd" },
	{ 1636,L"ET&T Technology Co., Ltd." },
	{ 1637,L"Cypress Semiconductor" },
	{ 1639,L"Aiwa Co., Ltd" },
	{ 1640,L"WordWand" },
	{ 1641,L"Oce' Printing Systems GmbH" },
	{ 1642,L"Total Technologies, Ltd" },
	{ 1643,L"Linksys, Inc." },
	{ 1645,L"Entrega, Inc." },
	{ 1646,L"Acer Semiconductor America, Inc." },
	{ 1647,L"SigmaTel, Inc." },
	{ 1648,L"Sequel Imaging" },
	{ 1650,L"Labtec, Inc." },
	{ 1651,L"HCL" },
	{ 1652,L"Key Mouse Electronic Enterprise Co., Ltd" },
	{ 1653,L"DrayTek Corp." },
	{ 1654,L"Teles AG" },
	{ 1655,L"Aiwa Co., Ltd" },
	{ 1656,L"ACard Technology Corp." },
	{ 1659,L"Prolific Technology, Inc." },
	{ 1660,L"Efficient Networks, Inc." },
	{ 1661,L"Hohner Corp." },
	{ 1662,L"Intermec Technologies Corp." },
	{ 1663,L"Virata, Ltd" },
	{ 1664,L"Realtek Semiconductor Corp., CPP Div. (Avance Logic)" },
	{ 1665,L"Siemens Information and Communication Products" },
	{ 1666,L"Victor Company of Japan, Ltd" },
	{ 1668,L"Actiontec Electronics, Inc." },
	{ 1669,L"ZD Incorporated" },
	{ 1670,L"Minolta Co., Ltd" },
	{ 1674,L"Pertech, Inc." },
	{ 1675,L"Potrans International, Inc." },
	{ 1678,L"CH Products, Inc." },
	{ 1680,L"Golden Bridge Electech, Inc." },
	{ 1683,L"Hagiwara Sys-Com Co., Ltd" },
	{ 1684,L"Lego Group" },
	{ 1688,L"Chuntex (CTX)" },
	{ 1689,L"Tektronix, Inc." },
	{ 1690,L"Askey Computer Corp." },
	{ 1691,L"Thomson, Inc." },
	{ 1693,L"Hughes Network Systems (HNS)" },
	{ 1694,L"Welcat Inc." },
	{ 1695,L"Allied Data Technologies BV" },
	{ 1698,L"Topro Technology, Inc." },
	{ 1699,L"Saitek PLC" },
	{ 1700,L"Xiamen Doowell Electron Co., Ltd" },
	{ 1701,L"Divio" },
	{ 1703,L"MicroStore, Inc." },
	{ 1704,L"Topaz Systems, Inc." },
	{ 1705,L"Westell" },
	{ 1706,L"Sysgration, Ltd" },
	{ 1708,L"Fujitsu Laboratories of America, Inc." },
	{ 1709,L"Greatland Electronics Taiwan, Ltd" },
	{ 1710,L"Professional Multimedia Testing Centre" },
	{ 1711,L"Harting, Inc. of North America" },
	{ 1720,L"Pixela Corp." },
	{ 1721,L"Alcatel Telecom" },
	{ 1722,L"Smooth Cord & Connector Co., Ltd" },
	{ 1723,L"EDA, Inc." },
	{ 1724,L"Oki Data Corp." },
	{ 1725,L"AGFA-Gevaert NV" },
	{ 1726,L"AME Optimedia Technology Co., Ltd" },
	{ 1727,L"Leoco Corp." },
	{ 1730,L"Phidgets Inc. (formerly GLAB)" },
	{ 1732,L"Bizlink International Corp." },
	{ 1733,L"Hagenuk, GmbH" },
	{ 1734,L"Infowave Software, Inc." },
	{ 1736,L"SIIG, Inc." },
	{ 1737,L"Taxan (Europe), Ltd" },
	{ 1738,L"Newer Technology, Inc." },
	{ 1739,L"Synaptics, Inc." },
	{ 1740,L"Terayon Communication Systems" },
	{ 1741,L"Keyspan" },
	{ 1742,L"Contec" },
	{ 1743,L"SpheronVR AG" },
	{ 1744,L"LapLink, Inc." },
	{ 1745,L"Daewoo Electronics Co., Ltd" },
	{ 1747,L"Mitsubishi Electric Corp." },
	{ 1748,L"Cisco Systems" },
	{ 1749,L"Toshiba" },
	{ 1750,L"Aashima Technology B.V." },
	{ 1751,L"Network Computing Devices (NCD)" },
	{ 1752,L"Technical Marketing Research, Inc." },
	{ 1754,L"Phoenixtec Power Co., Ltd" },
	{ 1755,L"Paradyne" },
	{ 1756,L"Foxlink Image Technology Co., Ltd" },
	{ 1758,L"Heisei Electronics Co., Ltd" },
	{ 1760,L"Multi-Tech Systems, Inc." },
	{ 1761,L"ADS Technologies, Inc." },
	{ 1764,L"Alcatel Microelectronics" },
	{ 1766,L"Tiger Jet Network, Inc." },
	{ 1770,L"Sirius Technologies" },
	{ 1771,L"PC Expert Tech. Co., Ltd" },
	{ 1775,L"I.A.C. Geometrische Ingenieurs B.V." },
	{ 1776,L"T.N.C Industrial Co., Ltd" },
	{ 1777,L"Opcode Systems, Inc." },
	{ 1778,L"Emine Technology Co." },
	{ 1782,L"Wintrend Technology Co., Ltd" },
	{ 1783,L"Wailly Technology Ltd" },
	{ 1784,L"Guillemot Corp." },
	{ 1785,L"ASYST electronic d.o.o." },
	{ 1786,L"HSD S.r.L" },
	{ 1788,L"Motorola Semiconductor Products Sector" },
	{ 1789,L"Boston Acoustics" },
	{ 1790,L"Gallant Computer, Inc." },
	{ 1793,L"Supercomal Wire & Cable SDN. BHD." },
	{ 1795,L"Bvtech Industry, Inc." },
	{ 1797,L"NKK Corp." },
	{ 1798,L"Ariel Corp." },
	{ 1799,L"Standard Microsystems Corp." },
	{ 1800,L"Putercom Co., Ltd" },
	{ 1801,L"Silicon Systems, Ltd (SSL)" },
	{ 1802,L"Oki Electric Industry Co., Ltd" },
	{ 1805,L"Comoss Electronic Co., Ltd" },
	{ 1806,L"Excel Cell Electronic Co., Ltd" },
	{ 1808,L"Connect Tech, Inc." },
	{ 1809,L"Magic Control Technology Corp." },
	{ 1811,L"Interval Research Corp." },
	{ 1812,L"NewMotion, Inc." },
	{ 1815,L"ZNK Corp." },
	{ 1816,L"Imation Corp." },
	{ 1817,L"Tremon Enterprises Co., Ltd" },
	{ 1819,L"Domain Technologies, Inc." },
	{ 1820,L"Xionics Document Technologies, Inc." },
	{ 1821,L"Eicon Networks Corp." },
	{ 1822,L"Ariston Technologies" },
	{ 1827,L"Centillium Communications Corp." },
	{ 1830,L"Vanguard International Semiconductor-America" },
	{ 1833,L"Amitm" },
	{ 1838,L"Sunix Co., Ltd" },
	{ 1839,L"Advanced Card Systems, Ltd" },
	{ 1841,L"Susteen, Inc." },
	{ 1842,L"Goldfull Electronics & Telecommunications Corp." },
	{ 1843,L"ViewQuest Technologies, Inc." },
	{ 1844,L"Lasat Communications A/S" },
	{ 1845,L"Asuscom Network" },
	{ 1846,L"Lorom Industrial Co., Ltd" },
	{ 1848,L"Mad Catz, Inc." },
	{ 1850,L"Chaplet Systems, Inc." },
	{ 1851,L"Suncom Technologies" },
	{ 1852,L"Industrial Electronic Engineers, Inc." },
	{ 1853,L"Eutron S.p.a." },
	{ 1854,L"NEC, Inc." },
	{ 1858,L"Stollmann" },
	{ 1861,L"Syntech Information Co., Ltd" },
	{ 1862,L"Onkyo Corp." },
	{ 1863,L"Labway Corp." },
	{ 1864,L"Strong Man Enterprise Co., Ltd" },
	{ 1865,L"EVer Electronics Corp." },
	{ 1866,L"Ming Fortune Industry Co., Ltd" },
	{ 1867,L"Polestar Tech. Corp." },
	{ 1868,L"C-C-C Group PLC" },
	{ 1869,L"Micronas GmbH" },
	{ 1870,L"Digital Stream Corp." },
	{ 1877,L"Aureal Semiconductor" },
	{ 1879,L"Network Technologies, Inc." },
	{ 1883,L"Sophisticated Circuits, Inc." },
	{ 1891,L"Midiman" },
	{ 1892,L"Cyber Power System, Inc." },
	{ 1893,L"X-Rite, Inc." },
	{ 1894,L"Jess-Link Products Co., Ltd" },
	{ 1895,L"Tokheim Corp." },
	{ 1896,L"Camtel Technology Corp." },
	{ 1897,L"Surecom Technology Corp." },
	{ 1898,L"Smart Technology Enablers, Inc." },
	{ 1899,L"OmniKey AG" },
	{ 1900,L"Partner Tech" },
	{ 1901,L"Denso Corp." },
	{ 1902,L"Kuan Tech Enterprise Co., Ltd" },
	{ 1903,L"Jhen Vei Electronic Co., Ltd" },
	{ 1904,L"Welch Allyn, Inc - Medical Division" },
	{ 1905,L"Observator Instruments BV" },
	{ 1906,L"Your data Our Care" },
	{ 1908,L"AmTRAN Technology Co., Ltd" },
	{ 1909,L"Longshine Electronics Corp." },
	{ 1910,L"Inalways Corp." },
	{ 1911,L"Comda Enterprise Corp." },
	{ 1912,L"Volex, Inc." },
	{ 1913,L"Fairchild Semiconductor" },
	{ 1914,L"Sankyo Seiki Mfg. Co., Ltd" },
	{ 1915,L"Linksys" },
	{ 1916,L"Forward Electronics Co., Ltd" },
	{ 1917,L"Griffin Technology" },
	{ 1919,L"Well Excellent & Most Corp." },
	{ 1920,L"Sagem Monetel GmbH" },
	{ 1921,L"SanDisk Corp." },
	{ 1922,L"Trackerball" },
	{ 1923,L"C3PO" },
	{ 1924,L"Vivitar, Inc." },
	{ 1925,L"NTT-ME" },
	{ 1929,L"Logitec Corp." },
	{ 1931,L"Happ Controls, Inc." },
	{ 1932,L"GTCO/CalComp" },
	{ 1934,L"Brincom, Inc." },
	{ 1936,L"Pro-Image Manufacturing Co., Ltd" },
	{ 1937,L"Copartner Wire and Cable Mfg. Corp." },
	{ 1938,L"Axis Communications AB" },
	{ 1939,L"Wha Yu Industrial Co., Ltd" },
	{ 1940,L"ABL Electronics Corp." },
	{ 1941,L"RealChip, Inc." },
	{ 1942,L"Certicom Corp." },
	{ 1943,L"Grandtech Semiconductor Corp." },
	{ 1944,L"Optelec" },
	{ 1945,L"Altera" },
	{ 1947,L"Sagem" },
	{ 1949,L"Alfadata Computer Corp." },
	{ 1953,L"Digicom S.p.A." },
	{ 1954,L"National Technical Systems" },
	{ 1955,L"Onnto Corp." },
	{ 1956,L"Be, Inc." },
	{ 1958,L"ADMtek, Inc." },
	{ 1962,L"Corega K.K." },
	{ 1963,L"Freecom Technologies" },
	{ 1967,L"Microtech" },
	{ 1968,L"Trust Technologies" },
	{ 1969,L"IMP, Inc." },
	{ 1970,L"Motorola BCS, Inc." },
	{ 1971,L"Plustek, Inc." },
	{ 1972,L"Olympus Optical Co., Ltd" },
	{ 1973,L"Mega World International, Ltd" },
	{ 1974,L"Marubun Corp." },
	{ 1975,L"TIME Interconnect, Ltd" },
	{ 1976,L"AboCom Systems Inc" },
	{ 1980,L"Canon Computer Systems, Inc." },
	{ 1981,L"Webgear, Inc." },
	{ 1982,L"Veridicom" },
	{ 1984,L"Code Mercenaries Hard- und Software GmbH" },
	{ 1985,L"Keisokugiken" },
	{ 1988,L"Datafab Systems, Inc." },
	{ 1989,L"APG Cash Drawer" },
	{ 1990,L"ShareWave, Inc." },
	{ 1991,L"Powertech Industrial Co., Ltd" },
	{ 1992,L"B.U.G., Inc." },
	{ 1993,L"Allied Telesyn International" },
	{ 1994,L"AVerMedia Technologies, Inc." },
	{ 1995,L"Kingmax Technology, Inc." },
	{ 1996,L"Carry Computer Eng., Co., Ltd" },
	{ 1997,L"Elektor" },
	{ 1999,L"Casio Computer Co., Ltd" },
	{ 2000,L"Dazzle" },
	{ 2001,L"D-Link System" },
	{ 2002,L"Aptio Products, Inc." },
	{ 2003,L"Cyberdata Corp." },
	{ 2005,L"Radiant Systems" },
	{ 2007,L"GCC Technologies, Inc." },
	{ 2010,L"Arasan Chip Systems" },
	{ 2014,L"Diamond Multimedia" },
	{ 2015,L"David Electronics Co., Ltd" },
	{ 2016,L"NCP engineering GmbH" },
	{ 2017,L"Ambient Technologies, Inc." },
	{ 2018,L"Elmeg GmbH & Co., Ltd" },
	{ 2019,L"Planex Communications, Inc." },
	{ 2020,L"Movado Enterprise Co., Ltd" },
	{ 2021,L"QPS, Inc." },
	{ 2022,L"Allied Cable Corp." },
	{ 2023,L"Mirvo Toys, Inc." },
	{ 2024,L"Labsystems" },
	{ 2026,L"Iwatsu Electric Co., Ltd" },
	{ 2027,L"Double-H Technology Co., Ltd" },
	{ 2028,L"Taiyo Electric Wire & Cable Co., Ltd" },
	{ 2030,L"Torex Retail (formerly Logware)" },
	{ 2031,L"STSN" },
	{ 2034,L"Microcomputer Applications, Inc." },
	{ 2038,L"Circuit Assembly Corp." },
	{ 2039,L"Century Corp." },
	{ 2041,L"Dotop Technology, Inc." },
	{ 2042,L"DrayTek Corp." },
	{ 2044,L"Thomann" },
	{ 2045,L"Mark of the Unicorn" },
	{ 2047,L"Unknown" },
	{ 2049,L"MagTek" },
	{ 2050,L"Mako Technologies, LLC" },
	{ 2051,L"Zoom Telephonics, Inc." },
	{ 2057,L"Genicom Technology, Inc." },
	{ 2058,L"Evermuch Technology Co., Ltd" },
	{ 2059,L"Cross Match Technologies" },
	{ 2060,L"Datalogic S.p.A." },
	{ 2061,L"Teco Image Systems Co., Ltd" },
	{ 2064,L"Personal Communication Systems, Inc." },
	{ 2067,L"Mattel, Inc." },
	{ 2073,L"eLicenser" },
	{ 2074,L"MG Logic" },
	{ 2075,L"Indigita Corp." },
	{ 2076,L"Mipsys" },
	{ 2078,L"AlphaSmart, Inc." },
	{ 2082,L"Reudo Corp." },
	{ 2085,L"GC Protronics" },
	{ 2086,L"Data Transit" },
	{ 2087,L"BroadLogic, Inc." },
	{ 2088,L"Sato Corp." },
	{ 2089,L"DirecTV Broadband, Inc. (Telocity)" },
	{ 2093,L"Handspring" },
	{ 2096,L"Palm, Inc." },
	{ 2098,L"Kouwell Electronics Corp." },
	{ 2099,L"Sourcenext Corp." },
	{ 2101,L"Action Star Enterprise Co., Ltd" },
	{ 2102,L"TrekStor" },
	{ 2105,L"Samsung Techwin Co., Ltd" },
	{ 2106,L"Accton Technology Corp." },
	{ 2111,L"Global Village" },
	{ 2112,L"Argosy Research, Inc." },
	{ 2113,L"Rioport.com, Inc." },
	{ 2116,L"Welland Industrial Co., Ltd" },
	{ 2118,L"NetGear, Inc." },
	{ 2125,L"Minton Optic Industry Co., Inc." },
	{ 2126,L"KB Gear" },
	{ 2127,L"Empeg" },
	{ 2128,L"Fast Point Technologies, Inc." },
	{ 2129,L"Macronix International Co., Ltd" },
	{ 2130,L"CSEM" },
	{ 2131,L"Topre Corporation" },
	{ 2132,L"ActiveWire, Inc." },
	{ 2134,L"B&B Electronics" },
	{ 2136,L"Hitachi Maxell, Ltd" },
	{ 2137,L"Minolta Systems Laboratory, Inc." },
	{ 2138,L"Xircom" },
	{ 2140,L"ColorVision, Inc." },
	{ 2146,L"Teletrol Systems, Inc." },
	{ 2147,L"Filanet Corp." },
	{ 2148,L"NetGear, Inc." },
	{ 2151,L"Data Translation, Inc." },
	{ 2154,L"Emagic Soft- und Hardware GmbH" },
	{ 2156,L"DeTeWe - Deutsche Telephonwerke AG & Co." },
	{ 2158,L"System TALKS, Inc." },
	{ 2159,L"MEC IMEX, Inc." },
	{ 2160,L"Metricom" },
	{ 2161,L"SanDisk, Inc." },
	{ 2163,L"Xpeed, Inc." },
	{ 2164,L"A-Tec Subsystem, Inc." },
	{ 2169,L"Comtrol Corp." },
	{ 2172,L"Adesso/Kbtek America, Inc." },
	{ 2173,L"Jaton Corp." },
	{ 2174,L"Fujitsu Computer Products of America" },
	{ 2175,L"QualCore Logic Inc." },
	{ 2176,L"APT Technologies, Inc." },
	{ 2179,L"Recording Industry Association of America (RIAA)" },
	{ 2181,L"Boca Research, Inc." },
	{ 2182,L"XAC Automation Corp." },
	{ 2183,L"Hannstar Electronics Corp." },
	{ 2186,L"TechTools" },
	{ 2187,L"MassWorks, Inc." },
	{ 2188,L"Swecoin AB" },
	{ 2190,L"iLok" },
	{ 2194,L"DioGraphy, Inc." },
	{ 2196,L"TSI Incorporated" },
	{ 2199,L"Lauterbach" },
	{ 2204,L"United Technologies Research Cntr." },
	{ 2205,L"Icron Technologies Corp." },
	{ 2206,L"NST Co., Ltd" },
	{ 2207,L"Primex Aerospace Co." },
	{ 2213,L"e9, Inc." },
	{ 2214,L"Toshiba TEC" },
	{ 2216,L"Andrea Electronics" },
	{ 2217,L"CWAV Inc." },
	{ 2220,L"Macraigor Systems LLC" },
	{ 2222,L"Macally (Mace Group, Inc.)" },
	{ 2224,L"Metrohm" },
	{ 2228,L"Sorenson Vision, Inc." },
	{ 2231,L"NATSU" },
	{ 2232,L"J. Gordon Electronic Design, Inc." },
	{ 2233,L"RadioShack Corp. (Tandy)" },
	{ 2235,L"Texas Instruments" },
	{ 2237,L"Citizen Watch Co., Ltd" },
	{ 2243,L"Precise Biometrics" },
	{ 2244,L"Proxim, Inc." },
	{ 2247,L"Key Nice Enterprise Co., Ltd" },
	{ 2248,L"2Wire, Inc." },
	{ 2249,L"Nippon Telegraph and Telephone Corp." },
	{ 2250,L"Aiptek International, Inc." },
	{ 2253,L"Jue Hsun Ind. Corp." },
	{ 2254,L"Long Well Electronics Corp." },
	{ 2255,L"Productivity Enhancement Products" },
	{ 2257,L"smartBridges, Inc." },
	{ 2259,L"Virtual Ink" },
	{ 2260,L"Fujitsu Siemens Computers" },
	{ 2264,L"IXXAT Automation GmbH" },
	{ 2265,L"Increment P Corp." },
	{ 2269,L"Billionton Systems, Inc." },
	{ 2270,L"???" },
	{ 2271,L"Spyrus, Inc." },
	{ 2275,L"Olitec, Inc." },
	{ 2276,L"Pioneer Corp." },
	{ 2277,L"Litronic" },
	{ 2278,L"Gemalto (was Gemplus)" },
	{ 2279,L"Pan-International Wire & Cable" },
	{ 2280,L"Integrated Memory Logic" },
	{ 2281,L"Extended Systems, Inc." },
	{ 2282,L"Ericsson, Inc., Blue Ridge Labs" },
	{ 2284,L"M-Systems Flash Disk Pioneers" },
	{ 2285,L"MediaTek Inc." },
	{ 2286,L"CCSI/Hesso" },
	{ 2288,L"Corex Technologies" },
	{ 2289,L"CTI Electronics Corp." },
	{ 2290,L"Gotop Information Inc." },
	{ 2293,L"SysTec Co., Ltd" },
	{ 2294,L"Logic 3 International, Ltd" },
	{ 2295,L"Vernier" },
	{ 2296,L"Keen Top International Enterprise Co., Ltd" },
	{ 2297,L"Wipro Technologies" },
	{ 2298,L"Caere" },
	{ 2299,L"Socket Communications" },
	{ 2300,L"Sicon Cable Technology Co., Ltd" },
	{ 2301,L"Digianswer A/S" },
	{ 2303,L"AuthenTec, Inc." },
	{ 2304,L"Pinnacle Systems, Inc." },
	{ 2305,L"VST Technologies" },
	{ 2310,L"Faraday Technology Corp." },
	{ 2312,L"Siemens AG" },
	{ 2313,L"Audio-Technica Corp." },
	{ 2314,L"Trumpion Microelectronics, Inc." },
	{ 2315,L"Neurosmith" },
	{ 2316,L"Silicon Motion, Inc. - Taiwan (formerly Feiya Technology Corp.)" },
	{ 2317,L"Multiport Computer Vertriebs GmbH" },
	{ 2318,L"Shining Technology, Inc." },
	{ 2319,L"Fujitsu Devices, Inc." },
	{ 2320,L"Alation Systems, Inc." },
	{ 2321,L"Philips Speech Processing" },
	{ 2322,L"Voquette, Inc." },
	{ 2325,L"GlobeSpan, Inc." },
	{ 2327,L"SmartDisk Corp." },
	{ 2329,L"Tiger Electronics" },
	{ 2334,L"Garmin International" },
	{ 2336,L"Echelon Co." },
	{ 2337,L"GoHubs, Inc." },
	{ 2338,L"Dymo-CoStar Corp." },
	{ 2339,L"IC Media Corp." },
	{ 2340,L"Xerox" },
	{ 2341,L"Lakeview Research" },
	{ 2343,L"Summus, Ltd" },
	{ 2344,L"PLX Technology, Inc. (formerly Oxford Semiconductor, Ltd)" },
	{ 2345,L"American Biometric Co." },
	{ 2346,L"Toshiba Information & Industrial Sys. And Services" },
	{ 2347,L"Sena Technologies, Inc." },
	{ 2351,L"Northern Embedded Science/CAVNEX" },
	{ 2352,L"Toshiba Corp." },
	{ 2353,L"Harmonic Data Systems, Ltd" },
	{ 2354,L"Crescentec Corp." },
	{ 2355,L"Quantum Corp." },
	{ 2356,L"Spirent Communications" },
	{ 2358,L"NuTesla" },
	{ 2361,L"Lumberg, Inc." },
	{ 2362,L"Pixart Imaging, Inc." },
	{ 2363,L"Plextor Corp." },
	{ 2364,L"Intrepid Control Systems, Inc." },
	{ 2365,L"InnoSync, Inc." },
	{ 2366,L"J.S.T. Mfg. Co., Ltd" },
	{ 2367,L"Olympia Telecom Vertriebs GmbH" },
	{ 2368,L"Japan Storage Battery Co., Ltd" },
	{ 2369,L"Photobit Corp." },
	{ 2370,L"i2Go.com, LLC" },
	{ 2371,L"HCL Technologies India Private, Ltd" },
	{ 2372,L"KORG, Inc." },
	{ 2373,L"Pasco Scientific" },
	{ 2376,L"Kronauer music in digital" },
	{ 2379,L"Linkup Systems Corp." },
	{ 2381,L"Cable Television Laboratories" },
	{ 2383,L"Yano" },
	{ 2385,L"Kingston Technology" },
	{ 2388,L"RPM Systems Corp." },
	{ 2389,L"NVidia Corp." },
	{ 2390,L"BSquare Corp." },
	{ 2391,L"Agilent Technologies, Inc." },
	{ 2392,L"CompuLink Research, Inc." },
	{ 2393,L"Cologne Chip AG" },
	{ 2394,L"Portsmith" },
	{ 2395,L"Medialogic Corp." },
	{ 2396,L"K-Tec Electronics" },
	{ 2397,L"Polycom, Inc." },
	{ 2407,L"Acer NeWeb Corp." },
	{ 2408,L"Catalyst Enterprises, Inc." },
	{ 2414,L"Feitian Technologies, Inc." },
	{ 2417,L"Gretag-Macbeth AG" },
	{ 2419,L"Schlumberger" },
	{ 2420,L"Datagraphix, a business unit of Anacomp" },
	{ 2421,L"OL'E Communications, Inc." },
	{ 2422,L"Adirondack Wire & Cable" },
	{ 2423,L"Lightsurf Technologies" },
	{ 2424,L"Beckhoff GmbH" },
	{ 2425,L"Jeilin Technology Corp., Ltd" },
	{ 2426,L"Minds At Work LLC" },
	{ 2427,L"Knudsen Engineering, Ltd" },
	{ 2428,L"Marunix Co., Ltd" },
	{ 2429,L"Rosun Technologies, Inc." },
	{ 2430,L"Biopac Systems Inc." },
	{ 2431,L"Barun Electronics Co., Ltd" },
	{ 2433,L"Oak Technology, Ltd" },
	{ 2436,L"Apricorn" },
	{ 2437,L"cab Produkttechnik GmbH & Co KG" },
	{ 2438,L"Matsushita Electric Works, Ltd." },
	{ 2444,L"Vitana Corp." },
	{ 2445,L"INDesign" },
	{ 2446,L"Integrated Intellectual Property, Inc." },
	{ 2447,L"Kenwood TMI Corp." },
	{ 2451,L"Gemstar eBook Group, Ltd" },
	{ 2454,L"Integrated Telecom Express, Inc." },
	{ 2458,L"Zippy Technology Corp." },
	{ 2462,L"Trimble Navigation, Ltd" },
	{ 2467,L"PairGain Technologies" },
	{ 2468,L"Contech Research, Inc." },
	{ 2469,L"VCON Telecommunications" },
	{ 2470,L"Poinchips" },
	{ 2471,L"Data Transmission Network Corp." },
	{ 2472,L"Lin Shiung Enterprise Co., Ltd" },
	{ 2473,L"Smart Card Technologies Co., Ltd" },
	{ 2474,L"Intersil Corp." },
	{ 2475,L"Japan Cash Machine Co., Ltd." },
	{ 2478,L"Tripp Lite" },
	{ 2482,L"Franklin Electronic Publishers, Inc." },
	{ 2483,L"Altius Solutions, Inc." },
	{ 2484,L"MDS Telephone Systems" },
	{ 2485,L"Celltrix Technology Co., Ltd" },
	{ 2492,L"Grundig" },
	{ 2494,L"MySmart.Com" },
	{ 2495,L"Auerswald GmbH & Co. KG" },
	{ 2496,L"Genpix Electronics, LLC" },
	{ 2497,L"Arris Interactive LLC" },
	{ 2498,L"Nisca Corp." },
	{ 2499,L"ActivCard, Inc." },
	{ 2500,L"ACTiSYS Corp." },
	{ 2501,L"Memory Corp." },
	{ 2506,L"BMC Messsysteme GmbH" },
	{ 2507,L"FLIR Systems" },
	{ 2508,L"Workbit Corp." },
	{ 2509,L"Psion Dacom Home Networks, Ltd" },
	{ 2510,L"City Electronics, Ltd" },
	{ 2511,L"Electronics Testing Center, Taiwan" },
	{ 2513,L"NeoMagic, Inc." },
	{ 2514,L"Vreelin Engineering, Inc." },
	{ 2515,L"Com One" },
	{ 2519,L"NovAtel Inc." },
	{ 2520,L"ELATEC" },
	{ 2521,L"KRF Tech, Ltd" },
	{ 2522,L"A4Tech Co., Ltd." },
	{ 2523,L"Measurement Computing Corp." },
	{ 2524,L"Aimex Corp." },
	{ 2525,L"Fellowes, Inc." },
	{ 2527,L"Addonics Technologies Corp." },
	{ 2529,L"Intellon Corp." },
	{ 2533,L"Jo-Dan International, Inc." },
	{ 2534,L"Silutia, Inc." },
	{ 2535,L"Real 3D, Inc." },
	{ 2536,L"AKAI  Professional M.I. Corp." },
	{ 2537,L"Chen-Source, Inc." },
	{ 2539,L"IM Networks, Inc." },
	{ 2543,L"Xitel" },
	{ 2547,L"GoFlight, Inc." },
	{ 2549,L"AresCom" },
	{ 2550,L"RocketChips, Inc." },
	{ 2551,L"Edu-Science (H.K.), Ltd" },
	{ 2552,L"SoftConnex Technologies, Inc." },
	{ 2553,L"Bay Associates" },
	{ 2554,L"Mtek Vision" },
	{ 2555,L"Altera" },
	{ 2559,L"Gain Technology Corp." },
	{ 2560,L"Liquid Audio" },
	{ 2561,L"ViA, Inc." },
	{ 2565,L"Unknown Manufacturer" },
	{ 2567,L"Ontrak Control Systems Inc." },
	{ 2571,L"Cybex Computer Products Co." },
	{ 2573,L"Servergy, Inc" },
	{ 2577,L"Xentec, Inc." },
	{ 2578,L"Cambridge Silicon Radio, Ltd" },
	{ 2579,L"Telebyte, Inc." },
	{ 2580,L"Spacelabs Medical, Inc." },
	{ 2581,L"Scalar Corp." },
	{ 2582,L"Trek Technology (S) PTE, Ltd" },
	{ 2583,L"Pentax Corp." },
	{ 2584,L"Heidelberger Druckmaschinen AG" },
	{ 2585,L"Hua Geng Technologies, Inc." },
	{ 2593,L"Medtronic Physio Control Corp." },
	{ 2594,L"Century Semiconductor USA, Inc." },
	{ 2599,L"Datacard Group" },
	{ 2604,L"AK-Modul-Bus Computer GmbH" },
	{ 2612,L"TG3 Electronics, Inc." },
	{ 2613,L"Radikal Technologies" },
	{ 2617,L"Gilat Satellite Networks, Ltd" },
	{ 2618,L"PentaMedia Co., Ltd" },
	{ 2620,L"NTT DoCoMo, Inc." },
	{ 2621,L"Varo Vision" },
	{ 2623,L"Swissonic AG" },
	{ 2627,L"Boca Systems, Inc." },
	{ 2630,L"Davicom Semiconductor, Inc." },
	{ 2631,L"Hirose Electric" },
	{ 2632,L"I/O Interconnect" },
	{ 2634,L"Ploytec GmbH" },
	{ 2635,L"Fujitsu Media Devices, Ltd" },
	{ 2636,L"Computex Co., Ltd" },
	{ 2637,L"Evolution Electronics, Ltd" },
	{ 2638,L"Steinberg Soft-und Hardware GmbH" },
	{ 2639,L"Litton Systems, Inc." },
	{ 2640,L"Mimaki Engineering Co., Ltd" },
	{ 2641,L"Sony Electronics, Inc." },
	{ 2642,L"Jebsee Electronics Co., Ltd" },
	{ 2643,L"Portable Peripheral Co., Ltd" },
	{ 2650,L"Electronics For Imaging, Inc." },
	{ 2651,L"EAsics NV" },
	{ 2652,L"Broadcom Corp." },
	{ 2653,L"Diatrend Corp." },
	{ 2655,L"Zebra" },
	{ 2658,L"MPMan" },
	{ 2662,L"ClearCube Technology" },
	{ 2663,L"Medeli Electronics Co., Ltd" },
	{ 2664,L"Comaide Corp." },
	{ 2665,L"Chroma ate, Inc." },
	{ 2667,L"Green House Co., Ltd" },
	{ 2668,L"Integrated Circuit Systems, Inc." },
	{ 2669,L"UPS Manufacturing" },
	{ 2670,L"Benwin" },
	{ 2671,L"Core Technology, Inc." },
	{ 2672,L"International Game Technology" },
	{ 2673,L"VIPColor Technologies USA, Inc." },
	{ 2674,L"Sanwa Denshi" },
	{ 2675,L"Mackie Designs" },
	{ 2685,L"NSTL, Inc." },
	{ 2686,L"Octagon Systems Corp." },
	{ 2688,L"Rexon Technology Corp., Ltd" },
	{ 2689,L"Chesen Electronics Corp." },
	{ 2690,L"Syscan" },
	{ 2691,L"NextComm, Inc." },
	{ 2692,L"Maui Innovative Peripherals" },
	{ 2693,L"Idexx Labs" },
	{ 2694,L"NITGen Co., Ltd" },
	{ 2697,L"Aktiv" },
	{ 2701,L"Picturetel" },
	{ 2702,L"Japan Aviation Electronics Industry, Ltd" },
	{ 2704,L"Candy Technology Co., Ltd" },
	{ 2705,L"Globlink Technology, Inc." },
	{ 2706,L"EGO SYStems, Inc." },
	{ 2707,L"C Technologies AB" },
	{ 2708,L"Intersense" },
	{ 2723,L"Lava Computer Mfg., Inc." },
	{ 2724,L"Develco Elektronik" },
	{ 2725,L"First International Digital" },
	{ 2726,L"Perception Digital, Ltd" },
	{ 2727,L"Wincor Nixdorf International GmbH" },
	{ 2728,L"TriGem Computer, Inc." },
	{ 2729,L"Baromtec Co." },
	{ 2730,L"Japan CBM Corp." },
	{ 2731,L"Vision Shape Europe SA" },
	{ 2732,L"iCompression, Inc." },
	{ 2733,L"Rohde & Schwarz GmbH & Co. KG" },
	{ 2734,L"NEC infrontia Corp. (Nitsuko)" },
	{ 2735,L"Digitalway Co., Ltd" },
	{ 2736,L"Arrow Strong Electronics Co., Ltd" },
	{ 2737,L"FEIG ELECTRONIC GmbH" },
	{ 2746,L"Ellisys" },
	{ 2750,L"Stereo-Link" },
	{ 2751,L"Diolan" },
	{ 2755,L"Sanyo Semiconductor Company Micro" },
	{ 2756,L"Leco Corp." },
	{ 2757,L"I & C Corp." },
	{ 2758,L"Singing Electrons, Inc." },
	{ 2759,L"Panwest Corp." },
	{ 2760,L"Z-Star Microelectronics Corp." },
	{ 2761,L"Micro Solutions, Inc." },
	{ 2762,L"OPEN Networks Ltd" },
	{ 2764,L"Koga Electronics Co." },
	{ 2765,L"ID Tech" },
	{ 2766,L"ZyDAS" },
	{ 2767,L"Intoto, Inc." },
	{ 2768,L"Intellix Corp." },
	{ 2769,L"Remotec Technology, Ltd" },
	{ 2770,L"Service & Quality Technology Co., Ltd" },
	{ 2778,L"Data Encryption Systems Ltd." },
	{ 2787,L"Allion Computer, Inc." },
	{ 2788,L"Taito Corp." },
	{ 2791,L"Neodym Systems, Inc." },
	{ 2792,L"System Support Co., Ltd" },
	{ 2793,L"North Shore Circuit Design L.L.P." },
	{ 2794,L"SciEssence, LLC" },
	{ 2795,L"TTP Communications, Ltd" },
	{ 2796,L"Neodio Technologies Corp." },
	{ 2800,L"Option" },
	{ 2806,L"Silver I Co., Ltd" },
	{ 2807,L"B2C2, Inc." },
	{ 2809,L"Hama, Inc." },
	{ 2810,L"DMC Co., Ltd." },
	{ 2812,L"Zaptronix Ltd" },
	{ 2813,L"Tateno Dennou, Inc." },
	{ 2814,L"Cummins Engine Co." },
	{ 2815,L"Jump Zone Network Products, Inc." },
	{ 2816,L"INGENICO" },
	{ 2821,L"ASUSTek Computer, Inc." },
	{ 2827,L"Datamax-O'Neil" },
	{ 2828,L"Todos AB" },
	{ 2829,L"ProjectLab" },
	{ 2830,L"GN Netcom" },
	{ 2831,L"AVID Technology" },
	{ 2832,L"Pcally" },
	{ 2833,L"I Tech Solutions Co., Ltd" },
	{ 2846,L"Electronic Warfare Assoc., Inc. (EWA)" },
	{ 2847,L"Insyde Software Corp." },
	{ 2848,L"TransDimension, Inc." },
	{ 2849,L"Yokogawa Electric Corp." },
	{ 2850,L"Japan System Development Co., Ltd" },
	{ 2851,L"Pan-Asia Electronics Co., Ltd" },
	{ 2852,L"Link Evolution Corp." },
	{ 2855,L"Ritek Corp." },
	{ 2856,L"Kenwood Corp." },
	{ 2860,L"Village Center, Inc." },
	{ 2864,L"PNY Technologies, Inc." },
	{ 2867,L"Contour Design, Inc." },
	{ 2871,L"Hitachi ULSI Systems Co., Ltd" },
	{ 2872,L"Gear Head" },
	{ 2873,L"Omnidirectional Control Technology, Inc." },
	{ 2874,L"IPaxess" },
	{ 2875,L"Tekram Technology Co., Ltd" },
	{ 2876,L"Olivetti Techcenter" },
	{ 2878,L"Kikusui Electronics Corp." },
	{ 2881,L"Hal Corp." },
	{ 2883,L"Play.com, Inc." },
	{ 2887,L"Sportbug.com, Inc." },
	{ 2888,L"TechnoTrend AG" },
	{ 2889,L"ASCII Corp." },
	{ 2891,L"Pine Corp. Ltd." },
	{ 2893,L"Graphtec America, Inc." },
	{ 2894,L"Musical Electronics, Ltd" },
	{ 2896,L"Dumpries Co., Ltd" },
	{ 2897,L"Comfort Keyboard Co." },
	{ 2898,L"Colorado MicroDisplay, Inc." },
	{ 2900,L"Sinbon Electronics Co., Ltd" },
	{ 2902,L"TYI Systems, Ltd" },
	{ 2903,L"Beijing HanwangTechnology Co., Ltd" },
	{ 2905,L"Lake Communications, Ltd" },
	{ 2906,L"Corel Corp." },
	{ 2911,L"Green Electronics Co., Ltd" },
	{ 2912,L"Nsine, Ltd" },
	{ 2913,L"NEC Viewtechnology, Ltd" },
	{ 2914,L"Orange Micro, Inc." },
	{ 2915,L"ADLink Technology, Inc." },
	{ 2916,L"Wonderful Wire Cable Co., Ltd" },
	{ 2917,L"Expert Magnetics Corp." },
	{ 2918,L"Cybiko Inc." },
	{ 2919,L"Fairbanks Scales" },
	{ 2921,L"CacheVision" },
	{ 2922,L"Maxim Integrated Products" },
	{ 2927,L"Nagano Japan Radio Co., Ltd" },
	{ 2928,L"PortalPlayer, Inc." },
	{ 2929,L"SHIN-EI Sangyo Co., Ltd" },
	{ 2930,L"Embedded Wireless Technology Co., Ltd" },
	{ 2931,L"Computone Corp." },
	{ 2933,L"Roland DG Corp." },
	{ 2937,L"Sunrise Telecom, Inc." },
	{ 2938,L"Zeevo, Inc." },
	{ 2939,L"Taiko Denki Co., Ltd" },
	{ 2940,L"ITRAN Communications, Ltd" },
	{ 2941,L"Astrodesign, Inc." },
	{ 2945,L"id3 Technologies" },
	{ 2948,L"Rextron Technology, Inc." },
	{ 2949,L"Elkat Electronics, Sdn., Bhd." },
	{ 2950,L"Exputer Systems, Inc." },
	{ 2951,L"Plus-One I & T, Inc." },
	{ 2952,L"Sigma Koki Co., Ltd, Technology Center" },
	{ 2953,L"Advanced Digital Broadcast, Ltd" },
	{ 2956,L"SMART Technologies Inc." },
	{ 2965,L"ASIX Electronics Corp." },
	{ 2966,L"Sewon Telecom" },
	{ 2967,L"O2 Micro, Inc." },
	{ 2968,L"Playmates Toys, Inc." },
	{ 2969,L"Audio International, Inc." },
	{ 2971,L"Dipl.-Ing. Stefan Kunde" },
	{ 2973,L"Softprotec Co." },
	{ 2975,L"Chippo Technologies" },
	{ 2991,L"U.S. Robotics" },
	{ 2992,L"Concord Camera Corp." },
	{ 2993,L"Infinilink Corp." },
	{ 2994,L"Ambit Microsystems Corp." },
	{ 2995,L"Ofuji Technology" },
	{ 2996,L"HTC (High Tech Computer Corp.)" },
	{ 2997,L"Murata Manufacturing Co., Ltd" },
	{ 2998,L"Network Alchemy" },
	{ 2999,L"Joytech Computer Co., Ltd" },
	{ 3000,L"Hitachi Semiconductor and Devices Sales Co., Ltd" },
	{ 3001,L"Eiger M&C Co., Ltd" },
	{ 3002,L"ZAccess Systems" },
	{ 3003,L"General Meters Corp." },
	{ 3004,L"Assistive Technology, Inc." },
	{ 3005,L"System Connection, Inc." },
	{ 3008,L"Knilink Technology, Inc." },
	{ 3009,L"Fuw Yng Electronics Co., Ltd" },
	{ 3010,L"Seagate RSS LLC" },
	{ 3011,L"IPWireless, Inc." },
	{ 3012,L"Microcube Corp." },
	{ 3013,L"JCN Co., Ltd" },
	{ 3014,L"ExWAY, Inc." },
	{ 3015,L"X10 Wireless Technology, Inc." },
	{ 3016,L"Telmax Communications" },
	{ 3017,L"ECI Telecom, Ltd" },
	{ 3018,L"Startek Engineering, Inc." },
	{ 3019,L"Perfect Technic Enterprise Co., Ltd" },
	{ 3031,L"Andrew Pargeter & Associates" },
	{ 3034,L"Realtek Semiconductor Corp." },
	{ 3035,L"Ericsson Business Mobile Networks BV" },
	{ 3036,L"Y Media Corp." },
	{ 3037,L"Orange PCS" },
	{ 3042,L"Kanda Tsushin Kogyo Co., Ltd" },
	{ 3043,L"TOYO Corp." },
	{ 3044,L"Elka International, Ltd" },
	{ 3045,L"DOME imaging systems, Inc." },
	{ 3046,L"Dong Guan Humen Wonderful Wire Cable Factory" },
	{ 3053,L"MEI" },
	{ 3054,L"LTK Industries, Ltd" },
	{ 3055,L"Way2Call Communications" },
	{ 3056,L"Pace Micro Technology PLC" },
	{ 3057,L"Intracom S.A." },
	{ 3058,L"Konexx" },
	{ 3062,L"Addonics Technologies, Inc." },
	{ 3063,L"Sunny Giken, Inc." },
	{ 3064,L"Fujitsu Siemens Computers" },
	{ 3069,L"Kvaser AB" },
	{ 3076,L"MOTO Development Group, Inc." },
	{ 3077,L"Appian Graphics" },
	{ 3078,L"Hasbro Games, Inc." },
	{ 3079,L"Infinite Data Storage, Ltd" },
	{ 3080,L"Agate" },
	{ 3081,L"Comjet Information System" },
	{ 3082,L"Highpoint Technologies, Inc." },
	{ 3083,L"Dura Micro, Inc. (Acomdata)" },
	{ 3090,L"Zeroplus" },
	{ 3093,L"Iris Graphics" },
	{ 3094,L"Gyration, Inc." },
	{ 3095,L"Cyberboard A/S" },
	{ 3096,L"SynerTek Korea, Inc." },
	{ 3097,L"cyberPIXIE, Inc." },
	{ 3098,L"Silicon Motion, Inc." },
	{ 3099,L"MIPS Technologies" },
	{ 3100,L"Hang Zhou Silan Electronics Co., Ltd" },
	{ 3106,L"Tally Printer Corp." },
	{ 3107,L"Lernout + Hauspie" },
	{ 3108,L"Taiyo Yuden" },
	{ 3109,L"Sampo Corp." },
	{ 3110,L"Prolific Technology Inc." },
	{ 3111,L"RFIDeas, Inc" },
	{ 3118,L"Metrologic Instruments" },
	{ 3125,L"Eagletron, Inc." },
	{ 3126,L"E Ink Corp." },
	{ 3127,L"e.Digital" },
	{ 3128,L"Der An Electric Wire & Cable Co., Ltd" },
	{ 3129,L"IFR" },
	{ 3130,L"Furui Precise Component (Kunshan) Co., Ltd" },
	{ 3131,L"Komatsu, Ltd" },
	{ 3132,L"Radius Co., Ltd" },
	{ 3133,L"Innocom, Inc." },
	{ 3134,L"Nextcell, Inc." },
	{ 3140,L"Motorola iDEN" },
	{ 3141,L"Microdia" },
	{ 3142,L"WaveRider Communications, Inc." },
	{ 3146,L"ALGE-TIMING GmbH" },
	{ 3147,L"Reiner SCT Kartensysteme GmbH" },
	{ 3148,L"Needham's Electronics" },
	{ 3154,L"Sealevel Systems, Inc." },
	{ 3155,L"ViewPLUS, Inc." },
	{ 3156,L"Glory, Ltd" },
	{ 3157,L"Spectrum Digital, Inc." },
	{ 3158,L"Billion Bright, Ltd" },
	{ 3159,L"Imaginative Design Operation Co., Ltd" },
	{ 3160,L"Vidar Systems Corp." },
	{ 3161,L"Dong Guan Shinko Wire Co., Ltd" },
	{ 3162,L"TRS International Mfg., Inc." },
	{ 3166,L"Xytronix Research & Design" },
	{ 3168,L"Apogee Electronics Corp." },
	{ 3170,L"Chant Sincere Co., Ltd" },
	{ 3171,L"Toko, Inc." },
	{ 3172,L"Signality System Engineering Co., Ltd" },
	{ 3173,L"Eminence Enterprise Co., Ltd" },
	{ 3174,L"Rexon Electronics Corp." },
	{ 3175,L"Concept Telecom, Ltd" },
	{ 3178,L"ACS" },
	{ 3180,L"JETI Technische Instrumente GmbH" },
	{ 3184,L"MCT Elektronikladen" },
	{ 3186,L"PEAK System" },
	{ 3188,L"Optronic Laboratories Inc." },
	{ 3190,L"JMTek, LLC." },
	{ 3191,L"Sipix Group, Ltd" },
	{ 3192,L"Detto Corp." },
	{ 3193,L"NuConnex Technologies Pte., Ltd" },
	{ 3194,L"Wing-Span Enterprise Co., Ltd" },
	{ 3206,L"NDA Technologies, Inc." },
	{ 3208,L"Kyocera Wireless Corp." },
	{ 3209,L"Honda Tsushin Kogyo Co., Ltd" },
	{ 3210,L"Pathway Connectivity, Inc." },
	{ 3211,L"Wavefly Corp." },
	{ 3212,L"Coactive Networks" },
	{ 3213,L"Tempo" },
	{ 3214,L"Cesscom Co., Ltd" },
	{ 3215,L"Applied Microsystems" },
	{ 3220,L"Cryptera" },
	{ 3224,L"Berkshire Products, Inc." },
	{ 3225,L"Innochips Co., Ltd" },
	{ 3226,L"Hanwool Robotics Corp." },
	{ 3227,L"Jobin Yvon, Inc." },
	{ 3229,L"SemTek" },
	{ 3234,L"Zyfer" },
	{ 3235,L"Sega Corp." },
	{ 3236,L"ST&T Instrument Corp." },
	{ 3237,L"BAE Systems Canada, Inc." },
	{ 3238,L"Castles Technology Co., Ltd" },
	{ 3239,L"Information Systems Laboratories" },
	{ 3245,L"Motorola CGISS" },
	{ 3246,L"Ascom Business Systems, Ltd" },
	{ 3247,L"Buslink" },
	{ 3248,L"Flying Pig Systems" },
	{ 3249,L"Innovonics, Inc." },
	{ 3254,L"Celestix Networks, Pte., Ltd" },
	{ 3255,L"Singatron Enterprise Co., Ltd" },
	{ 3256,L"Opticis Co., Ltd" },
	{ 3258,L"Trust Electronic (Shanghai) Co., Ltd" },
	{ 3259,L"Shanghai Darong Electronics Co., Ltd" },
	{ 3260,L"Palmax Technology Co., Ltd" },
	{ 3261,L"Pentel Co., Ltd (Electronics Equipment Div.)" },
	{ 3262,L"Keryx Technologies, Inc." },
	{ 3263,L"Union Genius Computer Co., Ltd" },
	{ 3264,L"Kuon Yi Industrial Corp." },
	{ 3265,L"Given Imaging, Ltd" },
	{ 3266,L"Timex Corp." },
	{ 3267,L"Rimage Corp." },
	{ 3268,L"emsys GmbH" },
	{ 3269,L"Sendo" },
	{ 3270,L"Intermagic Corp." },
	{ 3272,L"Technotools Corp." },
	{ 3273,L"BroadMAX Technologies, Inc." },
	{ 3274,L"Amphenol" },
	{ 3275,L"SKNet Co., Ltd" },
	{ 3276,L"Domex Technology Corp." },
	{ 3277,L"TerraTec Electronic GmbH" },
	{ 3284,L"Bang Olufsen" },
	{ 3285,L"LabJack Corporation" },
	{ 3287,L"NewChip S.r.l." },
	{ 3288,L"JS Digitech, Inc." },
	{ 3289,L"Hitachi Shin Din Cable, Ltd" },
	{ 3294,L"Z-Com" },
	{ 3301,L"Validation Technologies International" },
	{ 3305,L"Pico Technology" },
	{ 3313,L"e-Conn Electronic Co., Ltd" },
	{ 3314,L"ENE Technology, Inc." },
	{ 3315,L"Qualcomm Atheros Communications" },
	{ 3316,L"Fomtex Corp." },
	{ 3317,L"Cellink Co., Ltd" },
	{ 3318,L"Compucable Corp." },
	{ 3319,L"ishoni Networks" },
	{ 3320,L"Clarisys, Inc." },
	{ 3321,L"Central System Research Co., Ltd" },
	{ 3322,L"Inviso, Inc." },
	{ 3324,L"Minolta-QMS, Inc." },
	{ 3327,L"SAFA MEDIA Co., Ltd." },
	{ 3334,L"telos EDV Systementwicklung GmbH" },
	{ 3336,L"UTStarcom" },
	{ 3339,L"Contemporary Controls" },
	{ 3340,L"Astron Electronics Co., Ltd" },
	{ 3341,L"MKNet Corp." },
	{ 3342,L"Hybrid Networks, Inc." },
	{ 3343,L"Feng Shin Cable Co., Ltd" },
	{ 3344,L"Elastic Networks" },
	{ 3345,L"Maspro Denkoh Corp." },
	{ 3346,L"Hansol Electronics, Inc." },
	{ 3347,L"BMF Corp." },
	{ 3348,L"Array Comm, Inc." },
	{ 3349,L"OnStream b.v." },
	{ 3350,L"Hi-Touch Imaging Technologies Co., Ltd" },
	{ 3351,L"NALTEC, Inc." },
	{ 3352,L"coaXmedia" },
	{ 3353,L"Hank Connection Industrial Co., Ltd" },
	{ 3368,L"NXP" },
	{ 3378,L"Leo Hui Electric Wire & Cable Co., Ltd" },
	{ 3379,L"AirSpeak, Inc." },
	{ 3380,L"Rearden Steel Technologies" },
	{ 3381,L"Dah Kun Co., Ltd" },
	{ 3386,L"Posiflex Technologies, Inc." },
	{ 3388,L"Sri Cable Technology, Ltd" },
	{ 3389,L"Tangtop Technology Co., Ltd" },
	{ 3390,L"Fitcom, inc." },
	{ 3391,L"MTS Systems Corp." },
	{ 3392,L"Ascor, Inc." },
	{ 3393,L"Ta Yun Terminals Industrial Co., Ltd" },
	{ 3394,L"Full Der Co., Ltd" },
	{ 3398,L"Kobil Systems GmbH" },
	{ 3400,L"Promethean Limited" },
	{ 3401,L"Maxtor" },
	{ 3402,L"NF Corp." },
	{ 3403,L"Grape Systems, Inc." },
	{ 3404,L"Tedas AG" },
	{ 3405,L"Coherent, Inc." },
	{ 3406,L"Agere Systems Netherland BV" },
	{ 3407,L"EADS Airbus France" },
	{ 3408,L"Cleware GmbH" },
	{ 3409,L"Volex (Asia) Pte., Ltd" },
	{ 3411,L"HMI Co., Ltd" },
	{ 3412,L"Holon Corp." },
	{ 3413,L"ASKA Technologies, Inc." },
	{ 3414,L"AVLAB Technology, Inc." },
	{ 3415,L"Solomon Microtech, Ltd" },
	{ 3420,L"SMC Networks, Inc." },
	{ 3422,L"Myacom, Ltd" },
	{ 3423,L"CSI, Inc." },
	{ 3424,L"IVL Technologies, Ltd" },
	{ 3425,L"Meilu Electronics (Shenzhen) Co., Ltd" },
	{ 3426,L"Darfon Electronics Corp." },
	{ 3427,L"Fritz Gegauf AG" },
	{ 3428,L"DXG Technology Corp." },
	{ 3429,L"KMJP Co., Ltd" },
	{ 3430,L"TMT" },
	{ 3431,L"Advanet, Inc." },
	{ 3432,L"Super Link Electronics Co., Ltd" },
	{ 3433,L"NSI" },
	{ 3434,L"Megapower International Corp." },
	{ 3435,L"And-Or Logic" },
	{ 3440,L"Try Computer Co., Ltd" },
	{ 3441,L"Hirakawa Hewtech Corp." },
	{ 3442,L"Winmate Communication, Inc." },
	{ 3443,L"Hit's Communications, Inc." },
	{ 3446,L"MFP Korea, Inc." },
	{ 3447,L"Power Sentry/Newpoint" },
	{ 3448,L"Japan Distributor Corp." },
	{ 3450,L"MARX Datentechnik GmbH" },
	{ 3451,L"Wellco Technology Co., Ltd" },
	{ 3452,L"Taiwan Line Tek Electronic Co., Ltd" },
	{ 3453,L"Phison Electronics Corp." },
	{ 3454,L"American Computer & Digital Components" },
	{ 3455,L"Essential Reality LLC" },
	{ 3456,L"H.R. Silvine Electronics, Inc." },
	{ 3457,L"TechnoVision" },
	{ 3459,L"Think Outside, Inc." },
	{ 3463,L"Dolby Laboratories Inc." },
	{ 3465,L"Oz Software" },
	{ 3466,L"King Jim Co., Ltd" },
	{ 3467,L"Ascom Telecommunications, Ltd" },
	{ 3468,L"C-Media Electronics, Inc." },
	{ 3469,L"Promotion & Display Technology, Ltd" },
	{ 3470,L"Global Sun Technology, Inc." },
	{ 3471,L"Pitney Bowes" },
	{ 3472,L"Sure-Fire Electrical Corp." },
	{ 3478,L"Skanhex Technology, Inc." },
	{ 3479,L"Santa Barbara Instrument Group" },
	{ 3480,L"Mars Semiconductor Corp." },
	{ 3481,L"Trazer Technologies, Inc." },
	{ 3482,L"RTX Telecom AS" },
	{ 3483,L"Tat Shing Electrical Co." },
	{ 3484,L"Chee Chen Hi-Technology Co., Ltd" },
	{ 3485,L"Sanwa Supply, Inc." },
	{ 3486,L"Avaya" },
	{ 3487,L"Powercom Co., Ltd" },
	{ 3488,L"Danger Research" },
	{ 3489,L"Suzhou Peter's Precise Industrial Co., Ltd" },
	{ 3490,L"Land Instruments International, Ltd" },
	{ 3491,L"Nippon Electro-Sensory Devices Corp." },
	{ 3492,L"Polar Electro Oy" },
	{ 3495,L"IOGear, Inc." },
	{ 3496,L"softDSP Co., Ltd" },
	{ 3499,L"Cubig Group" },
	{ 3501,L"Westover Scientific" },
	{ 3504,L"Micro Star International" },
	{ 3505,L"Wen Te Electronics Co., Ltd" },
	{ 3506,L"Shian Hwi Plug Parts, Plastic Factory" },
	{ 3507,L"Tekram Technology Co., Ltd" },
	{ 3508,L"Chung Fu Chen Yeh Enterprise Corp." },
	{ 3509,L"Access IS" },
	{ 3511,L"ELCON Systemtechnik" },
	{ 3514,L"Digidesign" },
	{ 3516,L"A&D Medical" },
	{ 3518,L"Jiuh Shiuh Precision Industry Co., Ltd" },
	{ 3519,L"Jess-Link International" },
	{ 3520,L"G7 Solutions (formerly Great Notions)" },
	{ 3521,L"Tamagawa Seiki Co., Ltd" },
	{ 3523,L"Athena Smartcard Solutions, Inc." },
	{ 3524,L"inXtron, Inc." },
	{ 3525,L"SDK Co., Ltd" },
	{ 3526,L"Precision Squared Technology Corp." },
	{ 3527,L"First Cable Line, Inc." },
	{ 3533,L"NetworkFab Corp." },
	{ 3536,L"Access Solutions" },
	{ 3537,L"Contek Electronics Co., Ltd" },
	{ 3538,L"Power Quotient International Co., Ltd" },
	{ 3539,L"MediaQ" },
	{ 3540,L"Custom Engineering SPA" },
	{ 3541,L"California Micro Devices" },
	{ 3543,L"Kocom Co., Ltd" },
	{ 3544,L"Netac Technology Co., Ltd" },
	{ 3545,L"HighSpeed Surfing" },
	{ 3546,L"Integrated Circuit Solution, Inc." },
	{ 3547,L"Tamarack, Inc." },
	{ 3549,L"Datelink Technology Co., Ltd" },
	{ 3550,L"Ubicom, Inc." },
	{ 3552,L"BD Consumer Healthcare" },
	{ 3559,L"USBmicro" },
	{ 3562,L"UTECH Electronic (D.G.) Co., Ltd." },
	{ 3565,L"Novasonics" },
	{ 3566,L"Lifetime Memory Products" },
	{ 3567,L"Full Rise Electronic Co., Ltd" },
	{ 3572,L"NET&SYS" },
	{ 3574,L"Sitecom Europe B.V." },
	{ 3575,L"Mobile Action Technology, Inc." },
	{ 3578,L"Toyo Communication Equipment Co., Ltd" },
	{ 3580,L"GeneralTouch Technology Co., Ltd" },
	{ 3587,L"Nippon Systemware Co., Ltd" },
	{ 3592,L"Winbest Technology Co., Ltd" },
	{ 3595,L"Amigo Technology Inc." },
	{ 3596,L"Gesytec" },
	{ 3597,L"PicoQuant GmbH" },
	{ 3599,L"VMware, Inc." },
	{ 3606,L"JMTek, LLC" },
	{ 3607,L"Walex Electronic, Ltd" },
	{ 3610,L"Unisys" },
	{ 3611,L"Crewave" },
	{ 3616,L"Pegasus Technologies Ltd." },
	{ 3617,L"Cowon Systems, Inc." },
	{ 3618,L"Symbian Ltd." },
	{ 3619,L"Liou Yuane Enterprise Co., Ltd" },
	{ 3621,L"VinChip Systems, Inc." },
	{ 3622,L"J-Phone East Co., Ltd" },
	{ 3632,L"HeartMath LLC" },
	{ 3636,L"Micro Computer Control Corp." },
	{ 3637,L"3Pea Technologies, Inc." },
	{ 3638,L"TiePie engineering" },
	{ 3640,L"Stratitec, Inc." },
	{ 3641,L"Smart Modular Technologies, Inc." },
	{ 3642,L"Neostar Technology Co., Ltd" },
	{ 3643,L"Mansella, Ltd" },
	{ 3649,L"Line6, Inc." },
	{ 3652,L"Sun-Riseful Technology Co., Ltd." },
	{ 3656,L"Julia Corp., Ltd" },
	{ 3658,L"Shenzhen Bao Hing Electric Wire & Cable Mfr. Co." },
	{ 3660,L"Radica Games, Ltd" },
	{ 3664,L"TechnoData Interware" },
	{ 3669,L"Speed Dragon Multimedia, Ltd" },
	{ 3670,L"Kingston Technology Company, Inc." },
	{ 3674,L"Active Co., Ltd" },
	{ 3675,L"Union Power Information Industrial Co., Ltd" },
	{ 3676,L"Bitland Information Technology Co., Ltd" },
	{ 3677,L"Neltron Industrial Co., Ltd" },
	{ 3678,L"Conwise Technology Co., Ltd." },
	{ 3686,L"Hawking Technologies" },
	{ 3687,L"Fossil, Inc." },
	{ 3690,L"Megawin Technology Co., Ltd" },
	{ 3695,L"Logic3" },
	{ 3696,L"Tokyo Electronic Industry Co., Ltd" },
	{ 3698,L"Hsi-Chin Electronics Co., Ltd" },
	{ 3701,L"TVS Electronics, Ltd" },
	{ 3705,L"Archos, Inc." },
	{ 3707,L"On-Tech Industry Co., Ltd" },
	{ 3710,L"Gmate, Inc." },
	{ 3714,L"Ching Tai Electric Wire & Cable Co., Ltd" },
	{ 3715,L"Shin An Wire & Cable Co." },
	{ 3724,L"Well Force Electronic Co., Ltd" },
	{ 3725,L"MediaTek Inc." },
	{ 3727,L"GreenAsia Inc." },
	{ 3728,L"WiebeTech, LLC" },
	{ 3729,L"VTech Engineering Canada, Ltd" },
	{ 3730,L"C's Glory Enterprise Co., Ltd" },
	{ 3731,L"eM Technics Co., Ltd" },
	{ 3733,L"Future Technology Co., Ltd" },
	{ 3734,L"Aplux Communications, Ltd" },
	{ 3735,L"Fingerworks, Inc." },
	{ 3736,L"Advanced Analogic Technologies, Inc." },
	{ 3737,L"Parallel Dice Co., Ltd" },
	{ 3738,L"TA HSING Industries, Ltd" },
	{ 3739,L"ADTEC Corp." },
	{ 3740,L"Streamzap, Inc." },
	{ 3743,L"Tamura Corp." },
	{ 3744,L"Ours Technology, Inc." },
	{ 3750,L"Nihon Computer Co., Ltd" },
	{ 3751,L"MSL Enterprises Corp." },
	{ 3752,L"CenDyne, Inc." },
	{ 3757,L"Humax Co., Ltd" },
	{ 3760,L"NovaTech" },
	{ 3761,L"WIS Technologies, Inc." },
	{ 3762,L"Y-S Electronic Co., Ltd" },
	{ 3763,L"Saint Technology Corp." },
	{ 3767,L"Endor AG" },
	{ 3768,L"Mettler Toledo" },
	{ 3771,L"Thermo Fisher Scientific" },
	{ 3774,L"VWeb Corp." },
	{ 3775,L"Omega Technology of Taiwan, Inc." },
	{ 3776,L"LHI Technology (China) Co., Ltd" },
	{ 3777,L"Abit Computer Corp." },
	{ 3778,L"Sweetray Industrial, Ltd" },
	{ 3779,L"Axell Co., Ltd" },
	{ 3780,L"Ballracing Developments, Ltd" },
	{ 3781,L"GT Information System Co., Ltd" },
	{ 3782,L"InnoVISION Multimedia, Ltd" },
	{ 3783,L"Theta Link Corp." },
	{ 3789,L"Lite-On IT Corp." },
	{ 3790,L"TaiSol Electronics Co., Ltd" },
	{ 3791,L"Phogenix Imaging, LLC" },
	{ 3793,L"WinMaxGroup" },
	{ 3794,L"Kyoto Micro Computer Co., Ltd" },
	{ 3795,L"Wing-Tech Enterprise Co., Ltd" },
	{ 3797,L"Fiberbyte" },
	{ 3802,L"Noriake Itron Corp." },
	{ 3807,L"e-MDT Co., Ltd" },
	{ 3808,L"Shima Seiki Mfg., Ltd" },
	{ 3809,L"Sarotech Co., Ltd" },
	{ 3810,L"AMI Semiconductor, Inc." },
	{ 3811,L"ComTrue Technology Corp." },
	{ 3812,L"Sunrich Technology, Ltd" },
	{ 3822,L"Digital Stream Technology, Inc." },
	{ 3823,L"D-WAV Scientific Co., Ltd" },
	{ 3824,L"Hitachi Cable, Ltd" },
	{ 3825,L"Aichi Micro Intelligent Corp." },
	{ 3826,L"I/O Magic Corp." },
	{ 3827,L"Lynn Products, Inc." },
	{ 3828,L"DSI Datotech" },
	{ 3829,L"PointChips" },
	{ 3830,L"Yield Microelectronics Corp." },
	{ 3831,L"SM Tech Co., Ltd (Tulip)" },
	{ 3837,L"Oasis Semiconductor" },
	{ 3838,L"Wem Technology, Inc." },
	{ 3843,L"Unitek UPS Systems" },
	{ 3846,L"Visual Frontier Enterprise Co., Ltd" },
	{ 3848,L"CSL Wire & Plug (Shen Zhen) Co." },
	{ 3852,L"CAS Corp." },
	{ 3853,L"Hori Co., Ltd" },
	{ 3854,L"Energy Full Corp." },
	{ 3855,L"Silego Technology Inc" },
	{ 3857,L"LD Didactic GmbH" },
	{ 3858,L"Mars Engineering Corp." },
	{ 3859,L"Acetek Technology Co., Ltd" },
	{ 3860,L"Ingenico" },
	{ 3864,L"Finger Lakes Instrumentation" },
	{ 3865,L"Oracom Co., Ltd" },
	{ 3867,L"Onset Computer Corp." },
	{ 3868,L"Funai Electric Co., Ltd" },
	{ 3869,L"Iwill Corp." },
	{ 3873,L"IOI Technology Corp." },
	{ 3874,L"Senior Industries, Inc." },
	{ 3875,L"Leader Tech Manufacturer Co., Ltd" },
	{ 3876,L"Flex-P Industries, Snd., Bhd." },
	{ 3885,L"ViPower, Inc." },
	{ 3886,L"Geniality Maple Technology Co., Ltd" },
	{ 3887,L"Priva Design Services" },
	{ 3888,L"Jess Technology Co., Ltd" },
	{ 3889,L"Chrysalis Development" },
	{ 3890,L"YFC-BonEagle Electric Co., Ltd" },
	{ 3895,L"Kokuyo Co., Ltd" },
	{ 3896,L"Nien-Yi Industrial Corp." },
	{ 3897,L"TG3 Electronics" },
	{ 3901,L"Airprime, Incorporated" },
	{ 3905,L"RDC Semiconductor Co., Ltd" },
	{ 3906,L"Nital Consulting Services, Inc." },
	{ 3908,L"Polhemus" },
	{ 3915,L"St. John Technology Co., Ltd" },
	{ 3916,L"WorldWide Cable Opto Corp." },
	{ 3917,L"Microtune, Inc." },
	{ 3918,L"Freedom Scientific" },
	{ 3922,L"Wing Key Electrical Co., Ltd" },
	{ 3923,L"Dongguan White Horse Cable Factory, Ltd" },
	{ 3924,L"Kawai Musical Instruments Mfg. Co., Ltd" },
	{ 3925,L"AmbiCom, Inc." },
	{ 3932,L"Prairiecomm, Inc." },
	{ 3933,L"NewAge International, LLC" },
	{ 3935,L"Key Technology Corp." },
	{ 3936,L"NTK, Ltd" },
	{ 3937,L"Varian, Inc." },
	{ 3938,L"Acrox Technologies Co., Ltd" },
	{ 3939,L"LeapFrog Enterprises" },
	{ 3944,L"Kobe Steel, Ltd" },
	{ 3945,L"Dionex Corp." },
	{ 3946,L"Vibren Technologies, Inc." },
	{ 3950,L"INTELLIGENT SYSTEMS" },
	{ 3955,L"DFI" },
	{ 3960,L"Guntermann & Drunck GmbH" },
	{ 3964,L"DQ Technology, Inc." },
	{ 3965,L"NetBotz, Inc." },
	{ 3966,L"Fluke Corp." },
	{ 3976,L"VTech Holdings, Ltd" },
	{ 3979,L"Yazaki Corp." },
	{ 3980,L"Young Generation International Corp." },
	{ 3981,L"Uniwill Computer Corp." },
	{ 3982,L"Kingnet Technology Co., Ltd" },
	{ 3983,L"Soma Networks" },
	{ 3991,L"CviLux Corp." },
	{ 3992,L"CyberBank Corp." },
	{ 3996,L"Hyun Won, Inc." },
	{ 3998,L"Lucent Technologies" },
	{ 4003,L"Starconn Electronic Co., Ltd" },
	{ 4004,L"ATL Technology" },
	{ 4005,L"Sotec Co., Ltd" },
	{ 4007,L"Epox Computer Co., Ltd" },
	{ 4008,L"Logic Controls, Inc." },
	{ 4015,L"Winpoint Electronic Corp." },
	{ 4016,L"Haurtian Wire & Cable Co., Ltd" },
	{ 4017,L"Inclose Design, Inc." },
	{ 4018,L"Juan-Chern Industrial Co., Ltd" },
	{ 4022,L"Heber Ltd" },
	{ 4024,L"Wistron Corp." },
	{ 4025,L"AACom Corp." },
	{ 4026,L"San Shing Electronics Co., Ltd" },
	{ 4027,L"Bitwise Systems, Inc." },
	{ 4033,L"Mitac Internatinal Corp." },
	{ 4034,L"Plug and Jack Industrial, Inc." },
	{ 4037,L"Delcom Engineering" },
	{ 4038,L"Dataplus Supplies, Inc." },
	{ 4042,L"Research In Motion, Ltd." },
	{ 4046,L"Sony Ericsson Mobile Communications AB" },
	{ 4047,L"Dynastream Innovations, Inc." },
	{ 4048,L"Tulip Computers B.V." },
	{ 4049,L"Giant Electronics Ltd." },
	{ 4050,L"Seac Banche" },
	{ 4052,L"Tenovis GmbH & Co., KG" },
	{ 4053,L"Direct Access Technology, Inc." },
	{ 4057,L"Elgato Systems GmbH" },
	{ 4058,L"Quantec Networks GmbH" },
	{ 4060,L"Micro Plus" },
	{ 4062,L"Oregon Scientific" },
	{ 4064,L"Osterhout Design Group" },
	{ 4066,L"Air Techniques" },
	{ 4068,L"IN-Tech Electronics, Ltd" },
	{ 4069,L"Greenconn (U.S.A.), Inc." },
	{ 4070,L"ICS Advent" },
	{ 4073,L"DVICO" },
	{ 4074,L"United Computer Accessories" },
	{ 4075,L"CRS Electronic Co., Ltd" },
	{ 4076,L"UMC Electronics Co., Ltd" },
	{ 4077,L"Access Co., Ltd" },
	{ 4078,L"Xsido Corp." },
	{ 4079,L"MJ Research, Inc." },
	{ 4086,L"Core Valley Co., Ltd" },
	{ 4087,L"CHI SHING Computer Accessories Co., Ltd" },
	{ 4092,L"Clavia DMI AB" },
	{ 4093,L"EarlySense" },
	{ 4095,L"Aopen, Inc." },
	{ 4096,L"Speed Tech Corp." },
	{ 4097,L"Ritronics Components (S) Pte., Ltd" },
	{ 4099,L"Sigma Corp." },
	{ 4100,L"LG Electronics, Inc." },
	{ 4101,L"Apacer Technology, Inc." },
	{ 4102,L"iRiver, Ltd." },
	{ 4105,L"Emuzed, Inc." },
	{ 4106,L"AV Chaseway, Ltd" },
	{ 4107,L"Chou Chin Industrial Co., Ltd" },
	{ 4109,L"Netopia, Inc." },
	{ 4112,L"Fukuda Denshi Co., Ltd" },
	{ 4113,L"Mobile Media Tech." },
	{ 4114,L"SDKM Fibres, Wires & Cables Berhad" },
	{ 4115,L"TST-Touchless Sensor Technology AG" },
	{ 4116,L"Densitron Technologies PLC" },
	{ 4117,L"Softronics Pty., Ltd" },
	{ 4118,L"Xiamen Hung's Enterprise Co., Ltd" },
	{ 4119,L"Speedy Industrial Supplies, Pte., Ltd" },
	{ 4121,L"Elitegroup Computer Systems (ECS)" },
	{ 4128,L"Labtec" },
	{ 4130,L"Shinko Shoji Co., Ltd" },
	{ 4133,L"Hyper-Paltek" },
	{ 4134,L"Newly Corp." },
	{ 4135,L"Time Domain" },
	{ 4136,L"Inovys Corp." },
	{ 4137,L"Atlantic Coast Telesys" },
	{ 4138,L"Ramos Technology Co., Ltd" },
	{ 4139,L"Infotronic America, Inc." },
	{ 4140,L"Etoms Electronics Corp." },
	{ 4141,L"Winic Corp." },
	{ 4145,L"Comax Technology, Inc." },
	{ 4146,L"C-One Technology Corp." },
	{ 4147,L"Nucam Corp." },
	{ 4152,L"SteelSeries ApS" },
	{ 4153,L"devolo AG" },
	{ 4154,L"PSA" },
	{ 4157,L"Stanton" },
	{ 4163,L"iCreate Technologies Corp." },
	{ 4164,L"Chu Yuen Enterprise Co., Ltd" },
	{ 4166,L"Winbond Electronics Corp. [hex]" },
	{ 4168,L"Targus Group International" },
	{ 4171,L"Mylex / Buslogic" },
	{ 4172,L"AMCO TEC International, Inc." },
	{ 4173,L"Newport Corporation" },
	{ 4175,L"WB Electronics" },
	{ 4176,L"Yubico.com" },
	{ 4179,L"Immanuel Electronics Co., Ltd" },
	{ 4180,L"BMS International Beheer N.V." },
	{ 4181,L"Complex Micro Interconnection Co., Ltd" },
	{ 4182,L"Hsin Chen Ent Co., Ltd" },
	{ 4183,L"ON Semiconductor" },
	{ 4184,L"Western Digital Technologies, Inc." },
	{ 4185,L"Giesecke & Devrient GmbH" },
	{ 4187,L"Foxconn International, Inc." },
	{ 4188,L"Hong Ji Electric Wire & Cable (Dongguan) Co., Ltd" },
	{ 4189,L"Delkin Devices, Inc." },
	{ 4190,L"Valence Semiconductor Design, Ltd" },
	{ 4191,L"Chin Shong Enterprise Co., Ltd" },
	{ 4192,L"Easthome Industrial Co., Ltd" },
	{ 4195,L"Motorola Electronics Taiwan, Ltd [hex]" },
	{ 4197,L"CCYU Technology" },
	{ 4202,L"Loyal Legend, Ltd" },
	{ 4204,L"Curitel Communications, Inc." },
	{ 4205,L"San Chieh Manufacturing, Ltd" },
	{ 4206,L"ConectL" },
	{ 4207,L"Money Controls" },
	{ 4214,L"GCT Semiconductor, Inc." },
	{ 4219,L"Gateway, Inc." },
	{ 4221,L"Arlec Australia, Ltd" },
	{ 4222,L"Midoriya Electric Co., Ltd" },
	{ 4223,L"KidzMouse, Inc." },
	{ 4226,L"Shin-Etsukaken Co., Ltd" },
	{ 4227,L"Canon Electronics, Inc." },
	{ 4228,L"Pantech Co., Ltd" },
	{ 4234,L"Chloride Power Protection" },
	{ 4235,L"Grand-tek Technology Co., Ltd" },
	{ 4236,L"Robert Bosch GmbH" },
	{ 4238,L"Lotes Co., Ltd." },
	{ 4249,L"Surface Optics Corp." },
	{ 4250,L"DATASOFT Systems GmbH" },
	{ 4251,L"Hisense" },
	{ 4255,L"eSOL Co., Ltd" },
	{ 4256,L"Hirotech, Inc." },
	{ 4259,L"Mitsubishi Materials Corp." },
	{ 4265,L"SK Teletech Co., Ltd" },
	{ 4266,L"Cables To Go" },
	{ 4267,L"USI Co., Ltd" },
	{ 4268,L"Honeywell, Inc." },
	{ 4270,L"Princeton Technology Corp." },
	{ 4271,L"Liebert Corp." },
	{ 4277,L"Comodo (PLX?)" },
	{ 4280,L"DiBcom" },
	{ 4283,L"TM Technology, Inc." },
	{ 4284,L"Dinging Technology Co., Ltd" },
	{ 4285,L"TMT Technology, Inc." },
	{ 4287,L"SmartHome" },
	{ 4291,L"Universal Laser Systems, Inc." },
	{ 4292,L"Cygnal Integrated Products, Inc." },
	{ 4293,L"Sanei Electric, Inc." },
	{ 4294,L"Intec, Inc." },
	{ 4299,L"Eratech" },
	{ 4300,L"GBM Connector Co., Ltd" },
	{ 4301,L"Kycon, Inc." },
	{ 4302,L"Silicon Labs" },
	{ 4303,L"Velleman Components, Inc." },
	{ 4305,L"Hottinger Baldwin Measurement" },
	{ 4306,L"RayComposer - R. Adams" },
	{ 4308,L"Man Boon Manufactory, Ltd" },
	{ 4309,L"Uni Class Technology Co., Ltd" },
	{ 4310,L"Actions Semiconductor Co., Ltd" },
	{ 4318,L"Authenex, Inc." },
	{ 4319,L"In-Win Development, Inc." },
	{ 4320,L"Post-Op Video, Inc." },
	{ 4321,L"CablePlus, Ltd" },
	{ 4322,L"Nada Electronics, Ltd" },
	{ 4332,L"Vast Technologies, Inc." },
	{ 4336,L"Nexio Co., Ltd" },
	{ 4337,L"Importek" },
	{ 4341,L"Turtle Beach" },
	{ 4347,L"Pictos Technologies, Inc." },
	{ 4349,L"Anubis Electronics, Ltd" },
	{ 4350,L"Thrane & Thrane" },
	{ 4352,L"VirTouch, Ltd" },
	{ 4353,L"EasyPass Industrial Co., Ltd" },
	{ 4360,L"Brightcom Technologies, Ltd" },
	{ 4362,L"Moxa Technologies Co., Ltd." },
	{ 4368,L"Analog Devices Canada, Ltd (Allied Telesyn)" },
	{ 4369,L"Pandora International Ltd." },
	{ 4370,L"YM ELECTRIC CO., Ltd" },
	{ 4371,L"Medion AG" },
	{ 4382,L"VSO Electric Co., Ltd" },
	{ 4394,L"RedRat" },
	{ 4398,L"Master Hill Electric Wire and Cable Co., Ltd" },
	{ 4399,L"Cellon International, Inc." },
	{ 4400,L"Tenx Technology, Inc." },
	{ 4401,L"Integrated System Solution Corp." },
	{ 4402,L"Toshiba Corp., Digital Media Equipment [hex]" },
	{ 4406,L"CTS Electronincs" },
	{ 4412,L"Arin Tech Co., Ltd" },
	{ 4413,L"Mapower Electronics Co., Ltd" },
	{ 4417,L"V One Multimedia, Pte., Ltd" },
	{ 4418,L"CyberScan Technologies, Inc." },
	{ 4421,L"Japan Radio Company" },
	{ 4422,L"Shimane SANYO Electric Co., Ltd." },
	{ 4423,L"Ever Great Electric Wire and Cable Co., Ltd" },
	{ 4427,L"Sphairon Access Systems GmbH" },
	{ 4428,L"Tinius Olsen Testing Machine Co., Inc." },
	{ 4429,L"Alpha Imaging Technology Corp." },
	{ 4431,L"Wavecom" },
	{ 4443,L"Salix Technology Co., Ltd." },
	{ 4450,L"Secugen Corp." },
	{ 4451,L"DeLorme Publishing, Inc." },
	{ 4452,L"YUAN High-Tech Development Co., Ltd" },
	{ 4453,L"Telson Electronics Co., Ltd" },
	{ 4454,L"Bantam Interactive Technologies" },
	{ 4455,L"Salient Systems Corp." },
	{ 4456,L"BizConn International Corp." },
	{ 4462,L"Gigastorage Corp." },
	{ 4463,L"Silicon 10 Technology Corp." },
	{ 4469,L"Shengyih Steel Mold Co., Ltd" },
	{ 4477,L"Santa Electronic, Inc." },
	{ 4478,L"JNC, Inc." },
	{ 4482,L"Venture Corp., Ltd" },
	{ 4483,L"Compaq Computer Corp. [hex] (Digital Dream ??)" },
	{ 4484,L"Kyocera Elco Corp." },
	{ 4488,L"Bloomberg L.P." },
	{ 4489,L"Acer Communications & Multimedia" },
	{ 4495,L"You Yang Technology Co., Ltd" },
	{ 4496,L"Tripace" },
	{ 4497,L"Loyalty Founder Enterprise Co., Ltd" },
	{ 4502,L"Yankee Robotics, LLC" },
	{ 4503,L"Technoimagia Co., Ltd" },
	{ 4504,L"StarShine Technology Corp." },
	{ 4505,L"Sierra Wireless, Inc." },
	{ 4506,L"ZHAN QI Technology Co., Ltd" },
	{ 4507,L"ruwido austria GmbH" },
	{ 4512,L"Chipcon AS" },
	{ 4515,L"Technovas Co., Ltd" },
	{ 4522,L"GlobalMedia Group, LLC" },
	{ 4523,L"Exito Electronics Co., Ltd" },
	{ 4524,L"Nike" },
	{ 4528,L"ATECH FLASH TECHNOLOGY" },
	{ 4542,L"R&D International NV" },
	{ 4549,L"Inmax" },
	{ 4554,L"VeriFone Inc" },
	{ 4571,L"Topfield Co., Ltd." },
	{ 4582,L"K.I. Technology Co. Ltd." },
	{ 4597,L"Siemens AG" },
	{ 4598,L"Prolific" },
	{ 4599,L"Alcatel (?)" },
	{ 4611,L"TSC Auto ID Technology Co., Ltd" },
	{ 4617,L"InterBiometrics" },
	{ 4622,L"Hudson Soft Co., Ltd" },
	{ 4623,L"Magellan" },
	{ 4624,L"DigiTech" },
	{ 4638,L"Jungsoft Co., Ltd" },
	{ 4641,L"Unknown manufacturer" },
	{ 4643,L"SKYCABLE ENTERPRISE. CO., LTD." },
	{ 4648,L"Datapaq Limited" },
	{ 4656,L"Chipidea-Microelectronica, S.A." },
	{ 4659,L"Denver Electronics" },
	{ 4660,L"Brain Actuated Technologies" },
	{ 4661,L"Focusrite-Novation" },
	{ 4673,L"Belkin" },
	{ 4682,L"AirVast" },
	{ 4683,L"Nyko (Honey Bee)" },
	{ 4684,L"MXI - Memory Experts International, Inc." },
	{ 4700,L"Apogee Inc." },
	{ 4703,L"A-DATA Technology Co., Ltd." },
	{ 4704,L"Standard Microsystems Corp." },
	{ 4708,L"Covidien Energy-based Devices" },
	{ 4710,L"Pirelli Broadband Solutions" },
	{ 4711,L"Logic3 / SpectraVideo plc" },
	{ 4716,L"Aristocrat Technologies" },
	{ 4717,L"Bel Stewart" },
	{ 4718,L"Strobe Data, Inc." },
	{ 4719,L"TwinMOS" },
	{ 4724,L"Ensoniq" },
	{ 4725,L"Xaxero Marine Software Engineering, Ltd." },
	{ 4728,L"Starlight Xpress" },
	{ 4739,L"zebris Medical GmbH" },
	{ 4742,L"Marvell Semiconductor, Inc." },
	{ 4753,L"Qualcomm Flarion Technologies, Inc. / Leadtek Research, Inc." },
	{ 4754,L"Innomedia" },
	{ 4755,L"Belkin Components [hex]" },
	{ 4756,L"RISO KAGAKU CORP." },
	{ 4763,L"CyberTAN Technology" },
	{ 4775,L"Trendchip Technologies Corp." },
	{ 4779,L"Honey Bee Electronic International Ltd." },
	{ 4792,L"Zhejiang Xinya Electronic Technology Co., Ltd." },
	{ 4793,L"E28" },
	{ 4794,L"Licensed by Sony Computer Entertainment America" },
	{ 4797,L"Gembird" },
	{ 4804,L"Autocue Group Ltd" },
	{ 4815,L"DEXIN" },
	{ 4817,L"Huawei Technologies Co., Ltd." },
	{ 4818,L"LINE TECH INDUSTRIAL CO., LTD." },
	{ 4822,L"EMS Dr. Thomas Wuensche" },
	{ 4823,L"BETTER WIRE FACTORY CO., LTD." },
	{ 4824,L"Araneus Information Systems Oy" },
	{ 4838,L"Waldorf Music GmbH" },
	{ 4847,L"Tapwave, Inc." },
	{ 4853,L"Dynamic System Electronics Corp." },
	{ 4855,L"Memorex Products, Inc." },
	{ 4861,L"AIN Comm. Technology Co., Ltd" },
	{ 4863,L"Fascinating Electronics, Inc." },
	{ 4871,L"Transcend Information, Inc." },
	{ 4872,L"Shuttle, Inc." },
	{ 4880,L"Roper" },
	{ 4882,L"ICS Electronics" },
	{ 4883,L"ThorLabs" },
	{ 4893,L"Natural Point" },
	{ 4906,L"Envara Inc." },
	{ 4907,L"Konica Minolta" },
	{ 4926,L"Kemper Digital GmbH" },
	{ 4930,L"Mobility" },
	{ 4931,L"Citizen Systems" },
	{ 4933,L"Sino Lite Technology Corp." },
	{ 4935,L"Moravian Instruments" },
	{ 4936,L"Katsuragawa Electric Co., Ltd." },
	{ 4940,L"PanJit International Inc." },
	{ 4942,L"Digby's Bitpile, Inc. DBA D Bit" },
	{ 4951,L"P&E Microcomputer Systems" },
	{ 4959,L"Control Development Inc." },
	{ 4966,L"SEGGER" },
	{ 4971,L"STEC" },
	{ 4974,L"Andor Technology Ltd." },
	{ 4976,L"Swissbit" },
	{ 4977,L"CNet Technology Inc." },
	{ 4982,L"Vimtron Electronics Co., Ltd." },
	{ 4987,L"SCAPS GmbH" },
	{ 4997,L"Netgear, Inc" },
	{ 5002,L"Validity Sensors, Inc." },
	{ 5006,L"Jungo LTD" },
	{ 5008,L"TOMTOM B.V." },
	{ 5009,L"IdealTEK, Inc." },
	{ 5013,L"Sennheiser Communications" },
	{ 5015,L"BEHRINGER International GmbH" },
	{ 5016,L"Q-tec" },
	{ 5037,L"Baltech" },
	{ 5040,L"PerkinElmer Optoelectronics" },
	{ 5041,L"Linksys" },
	{ 5042,L"Alesis" },
	{ 5043,L"Nippon Dics Co., Ltd." },
	{ 5050,L"PCPlay" },
	{ 5054,L"Ricoh Printing Systems, Ltd." },
	{ 5066,L"JyeTai Precision Industrial Co., Ltd." },
	{ 5071,L"Wisair Ltd." },
	{ 5072,L"Techsan Electronics Co., Ltd." },
	{ 5073,L"A-Max Technology Macao Commercial Offshore Co. Ltd." },
	{ 5074,L"Shark Multimedia" },
	{ 5075,L"IMC Networks" },
	{ 5079,L"Guidance Software, Inc." },
	{ 5084,L"ALEREON, INC." },
	{ 5085,L"i.Tech Dynamic Limited" },
	{ 5089,L"Kaibo Wire & Cable (Shenzhen) Co., Ltd." },
	{ 5093,L"Rane" },
	{ 5094,L"TechnoScope Co., Ltd." },
	{ 5098,L"Hengstler" },
	{ 5100,L"Zydacron" },
	{ 5102,L"MosArt" },
	{ 5117,L"Initio Corporation" },
	{ 5118,L"Kingston Technology Company Inc." },
	{ 5120,L"Axxion Group Corp." },
	{ 5122,L"Bowe Bell & Howell" },
	{ 5123,L"Sitronix" },
	{ 5129,L"IDS Imaging Development Systems GmbH" },
	{ 5134,L"Telechips, Inc." },
	{ 5136,L"Novatel Wireless" },
	{ 5141,L"Nam Tai E&E Products Ltd. or OmniVision Technologies, Inc." },
	{ 5145,L"ABILITY ENTERPRISE CO., LTD." },
	{ 5153,L"Sensor Technology" },
	{ 5161,L"Vega Technologies Industrial (Austria) Co." },
	{ 5162,L"Thales E-Transactions" },
	{ 5163,L"Arbiter Systems, Inc." },
	{ 5168,L"RedOctane" },
	{ 5169,L"Pertech Resources, Inc." },
	{ 5173,L"Wistron NeWeb" },
	{ 5174,L"Denali Software, Inc." },
	{ 5180,L"Altek Corporation" },
	{ 5187,L"Digilent" },
	{ 5190,L"X.J.GROUP" },
	{ 5203,L"Radio Shack" },
	{ 5206,L"Extending Wire & Cable Co., Ltd." },
	{ 5207,L"First International Computer, Inc." },
	{ 5215,L"Trust" },
	{ 5216,L"Tatung Co." },
	{ 5217,L"Staccato Communications" },
	{ 5218,L"Micro Star International" },
	{ 5234,L"Huawei-3Com" },
	{ 5242,L"Formosa Industrial Computing, Inc." },
	{ 5246,L"Upek" },
	{ 5247,L"Hama GmbH & Co., KG" },
	{ 5250,L"Vaillant" },
	{ 5252,L"Elsa AG [hex]" },
	{ 5253,L"Silicom" },
	{ 5255,L"DSP Group, Ltd." },
	{ 5262,L"EVATRONIX SA" },
	{ 5263,L"Ralink Technology, Corp." },
	{ 5265,L"Futronic Technology Co. Ltd." },
	{ 5267,L"Suunto" },
	{ 5271,L"Panstrong Company Ltd." },
	{ 5272,L"Microtek International Inc." },
	{ 5274,L"Imagination Technologies" },
	{ 5290,L"WideView Technology Inc." },
	{ 5293,L"CTK Corporation" },
	{ 5294,L"Printronix Inc." },
	{ 5295,L"ATP Electronics Inc." },
	{ 5296,L"StarTech.com Ltd." },
	{ 5298,L"Ralink Technology, Corp." },
	{ 5312,L"Rockwell Automation, Inc." },
	{ 5314,L"Gemlight Computer, Ltd" },
	{ 5320,L"Zytronic" },
	{ 5325,L"Super Top" },
	{ 5336,L"JAMER INDUSTRIES CO., LTD." },
	{ 5341,L"Raritan Computer, Inc." },
	{ 5344,L"WiNRADiO Communications" },
	{ 5345,L"Dialogue Technology Corp." },
	{ 5349,L"SAIN Information & Communications Co., Ltd." },
	{ 5354,L"Planex Communications" },
	{ 5357,L"Shure Inc." },
	{ 5367,L"TechniSat Digital GmbH" },
	{ 5376,L"Ellisys" },
	{ 5377,L"Pine-Tum Enterprise Co., Ltd." },
	{ 5385,L"First International Computer, Inc." },
	{ 5395,L"medMobile" },
	{ 5396,L"Actel" },
	{ 5398,L"CompUSA" },
	{ 5400,L"Cheshire Engineering Corp." },
	{ 5401,L"Comneon" },
	{ 5408,L"Bitwire Corp." },
	{ 5412,L"ENE Technology Inc" },
	{ 5415,L"Silicon Portals" },
	{ 5417,L"UBIQUAM Co., Ltd." },
	{ 5418,L"Thesycon Systemsoftware & Consulting GmbH" },
	{ 5419,L"MIR Srl" },
	{ 5421,L"JMicron Technology Corp. / JMicron USA Technology Corp." },
	{ 5422,L"LG (HLDS)" },
	{ 5426,L"Razer USA, Ltd" },
	{ 5435,L"TerraTec Electronic GmbH" },
	{ 5446,L"U-Blox AG" },
	{ 5447,L"SG Intec Ltd & Co KG" },
	{ 5450,L"Celectronic GmbH" },
	{ 5451,L"PNY" },
	{ 5453,L"ConnectCounty Holdings Berhad" },
	{ 5454,L"D&M Holdings, Inc. (Denon/Marantz)" },
	{ 5455,L"SNBC CO., Ltd" },
	{ 5460,L"Prolink Microsystems Corp." },
	{ 5463,L"OQO" },
	{ 5480,L"Sunf Pu Technology Co., Ltd" },
	{ 5487,L"Quantum Corporation" },
	{ 5488,L"ALLTOP TECHNOLOGY CO., LTD." },
	{ 5499,L"Ketron SRL" },
	{ 5502,L"TRENDnet" },
	{ 5506,L"Fiberline" },
	{ 5511,L"SMA Technologie AG" },
	{ 5517,L"Oakley Inc." },
	{ 5518,L"JDS Uniphase Corporation (JDSU)" },
	{ 5528,L"Kunshan Guoji Electronics Co., Ltd." },
	{ 5538,L"Freescale Semiconductor, Inc." },
	{ 5540,L"Afatech Technologies, Inc." },
	{ 5544,L"Teams Power Limited" },
	{ 5545,L"Gemtek" },
	{ 5546,L"Gearway Electronics (Dong Guan) Co., Ltd." },
	{ 5549,L"VMware Inc." },
	{ 5562,L"Olimex Ltd." },
	{ 5568,L"XL Imaging" },
	{ 5570,L"SoundGraph Inc." },
	{ 5573,L"Advance Multimedia Internet Technology Inc. (AMIT)" },
	{ 5574,L"Laboratoires MXM" },
	{ 5576,L"KTF Technologies" },
	{ 5577,L"D-Box Technologies" },
	{ 5578,L"Textech International Ltd." },
	{ 5589,L"Coulomb Electronics Ltd." },
	{ 5593,L"Trust International B.V." },
	{ 5596,L"Hynix Semiconductor Inc." },
	{ 5600,L"Seong Ji Industrial Co., Ltd." },
	{ 5601,L"RSA" },
	{ 5604,L"Numark" },
	{ 5608,L"SohoWare" },
	{ 5609,L"Pacific Digital Corp." },
	{ 5612,L"Belcarra Technologies Corp." },
	{ 5620,L"HanfTek" },
	{ 5636,L"Tascam" },
	{ 5638,L"Umax" },
	{ 5640,L"Inside Out Networks [hex]" },
	{ 5642,L"VIA Technologies, Inc." },
	{ 5646,L"INRO" },
	{ 5652,L"Amoi Electronics" },
	{ 5655,L"Sony Corp." },
	{ 5657,L"L & K Precision Technology Co., Ltd." },
	{ 5665,L"Wionics Research" },
	{ 5672,L"Stonestreet One, Inc." },
	{ 5674,L"Airgo Networks Inc." },
	{ 5679,L"WiQuest Communications, Inc." },
	{ 5680,L"2Wire, Inc." },
	{ 5681,L"Good Way Technology" },
	{ 5701,L"Entrega [hex]" },
	{ 5705,L"SofTec Microsystems" },
	{ 5706,L"ChipX" },
	{ 5708,L"Matrix Vision GmbH" },
	{ 5719,L"Struck Innovative Systeme GmbH" },
	{ 5723,L"Frontier Design Group" },
	{ 5724,L"Kondo Kagaku" },
	{ 5728,L"Creatix Polymedia GmbH" },
	{ 5735,L"GIGA-TMS INC." },
	{ 5736,L"Actiontec Electronics, Inc. [hex]" },
	{ 5737,L"PiKRON Ltd. [hex]" },
	{ 5738,L"Clipsal" },
	{ 5751,L"China Huada Integrated Circuit Design (Group) Co., Ltd. (CIDC Group)" },
	{ 5753,L"Total Phase" },
	{ 5760,L"Golden Bridge Electech Inc." },
	{ 5761,L"Prevo Technologies, Inc." },
	{ 5762,L"Maxwise Production Enterprise Ltd." },
	{ 5764,L"Godspeed Computer Corp." },
	{ 5765,L"Delock" },
	{ 5766,L"ZOOM Corporation" },
	{ 5767,L"Kingmax Digital Inc." },
	{ 5768,L"Saab AB" },
	{ 5769,L"Razer USA, Ltd" },
	{ 5772,L"Atheros Communications" },
	{ 5776,L"Askey Computer Corp. [hex]" },
	{ 5782,L"Hitachi Video and Information System, Inc." },
	{ 5783,L"VTec Test, Inc." },
	{ 5797,L"Shenzhen Zhengerya Cable Co., Ltd." },
	{ 5798,L"Unigraf" },
	{ 5803,L"Global Sun Technology" },
	{ 5804,L"Dongguan ChingLung Wire & Cable Co., Ltd." },
	{ 5812,L"iStation" },
	{ 5813,L"Persentec, Inc." },
	{ 5824,L"Van Ooijen Technische Informatica" },
	{ 5834,L"Wireless Cables, Inc." },
	{ 5836,L"silex technology, Inc." },
	{ 5840,L"MCS" },
	{ 5841,L"Suprema Inc." },
	{ 5843,L"Frontline Test Equipment, Inc." },
	{ 5845,L"AnyDATA Corporation" },
	{ 5846,L"JABLOCOM s.r.o." },
	{ 5848,L"CMOTECH Co., Ltd." },
	{ 5852,L"Wiener, Plein & Baus" },
	{ 5855,L"King Billion Electronics Co., Ltd." },
	{ 5872,L"GN ReSound A/S" },
	{ 5877,L"Futurelogic Inc." },
	{ 5894,L"BlueView Technologies, Inc." },
	{ 5895,L"ARTIMI" },
	{ 5899,L"Swissonic" },
	{ 5901,L"Avnera" },
	{ 5905,L"Leica Microsystems" },
	{ 5924,L"Meyer Instruments (MIS)" },
	{ 5925,L"Vitesse Semiconductor" },
	{ 5926,L"Axesstel, Inc." },
	{ 5935,L"Waltop International Corp." },
	{ 5939,L"Cellink Technology Co., Ltd" },
	{ 5942,L"CANON IMAGING SYSTEM TECHNOLOGIES INC." },
	{ 5943,L"Linksys" },
	{ 5949,L"QSENN" },
	{ 5952,L"Senao" },
	{ 5955,L"General Atomics" },
	{ 5960,L"MQP Electronics" },
	{ 5964,L"ASMedia Technology Inc." },
	{ 5967,L"Syntek" },
	{ 5971,L"GERTEC Telecomunicacoes Ltda." },
	{ 5974,L"ENENSYS Technologies" },
	{ 5977,L"LucidPort Technology, Inc." },
	{ 5985,L"ASUSTek Computer, Inc. (wrong ID)" },
	{ 6002,L"System Level Solutions, Inc." },
	{ 6006,L"Arowana" },
	{ 6015,L"Sweex" },
	{ 6017,L"Multiple Vendors" },
	{ 6018,L"Spreadtrum Communications Inc." },
	{ 6020,L"TopSeed Technology Corp." },
	{ 6023,L"ATI AIB" },
	{ 6024,L"ShenZhen Litkconn Technology Co., Ltd." },
	{ 6038,L"Printrex, Inc." },
	{ 6039,L"JALCO CO., LTD." },
	{ 6041,L"Thales Norway A/S" },
	{ 6045,L"Ricavision International, Inc." },
	{ 6048,L"Samson Technologies Corp." },
	{ 6052,L"Concept2" },
	{ 6053,L"Advanced Connection Technology Inc." },
	{ 6055,L"MICOMSOFT CO., LTD." },
	{ 6056,L"Kamstrup A/S" },
	{ 6067,L"Grey Innovation" },
	{ 6069,L"Lunatone" },
	{ 6074,L"SAURIS GmbH" },
	{ 6083,L"Singim International Corp." },
	{ 6092,L"Native Instruments" },
	{ 6095,L"Hip Hing Cable & Plug Mfy. Ltd." },
	{ 6096,L"Sanford L.P." },
	{ 6099,L"Korea Techtron Co., Ltd." },
	{ 6121,L"DisplayLink" },
	{ 6123,L"Cornice, Inc." },
	{ 6127,L"Lenovo" },
	{ 6132,L"WaveSense" },
	{ 6133,L"K.K. Rocky" },
	{ 6134,L"Unicomp, Inc" },
	{ 6153,L"Advantech" },
	{ 6178,L"Twinhan" },
	{ 6193,L"Gwo Jinn Industries Co., Ltd." },
	{ 6194,L"Huizhou Shenghua Industrial Co., Ltd." },
	{ 6205,L"VIVOphone" },
	{ 6211,L"Vaisala" },
	{ 6217,L"ASRock Incorporation" },
	{ 6226,L"GYROCOM C&C Co., LTD" },
	{ 6228,L"Memory Devices Ltd." },
	{ 6235,L"Compro" },
	{ 6241,L"Tech Technology Industrial Company" },
	{ 6242,L"Teridian Semiconductor Corp." },
	{ 6256,L"Nexio Co., Ltd" },
	{ 6257,L"Aveo Technology Corp." },
	{ 6259,L"Navilock" },
	{ 6268,L"Alienware Corporation" },
	{ 6271,L"Siano Mobile Silicon" },
	{ 6290,L"Vast Technologies, Inc." },
	{ 6292,L"Topseed" },
	{ 6295,L"Evertop Wire Cable Co." },
	{ 6303,L"3Shape A/S" },
	{ 6308,L"CSSN" },
	{ 6309,L"Verbatim, Ltd" },
	{ 6321,L"Petalynx" },
	{ 6324,L"e3C Technologies" },
	{ 6326,L"Mikkon Technology Limited" },
	{ 6327,L"Zotek Electronic Co., Ltd." },
	{ 6341,L"AMIT Technology, Inc." },
	{ 6349,L"Ecamm" },
	{ 6353,L"Google Inc." },
	{ 6357,L"Starline International Group Limited" },
	{ 6361,L"Kaba" },
	{ 6364,L"LKC Technologies, Inc." },
	{ 6365,L"Planon System Solutions Inc." },
	{ 6371,L"Fitipower Integrated Technology Inc" },
	{ 6376,L"Qcom" },
	{ 6378,L"Matrox Graphics, Inc." },
	{ 6380,L"Arkmicro Technologies Inc." },
	{ 6392,L"[Maxxter]" },
	{ 6395,L"Scriptel Corporation" },
	{ 6397,L"FineArch Inc." },
	{ 6401,L"GE Healthcare" },
	{ 6408,L"GEMBIRD" },
	{ 6413,L"Motorola GSG" },
	{ 6420,L"Alco Digital Devices Limited" },
	{ 6421,L"Nordic Semiconductor ASA" },
	{ 6428,L"Innovative Technology LTD" },
	{ 6435,L"FitLinxx" },
	{ 6438,L"NextWindow" },
	{ 6447,L"Avago Technologies, Pte." },
	{ 6448,L"Shenzhen Xianhe Technology Co., Ltd." },
	{ 6449,L"Ningbo Broad Telecommunication Co., Ltd." },
	{ 6452,L"Feature Integration Technology Inc. (Fintek)" },
	{ 6456,L"Meinberg Funkuhren GmbH & Co. KG" },
	{ 6465,L"Dream Link" },
	{ 6467,L"Sensoray Co., Inc." },
	{ 6473,L"Lab126, Inc." },
	{ 6479,L"PreSonus Audio Electronics, Inc." },
	{ 6481,L"Hyperstone AG" },
	{ 6483,L"Ironkey Inc." },
	{ 6484,L"Radiient Technologies" },
	{ 6493,L"Itron Technology iONE" },
	{ 6501,L"Uniden Corporation" },
	{ 6503,L"CASIO HITACHI Mobile Communications Co., Ltd." },
	{ 6507,L"Wispro Technology Inc." },
	{ 6512,L"Dane-Elec Corp. USA" },
	{ 6517,L"Dongguan Guneetal Wire & Cable Co., Ltd." },
	{ 6518,L"Chipsbrand Microelectronics (HK) Co., Ltd." },
	{ 6519,L"T-Logic" },
	{ 6525,L"Leuze electronic" },
	{ 6537,L"Nuconn Technology Corp." },
	{ 6543,L"Beceem Communications Inc." },
	{ 6544,L"Acron Precision Industrial Co., Ltd." },
	{ 6549,L"Trillium Technology Pty. Ltd." },
	{ 6550,L"PixeLINK" },
	{ 6555,L"MicroStrain, Inc." },
	{ 6558,L"The Imaging Source Europe GmbH" },
	{ 6559,L"Benica Corporation" },
	{ 6568,L"Biforst Technology Inc." },
	{ 6571,L"Bodelin" },
	{ 6575,L"S Life" },
	{ 6578,L"Batronix" },
	{ 6580,L"Celestron" },
	{ 6581,L"B & W Group" },
	{ 6582,L"Infotech Logistic, LLC" },
	{ 6585,L"Data Robotics" },
	{ 6594,L"Futuba" },
	{ 6602,L"Mindtribe" },
	{ 6607,L"Parrot SA" },
	{ 6610,L"ZTE WCDMA Technologies MSM" },
	{ 6619,L"KFI Printers" },
	{ 6625,L"WeiDuan Electronic Accessory (S.Z.) Co., Ltd." },
	{ 6632,L"Industrial Technology Research Institute" },
	{ 6639,L"Pak Heng Technology (Shenzhen) Co., Ltd." },
	{ 6647,L"RODE Microphones" },
	{ 6650,L"Gampaq Co.Ltd" },
	{ 6655,L"Dynex" },
	{ 6664,L"Bellwood International, Inc." },
	{ 6666,L"USB-IF non-workshop" },
	{ 6674,L"KES Co., Ltd." },
	{ 6685,L"Veho" },
	{ 6693,L"Amphenol East Asia Ltd." },
	{ 6698,L"Seagate Branded Solutions" },
	{ 6700,L"China Resource Semico Co., Ltd" },
	{ 6706,L"Quanta Microsystems, Inc." },
	{ 6708,L"ACRUX" },
	{ 6710,L"Biwin Technology Ltd." },
	{ 6720,L"Terminus Technology Inc." },
	{ 6721,L"Action Electronics Co., Ltd." },
	{ 6724,L"VASCO Data Security International" },
	{ 6730,L"Silicon Image" },
	{ 6731,L"SafeBoot International B.V." },
	{ 6746,L"Tandberg Data" },
	{ 6753,L"Abbott Diabetes Care" },
	{ 6762,L"Spansion Inc." },
	{ 6765,L"SamYoung Electronics Co., Ltd" },
	{ 6766,L"Global Unichip Corp." },
	{ 6767,L"Sagem Orga GmbH" },
	{ 6770,L"Physik Instrumente" },
	{ 6777,L"Bayer Health Care LLC" },
	{ 6779,L"Lumberg Connect  GmbH & Co. KG" },
	{ 6780,L"Evoluent" },
	{ 6785,L"Holtek Semiconductor, Inc." },
	{ 6790,L"QinHeng Electronics" },
	{ 6793,L"Dynalith Systems Co., Ltd." },
	{ 6795,L"SGS Taiwan Ltd." },
	{ 6797,L"BandRich, Inc." },
	{ 6808,L"Leica Camera AG" },
	{ 6820,L"Data Drive Thru, Inc." },
	{ 6821,L"UBeacon Technologies, Inc." },
	{ 6822,L"eFortune Technology Corp." },
	{ 6829,L"KeeTouch" },
	{ 6833,L"Rigol Technologies" },
	{ 6859,L"Salcomp Plc" },
	{ 6860,L"Midiplus Co, Ltd." },
	{ 6865,L"Desay Wire Co., Ltd." },
	{ 6868,L"APS" },
	{ 6875,L"SEL C662 Serial Cable" },
	{ 6884,L"ic-design Reinhard Gottinger GmbH" },
	{ 6887,L"X-TENSIONS" },
	{ 6893,L"High Top Precision Electronic Co., Ltd." },
	{ 6895,L"Conntech Electronic (Suzhou) Corporation" },
	{ 6897,L"Connect One Ltd." },
	{ 6910,L"A. Eberle GmbH & Co. KG" },
	{ 6916,L"Meilhaus Electronic GmbH" },
	{ 6926,L"BLUTRONICS S.r.l." },
	{ 6940,L"Corsair" },
	{ 6943,L"eQ-3 Entwicklung GmbH" },
	{ 6944,L"MStar Semiconductor, Inc." },
	{ 6946,L"WiLinx Corp." },
	{ 6950,L"Cellex Power Products, Inc." },
	{ 6951,L"Current Electronics Inc." },
	{ 6952,L"NAVIsis Inc." },
	{ 6962,L"Ugobe Life Forms, Inc." },
	{ 6966,L"ViXS Systems, Inc." },
	{ 6971,L"iPassion Technology Inc." },
	{ 6975,L"Generalplus Technology Inc." },
	{ 6983,L"Energizer Holdings, Inc." },
	{ 6984,L"Plastron Precision Co., Ltd." },
	{ 6994,L"ARH Inc." },
	{ 7001,L"K.S. Terminals Inc." },
	{ 7002,L"Chao Zhou Kai Yuan Electric Co., Ltd." },
	{ 7013,L"The Hong Kong Standards and Testing Centre Ltd." },
	{ 7025,L"Fushicai" },
	{ 7026,L"ATERGI TECHNOLOGY CO., LTD." },
	{ 7027,L"Fresco Logic" },
	{ 7029,L"Ovislink Corp." },
	{ 7030,L"Legend Silicon Corp." },
	{ 7040,L"Afatech" },
	{ 7046,L"Dongguan Guanshang Electronics Co., Ltd." },
	{ 7048,L"ShenMing Electron (Dong Guan) Co., Ltd." },
	{ 7052,L"Altium Limited" },
	{ 7053,L"e-MOVE Technology Co., Ltd." },
	{ 7054,L"Amlogic, Inc." },
	{ 7055,L"MA LABS, Inc." },
	{ 7062,L"N-Trig" },
	{ 7064,L"YMax Communications Corp." },
	{ 7065,L"Shenzhen Yuanchuan Electronic" },
	{ 7073,L"JINQ CHERN ENTERPRISE CO., LTD." },
	{ 7074,L"Lite Metals & Plastic (Shenzhen) Co., Ltd." },
	{ 7076,L"Ember Corporation" },
	{ 7078,L"Abilis Systems" },
	{ 7080,L"China Telecommunication Technology Labs" },
	{ 7085,L"Harmonix Music" },
	{ 7086,L"Vuzix Corporation" },
	{ 7099,L"T & A Mobile Phones" },
	{ 7108,L"Ford Motor Co." },
	{ 7109,L"AVIXE Technology (China) Ltd." },
	{ 7111,L"Telit Wireless Solutions" },
	{ 7118,L"Contac Cable Industrial Limited" },
	{ 7119,L"Sunplus Innovation Technology Inc." },
	{ 7120,L"Hangzhou Riyue Electronic Co., Ltd." },
	{ 7125,L"BG Systems, Inc." },
	{ 7134,L"P-TWO INDUSTRIES, INC." },
	{ 7151,L"Shenzhen Tongyuan Network-Communication Cables Co., Ltd" },
	{ 7152,L"RealVision Inc." },
	{ 7157,L"Extranet Systems Inc." },
	{ 7158,L"Orient Semiconductor Electronics, Ltd." },
	{ 7165,L"TouchPack" },
	{ 7170,L"Kreton Corporation" },
	{ 7172,L"QNAP System Inc." },
	{ 7180,L"Ionics EMS, Inc." },
	{ 7181,L"Relm Wireless" },
	{ 7184,L"Lanterra Industrial Co., Ltd." },
	{ 7187,L"ALECTRONIC LIMITED" },
	{ 7194,L"Datel Electronics Ltd." },
	{ 7195,L"Volkswagen of America, Inc." },
	{ 7199,L"Goldvish S.A." },
	{ 7200,L"Fuji Electric Device Technology Co., Ltd." },
	{ 7201,L"ADDMM LLC" },
	{ 7202,L"ZHONGSHAN CHIANG YU ELECTRIC CO., LTD." },
	{ 7206,L"Shanghai Haiying Electronics Co., Ltd." },
	{ 7207,L"HuiYang D & S Cable Co., Ltd." },
	{ 7209,L"Elster GmbH" },
	{ 7217,L"LS Cable Ltd." },
	{ 7220,L"SpringCard" },
	{ 7223,L"Authorizer Technologies, Inc." },
	{ 7229,L"NONIN MEDICAL INC." },
	{ 7230,L"Wep Peripherals" },
	{ 7232,L"EZPrototypes" },
	{ 7241,L"Cherng Weei Technology Corp." },
	{ 7247,L"SiGma Micro" },
	{ 7275,L"Philips & Lite-ON Digital Solutions Corporation" },
	{ 7276,L"Skydigital Inc." },
	{ 7283,L"AMT" },
	{ 7287,L"Kaetat Industrial Co., Ltd." },
	{ 7288,L"Datascope Corp." },
	{ 7289,L"Unigen Corporation" },
	{ 7290,L"LighTuning Technology Inc." },
	{ 7291,L"LUXSHARE PRECISION INDUSTRY (SHENZHEN) CO., LTD." },
	{ 7299,L"Schomaecker GmbH" },
	{ 7303,L"2N TELEKOMUNIKACE a.s." },
	{ 7304,L"Somagic, Inc." },
	{ 7305,L"HONGKONG WEIDIDA ELECTRON LIMITED" },
	{ 7310,L"ASTRON INTERNATIONAL CORP." },
	{ 7320,L"ALPINE ELECTRONICS, INC." },
	{ 7326,L"OMEGA TECHNOLOGY" },
	{ 7328,L"ACCARIO Inc." },
	{ 7329,L"Symwave" },
	{ 7340,L"Kinstone" },
	{ 7347,L"Aces Electronic Co., Ltd." },
	{ 7348,L"OPEX CORPORATION" },
	{ 7350,L"IdeaCom Technology Inc." },
	{ 7358,L"Luminary Micro Inc." },
	{ 7359,L"FORTAT SKYMARK INDUSTRIAL COMPANY" },
	{ 7360,L"PlantSense" },
	{ 7370,L"NextWave Broadband Inc." },
	{ 7373,L"Bodatong Technology (Shenzhen) Co., Ltd." },
	{ 7380,L"adp corporation" },
	{ 7381,L"Firecomms Ltd." },
	{ 7382,L"Antonio Precise Products Manufactory Ltd." },
	{ 7390,L"Telecommunications Technology Association (TTA)" },
	{ 7391,L"WonTen Technology Co., Ltd." },
	{ 7392,L"EDIMAX TECHNOLOGY CO., LTD." },
	{ 7393,L"Amphenol KAE" },
	{ 7409,L"Dresden Elektronik" },
	{ 7420,L"ANDES TECHNOLOGY CORPORATION" },
	{ 7421,L"Flextronics Digital Design Japan, LTD." },
	{ 7427,L"iCON" },
	{ 7431,L"Solid-Motion" },
	{ 7432,L"NINGBO HENTEK DRAGON ELECTRONICS CO., LTD." },
	{ 7433,L"TechFaith Wireless Technology Limited" },
	{ 7434,L"Johnson Controls, Inc. The Automotive Business Unit" },
	{ 7435,L"HAN HUA CABLE & WIRE TECHNOLOGY (J.X.) CO., LTD." },
	{ 7439,L"Sonix Technology Co., Ltd." },
	{ 7444,L"ALPHA-SAT TECHNOLOGY LIMITED" },
	{ 7447,L"C-Thru Music Ltd." },
	{ 7449,L"Dexatek Technology Ltd." },
	{ 7455,L"Diostech Co., Ltd." },
	{ 7456,L"SAMTACK INC." },
	{ 7463,L"ASUS" },
	{ 7476,L"Dream Cheeky" },
	{ 7493,L"Touch" },
	{ 7501,L"PEGATRON CORPORATION" },
	{ 7504,L"OpenMoko, Inc." },
	{ 7511,L"Xenta" },
	{ 7515,L"Smartronix, Inc." },
	{ 7531,L"Linux Foundation" },
	{ 7568,L"Citizen" },
	{ 7649,L"Actions Microelectronics Co." },
	{ 7694,L"Qualcomm / Option" },
	{ 7696,L"Point Grey Research, Inc." },
	{ 7703,L"Mirion Technologies Dosimetry Services Division" },
	{ 7709,L"Lumension Security" },
	{ 7711,L"INVIA" },
	{ 7721,L"Festo AG & Co. KG" },
	{ 7741,L"Chipsbank Microelectronics Co., Ltd" },
	{ 7745,L"Cleverscope" },
	{ 7758,L"Cubeternet" },
	{ 7764,L"TypeMatrix" },
	{ 7784,L"TrekStor GmbH & Co. KG" },
	{ 7793,L"NZXT" },
	{ 7796,L"Coby Electronics Corporation" },
	{ 7805,L"ROCCAT" },
	{ 7867,L"NuCORE Technology, Inc." },
	{ 7898,L"AirTies Wireless Networks" },
	{ 7899,L"Blackmagic design" },
	{ 7912,L"ONDA COMMUNICATION S.p.a." },
	{ 7926,L"EADS Deutschland GmbH" },
	{ 7976,L"Cal-Comp" },
	{ 7994,L"Onda (unverified)" },
	{ 8004,L"The Neat Company" },
	{ 8008,L"H-TRONIC GmbH" },
	{ 8013,L"G-Tek Electronics Group" },
	{ 8047,L"Aliph" },
	{ 8053,L"Innostor Technology Corporation" },
	{ 8066,L"TANDBERG" },
	{ 8068,L"Alere, Inc." },
	{ 8071,L"Stantum" },
	{ 8091,L"Ubiquiti Networks, Inc." },
	{ 8107,L"Samsung Opto-Electroncs Co., Ltd." },
	{ 8125,L"Delphin Technology AG" },
	{ 8137,L"NXP Semiconductors" },
	{ 8158,L"ILX Lightwave Corporation" },
	{ 8167,L"Vertex Wireless Co., Ltd." },
	{ 8183,L"CVT Electronics.Co.,Ltd" },
	{ 8191,L"Ideofy Inc." },
	{ 8193,L"D-Link Corp." },
	{ 8194,L"DAP Technologies" },
	{ 8195,L"detectomat" },
	{ 8204,L"Reloop" },
	{ 8211,L"PCTV Systems" },
	{ 8217,L"PLANEX" },
	{ 8253,L"Encore Electronics Inc." },
	{ 8256,L"Hauppauge" },
	{ 8263,L"Texas Instruments" },
	{ 8280,L"Nano River Technology" },
	{ 8311,L"Taicang T&W Electronics Co. Ltd" },
	{ 8320,L"Barnes & Noble" },
	{ 8326,L"SIMPASS" },
	{ 8327,L"Cando" },
	{ 8352,L"Clay Logic" },
	{ 8369,L"XMOS Ltd" },
	{ 8371,L"Hanvon" },
	{ 8375,L"Qi Hardware" },
	{ 8398,L"Minicircuits" },
	{ 8415,L"Simtec Electronics" },
	{ 8433,L"NET New Electronic Technology GmbH" },
	{ 8436,L"TRENDnet" },
	{ 8439,L"XIMEA" },
	{ 8448,L"RT Systems" },
	{ 8449,L"ActionStar" },
	{ 8457,L"VIA Labs, Inc." },
	{ 8467,L"Softkinetic" },
	{ 8521,L"Advanced Silicon S.A." },
	{ 8546,L"Creative (?)" },
	{ 8580,L"GW Instek" },
	{ 8609,L"Emotiv Systems Pty. Ltd." },
	{ 8662,L"Agecodagis SARL" },
	{ 8738,L"MacAlly" },
	{ 8743,L"SAMWOO Enterprise" },
	{ 8754,L"Silicon Motion" },
	{ 8755,L"RadioShack Corporation" },
	{ 8759,L"Kobo Inc." },
	{ 8797,L"Morpho" },
	{ 8845,L"8D Technologies inc." },
	{ 8870,L"Pie Digital, Inc." },
	{ 8888,L"Motorola PCS" },
	{ 8889,L"eTurboTouch Technology, Inc." },
	{ 8890,L"Technology Innovation Holdings, Ltd" },
	{ 8964,L"Pinnacle Systems, Inc." },
	{ 8984,L"Shining Technologies, Inc. [hex]" },
	{ 9025,L"Arduino SA" },
	{ 9075,L"Pumatronix Ltda" },
	{ 9077,L"Digit@lway, Inc." },
	{ 9222,L"SANHO Digital Electronics Co., Ltd." },
	{ 9283,L"Aessent Technology Ltd" },
	{ 9336,L"Tripp-Lite" },
	{ 9354,L"Maxxter" },
	{ 9372,L"M2Tech s.r.l." },
	{ 9441,L"Paratronic" },
	{ 9778,L"TwinMOS" },
	{ 9785,L"Xsens" },
	{ 9808,L"Electronics For Imaging, Inc. [hex]" },
	{ 9817,L"Sundtek" },
	{ 9846,L"Basler AG" },
	{ 10032,L"Citizen" },
	{ 10037,L"DigitalWay" },
	{ 10047,L"Hughski Limited" },
	{ 10096,L"NHJ, Ltd" },
	{ 10168,L"ThingM" },
	{ 10273,L"ASUSTek Computer Inc." },
	{ 10393,L"Toptronic Industrial Co., Ltd" },
	{ 10395,L"Dracal/Raphnet technologies" },
	{ 10545,L"Jolla Oy" },
	{ 10755,L"dog hunter AG" },
	{ 10807,L"RTD Embedded Technologies, Inc." },
	{ 10821,L"Meizu Corp." },
	{ 11044,L"KeepKey LLC" },
	{ 11266,L"Planex Communications" },
	{ 11290,L"Dolphin Peripherals" },
	{ 12210,L"Fujitsu, Ltd" },
	{ 12581,L"Eagletron" },
	{ 12598,L"Navini Networks" },
	{ 12662,L"Whanam Electronics Co., Ltd" },
	{ 12693,L"Link Instruments" },
	{ 12917,L"VidzMedia Pte Ltd" },
	{ 13107,L"InLine" },
	{ 13108,L"AEI" },
	{ 13120,L"Yakumo" },
	{ 13124,L"Leaguer Microelectronics (LME)" },
	{ 13572,L"Micro Star" },
	{ 13624,L"Power Quotient International Co., Ltd" },
	{ 13689,L"DIVA" },
	{ 13693,L"Sharkoon" },
	{ 13878,L"InVibro" },
	{ 14392,L"WEM" },
	{ 14627,L"National Instruments Corp." },
	{ 16571,L"I-O Data" },
	{ 16641,L"i-rocks" },
	{ 16642,L"iRiver, Ltd." },
	{ 16700,L"Dell Computer Corp." },
	{ 16710,L"USBest Technology" },
	{ 16744,L"Targus" },
	{ 16962,L"USB Design by Example" },
	{ 16981,L"GoPro" },
	{ 17175,L"Broadcom Corp." },
	{ 17224,L"WinChipHead" },
	{ 17778,L"Shuttle, Inc." },
	{ 17798,L"Panram" },
	{ 18032,L"EMS Production" },
	{ 18258,L"Miditech" },
	{ 18263,L"GW Instek" },
	{ 18278,L"Aceeca" },
	{ 18517,L"Memorex" },
	{ 18801,L"SimpleTech" },
	{ 19782,L"Musical Fidelity" },
	{ 20530,L"Grandtec" },
	{ 20545,L"Linksys (?)" },
	{ 20674,L"Averatec (?)" },
	{ 20851,L"Sweex" },
	{ 21017,L"I-Tetra" },
	{ 21317,L"Owon" },
	{ 21324,L"SatoshiLabs" },
	{ 21332,L"Meyer Instruments (MIS)" },
	{ 21581,L"Transmeta Corp." },
	{ 21827,L"UC-Logic Technology Corp." },
	{ 21845,L"Epiphan Systems Inc." },
	{ 21930,L"OnSpec Electronic, Inc." },
	{ 22100,L"Gotview" },
	{ 22102,L"Uni-Trend Group Limited" },
	{ 22874,L"IRTOUCHSYSTEMS Co. Ltd." },
	{ 22918,L"Acer, Inc" },
	{ 23011,L"Nonolith Labs" },
	{ 23127,L"Zinwell" },
	{ 24576,L"Beholder International Ltd." },
	{ 24602,L"Ingenic Semiconductor Ltd." },
	{ 24969,L"Sitecom" },
	{ 25156,L"LightingSoft AG" },
	{ 25171,L"TwinHan Technology Co., Ltd" },
	{ 25452,L"CoreLogic, Inc." },
	{ 25714,L"Unknown (Sony?)" },
	{ 25927,L"Arkmicro Technologies Inc." },
	{ 26133,L"IRTOUCHSYSTEMS Co. Ltd." },
	{ 26214,L"Prototype product Vendor ID" },
	{ 26231,L"WiseGroup, Ltd." },
	{ 26769,L"3Com" },
	{ 26972,L"Opera1" },
	{ 27027,L"Yealink Network Technology Co., Ltd." },
	{ 27253,L"Shanghai Jujo Electronics Co., Ltd" },
	{ 28932,L"CME (Central Music Co.)" },
	{ 29292,L"StackFoundry LLC" },
	{ 29516,L"TBS Technologies China" },
	{ 29555,L"Beijing STONE Technology Co. Ltd." },
	{ 29586,L"Edimax Technology Co., Ltd" },
	{ 32902,L"Intel Corp." },
	{ 32903,L"Intel Corp." },
	{ 33006,L"VirtualBox" },
	{ 33410,L"Keio" },
	{ 33601,L"EGO Systems, Inc." },
	{ 34148,L"Transcend Information, Inc." },
	{ 34372,L"Intenso GmbG" },
	{ 36358,L"CH Products, Inc." },
	{ 36886,L"Sitecom" },
	{ 36898,L"TeVii Technology Ltd." },
	{ 37192,L"GeoLab, Ltd" },
	{ 38672,L"MosChip Semiconductor" },
	{ 38985,L"Bestmedia CD Recordable GmbH & Co. KG" },
	{ 39321,L"Odeon" },
	{ 39418,L"Grandtec" },
	{ 39620,L"J. Westhues" },
	{ 40584,L"Marvell Semiconductor, Inc." },
	{ 41256,L"AnMo Electronics Corp. / Dino-Lite (?)" },
	{ 41320,L"AnMo Electronics Corporation" },
	{ 42496,L"Asix" },
	{ 42791,L"3Com" },
	{ 43690,L"MXT" },
	{ 43981,L"Unknown" },
	{ 46478,L"Blue Microphones" },
	{ 49686,L"Card Device Expert Co., LTD" },
	{ 49745,L"Keil Software, Inc." },
	{ 51918,L"CACE Technologies Inc." },
	{ 52498,L"SMART TECHNOLOGY INDUSTRIAL LTD." },
	{ 53768,L"Ultimarc" },
	{ 53769,L"Ultimarc" },
	{ 55556,L"LogiLink" },
	{ 58596,L"Xorcom Ltd." },
	{ 60163,L"MakingThings" },
	{ 60186,L"eMPIA Technology, Inc." },
	{ 60202,L"KWorld" },
	{ 61208,L"SMART TECHNOLOGY INDUSTRIAL LTD." },
	{ 61443,L"Hewlett Packard" },
	{ 61826,L"Leap Motion" },
	{ 62700,L"Atten Electronics / Siglent Technologies" },
	{ 62701,L"Shenzhen Siglent Co., Ltd." },
	{ 63334,L"Hama" },
	{ 64520,L"Conrad Electronic SE" },
	{ 65518,L"FNK Tech" },
	{ 0, 0 }
};

TCHAR * translateURBSetupToRequest(PUSB_SETUP pSetup)
{
	if (((pSetup->bmRequestType >> 5) & 0x3))
		return NULL;

	// Standard 요청만 해석한다. Default Parser
	switch (pSetup->bRequest)
	{
	case USB_REQUEST_GET_STATUS:
		return _T("USB_REQUEST_GET_STATUS");
	case USB_REQUEST_CLEAR_FEATURE:
		return _T("USB_REQUEST_CLEAR_FEATURE");
	case USB_REQUEST_SET_FEATURE:
		return _T("USB_REQUEST_SET_FEATURE");
	case USB_REQUEST_SET_ADDRESS:
		return _T("USB_REQUEST_SET_ADDRESS");
	case USB_REQUEST_GET_DESCRIPTOR:
		return _T("USB_REQUEST_GET_DESCRIPTOR");
	case USB_REQUEST_SET_DESCRIPTOR:
		return _T("USB_REQUEST_SET_DESCRIPTOR");
	case USB_REQUEST_GET_CONFIGURATION:
		return _T("USB_REQUEST_GET_CONFIGURATION");
	case USB_REQUEST_SET_CONFIGURATION:
		return _T("USB_REQUEST_SET_CONFIGURATION");
	case USB_REQUEST_GET_INTERFACE:
		return _T("USB_REQUEST_GET_INTERFACE");
	case USB_REQUEST_SET_INTERFACE:
		return _T("USB_REQUEST_SET_INTERFACE");
	case USB_REQUEST_SYNC_FRAME:
		return _T("USB_REQUEST_SYNC_FRAME");
	default:
		return NULL;
	}
	return NULL;
}

TCHAR * translateURBSetupToValue(PUSB_SETUP pSetup)
{
	if (((pSetup->bmRequestType >> 5) & 0x3))
		return NULL;

	// Standard 요청만 해석한다. Default Parser
	if (pSetup->bRequest == USB_REQUEST_GET_DESCRIPTOR)
	{
		switch ((pSetup->wValue >> 8) & 0xFF)
		{
		case USB_DEVICE_DESCRIPTOR_TYPE:
			return _T("USB_DEVICE_DESCRIPTOR_TYPE");
		case USB_CONFIGURATION_DESCRIPTOR_TYPE:
			return _T("USB_CONFIGURATION_DESCRIPTOR_TYPE");
		case USB_STRING_DESCRIPTOR_TYPE:
			return _T("USB_STRING_DESCRIPTOR_TYPE");
		}
	}
	return NULL;
}

// ----------------------------------------------------------
WCHAR * parserHlpCmn_findClass(unsigned int classcode)
{
	WCHAR * bRet = L"Vendor Specific Class";
	int i = 0;
	while (1)
	{
		if (g_ClassInfo[i].pName == 0)
			break;
		if (g_ClassInfo[i].dwID == classcode)
		{
			bRet = (WCHAR *)g_ClassInfo[i].pName;
			break;
		}
		i++;
	}
	return bRet;
}

WCHAR * parserHlpCmn_findVID(unsigned int vid)
{
	WCHAR * bRet = L"Unknown";
	int i = 0;
	while (1)
	{
		if (g_IdInfo[i].pName == 0)
			break;
		if (g_IdInfo[i].dwID == vid)
		{
			bRet = (WCHAR *)g_IdInfo[i].pName;
			break;
		}
		i++;
	}
	return bRet;
}

TCHAR * parserHlpCmn_TranslatePipeHandle(PPARSER_CONTEXT pParset_Context, U64 PipeHandle)
{
	TCHAR *pPipeInformStr = NULL;
	SIZE_T RetSize = 0;
	BOOL bTranslateRet = FALSE;
	TCHAR *pEPType = NULL;
	USB_PIPE_TO_ENDPOINT * pUsbPipeToEndpoint = NULL;

	if (pParset_Context->hlp_function_list.pfn_HelpInformation == NULL)
		return 0;

	bTranslateRet = pParset_Context->hlp_function_list.pfn_HelpInformation(
		pParset_Context->MgrHandle, (void **)&pUsbPipeToEndpoint);

	if (bTranslateRet == TRUE)
	{
		USB_ENDPOINT_DESCRIPTOR * pEndpointDes = GetEndpointDescPtr(pUsbPipeToEndpoint, PipeHandle);

		if (pEndpointDes)
		{
			pPipeInformStr = (TCHAR *)malloc(100);
			if ((pEndpointDes->bmAttributes & 0x03) == 0) // control
			{
				pEPType = _T("USB_ENDPOINT_TYPE_CONTROL");
			}
			else if ((pEndpointDes->bmAttributes & 0x03) == 1) // iso
			{
				pEPType = _T("USB_ENDPOINT_TYPE_ISOCHRONOUS");
			}
			else if ((pEndpointDes->bmAttributes & 0x03) == 2) // bulk
			{
				pEPType = _T("USB_ENDPOINT_TYPE_BULK");
			}
			else if ((pEndpointDes->bmAttributes & 0x03) == 3) // int
			{
				pEPType = _T("USB_ENDPOINT_TYPE_INTERRUPT");
			}

			wsprintf(pPipeInformStr, _T("%ws:EPAddress(0x%X)"), pEPType, pEndpointDes->bEndpointAddress);
		}

	}
	return pPipeInformStr;
}

UCHAR parserHlpCmn_TranslatePipeHandleToEPType(PPARSER_CONTEXT pParset_Context, U64 PipeHandle)
{
	SIZE_T RetSize = 0;
	BOOL bTranslateRet = FALSE;
	UCHAR eptype = 0;
	USB_PIPE_TO_ENDPOINT * pUsbPipeToEndpoint = NULL;

	if (pParset_Context->hlp_function_list.pfn_HelpInformation == NULL)
		return 0;

	bTranslateRet = pParset_Context->hlp_function_list.pfn_HelpInformation(
		pParset_Context->MgrHandle, (void **)&pUsbPipeToEndpoint);

	if (bTranslateRet == TRUE)
	{
		USB_ENDPOINT_DESCRIPTOR * pEndpointDes = GetEndpointDescPtr(pUsbPipeToEndpoint, PipeHandle);

		if (pEndpointDes)
		{
			eptype = pEndpointDes->bmAttributes & 0x03;
		}
	}
	return eptype;
}

UCHAR parserHlpCmn_TranslatePipeHandleToEP(PPARSER_CONTEXT pParset_Context, U64 PipeHandle)
{
	SIZE_T RetSize = 0;
	BOOL bTranslateRet = FALSE;
	UCHAR ep = 0;

	USB_PIPE_TO_ENDPOINT * pUsbPipeToEndpoint = NULL;

	if (pParset_Context->hlp_function_list.pfn_HelpInformation == NULL)
		return 0;

	bTranslateRet = pParset_Context->hlp_function_list.pfn_HelpInformation(
		pParset_Context->MgrHandle, (void **)&pUsbPipeToEndpoint);

	if (bTranslateRet == TRUE)
	{
		USB_ENDPOINT_DESCRIPTOR * pEndpointDes = GetEndpointDescPtr(pUsbPipeToEndpoint, PipeHandle);

		if (pEndpointDes)
		{
			ep = pEndpointDes->bEndpointAddress;
		}
	}
	return ep;
}


BOOL	parserHlpCmn_free(PVOID handle, PVOID Buffer, SIZE_T Size)
{
	SIZE_T count = 0;
	PHJ_PARSING_FIXED_DATA_ENTRY pFixedData = NULL;
	PHJ_PARSING_FIXED_DATA pBaseFixedData = NULL;
	PHJ_PARSING_DATA	pDetailsPage = NULL;
	PHJ_PARSING_DATA_FOR_DATA	pDataPage = NULL;

	pBaseFixedData = (PHJ_PARSING_FIXED_DATA)Buffer;

	while (IsListEmpty(&pBaseFixedData->__AllDataEntryHeader__) == FALSE)
	{
		pFixedData = (PHJ_PARSING_FIXED_DATA_ENTRY)RemoveHeadList(&pBaseFixedData->__AllDataEntryHeader__);

		pDataPage = pFixedData->pDataPage;
		pDetailsPage = pFixedData->pDetailsPage;

		for (count = 0; count < pFixedData->ColCount; count++)
		{
			if (pFixedData->ColumnData[count].bIsConstant == FALSE)
			{
				if (pFixedData->ColumnData[count].pszData)
					free(pFixedData->ColumnData[count].pszData);
			}
		}

		free(pFixedData);

		if (pDataPage)
		{
			if (pDataPage->Size)
			{
				if (pDataPage->bIsConstant == FALSE)
				{
					free(pDataPage->pDataBuffer);
				}
			}
			free(pDataPage);
		}

		if (pDetailsPage)
			freePartialData(pDetailsPage);

	}

	free(pBaseFixedData);

	return TRUE;
}

void parserHlpCmn_FillField(PHJ_PARSING_FIELD_VALUE pField, WCHAR *FieldName, U32 Type, WCHAR *ValueName, SIZE_T ValueBitOff, SIZE_T ValueBitSize, SIZE_T ValueByteOff, SIZE_T ValueByteSize, void * pRealData)
{
	pField->pRowFieldName = FieldName;
	pField->bRowFieldValueType = Type;
	pField->pRowFieldValueName = ValueName;
	pField->nRowFieldValueBitsOffset = ValueBitOff;
	pField->nRowFieldValueBitsSize = ValueBitSize;
	pField->nRowFieldValueBytesOffset = ValueByteOff;
	pField->nRowFieldValueBytesSize = ValueByteSize;

	if (pRealData)
		memcpy(&pField->nRowFieldValue[0], pRealData, ValueByteSize);

}

PHJ_PARSING_DATA parserHlpCmn_CreateParsingData(PST_CF_IRP_URB pSource, WCHAR * DataTitleName, SIZE_T ExtensionSize)
{
	PHJ_PARSING_DATA pData = NULL;

	pData = (PHJ_PARSING_DATA)malloc(sizeof(HJ_PARSING_DATA) + ExtensionSize);

	pData->Type = HJ_STRUCT_TYPE__PARSING_DATA;
	InitializeListHead(&pData->__Sibling__);

	pData->Level = 1;
	InitializeListHead(&pData->__Child__);
	pData->ChildCount = 0;

	pData->pszTitle = DataTitleName;

	pData->nParsingValueCount = 0;
	InitializeListHead(&pData->__ValueHead__);

	return pData;
}

PHJ_PARSING_DATA parserHlpCmn_CreateAndLinkParsingDataToParsingData(PHJ_PARSING_DATA pMainData, WCHAR * DataTitleName)
{
	PHJ_PARSING_DATA pSubData = NULL;
	pMainData->ChildCount++;

	pSubData = parserHlpCmn_CreateParsingData(NULL, DataTitleName, 0);
	pSubData->Level = pMainData->Level + 1;

	InsertTailList(&pMainData->__Child__, &pSubData->__Sibling__);
	return pSubData;
}

PHJ_PARSING_VALUE parserHlpCmn_CreateAndLinkParsingDataToParsingValue(PHJ_PARSING_DATA pData, WCHAR * ValueTitleName)
{
	PHJ_PARSING_VALUE pValue = NULL;

	pValue = (PHJ_PARSING_VALUE)malloc(sizeof(HJ_PARSING_VALUE));

	pValue->nRowFieldNameCount = 0;
	InitializeListHead(&pValue->__FieldHead__);

	InsertTailList(&pData->__ValueHead__, &pValue->__Self__);
	pData->nParsingValueCount++;

	pValue->Title = ValueTitleName;
	return pValue;
}

PHJ_PARSING_FIELD_VALUE parserHlpCmn_CreateAndLinkParsingValueToParsingField(PHJ_PARSING_VALUE pValue)
{
	PHJ_PARSING_FIELD_VALUE pField = NULL;
	pField = (PHJ_PARSING_FIELD_VALUE)malloc(sizeof(HJ_PARSING_FIELD_VALUE));

	InsertTailList(&pValue->__FieldHead__, &pField->__Self__);
	pValue->nRowFieldNameCount++;
	return pField;
}

WCHAR * parserHlpCmn_translateURBStatus(USBD_STATUS status)
{
	WCHAR * bRet = NULL;

	switch (status)
	{
	case USBD_STATUS_SUCCESS:
		bRet = L"USBD_STATUS_SUCCESS";
		break;
	case USBD_STATUS_PENDING:
		bRet = L"USBD_STATUS_PENDING";
		break;
	case USBD_STATUS_CRC:
		bRet = L"USBD_STATUS_CRC";
		break;
	case USBD_STATUS_BTSTUFF:
		bRet = L"USBD_STATUS_BTSTUFF";
		break;
	case USBD_STATUS_DATA_TOGGLE_MISMATCH:
		bRet = L"USBD_STATUS_DATA_TOGGLE_MISMATCH";
		break;
	case USBD_STATUS_STALL_PID:
		bRet = L"USBD_STATUS_STALL_PID";
		break;
	case USBD_STATUS_DEV_NOT_RESPONDING:
		bRet = L"USBD_STATUS_DEV_NOT_RESPONDING";
		break;
	case USBD_STATUS_PID_CHECK_FAILURE:
		bRet = L"USBD_STATUS_PID_CHECK_FAILURE";
		break;
	case USBD_STATUS_UNEXPECTED_PID:
		bRet = L"USBD_STATUS_UNEXPECTED_PID";
		break;
	case USBD_STATUS_DATA_OVERRUN:
		bRet = L"USBD_STATUS_DATA_OVERRUN";
		break;
	case USBD_STATUS_DATA_UNDERRUN:
		bRet = L"USBD_STATUS_DATA_UNDERRUN";
		break;
	case USBD_STATUS_RESERVED1:
		bRet = L"USBD_STATUS_RESERVED1";
		break;
	case USBD_STATUS_RESERVED2:
		bRet = L"USBD_STATUS_RESERVED2";
		break;
	case USBD_STATUS_BUFFER_OVERRUN:
		bRet = L"USBD_STATUS_BUFFER_OVERRUN";
		break;
	case USBD_STATUS_BUFFER_UNDERRUN:
		bRet = L"USBD_STATUS_BUFFER_UNDERRUN";
		break;
	case USBD_STATUS_NOT_ACCESSED:
		bRet = L"USBD_STATUS_NOT_ACCESSED";
		break;
	case USBD_STATUS_FIFO:
		bRet = L"USBD_STATUS_FIFO";
		break;
	case USBD_STATUS_XACT_ERROR:
		bRet = L"USBD_STATUS_XACT_ERROR";
		break;
	case USBD_STATUS_BABBLE_DETECTED:
		bRet = L"USBD_STATUS_BABBLE_DETECTED";
		break;
	case USBD_STATUS_DATA_BUFFER_ERROR:
		bRet = L"USBD_STATUS_DATA_BUFFER_ERROR";
		break;
	case USBD_STATUS_ENDPOINT_HALTED:
		bRet = L"USBD_STATUS_ENDPOINT_HALTED";
		break;
	case USBD_STATUS_INVALID_URB_FUNCTION:
		bRet = L"USBD_STATUS_INVALID_URB_FUNCTION";
		break;
	case USBD_STATUS_INVALID_PARAMETER:
		bRet = L"USBD_STATUS_INVALID_PARAMETER";
		break;
	case USBD_STATUS_ERROR_BUSY:
		bRet = L"USBD_STATUS_ERROR_BUSY";
		break;
	case USBD_STATUS_INVALID_PIPE_HANDLE:
		bRet = L"USBD_STATUS_INVALID_PIPE_HANDLE";
		break;
	case USBD_STATUS_NO_BANDWIDTH:
		bRet = L"USBD_STATUS_NO_BANDWIDTH";
		break;
	case USBD_STATUS_INTERNAL_HC_ERROR:
		bRet = L"USBD_STATUS_INTERNAL_HC_ERROR";
		break;
	case USBD_STATUS_ERROR_SHORT_TRANSFER:
		bRet = L"USBD_STATUS_ERROR_SHORT_TRANSFER";
		break;
	case USBD_STATUS_BAD_START_FRAME:
		bRet = L"USBD_STATUS_BAD_START_FRAME";
		break;
	case USBD_STATUS_ISOCH_REQUEST_FAILED:
		bRet = L"USBD_STATUS_ISOCH_REQUEST_FAILED";
		break;
	case USBD_STATUS_FRAME_CONTROL_OWNED:
		bRet = L"USBD_STATUS_FRAME_CONTROL_OWNED";
		break;
	case USBD_STATUS_FRAME_CONTROL_NOT_OWNED:
		bRet = L"USBD_STATUS_FRAME_CONTROL_NOT_OWNED";
		break;
	case USBD_STATUS_NOT_SUPPORTED:
		bRet = L"USBD_STATUS_NOT_SUPPORTED";
		break;
	case USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR:
		bRet = L"USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR";
		break;
	case USBD_STATUS_INSUFFICIENT_RESOURCES:
		bRet = L"USBD_STATUS_INSUFFICIENT_RESOURCES";
		break;
	case USBD_STATUS_SET_CONFIG_FAILED:
		bRet = L"USBD_STATUS_SET_CONFIG_FAILED";
		break;
	case USBD_STATUS_BUFFER_TOO_SMALL:
		bRet = L"USBD_STATUS_BUFFER_TOO_SMALL";
		break;
	case USBD_STATUS_INTERFACE_NOT_FOUND:
		bRet = L"USBD_STATUS_INTERFACE_NOT_FOUND";
		break;
	case USBD_STATUS_INAVLID_PIPE_FLAGS:
		bRet = L"USBD_STATUS_INAVLID_PIPE_FLAGS";
		break;
	case USBD_STATUS_TIMEOUT:
		bRet = L"USBD_STATUS_TIMEOUT";
		break;
	case USBD_STATUS_DEVICE_GONE:
		bRet = L"USBD_STATUS_DEVICE_GONE";
		break;
	case USBD_STATUS_STATUS_NOT_MAPPED:
		bRet = L"USBD_STATUS_STATUS_NOT_MAPPED";
		break;
	case USBD_STATUS_HUB_INTERNAL_ERROR:
		bRet = L"USBD_STATUS_HUB_INTERNAL_ERROR";
		break;
	case USBD_STATUS_CANCELED:
		bRet = L"USBD_STATUS_CANCELED";
		break;
	case USBD_STATUS_ISO_NOT_ACCESSED_BY_HW:
		bRet = L"USBD_STATUS_ISO_NOT_ACCESSED_BY_HW";
		break;
	case USBD_STATUS_ISO_TD_ERROR:
		bRet = L"USBD_STATUS_ISO_TD_ERROR";
		break;
	case USBD_STATUS_ISO_NA_LATE_USBPORT:
		bRet = L"USBD_STATUS_ISO_NA_LATE_USBPORT";
		break;
	case USBD_STATUS_ISO_NOT_ACCESSED_LATE:
		bRet = L"USBD_STATUS_ISO_NOT_ACCESSED_LATE";
		break;
	case USBD_STATUS_BAD_DESCRIPTOR:
		bRet = L"USBD_STATUS_BAD_DESCRIPTOR";
		break;
	case USBD_STATUS_BAD_DESCRIPTOR_BLEN:
		bRet = L"USBD_STATUS_BAD_DESCRIPTOR_BLEN";
		break;
	case USBD_STATUS_BAD_DESCRIPTOR_TYPE:
		bRet = L"USBD_STATUS_BAD_DESCRIPTOR_TYPE";
		break;
	case USBD_STATUS_BAD_INTERFACE_DESCRIPTOR:
		bRet = L"USBD_STATUS_BAD_INTERFACE_DESCRIPTOR";
		break;
	case USBD_STATUS_BAD_ENDPOINT_DESCRIPTOR:
		bRet = L"USBD_STATUS_BAD_ENDPOINT_DESCRIPTOR";
		break;
	case USBD_STATUS_BAD_INTERFACE_ASSOC_DESCRIPTOR:
		bRet = L"USBD_STATUS_BAD_INTERFACE_ASSOC_DESCRIPTOR";
		break;
	case USBD_STATUS_BAD_CONFIG_DESC_LENGTH:
		bRet = L"USBD_STATUS_BAD_CONFIG_DESC_LENGTH";
		break;
	case USBD_STATUS_BAD_NUMBER_OF_INTERFACES:
		bRet = L"USBD_STATUS_BAD_NUMBER_OF_INTERFACES";
		break;
	case USBD_STATUS_BAD_NUMBER_OF_ENDPOINTS:
		bRet = L"USBD_STATUS_BAD_NUMBER_OF_ENDPOINTS";
		break;
	case USBD_STATUS_BAD_ENDPOINT_ADDRESS:
		bRet = L"USBD_STATUS_BAD_ENDPOINT_ADDRESS";
		break;
	default:
		bRet = L"USBD_STATUS_UNKNOWN";
		break;
	}
	return bRet;
}

PHJ_PARSING_DATA parserHlpCmn_FillDetailsPage(PPARSER_CONTEXT pParset_Context, PST_CF_IRP_URB pSource, SIZE_T	Level)
{
	int newIndex = 0;
	PHJ_PARSING_DATA pData = NULL;
	PHJ_PARSING_DATA pMainData = NULL;
	PHJ_PARSING_VALUE pValue = NULL;
	PHJ_PARSING_FIELD_VALUE pFieldValue = NULL;

	ST_CF_IRP_URB_SELECT_CONFIGURATION * pSource_SelectConfig = NULL;
	PNEW_USBD_INTERFACE_INFORMATION pSource_InterfaceInformation = NULL;

	PSPECIFIC_INFO pSpecificInfo = NULL;
	BOOL bFeatureSelect_Device = FALSE;
	BOOL bFeatureSelect_Interface = FALSE;
	BOOL bFeatureSelect_Endpoint = FALSE;

	BOOL bCompletion = FALSE;
	int InterfaceCount = 0;
	int count = 0;

	SIZE_T nDesc;
	USB_COMMON_DESCRIPTOR * pUSBCommonDesc = NULL;
	PUSB_DEVICE_DESCRIPTOR pDeviceDesc = NULL;
	PUSB_CONFIGURATION_DESCRIPTOR pConfigDesc = NULL;
	PUSB_INTERFACE_DESCRIPTOR pInterfaceDesc = NULL;
	PUSB_ENDPOINT_DESCRIPTOR pEndpointDesc = NULL;
	PUSB_STRING_DESCRIPTOR pStringDesc = NULL;

	TCHAR *pPipeInformStr = NULL;
	TCHAR *pSetupStr = NULL;

	PUSB_SETUP pSetup = NULL;

	int arraycontentcount = 0;
	int arraycontentcount1 = 0;
	int arraycontentcount2 = 0;

	//////////////////////////////////////////////////////////////////////////////////////
	// 이곳에서는 세부속성과 관련된 파싱을 처리한다
	// Main 이 되는 Parsing Data를 먼저 만듬
	// Specific 메모리의 크기가 얼마나 필요한지를 조사해서 적당한 만큼만 할당합니다.

	switch (pSource->stUrbHeader.u16Function)
	{
		case URB_FUNCTION_CONTROL_TRANSFER:
		case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
		case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE:
		case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT:
		case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE:
		case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE:
		case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT:
			pData = parserHlpCmn_CreateParsingData(pSource, GetTitleNameFromUrb(pSource->stUrbHeader.u16Function), sizeof(SPECIFIC_INFO_DESC));
			break;
		default:
			pData = parserHlpCmn_CreateParsingData(pSource, GetTitleNameFromUrb(pSource->stUrbHeader.u16Function), sizeof(SPECIFIC_INFO));
			break;
	}

	pMainData = pData;

	pSpecificInfo = (PSPECIFIC_INFO)(pData + 1); // 나중을 위해서 예비한 메모리영역

												 //////////////////////////////////////////////////////////////////////////////////////
												 // Parsing Value를 만듬(IRP)
	pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("IRP Parameter"));

	WCHAR * pTempString;
	if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
	{
		bCompletion = TRUE;
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"요청시작/완료", HJ_VALUE_ENDIAN_IGN, L"완료", 0, 0, 0, 0, NULL);
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		pTempString = translateIRPStatus(pSource->stUrbHeader.stHeader.u32NtStatus);
		parserHlpCmn_FillField(pFieldValue, _T("NtStatus"), HJ_VALUE_ENDIAN_LSB, pTempString, 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32NtStatus);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, _T("Information"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource->stUrbHeader.stHeader.u64Information);
	}
	else
	{
		bCompletion = FALSE;
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"요청시작/완료", HJ_VALUE_ENDIAN_IGN, L"시작", 0, 0, 0, 0, NULL);
	}

	pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
	if (pSource->stUrbHeader.stHeader.u32RequestorMode == 0)
	{
		parserHlpCmn_FillField(pFieldValue, _T("RequestorMode"), HJ_VALUE_ENDIAN_LSB, L"KernelMode", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32RequestorMode);
	}
	else if (pSource->stUrbHeader.stHeader.u32RequestorMode == 1)
	{
		parserHlpCmn_FillField(pFieldValue, _T("RequestorMode"), HJ_VALUE_ENDIAN_LSB, L"UserMode", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32RequestorMode);
	}
	else
	{
		parserHlpCmn_FillField(pFieldValue, _T("RequestorMode"), HJ_VALUE_ENDIAN_LSB, L"Unknown", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32RequestorMode);
	}
	pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
	parserHlpCmn_FillField(pFieldValue, _T("Time"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource->stUrbHeader.stHeader.u64Time);
	pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
	parserHlpCmn_FillField(pFieldValue, _T("MajorFunction"), HJ_VALUE_ENDIAN_LSB, GetMajorFunction(pSource->stUrbHeader.stHeader.u8MajorFunction), 0, 8, 0, sizeof(U8), &pSource->stUrbHeader.stHeader.u8MajorFunction);
	pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
	parserHlpCmn_FillField(pFieldValue, _T("MinorFunction"), HJ_VALUE_ENDIAN_LSB, GetMinorFunction(pSource->stUrbHeader.stHeader.u8MajorFunction, pSource->stUrbHeader.stHeader.u8MinorFunction), 0, 8, 0, sizeof(U8), &pSource->stUrbHeader.stHeader.u8MinorFunction);

	pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
	switch (pSource->stUrbHeader.stHeader.u32IoControlCode)
	{
	case IOCTL_INTERNAL_USB_SUBMIT_URB:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_SUBMIT_URB", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_RESET_PORT:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_RESET_PORT", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_PORT_STATUS:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_PORT_STATUS", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_ENABLE_PORT:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_ENABLE_PORT", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_HUB_COUNT:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_HUB_COUNT", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_CYCLE_PORT:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_CYCLE_PORT", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_HUB_NAME:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_HUB_NAME", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_BUS_INFO:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_BUS_INFO", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_BUSGUID_INFO:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_BUSGUID_INFO", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_NOTIFY_IDLE_READY:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_NOTIFY_IDLE_READY", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_REQ_GLOBAL_SUSPEND:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_REQ_GLOBAL_SUSPEND", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_REQ_GLOBAL_RESUME:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_REQ_GLOBAL_RESUME", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_RECORD_FAILURE:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_RECORD_FAILURE", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_TT_DEVICE_HANDLE:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_TT_DEVICE_HANDLE", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_TOPOLOGY_ADDRESS:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_TOPOLOGY_ADDRESS", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_HCD_GET_STATS_1:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_HCD_GET_STATS_1", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_HCD_DISABLE_PORT:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_HCD_DISABLE_PORT", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_HCD_ENABLE_PORT:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_HCD_ENABLE_PORT", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_DIAGNOSTIC_MODE_ON:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_DIAGNOSTIC_MODE_ON", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_DIAGNOSTIC_MODE_OFF:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_DIAGNOSTIC_MODE_OFF", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_GET_ROOT_HUB_NAME:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_GET_ROOT_HUB_NAME", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_GET_NODE_CONNECTION_INFORMATION", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_GET_NODE_CONNECTION_NAME:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_GET_NODE_CONNECTION_NAME", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_DIAG_IGNORE_HUBS_ON:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_DIAG_IGNORE_HUBS_ON", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_DIAG_IGNORE_HUBS_OFF:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_DIAG_IGNORE_HUBS_OFF", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_GET_HUB_CAPABILITIES:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_GET_HUB_CAPABILITIES", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_HUB_CYCLE_PORT:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_HUB_CYCLE_PORT", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_GET_NODE_CONNECTION_ATTRIBUTES:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_GET_NODE_CONNECTION_ATTRIBUTES", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_RESET_HUB:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_RESET_HUB", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	case IOCTL_USB_GET_HUB_CAPABILITIES_EX:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"IOCTL_USB_GET_HUB_CAPABILITIES_EX", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	default:
		parserHlpCmn_FillField(pFieldValue, _T("IoControlCode"), HJ_VALUE_ENDIAN_LSB, L"Unknown", 0, 32, 0, sizeof(U32), &pSource->stUrbHeader.stHeader.u32IoControlCode);
		break;
	}


	pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
	parserHlpCmn_FillField(pFieldValue, _T("OrignialIrp"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource->stUrbHeader.stHeader.u64OrignialIrp);

	switch (pSource->stUrbHeader.u16Function)
	{
	case URB_FUNCTION_SELECT_INTERFACE:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_SELECT_INTERFACE"));
		if (bCompletion == FALSE) // 시작
		{
			// 1. ConfigHandle
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("ConfigHandle"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &((U64)pSource->stUrbSelectInterface.u64ConfigurationHandle));

			// 2. InterfaceNumber
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("InterfaceNumber"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &((U8)pSource->stUrbSelectInterface.stInterface.InterfaceNumber));

			// 3. AlternateSetting
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("AlternateSetting"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &((U8)pSource->stUrbSelectInterface.stInterface.AlternateSetting));
		}
		else // 완료
		{
			// 1. urbStatus
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbSelectInterface.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbSelectInterface.stHeader.u32Status);

			// 2. InterfaceNumber
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("InterfaceNumber"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &((U8)pSource->stUrbSelectInterface.stInterface.InterfaceNumber));

			// 3. AlternateSetting
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("AlternateSetting"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &((U8)pSource->stUrbSelectInterface.stInterface.AlternateSetting));

			// 4. Class
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("Class"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &((U8)pSource->stUrbSelectInterface.stInterface.Class));

			// 5. SubClass
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("SubClass"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &((U8)pSource->stUrbSelectInterface.stInterface.SubClass));

			// 6. Protocol
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("Protocol"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &((U8)pSource->stUrbSelectInterface.stInterface.Protocol));

			// 7. InterfaceHandle
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("InterfaceHandle"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &((U64)pSource->stUrbSelectInterface.stInterface.InterfaceHandle));

			// 8. NumberOfPipes
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("NumberOfPipes"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &((U32)pSource->stUrbSelectInterface.stInterface.NumberOfPipes));

			// 9. Array
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("Array"), HJ_STRUCT_TYPE__SPLIT_ARRAY_BEGIN, NULL, 0, 32, 0, sizeof(U32), &((U32)pSource->stUrbSelectInterface.stInterface.NumberOfPipes));

			for (count = 0; count < (int)pSource->stUrbSelectInterface.stInterface.NumberOfPipes; count++)
			{
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("PipeInformation"), HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN, NULL, 0, 32, 0, sizeof(U32), &count);

				// 1
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("PIPE#"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &count);

				// 2
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, L"MaximumPacketSize", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource->stUrbSelectInterface.stInterface.Pipes[count].MaximumPacketSize);

				// 3
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, L"EndpointAddress", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource->stUrbSelectInterface.stInterface.Pipes[count].EndpointAddress);

				// 4
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, L"Interval", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource->stUrbSelectInterface.stInterface.Pipes[count].Interval);

				// 5
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, L"MaximumTransferSize", HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource->stUrbSelectInterface.stInterface.Pipes[count].MaximumTransferSize);

				// 6
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

				switch (pSource->stUrbSelectInterface.stInterface.Pipes[count].PipeType)
				{
				case UsbdPipeTypeControl:
					parserHlpCmn_FillField(pFieldValue, L"PipeType", HJ_VALUE_ENDIAN_LSB, L"UsbdPipeTypeControl", 0, 32, 0, sizeof(U32), &pSource->stUrbSelectInterface.stInterface.Pipes[count].PipeType);
					break;
				case UsbdPipeTypeIsochronous:
					parserHlpCmn_FillField(pFieldValue, L"PipeType", HJ_VALUE_ENDIAN_LSB, L"UsbdPipeTypeIsochronous", 0, 32, 0, sizeof(U32), &pSource->stUrbSelectInterface.stInterface.Pipes[count].PipeType);
					break;
				case UsbdPipeTypeBulk:
					parserHlpCmn_FillField(pFieldValue, L"PipeType", HJ_VALUE_ENDIAN_LSB, L"UsbdPipeTypeBulk", 0, 32, 0, sizeof(U32), &pSource->stUrbSelectInterface.stInterface.Pipes[count].PipeType);
					break;
				case UsbdPipeTypeInterrupt:
					parserHlpCmn_FillField(pFieldValue, L"PipeType", HJ_VALUE_ENDIAN_LSB, L"UsbdPipeTypeInterrupt", 0, 32, 0, sizeof(U32), &pSource->stUrbSelectInterface.stInterface.Pipes[count].PipeType);
					break;
				}

				// 7
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				pPipeInformStr = parserHlpCmn_TranslatePipeHandle(pParset_Context, pSource->stUrbSelectInterface.stInterface.Pipes[count].PipeHandle);
				if (pPipeInformStr)
				{
					parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB | HJ_VALUE_EXTRA_PRIVATEBUFF, pPipeInformStr, 0, 64, 0, sizeof(U64), &pSource->stUrbSelectInterface.stInterface.Pipes[count].PipeHandle);
					pFieldValue->nRowFieldPrivateBufferMaxLength = 0;
					pFieldValue->nRowFieldPrivateBufferLength = 0;
					pFieldValue->nRowFieldPrivateBuffer = (U8 *)pPipeInformStr;
				}
				else
				{
					parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource->stUrbSelectInterface.stInterface.Pipes[count].PipeHandle);
				}

				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("PipeInformation"), HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END, NULL, 0, 0, 0, 0, NULL);
			}
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("Array"), HJ_STRUCT_TYPE__SPLIT_ARRAY_END, NULL, 0, 0, 0, 0, NULL);
		}
		break;

	case URB_FUNCTION_SELECT_CONFIGURATION:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_SELECT_CONFIGURATION"));

		pSource_SelectConfig = (ST_CF_IRP_URB_SELECT_CONFIGURATION *)pSource;
		pSource_SelectConfig++;
		pConfigDesc = (USB_CONFIGURATION_DESCRIPTOR *)pSource_SelectConfig;
		pSource_InterfaceInformation = (PNEW_USBD_INTERFACE_INFORMATION)((U8 *)pConfigDesc + pConfigDesc->wTotalLength);

		if (bCompletion == FALSE) // 시작
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("Array"), HJ_STRUCT_TYPE__SPLIT_ARRAY_BEGIN, NULL, 0, 8, 0, sizeof(U8), &pConfigDesc->bNumInterfaces);

			//				arraycontentcount = 2;

			for (count = 0; count < pConfigDesc->bNumInterfaces; count++)
			{
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("InterfaceInformation"), HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN, NULL, 0, 32, 0, sizeof(U32), &count);

				// 1. InterfaceNumber
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("InterfaceNumber"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource_InterfaceInformation->InterfaceNumber);

				// 2. AlternateSetting
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("AlternateSetting"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource_InterfaceInformation->AlternateSetting);

				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("InterfaceInformation"), HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END, NULL, 0, 0, 0, 0, NULL);

				// pSource_InterfaceInformation 포인터를 다음위치로 옮겨야 한다
				{
					PNEW_USBD_INTERFACE_INFORMATION pNextSource_InterfaceInformation = NULL;
					pNextSource_InterfaceInformation = (PNEW_USBD_INTERFACE_INFORMATION)(((unsigned char *)pSource_InterfaceInformation) + pSource_InterfaceInformation->Length);
					pSource_InterfaceInformation = pNextSource_InterfaceInformation;
				}
			}

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("Array"), HJ_STRUCT_TYPE__SPLIT_ARRAY_END, NULL, 0, 0, 0, 0, NULL);
		}
		else // 완료
		{
			// 1. urbStatus
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbSelectConfiguration.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbSelectConfiguration.stHeader.u32Status);

			// 2. ConfigHandle
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("ConfigHandle"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &((U64)pSource->stUrbSelectConfiguration.u64ConfigurationHandle));

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("Array"), HJ_STRUCT_TYPE__SPLIT_ARRAY_BEGIN, NULL, 0, 8, 0, sizeof(U8), &pConfigDesc->bNumInterfaces);

			//				arraycontentcount1 = 8;
			//				arraycontentcount2 = 7;

			for (int count = 0; count < pConfigDesc->bNumInterfaces; count++)
			{

				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("InterfaceInformation"), HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN, NULL, 0, 32, 0, sizeof(U32), &count);

				// 1. InterfaceNumber
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("InterfaceNumber"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource_InterfaceInformation->InterfaceNumber);

				// 2. AlternateSetting
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("AlternateSetting"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource_InterfaceInformation->AlternateSetting);

				// 3. Class
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("Class"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource_InterfaceInformation->Class);

				// 4. SubClass
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("SubClass"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource_InterfaceInformation->SubClass);

				// 5. Protocol
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("Protocol"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource_InterfaceInformation->Protocol);

				// 6. InterfaceHandle
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("InterfaceHandle"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource_InterfaceInformation->InterfaceHandle);

				// 7. NumberOfPipes
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("NumberOfPipes"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource_InterfaceInformation->NumberOfPipes);

				// 8.
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("Array"), HJ_STRUCT_TYPE__SPLIT_ARRAY_BEGIN, NULL, 0, 32, 0, sizeof(U32), &pSource_InterfaceInformation->NumberOfPipes);

				for (int count2 = 0; count2 < (int)pSource_InterfaceInformation->NumberOfPipes; count2++)
				{
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, _T("PipeInformation"), HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN, NULL, 0, 32, 0, sizeof(U32), &count2);

					// 1
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, _T("PIPE#"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &count2);

					// 2
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"MaximumPacketSize", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource_InterfaceInformation->Pipes[count2].MaximumPacketSize);

					// 3
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"EndpointAddress", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource_InterfaceInformation->Pipes[count2].EndpointAddress);

					// 4
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"Interval", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource_InterfaceInformation->Pipes[count2].Interval);

					// 5
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"MaximumTransferSize", HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource_InterfaceInformation->Pipes[count2].MaximumTransferSize);

					// 6
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

					switch (pSource_InterfaceInformation->Pipes[count2].PipeType)
					{
					case UsbdPipeTypeControl:
						parserHlpCmn_FillField(pFieldValue, L"PipeType", HJ_VALUE_ENDIAN_LSB, L"UsbdPipeTypeControl", 0, 32, 0, sizeof(U32), &pSource_InterfaceInformation->Pipes[count2].PipeType);
						break;
					case UsbdPipeTypeIsochronous:
						parserHlpCmn_FillField(pFieldValue, L"PipeType", HJ_VALUE_ENDIAN_LSB, L"UsbdPipeTypeIsochronous", 0, 32, 0, sizeof(U32), &pSource_InterfaceInformation->Pipes[count2].PipeType);
						break;
					case UsbdPipeTypeBulk:
						parserHlpCmn_FillField(pFieldValue, L"PipeType", HJ_VALUE_ENDIAN_LSB, L"UsbdPipeTypeBulk", 0, 32, 0, sizeof(U32), &pSource_InterfaceInformation->Pipes[count2].PipeType);
						break;
					case UsbdPipeTypeInterrupt:
						parserHlpCmn_FillField(pFieldValue, L"PipeType", HJ_VALUE_ENDIAN_LSB, L"UsbdPipeTypeInterrupt", 0, 32, 0, sizeof(U32), &pSource_InterfaceInformation->Pipes[count2].PipeType);
						break;
					}

					// 7
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

					pPipeInformStr = parserHlpCmn_TranslatePipeHandle(pParset_Context, pSource_InterfaceInformation->Pipes[count2].PipeHandle);
					if (pPipeInformStr)
					{
						parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB | HJ_VALUE_EXTRA_PRIVATEBUFF, pPipeInformStr, 0, 64, 0, sizeof(U64), &pSource_InterfaceInformation->Pipes[count2].PipeHandle);
						pFieldValue->nRowFieldPrivateBufferMaxLength = 0;
						pFieldValue->nRowFieldPrivateBufferLength = 0;
						pFieldValue->nRowFieldPrivateBuffer = (U8 *)pPipeInformStr;
					}
					else
					{
						parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource_InterfaceInformation->Pipes[count2].PipeHandle);
					}

					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, _T("PipeInformation"), HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END, NULL, 0, 0, 0, 0, NULL);
				}

				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("Array"), HJ_STRUCT_TYPE__SPLIT_ARRAY_END, NULL, 0, 0, 0, 0, NULL);

				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, _T("InterfaceInformation"), HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END, NULL, 0, 0, 0, 0, NULL);

				// pSource_InterfaceInformation 포인터를 다음위치로 옮겨야 한다
				{
					PNEW_USBD_INTERFACE_INFORMATION pNextSource_InterfaceInformation = NULL;
					pNextSource_InterfaceInformation = (PNEW_USBD_INTERFACE_INFORMATION)(((unsigned char *)pSource_InterfaceInformation) + pSource_InterfaceInformation->Length);
					pSource_InterfaceInformation = pNextSource_InterfaceInformation;
				}
			}
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("Array"), HJ_STRUCT_TYPE__SPLIT_ARRAY_END, NULL, 0, 0, 0, 0, NULL);
		}
		break;
	case URB_FUNCTION_ABORT_PIPE:
	case URB_FUNCTION_RESET_PIPE:
		if (pSource->stUrbHeader.u16Function == URB_FUNCTION_ABORT_PIPE)
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_ABORT_PIPE"));
		else
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_RESET_PIPE"));

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

		pPipeInformStr = parserHlpCmn_TranslatePipeHandle(pParset_Context, pSource->stUrbPipeRequest.u64PipeHandle);
		if (pPipeInformStr)
		{
			parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB | HJ_VALUE_EXTRA_PRIVATEBUFF, pPipeInformStr, 0, 64, 0, sizeof(U64), &pSource->stUrbPipeRequest.u64PipeHandle);
			pFieldValue->nRowFieldPrivateBufferMaxLength = 0;
			pFieldValue->nRowFieldPrivateBufferLength = 0;
			pFieldValue->nRowFieldPrivateBuffer = (U8 *)pPipeInformStr;
		}
		else
		{
			parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource->stUrbPipeRequest.u64PipeHandle);
		}

		if (bCompletion == FALSE) // 시작
		{
		}
		else // 완료
		{
			// 1. urbStatus
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbPipeRequest.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbPipeRequest.stHeader.u32Status);
		}
		break;
	case URB_FUNCTION_GET_CURRENT_FRAME_NUMBER:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_GET_CURRENT_FRAME_NUMBER"));

		if (bCompletion == TRUE)
		{
			// 1. urbStatus
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbGetCurrentFrameNumber.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbGetCurrentFrameNumber.stHeader.u32Status);

			// 2. FrameNumber
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"FrameNumber", HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource->stUrbGetCurrentFrameNumber.u32FrameNumber);
		}
		break;
	case URB_FUNCTION_CONTROL_TRANSFER:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_CONTROL_TRANSFER"));

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

		pPipeInformStr = parserHlpCmn_TranslatePipeHandle(pParset_Context, pSource->stUrbControlTransfer.u64PipeHandle);
		if (pPipeInformStr)
		{
			parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB | HJ_VALUE_EXTRA_PRIVATEBUFF, pPipeInformStr, 0, 64, 0, sizeof(U64), &pSource->stUrbControlTransfer.u64PipeHandle);
			pFieldValue->nRowFieldPrivateBufferMaxLength = 0;
			pFieldValue->nRowFieldPrivateBufferLength = _tcslen(pPipeInformStr) * sizeof(TCHAR);
			pFieldValue->nRowFieldPrivateBuffer = (U8 *)pPipeInformStr;
		}
		else
		{
			parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource->stUrbControlTransfer.u64PipeHandle);
		}

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		translateUrbFlagString(pSource->stUrbControlTransfer.u32TransferFlags, pSpecificInfo->UrbFlagString);
		parserHlpCmn_FillField(pFieldValue, L"TransferFlags", HJ_VALUE_ENDIAN_LSB, pSpecificInfo->UrbFlagString, 0, 32, 0, sizeof(U32), &pSource->stUrbControlTransfer.u32TransferFlags);
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer);
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

		pSetupStr = parserHlpCmn_translateString(NULL, pSource->stUrbControlTransfer.u8SetupPacket, 8, 8);
		if (pSetupStr)
		{
			parserHlpCmn_FillField(pFieldValue, L"SetupPacket", HJ_VALUE_ENDIAN_LSB | HJ_VALUE_EXTRA_PRIVATEBUFF, pSetupStr, 0, 64, 0, sizeof(U64), pSource->stUrbControlTransfer.u8SetupPacket);
			pFieldValue->nRowFieldPrivateBufferMaxLength = 0;
			pFieldValue->nRowFieldPrivateBufferLength = _tcslen(pSetupStr) * sizeof(TCHAR);
			pFieldValue->nRowFieldPrivateBuffer = (U8 *)pSetupStr;
		}
		else
			parserHlpCmn_FillField(pFieldValue, L"SetupPacket", HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), pSource->stUrbControlTransfer.u8SetupPacket);

		if (bCompletion == TRUE) // 완료
		{
			// 1. urbStatus
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbControlTransfer.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbControlTransfer.stHeader.u32Status);

			if (pSource->stUrbControlTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
				if (pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer)
				{
					// Data Output
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
					pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer);
					memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		else
		{
			if (pSource->stUrbControlTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
			}
			else //OUT
			{
				if (pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer)
				{
					// Data Output
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
					pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer);
					memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer);
				}
			}
		}

		pSetup = (PUSB_SETUP)pSource->stUrbControlTransfer.u8SetupPacket;
		// SETUP 패킷을 분석해서 출력해주어야 한다

		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB Setup Data"));

		if (((pSetup->bmRequestType >> 5) & 0x3) == 0)
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("bmRequestType"), HJ_VALUE_ENDIAN_LSB, _T("Standard"), 0, 8, 0, sizeof(U8), &pSetup->bmRequestType);
		}
		else if (((pSetup->bmRequestType >> 5) & 0x3) == 0)
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("bmRequestType"), HJ_VALUE_ENDIAN_LSB, _T("Class"), 0, 8, 0, sizeof(U8), &pSetup->bmRequestType);
		}
		else if (((pSetup->bmRequestType >> 5) & 0x3) == 0)
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("bmRequestType"), HJ_VALUE_ENDIAN_LSB, _T("Vendor"), 0, 8, 0, sizeof(U8), &pSetup->bmRequestType);
		}
		else if (((pSetup->bmRequestType >> 5) & 0x3) == 0)
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("bmRequestType"), HJ_VALUE_ENDIAN_LSB, _T("Reserved"), 0, 8, 0, sizeof(U8), &pSetup->bmRequestType);
		}
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, _T("bRequest"), HJ_VALUE_ENDIAN_LSB, translateURBSetupToRequest(pSetup), 0, 8, 0, sizeof(U8), &pSetup->bRequest);
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, _T("wValue"), HJ_VALUE_ENDIAN_LSB, translateURBSetupToValue(pSetup), 0, 16, 0, sizeof(U16), &pSetup->wValue);
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, _T("wIndex"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSetup->wIndex);
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, _T("wLength"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSetup->wLength);

		if ((bCompletion == TRUE) && (pSource->stUrbControlTransfer.stHeader.u32Status == 0))// 완료
		{
			if (pSource->stUrbControlTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
				if (pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer)
				{
					if ((((pSetup->bmRequestType >> 5) & 0x3) == 0) && (pSetup->bRequest == 6)) // GETDESC
					{
						// Descriptor의 경우를 위해 해석한다
						// Descriptor View Node 생성
						// Desc 개수를 먼저 확인한다
						{
							pUSBCommonDesc = (USB_COMMON_DESCRIPTOR *)((U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer);
							pDeviceDesc = (PUSB_DEVICE_DESCRIPTOR)pUSBCommonDesc;
							pConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)pUSBCommonDesc;
							pInterfaceDesc = (PUSB_INTERFACE_DESCRIPTOR)pUSBCommonDesc;
							pEndpointDesc = (PUSB_ENDPOINT_DESCRIPTOR)pUSBCommonDesc;
							pStringDesc = (PUSB_STRING_DESCRIPTOR)pUSBCommonDesc;

							nDesc = CountOfDescriptor(pUSBCommonDesc, (SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer);
							for (count = 0; count < (int)nDesc; count++)
							{
								switch (pUSBCommonDesc->bDescriptorType)
								{
								case USB_DEVICE_DESCRIPTOR_TYPE:
									pDeviceDesc = (PUSB_DEVICE_DESCRIPTOR)pUSBCommonDesc;
									pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_DEVICE_DESCRIPTOR"));

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->bLength);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_DEVICE_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDescriptorType);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bcdUSB", HJ_VALUE_ENDIAN_LSB, L"Version", 0, 16, 0, sizeof(U16), &pDeviceDesc->bcdUSB);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bDeviceClass", HJ_VALUE_ENDIAN_LSB, L"제품 클래스코드", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDeviceClass);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bDeviceSubClass", HJ_VALUE_ENDIAN_LSB, L"제품 서브클래스코드", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDeviceSubClass);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bDeviceProtocol", HJ_VALUE_ENDIAN_LSB, L"제품 프로토콜코드", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDeviceProtocol);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bMaxPacketSize0", HJ_VALUE_ENDIAN_LSB, L"디폴트엔드포인트 MaxPacketSize", 0, 8, 0, sizeof(U8), &pDeviceDesc->bMaxPacketSize0);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"idVendor", HJ_VALUE_ENDIAN_LSB, L"제조사 ID", 0, 16, 0, sizeof(U16), &pDeviceDesc->idVendor);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"idProduct", HJ_VALUE_ENDIAN_LSB, L"제품 ID", 0, 16, 0, sizeof(U16), &pDeviceDesc->idProduct);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bcdDevice", HJ_VALUE_ENDIAN_LSB, L"디바이스 릴리즈 번호", 0, 16, 0, sizeof(U16), &pDeviceDesc->bcdDevice);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"iManufacturer", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->iManufacturer);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"iProduct", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->iProduct);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"iSerialNumber", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->iSerialNumber);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bNumConfigurations", HJ_VALUE_ENDIAN_LSB, L"지원하는 Config개수", 0, 8, 0, sizeof(U8), &pDeviceDesc->bNumConfigurations);
									break;

								case USB_CONFIGURATION_DESCRIPTOR_TYPE:
									pConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)pUSBCommonDesc;
									pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_CONFIGURATION_DESCRIPTOR"));

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pConfigDesc->bLength);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_CONFIGURATION_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pConfigDesc->bDescriptorType);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"wTotalLength", HJ_VALUE_ENDIAN_LSB, L"Config 디스크립터 총 바이트길이", 0, 16, 0, sizeof(U16), &pConfigDesc->wTotalLength);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bNumInterfaces", HJ_VALUE_ENDIAN_LSB, L"지원하는 인터페이스개수", 0, 8, 0, sizeof(U8), &pConfigDesc->bNumInterfaces);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bConfigurationValue", HJ_VALUE_ENDIAN_LSB, L"현재 Config 식별자", 0, 8, 0, sizeof(U8), &pConfigDesc->bConfigurationValue);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"iConfiguration", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pConfigDesc->iConfiguration);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

									translateConfigDescBmAttr(pConfigDesc->bmAttributes, pSpecificInfo->ConfigAtt);
									parserHlpCmn_FillField(pFieldValue, L"bmAttributes", HJ_VALUE_ENDIAN_LSB, pSpecificInfo->ConfigAtt, 0, 8, 0, sizeof(U8), &pConfigDesc->bmAttributes);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"MaxPower", HJ_VALUE_ENDIAN_LSB, L"Configured Power", 0, 8, 0, sizeof(U8), &pConfigDesc->MaxPower);

									break;
								case USB_STRING_DESCRIPTOR_TYPE:
									pStringDesc = (PUSB_STRING_DESCRIPTOR)pUSBCommonDesc;
									pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_STRING_DESCRIPTOR"));

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pStringDesc->bLength);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_STRING_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pStringDesc->bDescriptorType);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bString", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
									pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pStringDesc->bLength - 2;
									pFieldValue->nRowFieldBufferLength = (SIZE_T)pStringDesc->bLength - 2;
									pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pStringDesc->bLength - 2);
									memcpy(pFieldValue->nRowFieldBuffer, pStringDesc->bString, (SIZE_T)pStringDesc->bLength - 2);

									break;
								case USB_INTERFACE_DESCRIPTOR_TYPE:
									pInterfaceDesc = (PUSB_INTERFACE_DESCRIPTOR)pUSBCommonDesc;
									pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_INTERFACE_DESCRIPTOR"));

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pInterfaceDesc->bLength);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_INTERFACE_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bDescriptorType);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bInterfaceNumber", HJ_VALUE_ENDIAN_LSB, L"인터페이스 넘버", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceNumber);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bAlternateSetting", HJ_VALUE_ENDIAN_LSB, L"대체 인터페이스 넘버", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bAlternateSetting);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bNumEndpoints", HJ_VALUE_ENDIAN_LSB, L"포함하는 엔드포인트 개수", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bNumEndpoints);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bInterfaceClass", HJ_VALUE_ENDIAN_LSB, parserHlpCmn_findClass(pInterfaceDesc->bInterfaceClass), 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceClass);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bInterfaceSubClass", HJ_VALUE_ENDIAN_LSB, L"인터페이스 서브클래스", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceSubClass);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bInterfaceProtocol", HJ_VALUE_ENDIAN_LSB, L"인터페이스 프로토콜", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceProtocol);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"iInterface", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pInterfaceDesc->iInterface);

									break;
								case USB_ENDPOINT_DESCRIPTOR_TYPE:
									pEndpointDesc = (PUSB_ENDPOINT_DESCRIPTOR)pUSBCommonDesc;
									pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_ENDPOINT_DESCRIPTOR"));

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pEndpointDesc->bLength);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_ENDPOINT_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pEndpointDesc->bDescriptorType);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bEndpointAddress", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pEndpointDesc->bEndpointAddress);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

									newIndex = pEndpointDesc->bEndpointAddress & 0x0F;
									if (pEndpointDesc->bEndpointAddress & 0x80)
									{
										newIndex += 0x10;
									}
									translateEndpointDescBmAttr(pEndpointDesc->bmAttributes, &pSpecificInfo->EndpointAtt[newIndex][0]);
									parserHlpCmn_FillField(pFieldValue, L"bmAttributes", HJ_VALUE_ENDIAN_LSB, &pSpecificInfo->EndpointAtt[newIndex][0], 0, 8, 0, sizeof(U8), &pEndpointDesc->bmAttributes);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"wMaxPacketSize", HJ_VALUE_ENDIAN_LSB, L"엔드포인트 MaxPacketSize", 0, 16, 0, sizeof(U16), &pEndpointDesc->wMaxPacketSize);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bInterval", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pEndpointDesc->bInterval);
									break;
								default:
									pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_COMMON_DESCRIPTOR"));

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pUSBCommonDesc->bLength);

									pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
									parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pUSBCommonDesc->bDescriptorType);
									break;
								}
								pUSBCommonDesc = NextDescriptor(pUSBCommonDesc);
							}
						}
					}
				}
			}
		}
		break;
	case URB_FUNCTION_CONTROL_TRANSFER_EX:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_CONTROL_TRANSFER_EX"));

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"Timeout(ms)", HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource->stUrbControlTransferEx.u32Timeout);
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

		pPipeInformStr = parserHlpCmn_TranslatePipeHandle(pParset_Context, pSource->stUrbControlTransferEx.u64PipeHandle);
		if (pPipeInformStr)
		{
			parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB | HJ_VALUE_EXTRA_PRIVATEBUFF, pPipeInformStr, 0, 64, 0, sizeof(U64), &pSource->stUrbControlTransferEx.u64PipeHandle);
			pFieldValue->nRowFieldPrivateBufferMaxLength = 0;
			pFieldValue->nRowFieldPrivateBufferLength = _tcslen(pPipeInformStr) * sizeof(TCHAR);
			pFieldValue->nRowFieldPrivateBuffer = (U8 *)pPipeInformStr;
		}
		else
		{
			parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource->stUrbControlTransferEx.u64PipeHandle);
		}

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		translateUrbFlagString(pSource->stUrbControlTransferEx.u32TransferFlags, pSpecificInfo->UrbFlagString);
		parserHlpCmn_FillField(pFieldValue, L"TransferFlags", HJ_VALUE_ENDIAN_LSB, pSpecificInfo->UrbFlagString, 0, 32, 0, sizeof(U32), &pSource->stUrbControlTransferEx.u32TransferFlags);
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer);
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"SetupPacket", HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), pSource->stUrbControlTransferEx.u8SetupPacket);

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbControlTransferEx.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbControlTransferEx.stHeader.u32Status);

			if (pSource->stUrbControlTransferEx.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
				if (pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer)
				{
					// Data Output
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
					pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer);
					memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlTransferEx.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		else
		{
			if (pSource->stUrbControlTransferEx.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
			}
			else //OUT
			{
				if (pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer)
				{
					// Data Output
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
					pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer);
					memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlTransferEx.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		break;
	case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER"));

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

		pPipeInformStr = parserHlpCmn_TranslatePipeHandle(pParset_Context, pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle);
		if (pPipeInformStr)
		{
			parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB | HJ_VALUE_EXTRA_PRIVATEBUFF, pPipeInformStr, 0, 64, 0, sizeof(U64), &pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle);
			pFieldValue->nRowFieldPrivateBufferMaxLength = 0;
			pFieldValue->nRowFieldPrivateBufferLength = _tcslen(pPipeInformStr) * sizeof(TCHAR);
			pFieldValue->nRowFieldPrivateBuffer = (U8 *)pPipeInformStr;
		}
		else
		{
			parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource->stUrbBulkOrInterruptTransfer.u64PipeHandle);
		}

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		translateUrbFlagString(pSource->stUrbBulkOrInterruptTransfer.u32TransferFlags, pSpecificInfo->UrbFlagString);
		parserHlpCmn_FillField(pFieldValue, L"TransferFlags", HJ_VALUE_ENDIAN_LSB, pSpecificInfo->UrbFlagString, 0, 32, 0, sizeof(U32), &pSource->stUrbBulkOrInterruptTransfer.u32TransferFlags);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbBulkOrInterruptTransfer.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbBulkOrInterruptTransfer.stHeader.u32Status);

			if (pSource->stUrbBulkOrInterruptTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
				if (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer)
				{
					// Data Output
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
					pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);
					memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		else
		{

			if (pSource->stUrbBulkOrInterruptTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
			}
			else
			{
				if (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer)
				{
					// Data Output
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
					pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);
					memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		break;
	case URB_FUNCTION_ISOCH_TRANSFER:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_ISOCH_TRANSFER"));

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

		pPipeInformStr = parserHlpCmn_TranslatePipeHandle(pParset_Context, pSource->stUrbIsochronousTransfer.u64PipeHandle);
		if (pPipeInformStr)
		{
			parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB | HJ_VALUE_EXTRA_PRIVATEBUFF, pPipeInformStr, 0, 64, 0, sizeof(U64), &pSource->stUrbIsochronousTransfer.u64PipeHandle);
			pFieldValue->nRowFieldPrivateBufferMaxLength = 0;
			pFieldValue->nRowFieldPrivateBufferLength = _tcslen(pPipeInformStr) * sizeof(TCHAR);
			pFieldValue->nRowFieldPrivateBuffer = (U8 *)pPipeInformStr;
		}
		else
		{
			parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource->stUrbIsochronousTransfer.u64PipeHandle);
		}

		translateUrbFlagString(pSource->stUrbIsochronousTransfer.u32TransferFlags, pSpecificInfo->UrbFlagString);
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferFlags", HJ_VALUE_ENDIAN_LSB, pSpecificInfo->UrbFlagString, 0, 32, 0, sizeof(U32), &pSource->stUrbIsochronousTransfer.u32TransferFlags);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, _T("StartFrame"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource->stUrbIsochronousTransfer.u32StartFrame);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, _T("NumberOfPackets"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource->stUrbIsochronousTransfer.u32NumberOfPackets);

		// packets array
		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"Array", HJ_STRUCT_TYPE__SPLIT_ARRAY_BEGIN, NULL, 0, 32, 0, sizeof(U32), &pSource->stUrbIsochronousTransfer.u32NumberOfPackets);

		//			arraycontentcount = 3;

		// 이하의 필드는 자식의 성격을 가지고 있지만, 사실 그냥 나열해도 무방해보인다
		for (count = 0; count < (int)pSource->stUrbIsochronousTransfer.u32NumberOfPackets; count++)
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("Packets"), HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN, NULL, 0, 32, 0, sizeof(U32), &count);

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"Offset", HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource->stUrbIsochronousTransfer.stIsoPacket[count].Offset);

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"Length", HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource->stUrbIsochronousTransfer.stIsoPacket[count].Length);

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"USBStatus", HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbIsochronousTransfer.stIsoPacket[count].Status), 0, 32, 0, sizeof(U32), &pSource->stUrbIsochronousTransfer.stIsoPacket[count].Status);

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("Packets"), HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END, NULL, 0, 0, 0, 0, NULL);
		}

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, NULL, HJ_STRUCT_TYPE__SPLIT_ARRAY_END, NULL, 0, 0, 0, 0, NULL);

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbIsochronousTransfer.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbIsochronousTransfer.stHeader.u32Status);

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("ErrorCount"), HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource->stUrbIsochronousTransfer.u32ErrorCount);

			if (pSource->stUrbIsochronousTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
				if (pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer)
				{
					// Data Output
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
					pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer);
					memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbIsochronousTransfer.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		else // no completion(start)
		{
			if (pSource->stUrbIsochronousTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
			}
			else
			{
				if (pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer)
				{
					// Data Output
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
					pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer);
					memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbIsochronousTransfer.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		break;
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT:
		if (pSource->stUrbHeader.u16Function == URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT"));
		}

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		switch (pSource->stUrbControlDescriptorRequest.u8DescriptorType)
		{
		case USB_DEVICE_DESCRIPTOR_TYPE:
			parserHlpCmn_FillField(pFieldValue, L"Type", HJ_VALUE_ENDIAN_LSB, _T("USB_DEVICE_DESCRIPTOR_TYPE"), 0, 8, 0, sizeof(U8), &pSource->stUrbControlDescriptorRequest.u8DescriptorType);
			break;
		case USB_CONFIGURATION_DESCRIPTOR_TYPE:
			parserHlpCmn_FillField(pFieldValue, L"Type", HJ_VALUE_ENDIAN_LSB, _T("USB_CONFIGURATION_DESCRIPTOR_TYPE"), 0, 8, 0, sizeof(U8), &pSource->stUrbControlDescriptorRequest.u8DescriptorType);
			break;
		case USB_STRING_DESCRIPTOR_TYPE:
			parserHlpCmn_FillField(pFieldValue, L"Type", HJ_VALUE_ENDIAN_LSB, _T("USB_STRING_DESCRIPTOR_TYPE"), 0, 8, 0, sizeof(U8), &pSource->stUrbControlDescriptorRequest.u8DescriptorType);
			break;
		default:
			parserHlpCmn_FillField(pFieldValue, L"Type", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource->stUrbControlDescriptorRequest.u8DescriptorType);
			break;
		}

		if (pSource->stUrbControlDescriptorRequest.u8DescriptorType != USB_DEVICE_DESCRIPTOR_TYPE)
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"Index", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource->stUrbControlDescriptorRequest.u8Index);
		}

		if (pSource->stUrbControlDescriptorRequest.u8DescriptorType == USB_STRING_DESCRIPTOR_TYPE)
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"LanguageId", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource->stUrbControlDescriptorRequest.u16LanguageId);
		}

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbControlDescriptorRequest.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbControlDescriptorRequest.stHeader.u32Status);

			if (pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer)
			{
				// Data Output
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
				pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer);
				memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer);

				// Descriptor View Node 생성
				// Desc 개수를 먼저 확인한다
				{
					pUSBCommonDesc = (USB_COMMON_DESCRIPTOR *)pFieldValue->nRowFieldBuffer;
					pDeviceDesc = (PUSB_DEVICE_DESCRIPTOR)pUSBCommonDesc;
					pConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)pUSBCommonDesc;
					pInterfaceDesc = (PUSB_INTERFACE_DESCRIPTOR)pUSBCommonDesc;
					pEndpointDesc = (PUSB_ENDPOINT_DESCRIPTOR)pUSBCommonDesc;
					pStringDesc = (PUSB_STRING_DESCRIPTOR)pUSBCommonDesc;

					nDesc = CountOfDescriptor(pUSBCommonDesc, (SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer);
					for (count = 0; count < (int)nDesc; count++)
					{
						switch (pUSBCommonDesc->bDescriptorType)
						{
						case USB_DEVICE_DESCRIPTOR_TYPE:
							pDeviceDesc = (PUSB_DEVICE_DESCRIPTOR)pUSBCommonDesc;
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_DEVICE_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_DEVICE_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDescriptorType);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bcdUSB", HJ_VALUE_ENDIAN_LSB, L"Version", 0, 16, 0, sizeof(U16), &pDeviceDesc->bcdUSB);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDeviceClass", HJ_VALUE_ENDIAN_LSB, L"제품 클래스코드", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDeviceClass);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDeviceSubClass", HJ_VALUE_ENDIAN_LSB, L"제품 서브클래스코드", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDeviceSubClass);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDeviceProtocol", HJ_VALUE_ENDIAN_LSB, L"제품 프로토콜코드", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDeviceProtocol);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bMaxPacketSize0", HJ_VALUE_ENDIAN_LSB, L"디폴트엔드포인트 MaxPacketSize", 0, 8, 0, sizeof(U8), &pDeviceDesc->bMaxPacketSize0);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"idVendor", HJ_VALUE_ENDIAN_LSB, L"제조사 ID", 0, 16, 0, sizeof(U16), &pDeviceDesc->idVendor);
							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"idProduct", HJ_VALUE_ENDIAN_LSB, L"제품 ID", 0, 16, 0, sizeof(U16), &pDeviceDesc->idProduct);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bcdDevice", HJ_VALUE_ENDIAN_LSB, L"디바이스 릴리즈 번호", 0, 16, 0, sizeof(U16), &pDeviceDesc->bcdDevice);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"iManufacturer", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->iManufacturer);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"iProduct", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->iProduct);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"iSerialNumber", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->iSerialNumber);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bNumConfigurations", HJ_VALUE_ENDIAN_LSB, L"지원하는 Config개수", 0, 8, 0, sizeof(U8), &pDeviceDesc->bNumConfigurations);
							break;

						case USB_CONFIGURATION_DESCRIPTOR_TYPE:
							pConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)pUSBCommonDesc;
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_CONFIGURATION_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pConfigDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_CONFIGURATION_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pConfigDesc->bDescriptorType);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"wTotalLength", HJ_VALUE_ENDIAN_LSB, L"Config 디스크립터 총 바이트길이", 0, 16, 0, sizeof(U16), &pConfigDesc->wTotalLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bNumInterfaces", HJ_VALUE_ENDIAN_LSB, L"지원하는 인터페이스개수", 0, 8, 0, sizeof(U8), &pConfigDesc->bNumInterfaces);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bConfigurationValue", HJ_VALUE_ENDIAN_LSB, L"현재 Config 식별자", 0, 8, 0, sizeof(U8), &pConfigDesc->bConfigurationValue);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"iConfiguration", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pConfigDesc->iConfiguration);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							translateConfigDescBmAttr(pConfigDesc->bmAttributes, pSpecificInfo->ConfigAtt);
							parserHlpCmn_FillField(pFieldValue, L"bmAttributes", HJ_VALUE_ENDIAN_LSB, pSpecificInfo->ConfigAtt, 0, 8, 0, sizeof(U8), &pConfigDesc->bmAttributes);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"MaxPower", HJ_VALUE_ENDIAN_LSB, L"Configured Power", 0, 8, 0, sizeof(U8), &pConfigDesc->MaxPower);

							break;
						case USB_STRING_DESCRIPTOR_TYPE:
							pStringDesc = (PUSB_STRING_DESCRIPTOR)pUSBCommonDesc;
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_STRING_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pStringDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_STRING_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pStringDesc->bDescriptorType);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bString", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
							pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pStringDesc->bLength - 2;
							pFieldValue->nRowFieldBufferLength = (SIZE_T)pStringDesc->bLength - 2;
							pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pStringDesc->bLength - 2);
							memcpy(pFieldValue->nRowFieldBuffer, pStringDesc->bString, (SIZE_T)pStringDesc->bLength - 2);

							break;
						case USB_INTERFACE_DESCRIPTOR_TYPE:
							pInterfaceDesc = (PUSB_INTERFACE_DESCRIPTOR)pUSBCommonDesc;
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_INTERFACE_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pInterfaceDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_INTERFACE_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bDescriptorType);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bInterfaceNumber", HJ_VALUE_ENDIAN_LSB, L"인터페이스 넘버", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceNumber);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bAlternateSetting", HJ_VALUE_ENDIAN_LSB, L"대체 인터페이스 넘버", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bAlternateSetting);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bNumEndpoints", HJ_VALUE_ENDIAN_LSB, L"포함하는 엔드포인트 개수", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bNumEndpoints);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bInterfaceClass", HJ_VALUE_ENDIAN_LSB, parserHlpCmn_findClass(pInterfaceDesc->bInterfaceClass), 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceClass);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bInterfaceSubClass", HJ_VALUE_ENDIAN_LSB, L"인터페이스 서브클래스", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceSubClass);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bInterfaceProtocol", HJ_VALUE_ENDIAN_LSB, L"인터페이스 프로토콜", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceProtocol);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"iInterface", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pInterfaceDesc->iInterface);

							break;
						case USB_ENDPOINT_DESCRIPTOR_TYPE:
							pEndpointDesc = (PUSB_ENDPOINT_DESCRIPTOR)pUSBCommonDesc;
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_ENDPOINT_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pEndpointDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_ENDPOINT_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pEndpointDesc->bDescriptorType);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bEndpointAddress", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pEndpointDesc->bEndpointAddress);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							newIndex = pEndpointDesc->bEndpointAddress & 0x0F;
							if (pEndpointDesc->bEndpointAddress & 0x80)
							{
								newIndex += 0x10;
							}
							translateEndpointDescBmAttr(pEndpointDesc->bmAttributes, &pSpecificInfo->EndpointAtt[newIndex][0]);
							parserHlpCmn_FillField(pFieldValue, L"bmAttributes", HJ_VALUE_ENDIAN_LSB, &pSpecificInfo->EndpointAtt[newIndex][0], 0, 8, 0, sizeof(U8), &pEndpointDesc->bmAttributes);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"wMaxPacketSize", HJ_VALUE_ENDIAN_LSB, L"엔드포인트 MaxPacketSize", 0, 16, 0, sizeof(U16), &pEndpointDesc->wMaxPacketSize);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bInterval", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pEndpointDesc->bInterval);
							break;
						default:
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_COMMON_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pUSBCommonDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pUSBCommonDesc->bDescriptorType);
							break;
						}
						pUSBCommonDesc = NextDescriptor(pUSBCommonDesc);
					}
				}
			}
		}
		break;

	case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT:
		if (pSource->stUrbHeader.u16Function == URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT"));
		}

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"Type", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource->stUrbControlDescriptorRequest.u8DescriptorType);

		if (pSource->stUrbControlDescriptorRequest.u8DescriptorType != USB_DEVICE_DESCRIPTOR_TYPE)
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"Index", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource->stUrbControlDescriptorRequest.u8Index);
		}

		if (pSource->stUrbControlDescriptorRequest.u8DescriptorType == USB_STRING_DESCRIPTOR_TYPE)
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"LanguageId", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource->stUrbControlDescriptorRequest.u16LanguageId);
		}

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbControlDescriptorRequest.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbControlDescriptorRequest.stHeader.u32Status);
		}
		else // start
		{
			if (pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer)
			{
				// Data Output
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
				pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer);
				memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer);

				// Descriptor View Node 생성
				// Desc 개수를 먼저 확인한다
				{
					pUSBCommonDesc = (USB_COMMON_DESCRIPTOR *)pFieldValue->nRowFieldBuffer;
					pDeviceDesc = (PUSB_DEVICE_DESCRIPTOR)pUSBCommonDesc;
					pConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)pUSBCommonDesc;
					pInterfaceDesc = (PUSB_INTERFACE_DESCRIPTOR)pUSBCommonDesc;
					pEndpointDesc = (PUSB_ENDPOINT_DESCRIPTOR)pUSBCommonDesc;
					pStringDesc = (PUSB_STRING_DESCRIPTOR)pUSBCommonDesc;

					nDesc = CountOfDescriptor(pUSBCommonDesc, (SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer);
					for (count = 0; count < (int)nDesc; count++)
					{
						switch (pUSBCommonDesc->bDescriptorType)
						{
						case USB_DEVICE_DESCRIPTOR_TYPE:
							pDeviceDesc = (PUSB_DEVICE_DESCRIPTOR)pUSBCommonDesc;
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_DEVICE_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_DEVICE_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDescriptorType);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bcdUSB", HJ_VALUE_ENDIAN_LSB, L"Version", 0, 16, 0, sizeof(U16), &pDeviceDesc->bcdUSB);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDeviceClass", HJ_VALUE_ENDIAN_LSB, L"제품 클래스코드", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDeviceClass);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDeviceSubClass", HJ_VALUE_ENDIAN_LSB, L"제품 서브클래스코드", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDeviceSubClass);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDeviceProtocol", HJ_VALUE_ENDIAN_LSB, L"제품 프로토콜코드", 0, 8, 0, sizeof(U8), &pDeviceDesc->bDeviceProtocol);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bMaxPacketSize0", HJ_VALUE_ENDIAN_LSB, L"디폴트엔드포인트 MaxPacketSize", 0, 8, 0, sizeof(U8), &pDeviceDesc->bMaxPacketSize0);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"idVendor", HJ_VALUE_ENDIAN_LSB, L"제조사 ID", 0, 16, 0, sizeof(U16), &pDeviceDesc->idVendor);
							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"idProduct", HJ_VALUE_ENDIAN_LSB, L"제품 ID", 0, 16, 0, sizeof(U16), &pDeviceDesc->idProduct);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bcdDevice", HJ_VALUE_ENDIAN_LSB, L"디바이스 릴리즈 번호", 0, 16, 0, sizeof(U16), &pDeviceDesc->bcdDevice);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"iManufacturer", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->iManufacturer);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"iProduct", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->iProduct);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"iSerialNumber", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pDeviceDesc->iSerialNumber);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bNumConfigurations", HJ_VALUE_ENDIAN_LSB, L"지원하는 Config개수", 0, 8, 0, sizeof(U8), &pDeviceDesc->bNumConfigurations);
							break;

						case USB_CONFIGURATION_DESCRIPTOR_TYPE:
							pConfigDesc = (PUSB_CONFIGURATION_DESCRIPTOR)pUSBCommonDesc;
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_CONFIGURATION_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pConfigDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_CONFIGURATION_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pConfigDesc->bDescriptorType);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"wTotalLength", HJ_VALUE_ENDIAN_LSB, L"Config 디스크립터 총 바이트길이", 0, 16, 0, sizeof(U16), &pConfigDesc->wTotalLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bNumInterfaces", HJ_VALUE_ENDIAN_LSB, L"지원하는 인터페이스개수", 0, 8, 0, sizeof(U8), &pConfigDesc->bNumInterfaces);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bConfigurationValue", HJ_VALUE_ENDIAN_LSB, L"현재 Config 식별자", 0, 8, 0, sizeof(U8), &pConfigDesc->bConfigurationValue);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"iConfiguration", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pConfigDesc->iConfiguration);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							translateConfigDescBmAttr(pConfigDesc->bmAttributes, pSpecificInfo->ConfigAtt);
							parserHlpCmn_FillField(pFieldValue, L"bmAttributes", HJ_VALUE_ENDIAN_LSB, pSpecificInfo->ConfigAtt, 0, 8, 0, sizeof(U8), &pConfigDesc->bmAttributes);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"MaxPower", HJ_VALUE_ENDIAN_LSB, L"Configured Power", 0, 8, 0, sizeof(U8), &pConfigDesc->MaxPower);

							break;
						case USB_STRING_DESCRIPTOR_TYPE:
							pStringDesc = (PUSB_STRING_DESCRIPTOR)pUSBCommonDesc;
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_STRING_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pStringDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_STRING_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pStringDesc->bDescriptorType);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bString", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
							pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pStringDesc->bLength - 2;
							pFieldValue->nRowFieldBufferLength = (SIZE_T)pStringDesc->bLength - 2;
							pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pStringDesc->bLength - 2);
							memcpy(pFieldValue->nRowFieldBuffer, pStringDesc->bString, (SIZE_T)pStringDesc->bLength - 2);

							break;
						case USB_INTERFACE_DESCRIPTOR_TYPE:
							pInterfaceDesc = (PUSB_INTERFACE_DESCRIPTOR)pUSBCommonDesc;
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_INTERFACE_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pInterfaceDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_INTERFACE_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bDescriptorType);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bInterfaceNumber", HJ_VALUE_ENDIAN_LSB, L"인터페이스 넘버", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceNumber);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bAlternateSetting", HJ_VALUE_ENDIAN_LSB, L"대체 인터페이스 넘버", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bAlternateSetting);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bNumEndpoints", HJ_VALUE_ENDIAN_LSB, L"포함하는 엔드포인트 개수", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bNumEndpoints);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bInterfaceClass", HJ_VALUE_ENDIAN_LSB, parserHlpCmn_findClass(pInterfaceDesc->bInterfaceClass), 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceClass);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bInterfaceSubClass", HJ_VALUE_ENDIAN_LSB, L"인터페이스 서브클래스", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceSubClass);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bInterfaceProtocol", HJ_VALUE_ENDIAN_LSB, L"인터페이스 프로토콜", 0, 8, 0, sizeof(U8), &pInterfaceDesc->bInterfaceProtocol);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"iInterface", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pInterfaceDesc->iInterface);

							break;
						case USB_ENDPOINT_DESCRIPTOR_TYPE:
							pEndpointDesc = (PUSB_ENDPOINT_DESCRIPTOR)pUSBCommonDesc;
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_ENDPOINT_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pEndpointDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, L"USB_ENDPOINT_DESCRIPTOR_TYPE", 0, 8, 0, sizeof(U8), &pEndpointDesc->bDescriptorType);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bEndpointAddress", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pEndpointDesc->bEndpointAddress);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							newIndex = pEndpointDesc->bEndpointAddress & 0x0F;
							if (pEndpointDesc->bEndpointAddress & 0x80)
							{
								newIndex += 0x10;
							}
							translateEndpointDescBmAttr(pEndpointDesc->bmAttributes, &pSpecificInfo->EndpointAtt[newIndex][0]);
							parserHlpCmn_FillField(pFieldValue, L"bmAttributes", HJ_VALUE_ENDIAN_LSB, &pSpecificInfo->EndpointAtt[newIndex][0], 0, 8, 0, sizeof(U8), &pEndpointDesc->bmAttributes);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"wMaxPacketSize", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pEndpointDesc->wMaxPacketSize);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bInterval", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pEndpointDesc->bInterval);
							break;
						default:
							pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("USB_COMMON_DESCRIPTOR"));

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bLength", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pUSBCommonDesc->bLength);

							pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
							parserHlpCmn_FillField(pFieldValue, L"bDescriptorType", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pUSBCommonDesc->bDescriptorType);
							break;
						}
						pUSBCommonDesc = NextDescriptor(pUSBCommonDesc);
					}
				}
			}
		}
		break;

	case URB_FUNCTION_GET_STATUS_FROM_DEVICE:
	case URB_FUNCTION_GET_STATUS_FROM_INTERFACE:
	case URB_FUNCTION_GET_STATUS_FROM_ENDPOINT:
		if (pSource->stUrbHeader.u16Function == URB_FUNCTION_GET_STATUS_FROM_DEVICE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_GET_STATUS_FROM_DEVICE"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_GET_STATUS_FROM_INTERFACE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_GET_STATUS_FROM_INTERFACE"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_GET_STATUS_FROM_ENDPOINT)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_GET_STATUS_FROM_ENDPOINT"));
		}

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64LengthBuffer);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"Index", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource->stUrbControlGetStatusRequest.u16Index);

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbControlDescriptorRequest.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbControlDescriptorRequest.stHeader.u32Status);

			if (pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64LengthBuffer)
			{
				// Data Output
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
				pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64LengthBuffer);
				memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlGetStatusRequest.stTransferBuffer.u64LengthBuffer);
			}
		}

		break;
	case URB_FUNCTION_SET_FEATURE_TO_DEVICE:
	case URB_FUNCTION_SET_FEATURE_TO_INTERFACE:
	case URB_FUNCTION_SET_FEATURE_TO_ENDPOINT:
	case URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE:
	case URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE:
	case URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT:
		bFeatureSelect_Device = FALSE;
		bFeatureSelect_Interface = FALSE;
		bFeatureSelect_Endpoint = FALSE;

		if (pSource->stUrbHeader.u16Function == URB_FUNCTION_SET_FEATURE_TO_DEVICE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_SET_FEATURE_TO_DEVICE"));
			bFeatureSelect_Device = TRUE;
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_SET_FEATURE_TO_INTERFACE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_SET_FEATURE_TO_INTERFACE"));
			bFeatureSelect_Interface = TRUE;
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_SET_FEATURE_TO_ENDPOINT)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_SET_FEATURE_TO_ENDPOINT"));
			bFeatureSelect_Endpoint = TRUE;
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE"));
			bFeatureSelect_Device = TRUE;
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE"));
			bFeatureSelect_Interface = TRUE;
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT"));
			bFeatureSelect_Endpoint = TRUE;
		}

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"Index", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource->stUrbControlFeatureRequest.u16Index);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"FeatureSelector", HJ_VALUE_ENDIAN_LSB,
			translateFeatureSelector(pSource->stUrbControlFeatureRequest.u16FeatureSelector, bFeatureSelect_Device, bFeatureSelect_Interface, bFeatureSelect_Endpoint)
			, 0, 16, 0, sizeof(U16), &pSource->stUrbControlFeatureRequest.u16FeatureSelector);

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbControlFeatureRequest.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbControlFeatureRequest.stHeader.u32Status);
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
		if (pSource->stUrbHeader.u16Function == URB_FUNCTION_VENDOR_DEVICE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_VENDOR_DEVICE"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_VENDOR_INTERFACE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_VENDOR_INTERFACE"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_VENDOR_ENDPOINT)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_VENDOR_ENDPOINT"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_VENDOR_OTHER)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_VENDOR_OTHER"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_CLASS_DEVICE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_CLASS_DEVICE"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_CLASS_INTERFACE)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_CLASS_INTERFACE"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_CLASS_ENDPOINT)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_CLASS_ENDPOINT"));
		}
		else if (pSource->stUrbHeader.u16Function == URB_FUNCTION_CLASS_OTHER)
		{
			pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_CLASS_OTHER"));
		}

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		translateUrbFlagString(pSource->stUrbControlVendorOrClassRequest.u32TransferFlags, pSpecificInfo->UrbFlagString);
		parserHlpCmn_FillField(pFieldValue, L"TransferFlags", HJ_VALUE_ENDIAN_LSB, pSpecificInfo->UrbFlagString, 0, 32, 0, sizeof(U32), &pSource->stUrbControlVendorOrClassRequest.u32TransferFlags);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"Request", HJ_VALUE_ENDIAN_LSB, L"Unknown", 0, 8, 0, sizeof(U8), &pSource->stUrbControlVendorOrClassRequest.u8Request);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"Index", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource->stUrbControlVendorOrClassRequest.u16Index);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"Value", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource->stUrbControlVendorOrClassRequest.u16Value);

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbControlVendorOrClassRequest.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbControlVendorOrClassRequest.stHeader.u32Status);

			if (pSource->stUrbControlVendorOrClassRequest.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
				if (pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer)
				{
					// Data Output
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
					pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer);
					memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		else
		{ // START
			if (pSource->stUrbControlVendorOrClassRequest.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN)
			{
			}
			else
			{
				if (pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer)
				{
					// Data Output
					pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
					parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
					pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer;
					pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer);
					memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer);
				}
			}
		}
		break;

	case URB_FUNCTION_GET_INTERFACE:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_GET_INTERFACE"));

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64LengthBuffer);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"Interface", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource->stUrbControlGetInterfaceRequest.u16Interface);

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbControlGetInterfaceRequest.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbControlGetInterfaceRequest.stHeader.u32Status);

			if (pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64LengthBuffer)
			{
				// Data Output
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
				pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64LengthBuffer);
				memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlGetInterfaceRequest.stTransferBuffer.u64LengthBuffer);
			}
		}
		break;
	case URB_FUNCTION_GET_CONFIGURATION:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_GET_CONFIGURATION"));

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbControlGetConfigurationRequest.stTransferBuffer.u64LengthBuffer);

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbControlGetConfigurationRequest.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbControlGetConfigurationRequest.stHeader.u32Status);

			if (pSource->stUrbControlGetConfigurationRequest.stTransferBuffer.u64LengthBuffer)
			{
				// Data Output
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
				pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbControlGetConfigurationRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbControlGetConfigurationRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbControlGetConfigurationRequest.stTransferBuffer.u64LengthBuffer);
				memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbControlGetConfigurationRequest.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbControlGetConfigurationRequest.stTransferBuffer.u64LengthBuffer);
			}
		}
		break;
	case URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR"));

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"TransferBufferLength", HJ_VALUE_ENDIAN_LSB, L"요청크기 또는 수행크기", 0, 64, 0, sizeof(U64), &pSource->stUrbOSFeatureDescriptorRequest.stTransferBuffer.u64LengthBuffer);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"Recipient", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource->stUrbOSFeatureDescriptorRequest.u8Recipient);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"InterfaceNumber", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource->stUrbOSFeatureDescriptorRequest.u8InterfaceNumber);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"MS_PageIndex", HJ_VALUE_ENDIAN_LSB, NULL, 0, 8, 0, sizeof(U8), &pSource->stUrbOSFeatureDescriptorRequest.u8MS_PageIndex);

		pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
		parserHlpCmn_FillField(pFieldValue, L"MS_FeatureDescriptorIndex", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource->stUrbOSFeatureDescriptorRequest.u16MS_FeatureDescriptorIndex);

		if (bCompletion == TRUE) // 완료
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbOSFeatureDescriptorRequest.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbOSFeatureDescriptorRequest.stHeader.u32Status);

			if (pSource->stUrbOSFeatureDescriptorRequest.stTransferBuffer.u64LengthBuffer)
			{
				// Data Output
				pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
				parserHlpCmn_FillField(pFieldValue, L"TransferBuffer", HJ_VALUE_EXTRA_BUFF, NULL, 0, 0, 0, 0, NULL);
				pFieldValue->nRowFieldBufferMaxLength = (SIZE_T)pSource->stUrbOSFeatureDescriptorRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBufferLength = (SIZE_T)pSource->stUrbOSFeatureDescriptorRequest.stTransferBuffer.u64LengthBuffer;
				pFieldValue->nRowFieldBuffer = (U8 *)malloc((SIZE_T)pSource->stUrbOSFeatureDescriptorRequest.stTransferBuffer.u64LengthBuffer);
				memcpy(pFieldValue->nRowFieldBuffer, (U8*)pSource + pSource->stUrbOSFeatureDescriptorRequest.stTransferBuffer.u64OffsetBuffer, (SIZE_T)pSource->stUrbOSFeatureDescriptorRequest.stTransferBuffer.u64LengthBuffer);
			}
		}
		break;
	case URB_FUNCTION_OPEN_STATIC_STREAMS:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("URB_FUNCTION_OPEN_STATIC_STREAMS"));

		if (bCompletion == TRUE)
		{
			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, _T("u32Status"), HJ_VALUE_ENDIAN_LSB, parserHlpCmn_translateURBStatus((USBD_STATUS)pSource->stUrbOpenStaticStreams.stHeader.u32Status), 0, 32, 0, sizeof(U32), &pSource->stUrbOpenStaticStreams.stHeader.u32Status);

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);

			pPipeInformStr = parserHlpCmn_TranslatePipeHandle(pParset_Context, pSource->stUrbOpenStaticStreams.u64PipeHandle);
			if (pPipeInformStr)
			{
				parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB | HJ_VALUE_EXTRA_PRIVATEBUFF, pPipeInformStr, 0, 64, 0, sizeof(U64), &pSource->stUrbOpenStaticStreams.u64PipeHandle);
				pFieldValue->nRowFieldPrivateBufferMaxLength = 0;
				pFieldValue->nRowFieldPrivateBufferLength = _tcslen(pPipeInformStr) * sizeof(TCHAR);
				pFieldValue->nRowFieldPrivateBuffer = (U8 *)pPipeInformStr;
			}
			else
			{
				parserHlpCmn_FillField(pFieldValue, L"PipeHandle", HJ_VALUE_ENDIAN_LSB, NULL, 0, 64, 0, sizeof(U64), &pSource->stUrbOpenStaticStreams.u64PipeHandle);
			}

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"NumberOfStreams", HJ_VALUE_ENDIAN_LSB, NULL, 0, 32, 0, sizeof(U32), &pSource->stUrbOpenStaticStreams.u32NumberOfStreams);

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"StreamInfoVersion", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource->stUrbOpenStaticStreams.u16StreamInfoVersion);

			pFieldValue = parserHlpCmn_CreateAndLinkParsingValueToParsingField(pValue);
			parserHlpCmn_FillField(pFieldValue, L"StreamInfoSize", HJ_VALUE_ENDIAN_LSB, NULL, 0, 16, 0, sizeof(U16), &pSource->stUrbOpenStaticStreams.u16StreamInfoSize);
		}
		break;
	default:
		pValue = parserHlpCmn_CreateAndLinkParsingDataToParsingValue(pData, _T("UNKNOWN"));
		break;
	}

	return pData;
}

PHJ_PARSING_DATA_FOR_DATA parserHlpCmn_FillDataPages(PPARSER_CONTEXT pParset_Context, PST_CF_IRP_URB pSource, SIZE_T	Level)
{
	BOOLEAN bCompletion = FALSE;
	PHJ_PARSING_DATA_FOR_DATA pData = NULL;

	if (pSource->stUrbHeader.stHeader.u8bCompletion == 1)
	{
		bCompletion = TRUE;
	}

	switch (pSource->stUrbHeader.u16Function)
	{
	case URB_FUNCTION_CONTROL_TRANSFER:
		switch (Level)
		{
		case 0:
			if ((bCompletion == TRUE) && (pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
			else if ((bCompletion == FALSE) && !(pSource->stUrbControlTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbControlTransfer.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbControlTransfer.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
			break;
		case 1:
			pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
			pData->bIsConstant = FALSE;
			pData->Size = 8;
			pData->pDataBuffer = malloc(pData->Size);
			memcpy(pData->pDataBuffer, pSource->stUrbControlTransfer.u8SetupPacket, pData->Size);
			break;
		}
		break;
	case URB_FUNCTION_CONTROL_TRANSFER_EX:
		switch (Level)
		{
		case 0:
			if ((bCompletion == TRUE) && (pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbControlTransferEx.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
			else if ((bCompletion == FALSE) && !(pSource->stUrbControlTransferEx.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbControlTransferEx.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbControlTransferEx.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
			break;
		case 1:
			pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
			pData->bIsConstant = FALSE;
			pData->Size = 8;
			pData->pDataBuffer = malloc(pData->Size);
			memcpy(pData->pDataBuffer, pSource->stUrbControlTransferEx.u8SetupPacket, pData->Size);
			break;
		}
		break;
	case URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER:
		switch (Level)
		{
		case 0:
		case 1:
			if ((bCompletion == TRUE) && (pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
			else if ((bCompletion == FALSE) && !(pSource->stUrbBulkOrInterruptTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbBulkOrInterruptTransfer.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
		}
		break;
	case URB_FUNCTION_ISOCH_TRANSFER:
		switch (Level)
		{
		case 0:
		case 1:
			if ((bCompletion == TRUE) && (pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbIsochronousTransfer.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
			else if ((bCompletion == FALSE) && !(pSource->stUrbIsochronousTransfer.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbIsochronousTransfer.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbIsochronousTransfer.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
		}
		break;
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE:
	case URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE:
	case URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT:
		switch (Level)
		{
		case 0:
		case 1:
			if ((bCompletion == TRUE) && (pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbControlDescriptorRequest.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
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
		switch (Level)
		{
		case 0:
		case 1:
			if ((bCompletion == TRUE) && (pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
			else if ((bCompletion == FALSE) && !(pSource->stUrbControlVendorOrClassRequest.u32TransferFlags & USBD_TRANSFER_DIRECTION_IN))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbControlVendorOrClassRequest.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
		}
		break;
	case URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR:
		switch (Level)
		{
		case 0:
			//			break;
		case 1:
			if ((bCompletion == TRUE) && (pSource->stUrbOSFeatureDescriptorRequest.stTransferBuffer.u64LengthBuffer))
			{
				pData = (PHJ_PARSING_DATA_FOR_DATA)malloc(sizeof(HJ_PARSING_DATA_FOR_DATA));
				pData->bIsConstant = FALSE;
				pData->Size = (SIZE_T)pSource->stUrbOSFeatureDescriptorRequest.stTransferBuffer.u64LengthBuffer;
				pData->pDataBuffer = malloc(pData->Size);
				memcpy(pData->pDataBuffer, (U8*)pSource + pSource->stUrbOSFeatureDescriptorRequest.stTransferBuffer.u64OffsetBuffer, pData->Size);
				break;
			}
		}
		break;
	}

	return pData;
}

WCHAR * parserHlpCmn_translateString(const WCHAR * Prefix, unsigned char * pSrc, U64 Bytes, U64 MaxBytes)
{
	WCHAR * pDes = NULL;
	WCHAR wData[3] = { 0, };
	int count = 0;
	WCHAR Upper, Lower;
	SIZE_T strsize = 0;
	SIZE_T allocsize = 0;
	BOOLEAN bOmitted = FALSE;

	allocsize = 4;

	if (Prefix)
	{
		allocsize += (wcslen(Prefix) * sizeof(WCHAR) + 2);
	}

	if (MaxBytes < Bytes)
	{
		Bytes = MaxBytes - 2;
		bOmitted = TRUE;
	}
	allocsize += (SIZE_T)(Bytes * 3 * sizeof(WCHAR));

	pDes = (WCHAR *)malloc(allocsize);
	memset(pDes, 0, allocsize);
	if (Prefix)
	{
		wcscpy(pDes, Prefix);
		wsprintf(pDes, L"%ws ", pDes);
		strsize = wcslen(pDes);
	}

	for (count = 0; count < (int)Bytes; count++)
	{
		Upper = (pSrc[count] & 0xF0) >> 4;
		Lower = pSrc[count] & 0x0F;
		if (Upper>9)
		{
			Upper = L'A' + (Upper - 10);
		}
		else
		{
			Upper = L'0' + Upper;
		}

		if (Lower>9)
		{
			Lower = L'A' + (Lower - 10);
		}
		else
		{
			Lower = L'0' + Lower;
		}
		pDes[strsize] = Upper;
		strsize++;
		pDes[strsize] = Lower;
		strsize++;
		if ((count + 1) >= (int)Bytes)
			break;
		pDes[strsize] = L' ';
		strsize++;
	}

	if (bOmitted == TRUE)
	{
		wsprintf(pDes, L"%ws..", pDes);
	}
	return pDes;
}

