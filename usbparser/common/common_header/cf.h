#include "cf_base_types.h"

#ifndef _CF_H_
#define _CF_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	TYPE_CLASS_USB = 0x0001,
	TYPE_CLASS_1394,
	TYPE_CLASS_MAX
} E_CF_TYPE_CLASS;

typedef enum
{
	TYPE_RECORD_IRP = 0x0001,
	TYPE_RECORD_IRP_URB,
	TYPE_RECORD_IRP_SRB,
	TYPE_RECORD_IRP_ILB,
	TYPE_RECORD_MAX
} E_CF_TYPE_RECORD;

#pragma pack(push)

#pragma pack(1)

typedef struct _ST_CF_RECORD
{
	U64						u64Length;	// Record의 길이(헤더포함, ST_CF_RECORD)
	E_CF_TYPE_RECORD		eTypeRecord;
	U64						u64Timestamp; // ns단위
	U8						Data[0];
} ST_CF_RECORD, *PST_CF_RECORD;

typedef struct _ST_CF
{
	U32						u32Signature;
	E_CF_TYPE_CLASS			eTypeClass;			// USB뭐하니, 1394뭐하니.. 등과 같은 제품의 부속저장파일을 명시한다
	U32						u32MajorVersionType;
	U32						u32MinorVersionType;
	U64						u64Length;									// SaveFile의 총길이(헤더포함, ST_CF, Extension을 포함한다)
	U64						u64LengthPayload;							// Payload의 길이(헤더제외, ST_CF, Extension을 제외한다)
	U64						u64LengthExtension;
	U64						u64OffsetExtension;							// ST_CF 부터 시작하는 오프셋
	U32						u32CountRecord;
//	ST_CF_RECORD			stRecord[0];								// CountRecord만큼의, 가변크기의 CF_RECORD가 이어진다
} ST_CF, *PST_CF;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif	// _CF_H_
