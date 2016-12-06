
#ifndef _CF_BASE_TYPES_H_
#define	_CF_BASE_TYPES_H_

#pragma warning(disable:4200)	// was declared deprecated

#ifdef __cplusplus
extern "C" {
#endif

typedef char				S8, *PS8, s8, *ps8;
typedef short				S16, *PS16, s16, *ps16;
typedef long				S32, *PS32, s32, *ps32;
typedef long long			S64, *PS64, s64, *ps64;
typedef unsigned char		U8, *PU8, u8, *pu8;
typedef unsigned short		U16, *PU16, u16, *pu16;
typedef unsigned long		U32, *PU32, u32, *pu32;
typedef unsigned long long	U64, *PU64, u64, *pu64;

#ifdef __cplusplus
}
#endif

#endif // _CF_BASE_TYPES_H_