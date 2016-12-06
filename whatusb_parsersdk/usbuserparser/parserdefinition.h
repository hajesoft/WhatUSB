#pragma once

#include ".\\..\\whatusb_common_header\\common.h"
#include ".\\..\\whatusb_common_header\\cf_irp_urb.h"
#include ".\\..\\whatusb_common_header\\parserhlp.h"

#if (_WIN64 || __amd64__)
#pragma comment(lib,".\\..\\whatusb_common_library\\x64\\parserhlp.lib")
#else
#pragma comment(lib,".\\..\\whatusb_common_library\\x86\\parserhlp.lib")
#endif

#define PARSER_TITLE	L"USB User Parser"
#define PARSER_DESC		L"사용자가 만든 샘플 번역기입니다"