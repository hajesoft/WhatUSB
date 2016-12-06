#pragma once

#include ".\\..\\whatusb_common_header\\common.h"
#include ".\\..\\whatusb_common_header\\cf_irp_urb.h"
#include ".\\..\\whatusb_common_header\\parserhlp.h"

#if (_WIN64 || __amd64__)
#pragma comment(lib,".\\..\\whatusb_common_library\\x64\\parserhlp.lib")
#else
#pragma comment(lib,".\\..\\whatusb_common_library\\x86\\parserhlp.lib")
#endif

#define PARSER_TITLE	L"USB Default Parser"
#define PARSER_DESC		L"모든 USB 장치에 대해 기본번역을 지원합니다"