#pragma once

#include <tchar.h>
#include <initguid.h>

#include "common.h"
#include "cf_irp_urb.h"
#include <winioctl.h>

#pragma pack(1)

#define NTSTATUS U32
#define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005L)
#define STATUS_DEVICE_POWERED_OFF        ((NTSTATUS)0x8000000FL)
#define STATUS_DEVICE_BUSY               ((NTSTATUS)0x80000011L)
#define STATUS_BUS_RESET                 ((NTSTATUS)0x8000001DL)
#define STATUS_UNSUCCESSFUL              ((NTSTATUS)0xC0000001L)
#define STATUS_NO_SUCH_DEVICE            ((NTSTATUS)0xC000000EL)
#define STATUS_INVALID_DEVICE_REQUEST    ((NTSTATUS)0xC0000010L)
#define STATUS_MORE_PROCESSING_REQUIRED  ((NTSTATUS)0xC0000016L)
#define STATUS_ACCESS_DENIED             ((NTSTATUS)0xC0000022L)
#define STATUS_BUFFER_TOO_SMALL          ((NTSTATUS)0xC0000023L)
#define STATUS_PORT_DISCONNECTED         ((NTSTATUS)0xC0000037L)
#define STATUS_DATA_OVERRUN              ((NTSTATUS)0xC000003CL)
#define STATUS_DATA_ERROR                ((NTSTATUS)0xC000003EL)
#define STATUS_CRC_ERROR                 ((NTSTATUS)0xC000003FL)
#define STATUS_SHARING_VIOLATION         ((NTSTATUS)0xC0000043L)
#define STATUS_DELETE_PENDING            ((NTSTATUS)0xC0000056L)
#define STATUS_CANCELLED                 ((NTSTATUS)0xC0000120L)

#define IRP_MJ_CREATE                   0x00
#define IRP_MJ_CREATE_NAMED_PIPE        0x01
#define IRP_MJ_CLOSE                    0x02
#define IRP_MJ_READ                     0x03
#define IRP_MJ_WRITE                    0x04
#define IRP_MJ_QUERY_INFORMATION        0x05
#define IRP_MJ_SET_INFORMATION          0x06
#define IRP_MJ_QUERY_EA                 0x07
#define IRP_MJ_SET_EA                   0x08
#define IRP_MJ_FLUSH_BUFFERS            0x09
#define IRP_MJ_QUERY_VOLUME_INFORMATION 0x0a
#define IRP_MJ_SET_VOLUME_INFORMATION   0x0b
#define IRP_MJ_DIRECTORY_CONTROL        0x0c
#define IRP_MJ_FILE_SYSTEM_CONTROL      0x0d
#define IRP_MJ_DEVICE_CONTROL           0x0e
#define IRP_MJ_INTERNAL_DEVICE_CONTROL  0x0f
#define IRP_MJ_SHUTDOWN                 0x10
#define IRP_MJ_LOCK_CONTROL             0x11
#define IRP_MJ_CLEANUP                  0x12
#define IRP_MJ_CREATE_MAILSLOT          0x13
#define IRP_MJ_QUERY_SECURITY           0x14
#define IRP_MJ_SET_SECURITY             0x15
#define IRP_MJ_POWER                    0x16
#define IRP_MJ_SYSTEM_CONTROL           0x17
#define IRP_MJ_DEVICE_CHANGE            0x18
#define IRP_MJ_QUERY_QUOTA              0x19
#define IRP_MJ_SET_QUOTA                0x1a
#define IRP_MJ_PNP                      0x1b

#define IRP_MJ_SCSI                     IRP_MJ_INTERNAL_DEVICE_CONTROL

#define IRP_MN_SCSI_CLASS               0x01

//
// PNP minor function codes.
//

#define IRP_MN_START_DEVICE                 0x00
#define IRP_MN_QUERY_REMOVE_DEVICE          0x01
#define IRP_MN_REMOVE_DEVICE                0x02
#define IRP_MN_CANCEL_REMOVE_DEVICE         0x03
#define IRP_MN_STOP_DEVICE                  0x04
#define IRP_MN_QUERY_STOP_DEVICE            0x05
#define IRP_MN_CANCEL_STOP_DEVICE           0x06
#define IRP_MN_QUERY_DEVICE_RELATIONS       0x07
#define IRP_MN_QUERY_INTERFACE              0x08
#define IRP_MN_QUERY_CAPABILITIES           0x09
#define IRP_MN_QUERY_RESOURCES              0x0A
#define IRP_MN_QUERY_RESOURCE_REQUIREMENTS  0x0B
#define IRP_MN_QUERY_DEVICE_TEXT            0x0C
#define IRP_MN_FILTER_RESOURCE_REQUIREMENTS 0x0D
#define IRP_MN_READ_CONFIG                  0x0F
#define IRP_MN_WRITE_CONFIG                 0x10
#define IRP_MN_EJECT                        0x11
#define IRP_MN_SET_LOCK                     0x12
#define IRP_MN_QUERY_ID                     0x13
#define IRP_MN_QUERY_PNP_DEVICE_STATE       0x14
#define IRP_MN_QUERY_BUS_INFORMATION        0x15
#define IRP_MN_DEVICE_USAGE_NOTIFICATION    0x16
#define IRP_MN_SURPRISE_REMOVAL             0x17

//
// POWER minor function codes
//
#define IRP_MN_WAIT_WAKE                    0x00
#define IRP_MN_POWER_SEQUENCE               0x01
#define IRP_MN_SET_POWER                    0x02
#define IRP_MN_QUERY_POWER                  0x03


//
// WMI minor function codes under IRP_MJ_SYSTEM_CONTROL
//

#define IRP_MN_QUERY_ALL_DATA               0x00
#define IRP_MN_QUERY_SINGLE_INSTANCE        0x01
#define IRP_MN_CHANGE_SINGLE_INSTANCE       0x02
#define IRP_MN_CHANGE_SINGLE_ITEM           0x03
#define IRP_MN_ENABLE_EVENTS                0x04
#define IRP_MN_DISABLE_EVENTS               0x05
#define IRP_MN_ENABLE_COLLECTION            0x06
#define IRP_MN_DISABLE_COLLECTION           0x07
#define IRP_MN_REGINFO                      0x08
#define IRP_MN_EXECUTE_METHOD               0x09
#define IRP_MN_REGINFO_EX                   0x0b

#pragma pack(1)
typedef struct _USB_PIPE_TO_ENDPOINT_INTERNAL
{
	U64							PipeHandle;
	USB_ENDPOINT_DESCRIPTOR		EndpointDesc;
}USB_PIPE_TO_ENDPOINT_INTERNAL, *PUSB_PIPE_TO_ENDPOINT_INTERNAL;

typedef struct _USB_PIPE_TO_ENDPOINT
{
	U64 Size;
	U64 NumberOfPipes;
	USB_PIPE_TO_ENDPOINT_INTERNAL	PipeToEndpoint[1];
}USB_PIPE_TO_ENDPOINT, *PUSB_PIPE_TO_ENDPOINT;
#pragma pack()

typedef struct _IDINFO_ {
	unsigned int dwID;
	const WCHAR *pName;
}IDINFO, *PIDINFO;

#include <PSHPACK1.H>
typedef struct _USB_SETUP {
	UCHAR bmRequestType;
	UCHAR bRequest;
	USHORT wValue;
	USHORT wIndex;
	USHORT wLength;
} USB_SETUP, *PUSB_SETUP;
#include <POPPACK.H>

typedef struct _SPECIFIC_INFO
{
	WCHAR UrbFlagString[1024];
	WCHAR ConfigAtt[1024];
	WCHAR EndpointAtt[0x20][1024];
}SPECIFIC_INFO, *PSPECIFIC_INFO;

typedef struct _SPECIFIC_INFO_DESC
{
	WCHAR UrbFlagString[1024];
	WCHAR ConfigAtt[1024];
	WCHAR EndpointAtt[0x20][1024];
}SPECIFIC_INFO_DESC, *PSPECIFIC_INFO_DESC;

//=============================================================================
// Parser 인터페이스 
//=============================================================================
// Main Utility Helper 함수들
//
typedef BOOL(*PHLP_FUNCTION_HELP_INFORMATION) (IN PVOID pParserHandle, OUT void ** ppFilterDriverHelpInformation);
typedef BOOL(*PHLP_FUNCTION_VIEW_OUTPUT) (IN PVOID pParserHandle, WCHAR * pString);
typedef int(*PHLP_FUNCTION_MESSAGEBOX) (IN PVOID pParserHandle, WCHAR * pTitle, WCHAR * pMessage, int Type, U32 dwmsecTimeout);

// serial
//

// Main Utility Helper 함수들 리스트테이블
typedef struct _HJ_EXPORT_HLP_FUNCTION_LIST
{
	U8		SupportedParserInterface_MajorVersion;
	U8		SupportedParserInterface_MinorVersion;

	PHLP_FUNCTION_HELP_INFORMATION	pfn_HelpInformation;
	PHLP_FUNCTION_VIEW_OUTPUT		pfn_ViewOutput;
	PHLP_FUNCTION_MESSAGEBOX		pfn_MessageBox;
} HJ_EXPORT_HLP_FUNCTION_LIST, *PHJ_EXPORT_HLP_FUNCTION_LIST;

typedef struct _PARSER_CONTEXT
{
	PVOID							MgrHandle;
	HJ_EXPORT_HLP_FUNCTION_LIST		hlp_function_list;
}PARSER_CONTEXT, *PPARSER_CONTEXT;

typedef PVOID(*PDLL_FUNCTION_INIT) (PVOID handle, PVOID phlp_function_list, WCHAR *pRegistryPath, WCHAR *FilePath);
typedef BOOL(*PDLL_FUNCTION_DEINIT) (PVOID handle);
typedef BOOL(*PDLL_FUNCTION_DATAPARSING) (PVOID handle, PVOID, SIZE_T, PVOID *, SIZE_T *);
typedef BOOL(*PDLL_FUNCTION_FREE) (PVOID handle, PVOID, SIZE_T);
typedef BOOL(*PDLL_FUNCTION_INFO) (PVOID pParserInfo);

// Parser DLL
typedef struct _HJ_EXPORT_FUNCTION_LIST
{
	WCHAR								szParserFilePath[MAX_PATH];
	WCHAR								szDllName[MAX_PATH];
	PDLL_FUNCTION_INIT					pfn_Init;
	PDLL_FUNCTION_DEINIT				pfn_Deinit;
	PDLL_FUNCTION_DATAPARSING			pfn_DataParsing;
	PDLL_FUNCTION_FREE					pfn_Free;
	PDLL_FUNCTION_INFO					pfn_Info; 
	HINSTANCE							hDll;
} HJ_EXPORT_FUNCTION_LIST, *PHJ_EXPORT_FUNCTION_LIST;

// for usb
typedef struct _HJ_USB_PARSER_INFO
{
	// 아래부분은 ParserInterface_MajorVersion 의존적이다
	U8		NumSupportedClass;

	// DLL 에서 처리할 수 있는 데이터의 종류를 USB Class 형태로 알려줘야 한다!!
	struct {
		U16		fFlag;
#define fVendorID 1
#define fProductID 2
#define fDeviceClass 4
#define fDeviceSubClass 8
#define fDeviceProtocol 16
#define fInterfaceClass 32
#define fInterfaceSubClass 64
#define fInterfaceProtocol 128
#define fUSBDefault 256

		// OR 가능
		U16		u16VendorID;
		U16		u16ProductID;
		U16		u16DeviceClass;
		U16		u16DeviceSubClass;
		U16		u16DeviceProtocol;
		U16		u16InterfaceClass;
		U16		u16InterfaceSubClass;
		U16		u16InterfaceProtocol;
		U16		u16USBbcdVersion;
	}SUPPORTED_CLASS[1];
} HJ_USB_PARSER_INFO, *PHJ_USB_PARSER_INFO;

// for serial
typedef struct _HJ_SERIAL_PARSER_INFO
{
	// 간단한 설명
	_TCHAR	szDescription[1024];

} HJ_SERIAL_PARSER_INFO, *PHJ_SERIAL_PARSER_INFO;

typedef struct _HJ_PARSER_INFO
{
	struct {
		U32					u32Size;
		HJPARSERTYPEENUM	Type;
		WCHAR				Title[1024];
		U32					MajorVersion;
		U32					MinorVersion;
		WCHAR				Desc[1024];
	}COMMON;

	union
	{
		HJ_USB_PARSER_INFO		usb;
	}u;

	struct {
		BOOL						bIsSupported;
		U32							ParserInterface_MajorVersion;
		U32							ParserInterface_MinorVersion;
		WCHAR						OptionalParserPath[MAX_PATH];
		PHJ_PARSING_FIXED_HEADER	pFixedHeaderInfo;
	}EXTRA;
} HJ_PARSER_INFO, *PHJ_PARSER_INFO;

typedef struct _PARSERMGR_FINDREQUEST
{
	HJPARSERTYPEENUM	ParserTypeEnum;
	union
	{
		struct _usb
		{
			U8	u8InterfaceClass;
			U8	u8InterfaceSubClass;
			U8	u8InterfaceProtocol;
		}usb;
	}u;
}PARSERMGR_FINDREQUEST, *PPARSERMGR_FINDREQUEST;

typedef struct _PARSERMGR_FINDRESULT
{
	SIZE_T		ParserCount;
	HJ_PARSER_INFO	ParserInfo[1];
}PARSERMGR_FINDRESULT, *PPARSERMGR_FINDRESULT;

typedef struct _HJ_PARSER_LICENSE
{
	BOOL				bIsPaid;
	BOOL				bIsFreeVersion;
	BOOL				bIsUsefulVersion;
} HJ_PARSER_LICENSE, *PHJ_PARSER_LICENSE;

static BOOLEAN
IsListEmpty(IN LIST_ENTRY * ListHead)
{
	return (BOOLEAN)(ListHead->Flink == ListHead);
}

static void
InitializeListHead(
	OUT PLIST_ENTRY ListHead
)
{
	ListHead->Flink = ListHead->Blink = ListHead;
}

static BOOLEAN
RemoveEntryList(
	IN PLIST_ENTRY Entry
)
{
	PLIST_ENTRY Blink;
	PLIST_ENTRY Flink;

	Flink = Entry->Flink;
	Blink = Entry->Blink;
	Blink->Flink = Flink;
	Flink->Blink = Blink;
	return (BOOLEAN)(Flink == Blink);
}

static PLIST_ENTRY
RemoveHeadList(
	IN OUT PLIST_ENTRY ListHead
)
{
	PLIST_ENTRY Flink;
	PLIST_ENTRY Entry;

	Entry = ListHead->Flink;
	Flink = Entry->Flink;
	ListHead->Flink = Flink;
	Flink->Blink = ListHead;
	return Entry;
}

static VOID
InsertTailList(
	IN OUT PLIST_ENTRY ListHead,
	IN OUT PLIST_ENTRY Entry
)
{
	PLIST_ENTRY Blink;

	Blink = ListHead->Blink;
	Entry->Flink = ListHead;
	Entry->Blink = Blink;
	Blink->Flink = Entry;
	ListHead->Blink = Entry;
}

static PLIST_ENTRY
PeekHeadListWithIndex(IN PLIST_ENTRY ListHead, IN int Index)
{
	PLIST_ENTRY Entry = NULL;

	PLIST_ENTRY Start;

	Start = ListHead->Flink;

	while (Start != ListHead)
	{
		Entry = Start;
		if (Index == 0)
			break;
		Start = Start->Flink;
		Index--;
	}
	//	if (Entry == ListHead)
	if (Index)
		Entry = NULL;
	return Entry;
}

// ----------------------------------------------------------------------------------------
// parser를 위한 라이브러리를 정의합니다
WCHAR * parserHlpCmn_findClass(unsigned int classcode);
WCHAR * parserHlpCmn_findVID(unsigned int vid);
TCHAR * parserHlpCmn_TranslatePipeHandle(PPARSER_CONTEXT pParset_Context, U64 PipeHandle);
UCHAR parserHlpCmn_TranslatePipeHandleToEPType(PPARSER_CONTEXT pParset_Context, U64 PipeHandle);
UCHAR parserHlpCmn_TranslatePipeHandleToEP(PPARSER_CONTEXT pParset_Context, U64 PipeHandle);
BOOL parserHlpCmn_free(PVOID handle, PVOID Buffer, SIZE_T Size);
void parserHlpCmn_FillField(PHJ_PARSING_FIELD_VALUE pField, WCHAR *FieldName, U32 Type, WCHAR *ValueName, SIZE_T ValueBitOff, SIZE_T ValueBitSize, SIZE_T ValueByteOff, SIZE_T ValueByteSize, void * pRealData);
PHJ_PARSING_DATA parserHlpCmn_CreateParsingData(PST_CF_IRP_URB pSource, WCHAR * DataTitleName, SIZE_T ExtensionSize);
PHJ_PARSING_DATA parserHlpCmn_CreateAndLinkParsingDataToParsingData(PHJ_PARSING_DATA pMainData, WCHAR * DataTitleName);
PHJ_PARSING_VALUE parserHlpCmn_CreateAndLinkParsingDataToParsingValue(PHJ_PARSING_DATA pData, WCHAR * ValueTitleName);
PHJ_PARSING_FIELD_VALUE parserHlpCmn_CreateAndLinkParsingValueToParsingField(PHJ_PARSING_VALUE pValue);
WCHAR * parserHlpCmn_translateURBStatus(USBD_STATUS status);
PHJ_PARSING_DATA parserHlpCmn_FillDetailsPage(PPARSER_CONTEXT pParset_Context, PST_CF_IRP_URB pSource, SIZE_T	Level);
PHJ_PARSING_DATA_FOR_DATA parserHlpCmn_FillDataPages(PPARSER_CONTEXT pParset_Context, PST_CF_IRP_URB pSource, SIZE_T	Level);
WCHAR * parserHlpCmn_translateString(const WCHAR * Prefix, unsigned char * pSrc, U64 Bytes, U64 MaxBytes);
BOOL parserHlpCmn_OutputStringToView(WCHAR * String);
int parserHlpCmn_MessageBoxToView(WCHAR * pTitle, WCHAR * pMessage, int Type, U32 dwmsecTimeout);

//
// ----------------------------------------------------------------------------------------
#pragma pack()
