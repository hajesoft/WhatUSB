#include <tchar.h>

#ifndef _COMMON_H_
#define _COMMON_H_

//#define MEMORY_LEAK_DEBUG

typedef char				S8, *PS8, s8, *ps8;
typedef short				S16, *PS16, s16, *ps16;
typedef long				S32, *PS32, s32, *ps32;
typedef long long			S64, *PS64, s64, *ps64;
typedef unsigned char		U8, *PU8, u8, *pu8;
typedef unsigned short		U16, *PU16, u16, *pu16;
typedef unsigned long		U32, *PU32, u32, *pu32;
typedef unsigned long long	U64, *PU64, u64, *pu64;

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef HJDRIVERAPI
#define HJDRIVERAPI extern "C" __declspec(dllexport)
#endif

#ifndef HJPARSERAPI
#define HJPARSERAPI extern "C" __declspec(dllexport)
#endif

#ifndef HJPARSERMGRAPI
#define HJPARSERMGRAPI extern "C" __declspec(dllexport)
#endif

typedef enum _HJFILTERTYPEENUM
{
	USB_FILTER = 0,
	SERIAL_FILTER
}HJFILTERTYPEENUM;

typedef enum _HJPARSERTYPEENUM
{
	USB_PARSER = 0,
	SERIAL_PARSER
}HJPARSERTYPEENUM;

typedef enum _HJ_STRUCT_TYPE
{
	HJ_STRUCT_TYPE__INFO = 0x00000001,
	HJ_STRUCT_TYPE__PARSING_DATA = 0x00000002
} HJ_STRUCT_TYPE;

#define HJ_VALUE_ENDIAN_IGN (U32)(0x00000000)
#define HJ_VALUE_ENDIAN_LSB (U32)(0x00000001)
#define HJ_VALUE_ENDIAN_MSB (U32)(0x00000002)
#define HJ_VALUE_EXTRA_BUFF (U32)(0x00000004) // Data Byte를 전달하기 위해서 사용하는 상수
#define HJ_VALUE_EXTRA_PRIVATEBUFF (U32)(0x00000008) // Private 용도로 사용하는 메모리할당을 예약하는 상수
#define HJ_STRUCT_TYPE__SPLIT_DESCRIPTOR (U32)(0x00000010)
#define HJ_STRUCT_TYPE__SPLIT_ARRAY_BEGIN (U32)(0x00000020)
#define HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_BEGIN (U32)(0x00000040)
#define HJ_STRUCT_TYPE__SPLIT_ARRAY_PART_END (U32)(0x00000080)
#define HJ_STRUCT_TYPE__SPLIT_ARRAY_END (U32)(0x00000100)
#define HJ_STRUCT_TYPE__SPLIT_COMMON (U32)(0x00000200)

#define HJ_STRUCT_ARRAY_DEPTH	(U32)(8) // 최대 4차원 배열까지만 지원한다

// usb
#define TT_USB_PIPEHANDLE	(int)(0x1000)

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

typedef struct _HJ_PARSING_FIELD_VALUE
{
	LIST_ENTRY			__Self__;
	SIZE_T				nRowFieldValueTotalSize; // bytes
	PTCHAR				pRowFieldName;
	PTCHAR				pRowFieldValueName;
	U32					bRowFieldValueType; // or Split Type
	SIZE_T				nRowFieldValueBytesOffset;
	SIZE_T				nRowFieldValueBitsOffset;
	SIZE_T				nRowFieldValueBytesSize; // bytes
	SIZE_T				nRowFieldValueBitsSize; // bits
	U8					nRowFieldValue[8];
	U8					*nRowFieldBuffer;
	SIZE_T				nRowFieldBufferMaxLength;
	SIZE_T				nRowFieldBufferLength;
	U8					*nRowFieldPrivateBuffer;
	SIZE_T				nRowFieldPrivateBufferMaxLength;
	SIZE_T				nRowFieldPrivateBufferLength;
}HJ_PARSING_FIELD_VALUE, *PHJ_PARSING_FIELD_VALUE;

typedef struct _HJ_PARSING_VALUE
{
	LIST_ENTRY			__Self__;
	PTCHAR				Title;
	SIZE_T				nValueBufferMaxLength;
	SIZE_T				nValueBufferLength;
	U8					*nValueBuffer;
	SIZE_T				nRowFieldNameCount;
	LIST_ENTRY			__FieldHead__;
//	PHJ_PARSING_FIELD_VALUE	pFieldInfo[1];
}HJ_PARSING_VALUE, *PHJ_PARSING_VALUE;

typedef struct _HJ_PARSING_DATA
{
	LIST_ENTRY				__Sibling__;
	SIZE_T					Level;
	LIST_ENTRY				__Child__;
	SIZE_T					ChildCount;

	HJ_STRUCT_TYPE			Type;

	PWCHAR					pszTitle;
	SIZE_T					nParsingValueCount;
	LIST_ENTRY				__ValueHead__;
} HJ_PARSING_DATA, *PHJ_PARSING_DATA;

typedef struct _HJ_PARSING_DATA_FOR_DATA
{
	U8						bIsConstant;
	SIZE_T					Size;
	void *					pDataBuffer;
} HJ_PARSING_DATA_FOR_DATA, *PHJ_PARSING_DATA_FOR_DATA;

typedef struct _HJ_COLUMNDATA
{
	U8					bIsConstant;
	U8					bIsString;
	int					nRadix;
	U64					Data;
	PWCHAR				pszData;
}HJ_COLUMNDATA, *PHJ_COLUMNDATA;


typedef struct _HJ_PARSING_FIXED_DATA_ENTRY
{
	LIST_ENTRY				__Self__;
	U32						IndexForViewerImage;

	SIZE_T					Level;

	BOOLEAN					bIsLast;
	PHJ_PARSING_DATA				pDetailsPage;
	PHJ_PARSING_DATA_FOR_DATA		pDataPage;
	SIZE_T					ColCount;
	HJ_COLUMNDATA			ColumnData[1];
} HJ_PARSING_FIXED_DATA_ENTRY, *PHJ_PARSING_FIXED_DATA_ENTRY;

typedef struct _HJ_PARSING_FIXED_DATA
{
	LIST_ENTRY				__AllDataEntryHeader__;
	SIZE_T					nCount;
} HJ_PARSING_FIXED_DATA, *PHJ_PARSING_FIXED_DATA;

typedef struct _HJ_COLUMN
{
	U8					bIsConstant;
	SIZE_T				MaxListCtrlSize;
	PWCHAR				pszData;
}HJ_COLUMN, *PHJ_COLUMN;

typedef struct _HJ_PARSING_FIXED_HEADER
{
	SIZE_T					ItemCount;
	HJ_COLUMN				ColumnHeader[1];
} HJ_PARSING_FIXED_HEADER, *PHJ_PARSING_FIXED_HEADER;

// View Image Index
#define VIEWIMAGEINDEX_DEFAULT	(0)
#define VIEWIMAGEINDEX_INOUT	(1)
#define VIEWIMAGEINDEX_OUT		(2)
#define VIEWIMAGEINDEX_IN		(3)

// View Dedicated Column
// 2개의 컬럼은 예약되어 있습니다
#define VIEWCOL_RECORD	(0)
#define VIEWCOL_TIME	(1)


#pragma pack()

#ifdef __cplusplus
}
#endif

#endif