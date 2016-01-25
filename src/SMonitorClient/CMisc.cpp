/*
This file is part of SMonitor.
Copyright  2015~2016  by: rayx  email rayx.cn@gmail.com
SMonitor is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SMonitor is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CMisc.h"
#include "CClientLog.h"
#include <QStringList>
#include <QFontDatabase>
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QTextCodec>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QDebug>
#include "common.h"
#include "ZHead.h"

#include <windows.h>
#include <WinBase.h>
#include <MMSystem.h>
#include <IPHlpApi.h>
#include <TlHelp32.h>
#include <QStandardPaths>

#pragma comment(lib, "Iphlpapi.lib")

//32位操作系统，(系统64位，程序64位) 应用程序信息路径
#define WIN32_APPLOCATE ("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")

//64位操作系统，程序32位 应用程序信息路径
#define WIN64_APPLOCATE ("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall")

//定义获取系统位数函数指针
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);   
LPFN_ISWOW64PROCESS	fnIsWow64Process = NULL;

bool CheckSysWow()
{
	//获取系统位数
	BOOL bIsSysWow32 = TRUE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle("kernel32"), "IsWow64Process");  
	if(NULL != fnIsWow64Process)   
	{   
		fnIsWow64Process(GetCurrentProcess(), &bIsSysWow32);  
		bIsSysWow32 = !bIsSysWow32;
	}   

	return TRUE == bIsSysWow32;
}

QString CheckCurOsVer()
{
	QString strOSVersionName = "";
	SYSTEM_INFO info;                                   //用SYSTEM_INFO结构判断64位AMD处理器
	GetSystemInfo(&info);                               //调用GetSystemInfo函数填充结构
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//下面根据版本信息判断操作系统名称
		switch(os.dwMajorVersion)
		{                        //判断主版本号
		case 4:
			switch(os.dwMinorVersion){                //判断次版本号
			case 0:
				if(os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					strOSVersionName = "Microsoft Windows NT 4.0";                //1996年7月发布
				else if(os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
					strOSVersionName = "Microsoft Windows 95";
				break;
			case 10:
				strOSVersionName = "Microsoft Windows 98";
				break;
			case 90:
				strOSVersionName = "Microsoft Windows Me";
				break;
			}
			break;
		case 5:
			switch(os.dwMinorVersion){               //再比较dwMinorVersion的值
			case 0:
				strOSVersionName = "Microsoft Windows 2000";                    //1999年12月发布
				break;
			case 1:
				strOSVersionName = "Microsoft Windows XP";                    //2001年8月发布
				break;
			case 2:
				if(os.wProductType == VER_NT_WORKSTATION && info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					strOSVersionName = "Microsoft Windows XP Professional x64 Edition";
				else if(GetSystemMetrics(SM_SERVERR2) == 0)
					strOSVersionName = "Microsoft Windows Server 2003";        //2003年3月发布
				else if(GetSystemMetrics(SM_SERVERR2) != 0)
					strOSVersionName = "Microsoft Windows Server 2003 R2";
				break;
			}
			break;
		case 6:
			switch(os.dwMinorVersion){
			case 0:
				if(os.wProductType == VER_NT_WORKSTATION)
					strOSVersionName = "Microsoft Windows Vista";
				else
					strOSVersionName = "Microsoft Windows Server 2008";          //服务器版本
				break;
			case 1:
				if(os.wProductType == VER_NT_WORKSTATION)
					strOSVersionName = "Microsoft Windows 7";
				else
					strOSVersionName = "Microsoft Windows Server 2008 R2";
				break;
			}
			break;
		default:
			strOSVersionName = "UNKNOWN";
		}

		if(os.dwMajorVersion > 6 || os.dwMajorVersion == 6 && os.dwMinorVersion > 1)
		{
			QSysInfo::WinVersion osVersion = QSysInfo::windowsVersion();
			switch(osVersion)
			{
			case QSysInfo::WV_WINDOWS8:
				strOSVersionName = "Microsoft Windows 8";
				break;
			case QSysInfo::WV_WINDOWS8_1:
				strOSVersionName = "Microsoft Windows 8.1";
				break;
			default:
				strOSVersionName = "HIGH VERSION";
				break;
			}
		}
	}
	return strOSVersionName;
}

//判断系统版本是否兼容
bool CMisc::IsOSSuit(QString strOS)
{
	//驱动字符串格式错误,正确格式为 "名称##系统##位数"
	QStringList osDriverNames = strOS.split("##");
	if(osDriverNames.count() != 3)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("驱动名称 [%1] 格式错误,请修改驱动名称为：名称##系统##系统位数， 如：驱动1##WIN8.1##WIN32").arg(strOS));
		return false;
	}

	//获取系统版本
	QString strSysVer = GetOSInfo().replace("Microsoft Windows ", "WIN").toUpper();
	QString strSysBits = m_bIsSys32 ? "WIN32" : "WIN64";
	if(strSysVer == "UNKNOWN" || strSysVer.isEmpty())
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("本地系统版本号未知 [%1] 驱动无法安装").arg(strOS));
		return false;
	}

	QString osVersion = osDriverNames.at(1).toUpper().replace("WINDOWS", "WIN");//得到驱动名称中的系统名称
	QString osSysBits = osDriverNames.at(2).toUpper().replace("WINDOWS", "WIN");//得到驱动名称中的位数
	if((osVersion == "WINALL" || strSysVer.contains(osVersion)) && (strSysBits == osSysBits || osSysBits == "WINALL"))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("当前系统 [%1] 兼容驱动 [%2] ，自动执行驱动安装操作").arg(strSysVer + strSysBits).arg(strOS));
		return true;
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("当前系统 [%1] 不兼容驱动 [%2] ，请联系管理员").arg(strSysVer + strSysBits).arg(strOS));
		return false;
	}
}

bool CMisc::m_bIsSys32 = CheckSysWow();
QStringList CMisc::m_UnSafeList;

QString CMisc::m_strOsVer = CheckCurOsVer();

QFont CMisc::FontAwesome(int pointSize /* = 10 */)
{
	int fontId = QFontDatabase::addApplicationFont(":/Resources/fontawesome-webfont.ttf");
	QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);

	return QFont(family, pointSize);
}

bool CMisc::GetLocalHostAddr(QString& IP, QString& MAC)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	DWORD AdapterInfoSize = 0;
	TCHAR szMac[32]   =   {0};
	DWORD Err;    

	Err = ::GetAdaptersInfo(NULL,   &AdapterInfoSize);  
	if((Err != 0) && (Err != ERROR_BUFFER_OVERFLOW))
	{  
		return  false;  
	}

	//   分配网卡信息内存  
	pAdapterInfo = (PIP_ADAPTER_INFO)::GlobalAlloc(GPTR, AdapterInfoSize);  
	if(pAdapterInfo == NULL)
	{  
		return   false;  
	}
	if(::GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize) != 0)
	{  
		GlobalFree(pAdapterInfo);  
		return   false;  
	}

	PIP_ADAPTER_INFO pAdapter = NULL;
	pAdapter = pAdapterInfo;

	while(pAdapter)
	{
		/*if(pAdapter->Type == MIB_IF_TYPE_ETHERNET)
		{
			sprintf_s(szMac, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", 
				pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2], 
				pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
			MAC = szMac;
			IP = pAdapter->IpAddressList.IpAddress.String;
			break;
			qDebug() << "IP: " << IP;
		}*/
		sprintf_s(szMac, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x", 
			pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2], 
			pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
		MAC = szMac;
		IP = pAdapter->IpAddressList.IpAddress.String;

		if(IP != "0.0.0.0" && IP != "127.0.0.1")
			break;
		pAdapter = pAdapter->Next;
	}

	::GlobalFree(pAdapterInfo);
	return true;
}

QStringList CMisc::GetLogicalDriveStringList()
{
	QStringList lDriverString;
	char szBuffer[MAX_PATH] = {"\0"};
	int nLength = GetLogicalDriveStrings(MAX_PATH - 1, szBuffer);
	if (0 == nLength){
		return QStringList();
	}
	for(char *pDrive = szBuffer; *pDrive; pDrive += strlen(szBuffer) + 1)
	{
		if(DRIVE_FIXED == GetDriveType(pDrive)){
			lDriverString << QString(pDrive).replace("\\", "/");
		}
	}
	return lDriverString;
}

//获取文件的MD5码
bool CMisc::getFileMD5(const QString &strFile, QString &strMD5)
{
	strMD5 = "";
	QFile file(strFile);

	if (!file.open(QFile::ReadOnly))
	{
		return false;
	}

	QByteArray md5Data = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);  
	strMD5.append(md5Data.toHex());  
	strMD5 = strMD5.toLower();
	file.close();  

	return true;  
}

QString CMisc::getFileTemporary(const QString& strFile)
{
	QString strMD5 = "";
	QByteArray byteArray(strFile.toStdString().c_str());
	QByteArray md5Data = QCryptographicHash::hash(byteArray, QCryptographicHash::Md5);
	strMD5.append(md5Data.toHex());  

	return strMD5;
}

//---------------------------------------------------------------------------------------------------
//描述：将一个压缩文件解压到某个目录下
//
//参数：lpSrcFile - [in]待解压的文件名(全路径名称或相对路径)，文件后缀只能为".zip"或".rar"
//		lpDesDir - [in]压缩文件中所有文件解压后的目标目录
//
//返回值：如果压缩文件中所有的文件都解压成功,函数返回TRUE,否则返回FALSE.
long CMisc::DecompressFiles(const QString& lpSrcFile, const QString& lpDesDir)
{
	if(lpSrcFile.isEmpty() || lpDesDir.isEmpty())
		return -1;

	//判断源文件后缀名是否为".zip"或".rar"
	QString strSrcFile = lpSrcFile;
	QFileInfo desFileinfo(lpSrcFile);
	qDebug() << desFileinfo.suffix();
	if(desFileinfo.suffix() != "zip" && desFileinfo.suffix() != "rar")
		return -1;

	QString strDesDir = lpDesDir;
	if(0 != strDesDir.right(1).compare("\\"))
		strDesDir += QString("\\");

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	QString strParam = QString("");
	if("zip" == desFileinfo.suffix())
	{//ZIP
		strParam = QString("7za.exe x -y \"%1\" -o\"%2\"").arg(lpSrcFile).arg(strDesDir);
	}
	else
	{//RAR
		strParam = QString("rar.exe x -y \"%1\" \"%2\"").arg(lpSrcFile).arg(strDesDir);
	}

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if(! CreateProcess(NULL,	// No module name (use command line). 
		(LPSTR)(LPCSTR)strParam.toLocal8Bit().constData(),				// Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		CREATE_NO_WINDOW, // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi)              // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		DWORD dwLastError = GetLastError();
		ClientLogger->AddLog(QString::fromLocal8Bit("[%1] CreateProcess启动失败，错误码[%2]").arg((LPSTR)(LPCSTR)strParam.toLocal8Bit().constData()).arg(dwLastError));
		return -1;			
	}

	WaitForSingleObject(pi.hProcess, INFINITE); 

	bool bRet = false;
	DWORD dwExitCode = 0;
	if(GetExitCodeProcess(pi.hProcess, &dwExitCode))
	{
		if(dwExitCode == STILL_ACTIVE)
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("[%1] 解压超时").arg(strParam));
			TerminateProcess(pi.hProcess, 0);
		}
		else
		{
			switch(dwExitCode)
			{
			case 0:
				bRet = true;
				break;
			case 1:
				bRet = true;
				ClientLogger->AddLog(QString::fromLocal8Bit("[%1] 解压成功，有部分文件被占用").arg(strParam));
				break;
			default:
				ClientLogger->AddLog(QString::fromLocal8Bit("[%1] 解压失败，错误码[%2]").arg(strParam).arg(dwExitCode));
				dwExitCode = -5;
				break;
			}
		}
	}
	else
	{//获取退出码失败，以防万一，强制关闭进程
		ClientLogger->AddLog(QString::fromLocal8Bit("[%1] 获取解压返回值失败").arg(strParam));
		TerminateProcess(pi.hProcess, 0);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return dwExitCode;
}

//通过zlib将一个文件解压至某个目录下
long CMisc::ZDecompressFiles(const QString& lpSrcFile, const QString& lpDesDir)
{
	if(lpSrcFile.isEmpty() || lpDesDir.isEmpty())
		return false;

	//判断源文件后缀名是否为".zip"或".rar"
	QString strSrcFile = lpSrcFile;
	QFileInfo desFileinfo(lpSrcFile);
	qDebug() << desFileinfo.suffix();
	if(desFileinfo.suffix() != "zip" && desFileinfo.suffix() != "rar")
		return false;

	QString strDesDir = QFileInfo(lpDesDir).absoluteFilePath();

	char chSrcPath[MAX_PATH] = {0};
	char chDestPath[MAX_PATH] = {0};

	memcpy(chSrcPath, lpSrcFile.toLocal8Bit().constData(), lpSrcFile.toLocal8Bit().size());
	memcpy(chDestPath, strDesDir.toLocal8Bit().constData(), strDesDir.toLocal8Bit().size());

	DWORD dwRet = UnZipToDirectory(chSrcPath, chDestPath);
	if(0 != dwRet)
	{
		DWORD dwSysRet = GetLastError();
		if(dwSysRet == 5)
		{
			dwRet = 5;
		}

		ClientLogger->AddLog(QString::fromLocal8Bit("[%1] 解压失败，错误码[%2] 系统错误码[%3]").arg(lpSrcFile).arg(dwRet).arg(dwSysRet));
	}

	return dwRet;
}

//在指定目录查找应用程序的所有可执行文件
bool CMisc::FindExe(const QString& strDirName, QStringList &strFileArray)
{
	if(strDirName.isEmpty())
		return false;

	WIN32_FIND_DATA fd;
	HANDLE hSearch = NULL;
	bool bFinished = false;
	bool bReturn = true;

	ZeroMemory(&fd, sizeof(WIN32_FIND_DATA));

	QString strDir = strDirName;
	if(! strDir.isEmpty() && strDir.right(1) != "\\")
	{
		strDir += "\\";
	}

	QString strSearch = strDir + "*.*";
	hSearch = FindFirstFile(strSearch.toLocal8Bit().constData(), &fd);
	if(INVALID_HANDLE_VALUE == hSearch)
	{
		return false;
	}

	while(! bFinished)
	{
		// "."表示当前目录 
		// ".."表示上一级目录
		if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))		
		{//是一个子目录
			strSearch = strDir + QString::fromLocal8Bit(fd.cFileName);
			//在子目录下继续查找e
			FindExe(strSearch, strFileArray);
		}
		else if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))
		{//为一个具体的文件
			QString strFFile = QString::fromLocal8Bit(fd.cFileName);
			if(strFFile.length() > 4 && strFFile.right(4) == ".exe")
			{
				strFileArray.push_back(strFFile);
			}	
		}

		if(! FindNextFile(hSearch, &fd))
		{
			bFinished = true;
		}
	}//while(! bFinished)

	FindClose(hSearch);

	return bReturn;
}

//杀掉所有进程
bool CMisc::KillAllProcessesByTaskKiller(QStringList &strProcessArray)
{
	int nCount = strProcessArray.count();

	if(0 == nCount)
		return true;

	QString strShellCmd = "TASKKILL /F";
	for(int i = 0; i < nCount; i++)
	{
		QString strProcessName = strProcessArray.at(i);
		if("SMonitorClient.exe" != strProcessName )
			strShellCmd += " /IM \"" + strProcessName + "\"";
	}

	strShellCmd += " /T";
	return ExcuteCmd(strShellCmd);
}

//杀掉自己
bool CMisc::KillSelf()
{
	QString strShellCmd = "TASKKILL /F /IM \"SMonitorClient.exe\" /T";
	return ExcuteCmd(strShellCmd);
}

//执行命令行
bool CMisc::ExcuteCmd(const QString& strApp)
{
	if(strApp.isEmpty()) return false;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if(! CreateProcess(NULL,	// No module name (use command line). 
		(LPTSTR)(LPCTSTR)(strApp.toLocal8Bit().constData()),				// Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		CREATE_NO_WINDOW, // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi)              // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		return false;			
	}

	WaitForSingleObject(pi.hProcess, 5000); 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}

//清空目标目录
bool CMisc::DeleteDirectory(const QString& DirName)
{
	if(DirName.isEmpty())
		return true;

	QDir dir(DirName);
	QString tmpdir = "";
	if(!dir.exists())
	{
		return true;
	}

	QFileInfoList fileInfoList = dir.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList)
	{
		if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if(fileInfo.isDir())
		{
			tmpdir = DirName + ("/") + fileInfo.fileName();
			if(!MiscDeleteFile(tmpdir)) /**< 移除子目录 */
			{
				return false;
			}
		}
		else if(fileInfo.isFile())
		{
			QString ss = fileInfo.filePath();
			if(!MiscDeleteFile(ss)) /**< 删除临时文件 */
			{
				return false;
			}
		}
		else
		{
			;
		}
	}

	dir.cdUp();            /**< 返回上级目录，因为只有返回上级目录，才可以删除这个目录 */
	if(dir.exists(DirName))
	{
		if(!MiscDeleteFile(DirName))
			return false;
	}
	return true;
} 

//执行外部EXE
bool CMisc::ExcuteExe(const QString& strExePath, bool bSlient, long lWaitTime, bool bTerminateProcess/* = true*/)
{
	if(strExePath.isEmpty())
		return false;

	PROCESS_INFORMATION processInfoPC;   
	STARTUPINFO startupInfoPC;   
	startupInfoPC.cb = sizeof(STARTUPINFO);   
	startupInfoPC.wShowWindow = bSlient ? SW_HIDE : SW_SHOWNORMAL;
	startupInfoPC.lpReserved  =   NULL;   
	startupInfoPC.lpDesktop   =   NULL;   
	startupInfoPC.lpTitle     =   NULL;   
	startupInfoPC.dwFlags     =   STARTF_USESHOWWINDOW;
	startupInfoPC.cbReserved2   =   0;   
	startupInfoPC.lpReserved2   =   NULL;   
	if(!CreateProcess(NULL, (char*)strExePath.toLocal8Bit().constData(), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfoPC, &processInfoPC))
	{
		DWORD dwLastError = GetLastError();
		ClientLogger->AddLog(QString::fromLocal8Bit("[%1] 启动失败，错误码[%2]").arg((char*)strExePath.toLocal8Bit().constData()).arg(dwLastError));
		return false;
	}

	WaitForSingleObject(processInfoPC.hProcess, -1 == lWaitTime ? INFINITE : lWaitTime);

	if(bTerminateProcess)
	{
		DWORD dwExitCode = 0;
		if(GetExitCodeProcess(processInfoPC.hProcess, &dwExitCode))
		{
			if(dwExitCode == STILL_ACTIVE)
			{
				TerminateProcess(processInfoPC.hProcess, 0);
			}
		}
		else
		{//获取退出码失败，以防万一，强制关闭进程
			TerminateProcess(processInfoPC.hProcess, 0);
		}
	}

	return true;
}

//执行外部EXE
bool CMisc::ShellExcuteExe(const QString& strExePath, bool bSlient, long lWaitTime, bool bTerminateProcess/* = true*/)
{
	if(strExePath.isEmpty())
		return false;

	SHELLEXECUTEINFO si = {0};
	si.cbSize = sizeof(SHELLEXECUTEINFO);
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.hwnd = NULL;
	si.lpVerb = "runas";
	char chFile[256];
	memset(chFile, 0, sizeof(chFile));
	memcpy(chFile, strExePath.toLocal8Bit().constData(), strExePath.toLocal8Bit().size());
	si.lpFile = chFile;
	si.lpParameters = "";
	si.lpDirectory = NULL;
	si.nShow = bSlient ? SW_HIDE : SW_SHOWNORMAL;
	si.hInstApp = NULL;

	if(!ShellExecuteEx(&si))
	{
		DWORD dwLastError = GetLastError();
		ClientLogger->AddLog(QString::fromLocal8Bit("[%1] 启动失败，错误码[%2]").arg((char*)strExePath.toLocal8Bit().constData()).arg(dwLastError));
		return false;
	}

	WaitForSingleObject(si.hProcess, -1 == lWaitTime ? INFINITE : lWaitTime);

	if(bTerminateProcess)
	{
		DWORD dwExitCode = 0;
		if(GetExitCodeProcess(si.hProcess, &dwExitCode))
		{
			if(dwExitCode == STILL_ACTIVE)
			{
				TerminateProcess(si.hProcess, 0);
			}
		}
		else
		{//获取退出码失败，以防万一，强制关闭进程
			TerminateProcess(si.hProcess, 0);
		}
	}

	CloseHandle(si.hProcess);
	return true;
}

//获取系统版本号
QString CMisc::GetOSInfo()
{
	return m_strOsVer;
}

bool CMisc::IsOsVerHigherXP()
{
	SYSTEM_INFO info;                                   //用SYSTEM_INFO结构判断64位AMD处理器
	GetSystemInfo(&info);                               //调用GetSystemInfo函数填充结构
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		if(os.dwMajorVersion >= 6)
			return true;
		else
			return false;
	}

	return true;//默认win7以上系统
}

//判断指定名称的软件本机是否已安装
bool CMisc::IsAppInstall(const QString& strAppName)
{
	//根据注册表获取本机应用程序目录
	if(IsSysWin32())
	{//32位系统
		return _IsAppInstall(WIN32_APPLOCATE, strAppName);
	}
	else
	{//64位系统有可能两个路径都存在
		if(_IsAppInstall(WIN32_APPLOCATE, strAppName))
			return true;

		return	_IsAppInstall(WIN64_APPLOCATE, strAppName);
	}

	return false;
}

//判断指定名称的软件本机是否已安装
bool CMisc::_IsAppInstall(const char* lpSubKey, const QString& strAppName)
{
	if(NULL == lpSubKey || strAppName.isEmpty()) return false;
	QString strCompareAppName = strAppName;
	strCompareAppName = strCompareAppName.toUpper();

	bool bRet = false;
	//打开注册表  
	HKEY hSubKey;   
	REGSAM openMark; 
	if(0 == strcmp(lpSubKey, WIN32_APPLOCATE) && !IsSysWin32())
		openMark = KEY_READ | KEY_WOW64_64KEY;
	else
		openMark = KEY_READ ;

	long lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, openMark, &hSubKey); 

	if(ERROR_SUCCESS == lResult)
	{//打开成功
		DWORD dwIndex = 0;  
		TCHAR szKeyName[256] = {0};   
		DWORD cbName = 256 * sizeof(TCHAR);   

		//开始枚举注册表项
		lResult = RegEnumKeyEx(hSubKey, dwIndex++, szKeyName, &cbName, 0, NULL, NULL, NULL);       
		while (ERROR_SUCCESS == lResult && ERROR_MORE_DATA != lResult)
		{//找到一个应用程序的注册表项   
			QString strKey = QString::fromLocal8Bit(szKeyName);
			if(-1 != strKey.indexOf(strCompareAppName))
			{
				bRet = true;
				break;
			}

			//根据属性displayname判断
			HKEY hItem;      
			if(ERROR_SUCCESS == RegOpenKeyEx(hSubKey, szKeyName, 0, openMark, &hItem))
			{  
				char  chBuf[256] = {0}; 
				DWORD dwSize = sizeof(chBuf);  
				DWORD dwRegType;

				//得到应用程序名
				if(ERROR_SUCCESS == RegQueryValueEx(hItem, "DisplayName", 0, &dwRegType, (LPBYTE)chBuf, &dwSize)) 
				{  
					strKey = QString::fromLocal8Bit(chBuf);
					if(-1 != strKey.indexOf(strCompareAppName))
					{
						bRet = true;
						break;
					}
				} 
			}

			//继续枚举注册表项
			cbName = 256 * sizeof(TCHAR);   
			lResult = RegEnumKeyEx(hSubKey, dwIndex++, szKeyName, &cbName, 0, NULL, NULL, NULL);   	
		}    
	}   

	//关闭注册表  
	RegCloseKey(hSubKey);   
	return bRet;   
}

//判断系统位数
bool CMisc::IsSysWin32()
{
	return m_bIsSys32;
}

bool CMisc::IsNeedUpdate(const QString& localVersion, const QString& svrVersion)
{
	if(svrVersion.length() != 8)
	{//服务器日期格式不正确
		return false;
	}

	if(localVersion.length() != 8)
	{//本地日期格式不对
		return true;
	}

	if(localVersion == svrVersion)
		return false;

	//比较年
	QString strlocal = localVersion.left(4);
	QString strSvr = svrVersion.left(4);
	int nCompareResult = CompareStringNum(strlocal, strSvr);

	if(nCompareResult < 0)
		return true;
	else if(nCompareResult > 0)
		return false;

	//比较月
	strlocal = localVersion.mid(4, 2);
	strSvr = svrVersion.mid(4, 2);
	nCompareResult = CompareStringNum(strlocal, strSvr);

	if(nCompareResult < 0)
		return true;
	else if(nCompareResult > 0)
		return false;

	//比较日
	strlocal = localVersion.mid(6, 2);
	strSvr = svrVersion.mid(6, 2);
	nCompareResult = CompareStringNum(strlocal, strSvr);

	if(nCompareResult < 0)
		return true;
	else if(nCompareResult > 0)
		return false;

	return false;
}

//比较数字型字符串 返回值： 0 相等 -1 小于 1 大于 -2 str1格式错误 2 str2格式错误
int CMisc::CompareStringNum(const QString& str1, const QString& str2)
{
	bool bOk = false;
	long lNum1 = str1.toLong(&bOk);
	if(!bOk)
		return -2;

	long lNum2 = str2.toLong(&bOk);
	if(!bOk)
		return 2;

	if(lNum1 < lNum2)
		return -1;
	else if(lNum1 > lNum2)
		return 1;
	else
		return 0;
}

//将指定文件投射到桌面快捷方式
bool CMisc::LinkToDesktop(const QString& strFilePath, const QString linkName)
{
	if(!CMisc::IsFileExist(strFilePath))
		return false;

	QString strLinkFile = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	if(!CMisc::IsFileExist(strLinkFile))
		return false;

	strLinkFile += "\\" + linkName + ".lnk";
	return QFile::link(strFilePath, strLinkFile);
}

QString CMisc::Encrypt(const QString& src)
{
	QByteArray byteArray = src.toLatin1();
	QByteArray result = qCompress(byteArray);
	QString strEncryBuffer = result.toBase64();
	qDebug() << strEncryBuffer;

	return strEncryBuffer;
}

QString CMisc::Decrypt(const QString& src)
{
	QByteArray byteArray = QByteArray::fromBase64(src.toLocal8Bit());
	QString result = qUncompress(byteArray);
	return result;
}

//通过读取版本号文件获得应用程序版本号
QString CMisc::GetVersionFileVersion(const QString& dir)
{
	QString strVersion = "";
	if(CMisc::IsFileExist(dir))
	{
		QString strVersionFile = dir;
		if(dir.right(1) != "\\" && dir.right(1) != "/")
		{
			strVersionFile += "\\";
		}
		strVersionFile += VERSIONFILE;

		QFile file;
		file.setFileName(strVersionFile);
		if(file.open(QIODevice::ReadOnly))
		{
			//设置文件共享打开
			file.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
			strVersion = file.readAll().constData();
			strVersion = strVersion.trimmed();
			file.close();
		}
	}

	return strVersion;
} 

//获取文件内容
bool CMisc::GetFileContent(const QString& strFilePath, QString &strContent)
{
	QFile file;
	file.setFileName(strFilePath);
	if(file.open(QIODevice::ReadOnly))
	{
		//设置文件共享打开
		file.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
		strContent = file.readAll().constData();
		strContent = strContent.trimmed();
		file.close();
		return true;
	}

	return false;
}

//通过域名解析出IP地址
QStringList CMisc::GetIpByHostName(const QString& strHostName)
{
	QStringList strIpList;
	HOSTENT *pIPHost;
	if((pIPHost = gethostbyname(strHostName.toLocal8Bit().constData())) != NULL)
	{
		int i = 0;
		while(pIPHost->h_addr_list[i])
		{
			char ip[32];
			memcpy(&ip, inet_ntoa(*((struct in_addr *)pIPHost->h_addr_list[i])), sizeof(ip));
			strIpList.push_back(ip);
			i++;
		}
	}

	return strIpList;
}


QStringList CMisc::BrandsNotNeedPasswd()
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	return settings.value("Passwd/BrandNotNeedPasswd", QStringList()).toStringList();
}

void CMisc::SetBrandsNotNeedPasswd(const QString& brand)
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

	QStringList brands = settings.value("Passwd/BrandNotNeedPasswd").toStringList();
	brands.removeAll(brand);
	brands.append(brand);

	settings.setValue("Passwd/BrandNotNeedPasswd", brands);
}

void CMisc::SetBrandsNeedPasswd(const QString& brand)
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

	QStringList brands = settings.value("Passwd/BrandNotNeedPasswd").toStringList();
	brands.removeAll(brand);

	settings.setValue("Passwd/BrandNotNeedPasswd", brands);
}

QString CMisc::GetBrandPasswd(const QString& brand)
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	return settings.value(QString("Passwd/%1").arg(brand), QString()).toString();
}

//读取服务器CrossLink状态
QString CMisc::GetCrossLinkRunInstallState()
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	return settings.value("Local/InstallState", QString()).toString();
}

//设置服务器存储的本地CrossLink安装状态
void CMisc::SetCrossLinkRunInstallState(const QString& state)
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	settings.setValue("Local/InstallState", state);
}

void CMisc::SetBrandPasswd(const QString& brand, const QString& passwd)
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

	settings.setValue(QString("Passwd/%1").arg(brand), passwd);
}

bool CMisc::IsFileExist(const char* pszNameFile)
{
	if(NULL == pszNameFile) return false;

	bool bRes = false;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(pszNameFile, &FindFileData);
	if(INVALID_HANDLE_VALUE != hFind)
	{
		bRes = true;
		FindClose(hFind);
	}

	return bRes;
}

bool CMisc::IsFileExist(const QString& strNameFile)
{
	return IsFileExist(strNameFile.toLocal8Bit().constData());
}

//删除文件/目录
bool CMisc::MiscDeleteFile(const QString& strPath)
{
	if(!CMisc::IsFileExist(strPath))
		return true;

	SHFILEOPSTRUCT fileSH;
	ZeroMemory((void*)&fileSH, sizeof(SHFILEOPSTRUCT));

	TCHAR chShortName[MAX_PATH] = {0};
	GetShortPathName(strPath.toLocal8Bit().constData(), chShortName, MAX_PATH);

	fileSH.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	fileSH.hNameMappings = NULL;
	fileSH.hwnd = NULL;
	fileSH.lpszProgressTitle = NULL;
	fileSH.pFrom = chShortName;
	fileSH.pTo = NULL;
	fileSH.wFunc = FO_DELETE;

	return  0 == SHFileOperation(&fileSH);
}

//判断目录是否安全
bool CMisc::isSafeDirectory(const QString& strPath)
{
	if(strPath.isEmpty())
		return false;

	QString strCompare = strPath.toLower();
	strCompare = strCompare.replace("/", "\\"); 

	int nSize = strCompare.length();
	if(strCompare[nSize - 1] == '\\')
	{
		strCompare = strCompare.remove(nSize - 1, 1);
	}

	if(m_UnSafeList.isEmpty())
	{
		m_UnSafeList << "c:" << "c:\\windows" << "c:\\program files" << "c:\\program files (x86)" << "c:\\windows\\system32"
			<< "c:\\windows\\system" << "d:" << "d:\\windows" << "d:\\program files" << "d:\\program files (x86)" << "d:\\windows\\system32"
			<< "d:\\windows\\system" << "e:" << "e:\\windows" << "e:\\program files" << "e:\\program files (x86)" << "e:\\windows\\system32"
			<< "e:\\windows\\system" << "f:" << "f:\\windows" << "f:\\program files" << "f:\\program files (x86)" << "f:\\windows\\system32"
			<< "f:\\windows\\system" << "g:" << "g:\\windows" << "g:\\program files" << "g:\\program files (x86)" << "g:\\windows\\system32"
			<< "g:\\windows\\system" << "h:" << "g:\\windows" << "h:\\program files" << "h:\\program files (x86)" << "h:\\windows\\system32"
			<< "h:\\windows\\system";
	}
	
	if(m_UnSafeList.contains(strCompare, Qt::CaseInsensitive))
	{
		return false;
	}

	return true;
}

unsigned long CMisc::GetMiscLastError()
{
	return GetLastError();
}

QIcon CMisc::GetFileIcon(const QString& path)
{
	//如果获取错误则返回默认图标
	QIcon icon(":/Resources/normal.png");

	//QFileInfo info(path);
	//if(info.isFile())
	//{
	//	QFileIconProvider iconProvider;
	//	//icon = iconProvider.icon(info);
	//	icon = iconProvider.icon(info.absoluteFilePath());
	//}

	if(path.contains("CrosslinkRun", Qt::CaseInsensitive))
	{
		icon = QIcon(":/Resources/crossrun.png");
	}
	
	return icon;
}