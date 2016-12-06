#include "cf_base_types.h"
#include "cf.h"

#ifndef _CF_IRP_COMMON_H_
#define	_CF_IRP_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)

#pragma pack(1)

typedef enum
{
	TYPE_BUFFER_USER = 0x0001,
	TYPE_BUFFER_SYSTEM,
	TYPE_BUFFER_MDL,
	TYPE_BUFFER_IOCTL_IN,
	TYPE_BUFFER_IOCTL_OUT,
	TYPE_BUFFER_MAX
} E_CF_TYPE_BUFFER;

typedef struct _ST_CF_BUFFER
{
	E_CF_TYPE_BUFFER		eTypeBuffer;
	U64						u64LengthBuffer;			// �������� �ǹ�. ��û�� ���
	U64						u64OffsetBuffer;			// ���ڵ��� �������� ���ٴºκ�(ST_CF_RECORD���� ������)
} ST_CF_BUFFER, *PST_CF_BUFFER;

typedef struct _ST_CF_IRP_HEADER
{
	U64						u64Tag;						// ���� IRP(u64OrignialIrp)�ּҸ� �ִ´�(32��Ʈ�� Ȯ��)
	U64						u64Length;
	U64						u64NumRecord;				// Record Number
	U8						u8bCompletion;				// 0 : Request, 1 : Completion
	U64						u64OrignialIrp;
	U64						u64NumRecordOrg;			// Original u64NumRecord

	U64						u64Time;
	U32						u32FlagsIrp;

	U8						u8MajorFunction;
	U8						u8MinorFunction;
	U32						u32IoControlCode;
	U8						u8FlagsStack;

	U32						u32NtStatus;
	U64						u64Information;
    U32						u32RequestorMode;			// KPROCESSOR_MODE

	union
	{
		struct
		{
			U64						u64Pid;
			U64						u64Tid;
			U64						u64Res1;
			U64						u64Res2;
		};
		struct
		{
			U16						u16ProcessName[16];
		};
	}u;
} ST_CF_IRP_HEADER, *PST_CF_IRP_HEADER;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif // _CF_IRP_COMMON_H_
