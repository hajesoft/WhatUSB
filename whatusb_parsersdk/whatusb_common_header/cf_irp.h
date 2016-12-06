#include "cf_base_types.h"
#include "cf.h"
#include "cf_irp_common.h"

#ifndef _CF_IRP_H_
#define	_CF_IRP_H_

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)

#pragma pack(1)

typedef struct _ST_CF_IRP_IOCTL
{
	ST_CF_IRP_HEADER		stHeader;
	U32						u32IoControlCode;
	ST_CF_BUFFER			stBufferIn;
	ST_CF_BUFFER			stBufferOut;
} ST_CF_IRP_IOCTL, *PST_CF_IRP_IOCTL;

typedef struct _ST_CF_IRP_CREATE
{
	ST_CF_IRP_HEADER		stHeader;
	U32						u32Options;
	U16						u16FileAttributes;
	U16						u16ShareAccess;
} ST_CF_IRP_CREATE, *PST_CF_IRP_CREATE;

typedef struct _ST_CF_IRP_READ
{
	ST_CF_IRP_HEADER		stHeader;
	ST_CF_BUFFER			stBufferIn;
	U64						u64ByteOffset;
} ST_CF_IRP_READ, *PST_CF_IRP_READ;

typedef struct _ST_CF_IRP_WRITE
{
	ST_CF_IRP_HEADER		stHeader;
	ST_CF_BUFFER			stBufferOut;
	U64						u64ByteOffset;
} ST_CF_IRP_WRITE, *PST_CF_IRP_WRITE;

typedef struct _ST_CF_IRP
{
	union
	{
		ST_CF_IRP_HEADER		stIrpHeader;
		ST_CF_IRP_CREATE		stIrpCreate;
		ST_CF_IRP_READ			stIrpRead;
		ST_CF_IRP_WRITE			stIrpWrite;
		ST_CF_IRP_IOCTL			stIrpIoctl;
	};
} ST_CF_IRP, *PST_CF_IRP;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif // _CF_IRP_H_
