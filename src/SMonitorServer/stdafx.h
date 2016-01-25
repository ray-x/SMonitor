// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
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