// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#include <QDateTime>
#include <winsock2.h>
#include <MSWSock.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

#include "../../include/ServerApi/Misc/FixedMemoryPool.h"
#include "../../include/ServerApi/Misc/SingleInstance.h"
#include "../../include/ServerApi/Define.h"

//#ifdef _DEBUG
//#pragma comment(lib, "../../lib/Debug/ServerApi.lib")
//#else
//#pragma comment(lib, "../../lib/Release/ServerApi.lib")
//#endif

#include "ServerLoger.h"
#include "CNetMessage.h"
#include "NetClass.h"
#include "Define.h"
#include "cdatamanagerthread.h"