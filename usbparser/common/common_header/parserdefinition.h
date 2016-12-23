#pragma once

#include ".\\..\\..\\common\\common_header\\common.h"
#include ".\\..\\..\\common\\common_header\\cf_irp_urb.h"
#include ".\\..\\..\\common\\common_header\\parserhlp.h"

#if (_WIN64 || __amd64__)
	#if _DEBUG
		#pragma comment(lib,".\\..\\..\\common\\common_library\\x64\\debug\\parserhlp.lib")
	#else
		#pragma comment(lib,".\\..\\..\\common\\common_library\\x64\\release\\parserhlp.lib")
	#endif
#else
	#if _DEBUG
		#pragma comment(lib,".\\..\\..\\common\\common_library\\x86\\debug\\parserhlp.lib")
	#else
		#pragma comment(lib,".\\..\\..\\common\\common_library\\x86\\release\\parserhlp.lib")
	#endif
#endif

#define PARSER_TITLE	L"USB Default Parser"
#define PARSER_DESC		L"��� USB ��ġ�� ���� �⺻������ �����մϴ�"