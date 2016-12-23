#include "cf_base_types.h"
#include "cf.h"
#include "cf_irp_common.h"
#include <usb.h>
#include <usbioctl.h>

#ifndef _CF_IRP_URB_H_
#define	_CF_IRP_URB_H_

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)

#pragma pack(1)

typedef struct _ST_CF_USBD_STREAM_INFORMATION
{
	U64														u64PipeHandle;
	U32														u32StreamID;
	U32														u32MaximumTransferSize;
	U32														u32PipeFlags;
} ST_CF_USBD_STREAM_INFORMATION, *PST_CF_USBD_STREAM_INFORMATION;

typedef struct _ST_CF_IRP_URB_HEADER
{
	ST_CF_IRP_HEADER										stHeader;
	U16														u16Function;
	U16														u16FunctionForCompletion;
	U32														u32MaximumEntries;
	U64														u32UsedEntries;
	U64														u32Status;
} ST_CF_IRP_URB_HEADER, *PST_CF_IRP_URB_HEADER;

typedef struct _NEW_USBD_PIPE_INFORMATION
{
	USHORT MaximumPacketSize;
	UCHAR EndpointAddress;
	UCHAR Interval;
	ULONG PipeType;
	U64 PipeHandle;
	ULONG MaximumTransferSize;
	ULONG PipeFlags;
} NEW_USBD_PIPE_INFORMATION, *PNEW_USBD_PIPE_INFORMATION;

typedef struct _NEW_USBD_INTERFACE_INFORMATION
{
	USHORT Length;
	UCHAR InterfaceNumber;
	UCHAR AlternateSetting;
	UCHAR Class;
	UCHAR SubClass;
	UCHAR Protocol;
	UCHAR Reserved;
	U64 InterfaceHandle;
	ULONG NumberOfPipes;
	NEW_USBD_PIPE_INFORMATION Pipes[1];
} NEW_USBD_INTERFACE_INFORMATION, *PNEW_USBD_INTERFACE_INFORMATION;

typedef struct _ST_CF_IRP_URB_SELECT_INTERFACE
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U64														u64ConfigurationHandle;
	NEW_USBD_INTERFACE_INFORMATION							stInterface;
} ST_CF_IRP_URB_SELECT_INTERFACE, *PST_CF_IRP_URB_SELECT_INTERFACE;

typedef struct _ST_CF_IRP_URB_SELECT_CONFIGURATION
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U64														u64ConfigurationHandle;
	
	// ST_CF_IRP_URB_SELECT_CONFIGURATION 구조체에 이어서, USB_CONFIGURATION_DESCRIPTOR -> NEW_CF_IRP_URB_USBD_INTERFACE_INFORMATION[] 순서대로 데이타가 발생된다

} ST_CF_IRP_URB_SELECT_CONFIGURATION, *PST_CF_IRP_URB_SELECT_CONFIGURATION;


typedef struct _ST_CF_IRP_URB_PIPE_REQUEST
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U64														u64PipeHandle;
} ST_CF_IRP_URB_PIPE_REQUEST, *PST_CF_IRP_URB_PIPE_REQUEST;

// Deprecated in Windows 2000 and later operating systems and is not supported by Microsoft. Do not use. 
typedef struct _ST_CF_IRP_URB_FRAME_LENGTH_CONTROL
{
	ST_CF_IRP_URB_HEADER									stHeader;
} ST_CF_IRP_URB_FRAME_LENGTH_CONTROL, *PST_CF_IRP_URB_FRAME_LENGTH_CONTROL;

// Deprecated in Windows 2000 and later operating systems and is not supported by Microsoft. Do not use. 
typedef struct _ST_CF_IRP_URB_GET_FRAME_LENGTH
{
	ST_CF_IRP_URB_HEADER									stHeader;
} ST_CF_IRP_URB_GET_FRAME_LENGTH, *PST_CF_IRP_URB_GET_FRAME_LENGTH;

// Deprecated in Windows 2000 and later operating systems and is not supported by Microsoft. Do not use. 
typedef struct _ST_CF_IRP_URB_SET_FRAME_LENGTH
{
	ST_CF_IRP_URB_HEADER									stHeader;
} ST_CF_IRP_URB_SET_FRAME_LENGTH, *PST_CF_IRP_URB_SET_FRAME_LENGTH;

typedef struct _ST_CF_IRP_URB_GET_CURRENT_FRAME_NUMBER
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U32														u32FrameNumber;
} ST_CF_IRP_URB_GET_CURRENT_FRAME_NUMBER, *PST_CF_IRP_URB_GET_CURRENT_FRAME_NUMBER;

typedef struct _ST_CF_IRP_URB_CONTROL_TRANSFER
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U64														u64PipeHandle;
	U32														u32TransferFlags;
	ST_CF_BUFFER											stTransferBuffer;
	U8														u8SetupPacket[8];
} ST_CF_IRP_URB_CONTROL_TRANSFER, *PST_CF_IRP_URB_CONTROL_TRANSFER;

typedef struct _ST_CF_IRP_URB_CONTROL_TRANSFER_EX
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U64														u64PipeHandle;
	U32														u32TransferFlags;
	ST_CF_BUFFER											stTransferBuffer;
	U32														u32Timeout;
	U8														u8SetupPacket[8];
} ST_CF_IRP_URB_CONTROL_TRANSFER_EX, *PST_CF_IRP_URB_CONTROL_TRANSFER_EX;

typedef struct _ST_CF_IRP_URB_BULK_OR_INTERRUPT_TRANSFER
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U64														u64PipeHandle;
	U32														u32TransferFlags;
	ST_CF_BUFFER											stTransferBuffer;
} ST_CF_IRP_URB_BULK_OR_INTERRUPT_TRANSFER, *PST_CF_IRP_URB_BULK_OR_INTERRUPT_TRANSFER;

typedef struct _ST_CF_IRP_URB_ISOCH_TRANSFER
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U64														u64PipeHandle;
	U32														u32TransferFlags;
	ST_CF_BUFFER											stTransferBuffer;
	U32														u32StartFrame;
	U32														u32NumberOfPackets;
	U32														u32ErrorCount;
	USBD_ISO_PACKET_DESCRIPTOR								stIsoPacket[0];
} ST_CF_IRP_URB_ISOCH_TRANSFER, *PST_CF_IRP_URB_ISOCH_TRANSFER;

typedef struct _ST_CF_IRP_URB_CONTROL_DESCRIPTOR_REQUEST
{
	ST_CF_IRP_URB_HEADER									stHeader;
	ST_CF_BUFFER											stTransferBuffer;
	U8														u8Index;
	U8														u8DescriptorType;
	U16														u16LanguageId;
} ST_CF_IRP_URB_CONTROL_DESCRIPTOR_REQUEST, *PST_CF_IRP_URB_CONTROL_DESCRIPTOR_REQUEST;

typedef struct _ST_CF_IRP_URB_CONTROL_GET_STATUS_REQUEST
{
	ST_CF_IRP_URB_HEADER									stHeader;
	ST_CF_BUFFER											stTransferBuffer;
	U16														u16Index;
} ST_CF_IRP_URB_CONTROL_GET_STATUS_REQUEST, *PST_CF_IRP_URB_CONTROL_GET_STATUS_REQUEST;

typedef struct _ST_CF_IRP_URB_CONTROL_FEATURE_REQUEST
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U16														u16FeatureSelector;
	U16														u16Index;
} ST_CF_IRP_URB_CONTROL_FEATURE_REQUEST, *PST_CF_IRP_URB_CONTROL_FEATURE_REQUEST;

typedef struct _ST_CF_IRP_URB_CONTROL_VENDOR_OR_CLASS_REQUEST
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U32														u32TransferFlags;
	ST_CF_BUFFER											stTransferBuffer;
	U8														u8Request;
	U16														u16Value;
	U16														u16Index;
} ST_CF_IRP_URB_CONTROL_VENDOR_OR_CLASS_REQUEST, *PST_CF_IRP_URB_CONTROL_VENDOR_OR_CLASS_REQUEST;

typedef struct _ST_CF_IRP_URB_CONTROL_GET_INTERFACE_REQUEST
{
	ST_CF_IRP_URB_HEADER									stHeader;
	ST_CF_BUFFER											stTransferBuffer;
	U16														u16Interface;
} ST_CF_IRP_URB_CONTROL_GET_INTERFACE_REQUEST, *PST_CF_IRP_URB_CONTROL_GET_INTERFACE_REQUEST;

typedef struct _ST_CF_IRP_URB_CONTROL_GET_CONFIGURATION_REQUEST
{
	ST_CF_IRP_URB_HEADER									stHeader;
	ST_CF_BUFFER											stTransferBuffer;
} ST_CF_IRP_URB_CONTROL_GET_CONFIGURATION_REQUEST, *PST_CF_IRP_URB_CONTROL_GET_CONFIGURATION_REQUEST;

typedef struct _ST_CF_IRP_URB_OS_FEATURE_DESCRIPTOR_REQUEST
{
	ST_CF_IRP_URB_HEADER									stHeader;
	ST_CF_BUFFER											stTransferBuffer;
	U8														u8Recipient;
	U8														u8InterfaceNumber;
	U8														u8MS_PageIndex;
	U16														u16MS_FeatureDescriptorIndex;
} ST_CF_IRP_URB_OS_FEATURE_DESCRIPTOR_REQUEST, *PST_CF_IRP_URB_OS_FEATURE_DESCRIPTOR_REQUEST;

// Windows8
typedef struct _ST_CF_IRP_URB_OPEN_STATIC_STREAMS
{
	ST_CF_IRP_URB_HEADER									stHeader;
	U64														u64PipeHandle;
	U32														u32NumberOfStreams;
	U16														u16StreamInfoVersion;
	U16														u16StreamInfoSize;
	ST_CF_USBD_STREAM_INFORMATION							stStreams[0];
} ST_CF_IRP_URB_OPEN_STATIC_STREAMS, *PST_CF_IRP_URB_OPEN_STATIC_STREAMS;

typedef struct _ST_CF_IRP_URB
{
	union
	{
		ST_CF_IRP_URB_HEADER								stUrbHeader; // URB를 사용하지 않는 IOCTL_INTERNAL_USB_GET_PORT_STATUS... 등과 같은 요청도 이 구조체를 사용한다
		ST_CF_IRP_URB_SELECT_INTERFACE						stUrbSelectInterface;
		ST_CF_IRP_URB_SELECT_CONFIGURATION					stUrbSelectConfiguration;
		ST_CF_IRP_URB_PIPE_REQUEST							stUrbPipeRequest;
		ST_CF_IRP_URB_FRAME_LENGTH_CONTROL					stUrbFrameLengthControl;
		ST_CF_IRP_URB_GET_FRAME_LENGTH						stUrbGetFrameLength;
		ST_CF_IRP_URB_SET_FRAME_LENGTH						stUrbSetFrameLength;
		ST_CF_IRP_URB_GET_CURRENT_FRAME_NUMBER				stUrbGetCurrentFrameNumber;
		ST_CF_IRP_URB_CONTROL_TRANSFER						stUrbControlTransfer;
		ST_CF_IRP_URB_CONTROL_TRANSFER_EX					stUrbControlTransferEx;
		ST_CF_IRP_URB_BULK_OR_INTERRUPT_TRANSFER			stUrbBulkOrInterruptTransfer;
		ST_CF_IRP_URB_ISOCH_TRANSFER						stUrbIsochronousTransfer;
		ST_CF_IRP_URB_CONTROL_DESCRIPTOR_REQUEST			stUrbControlDescriptorRequest;
		ST_CF_IRP_URB_CONTROL_GET_STATUS_REQUEST			stUrbControlGetStatusRequest;
		ST_CF_IRP_URB_CONTROL_FEATURE_REQUEST				stUrbControlFeatureRequest;
		ST_CF_IRP_URB_CONTROL_VENDOR_OR_CLASS_REQUEST		stUrbControlVendorOrClassRequest;
		ST_CF_IRP_URB_CONTROL_GET_INTERFACE_REQUEST			stUrbControlGetInterfaceRequest;
		ST_CF_IRP_URB_CONTROL_GET_CONFIGURATION_REQUEST		stUrbControlGetConfigurationRequest;
		ST_CF_IRP_URB_OS_FEATURE_DESCRIPTOR_REQUEST			stUrbOSFeatureDescriptorRequest;
		// Windows8
		ST_CF_IRP_URB_OPEN_STATIC_STREAMS					stUrbOpenStaticStreams;
	};
} ST_CF_IRP_URB, *PST_CF_IRP_URB;

#pragma pack(pop)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Application Interface

//#define IOCTL_BASE											(0xA00)

//#define IOCTL_IOMAN7_GET_VERSION							CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x0000, METHOD_BUFFERED, FILE_ANY_ACCESS)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define IOCTL_IOMAN7_GET_LAST_ERROR							CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x0002, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_IOMAN7_SET_LAST_ERROR							CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x0003, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_IOMAN7_GET_TRACE								CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x0004, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTL_IOMAN7_SET_TRACE								CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x0005, METHOD_BUFFERED, FILE_ANY_ACCESS)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IOCTL_IOMAN7_GET_CF_IRP_URB_PEEP					CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x0030, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_IOMAN7_GET_CF_IRP_URB							CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x0032, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define	IOCTL_IOMAN7_SET_CF_IRP_URB_EVENT_INPUT				CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + 0x0035, METHOD_BUFFERED, FILE_ANY_ACCESS)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // _CF_IRP_URB_H_
