#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <usb.h>
#include <tchar.h>

#include "parserdefinition.h"

#define INOUTDIR	(L"IN/OUT")
#define INDIR		(L"IN")
#define OUTDIR		(L"OUT")

#define COLUMNCOUNT 12 // 최소한 2개 이상의 컬럼을 가져야 합니다. 컬럼 1, 2 는 순서대로 레코드번호와 시간정보입니다

typedef struct _HJ_CUSTOM_PARSING_FIXED_HEADER
{
	SIZE_T					ItemCount;
	HJ_COLUMN				ColumnHeader[COLUMNCOUNT];
} HJ_CUSTOM_PARSING_FIXED_HEADER, *PHJ_CUSTOM_PARSING_FIXED_HEADER;


