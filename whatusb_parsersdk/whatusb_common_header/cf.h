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
	U64						u64Length;	// Record�� ����(�������, ST_CF_RECORD)
	E_CF_TYPE_RECORD		eTypeRecord;
	U64						u64Timestamp; // ns����
	U8						Data[0];
} ST_CF_RECORD, *PST_CF_RECORD;

typedef struct _ST_CF
{
	U32						u32Signature;
	E_CF_TYPE_CLASS			eTypeClass;			// USB���ϴ�, 1394���ϴ�.. ��� ���� ��ǰ�� �μ����������� ����Ѵ�
	U32						u32MajorVersionType;
	U32						u32MinorVersionType;
	U64						u64Length;									// SaveFile�� �ѱ���(�������, ST_CF, Extension�� �����Ѵ�)
	U64						u64LengthPayload;							// Payload�� ����(�������, ST_CF, Extension�� �����Ѵ�)
	U64						u64LengthExtension;
	U64						u64OffsetExtension;							// ST_CF ���� �����ϴ� ������
	U32						u32CountRecord;
//	ST_CF_RECORD			stRecord[0];								// CountRecord��ŭ��, ����ũ���� CF_RECORD�� �̾�����
} ST_CF, *PST_CF;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif	// _CF_H_
