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

#define COLUMNCOUNT 12 // �ּ��� 2�� �̻��� �÷��� ������ �մϴ�. �÷� 1, 2 �� ������� ���ڵ��ȣ�� �ð������Դϴ�

typedef struct _HJ_CUSTOM_PARSING_FIXED_HEADER
{
	SIZE_T					ItemCount;
	HJ_COLUMN				ColumnHeader[COLUMNCOUNT];
} HJ_CUSTOM_PARSING_FIXED_HEADER, *PHJ_CUSTOM_PARSING_FIXED_HEADER;


