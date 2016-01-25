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

//32λ����ϵͳ��(ϵͳ64λ������64λ) Ӧ�ó�����Ϣ·��
#define WIN32_APPLOCATE ("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")

//64λ����ϵͳ������32λ Ӧ�ó�����Ϣ·��
#define WIN64_APPLOCATE ("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall")

//�����ȡϵͳλ������ָ��
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);   
LPFN_ISWOW64PROCESS	fnIsWow64Process = NULL;

bool CheckSysWow()
{
	//��ȡϵͳλ��
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
	SYSTEM_INFO info;                                   //��SYSTEM_INFO�ṹ�ж�64λAMD������
	GetSystemInfo(&info);                               //����GetSystemInfo�������ṹ
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		//������ݰ汾��Ϣ�жϲ���ϵͳ����
		switch(os.dwMajorVersion)
		{                        //�ж����汾��
		case 4:
			switch(os.dwMinorVersion){                //�жϴΰ汾��
			case 0:
				if(os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					strOSVersionName = "Microsoft Windows NT 4.0";                //1996��7�·���
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
			switch(os.dwMinorVersion){               //�ٱȽ�dwMinorVersion��ֵ
			case 0:
				strOSVersionName = "Microsoft Windows 2000";                    //1999��12�·���
				break;
			case 1:
				strOSVersionName = "Microsoft Windows XP";                    //2001��8�·���
				break;
			case 2:
				if(os.wProductType == VER_NT_WORKSTATION && info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					strOSVersionName = "Microsoft Windows XP Professional x64 Edition";
				else if(GetSystemMetrics(SM_SERVERR2) == 0)
					strOSVersionName = "Microsoft Windows Server 2003";        //2003��3�·���
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
					strOSVersionName = "Microsoft Windows Server 2008";          //�������汾
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

//�ж�ϵͳ�汾�Ƿ����
bool CMisc::IsOSSuit(QString strOS)
{
	//�����ַ�����ʽ����,��ȷ��ʽΪ "����##ϵͳ##λ��"
	QStringList osDriverNames = strOS.split("##");
	if(osDriverNames.count() != 3)
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("�������� [%1] ��ʽ����,���޸���������Ϊ������##ϵͳ##ϵͳλ���� �磺����1##WIN8.1##WIN32").arg(strOS));
		return false;
	}

	//��ȡϵͳ�汾
	QString strSysVer = GetOSInfo().replace("Microsoft Windows ", "WIN").toUpper();
	QString strSysBits = m_bIsSys32 ? "WIN32" : "WIN64";
	if(strSysVer == "UNKNOWN" || strSysVer.isEmpty())
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("����ϵͳ�汾��δ֪ [%1] �����޷���װ").arg(strOS));
		return false;
	}

	QString osVersion = osDriverNames.at(1).toUpper().replace("WINDOWS", "WIN");//�õ����������е�ϵͳ����
	QString osSysBits = osDriverNames.at(2).toUpper().replace("WINDOWS", "WIN");//�õ����������е�λ��
	if((osVersion == "WINALL" || strSysVer.contains(osVersion)) && (strSysBits == osSysBits || osSysBits == "WINALL"))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("��ǰϵͳ [%1] �������� [%2] ���Զ�ִ��������װ����").arg(strSysVer + strSysBits).arg(strOS));
		return true;
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("��ǰϵͳ [%1] ���������� [%2] ������ϵ����Ա").arg(strSysVer + strSysBits).arg(strOS));
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

	//   ����������Ϣ�ڴ�  
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

//��ȡ�ļ���MD5��
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
//��������һ��ѹ���ļ���ѹ��ĳ��Ŀ¼��
//
//������lpSrcFile - [in]����ѹ���ļ���(ȫ·�����ƻ����·��)���ļ���׺ֻ��Ϊ".zip"��".rar"
//		lpDesDir - [in]ѹ���ļ��������ļ���ѹ���Ŀ��Ŀ¼
//
//����ֵ�����ѹ���ļ������е��ļ�����ѹ�ɹ�,��������TRUE,���򷵻�FALSE.
long CMisc::DecompressFiles(const QString& lpSrcFile, const QString& lpDesDir)
{
	if(lpSrcFile.isEmpty() || lpDesDir.isEmpty())
		return -1;

	//�ж�Դ�ļ���׺���Ƿ�Ϊ".zip"��".rar"
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
		ClientLogger->AddLog(QString::fromLocal8Bit("[%1] CreateProcess����ʧ�ܣ�������[%2]").arg((LPSTR)(LPCSTR)strParam.toLocal8Bit().constData()).arg(dwLastError));
		return -1;			
	}

	WaitForSingleObject(pi.hProcess, INFINITE); 

	bool bRet = false;
	DWORD dwExitCode = 0;
	if(GetExitCodeProcess(pi.hProcess, &dwExitCode))
	{
		if(dwExitCode == STILL_ACTIVE)
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("[%1] ��ѹ��ʱ").arg(strParam));
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
				ClientLogger->AddLog(QString::fromLocal8Bit("[%1] ��ѹ�ɹ����в����ļ���ռ��").arg(strParam));
				break;
			default:
				ClientLogger->AddLog(QString::fromLocal8Bit("[%1] ��ѹʧ�ܣ�������[%2]").arg(strParam).arg(dwExitCode));
				dwExitCode = -5;
				break;
			}
		}
	}
	else
	{//��ȡ�˳���ʧ�ܣ��Է���һ��ǿ�ƹرս���
		ClientLogger->AddLog(QString::fromLocal8Bit("[%1] ��ȡ��ѹ����ֵʧ��").arg(strParam));
		TerminateProcess(pi.hProcess, 0);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return dwExitCode;
}

//ͨ��zlib��һ���ļ���ѹ��ĳ��Ŀ¼��
long CMisc::ZDecompressFiles(const QString& lpSrcFile, const QString& lpDesDir)
{
	if(lpSrcFile.isEmpty() || lpDesDir.isEmpty())
		return false;

	//�ж�Դ�ļ���׺���Ƿ�Ϊ".zip"��".rar"
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

		ClientLogger->AddLog(QString::fromLocal8Bit("[%1] ��ѹʧ�ܣ�������[%2] ϵͳ������[%3]").arg(lpSrcFile).arg(dwRet).arg(dwSysRet));
	}

	return dwRet;
}

//��ָ��Ŀ¼����Ӧ�ó�������п�ִ���ļ�
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
		// "."��ʾ��ǰĿ¼ 
		// ".."��ʾ��һ��Ŀ¼
		if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))		
		{//��һ����Ŀ¼
			strSearch = strDir + QString::fromLocal8Bit(fd.cFileName);
			//����Ŀ¼�¼�������e
			FindExe(strSearch, strFileArray);
		}
		else if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, ".."))
		{//Ϊһ��������ļ�
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

//ɱ�����н���
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

//ɱ���Լ�
bool CMisc::KillSelf()
{
	QString strShellCmd = "TASKKILL /F /IM \"SMonitorClient.exe\" /T";
	return ExcuteCmd(strShellCmd);
}

//ִ��������
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

//���Ŀ��Ŀ¼
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
			if(!MiscDeleteFile(tmpdir)) /**< �Ƴ���Ŀ¼ */
			{
				return false;
			}
		}
		else if(fileInfo.isFile())
		{
			QString ss = fileInfo.filePath();
			if(!MiscDeleteFile(ss)) /**< ɾ����ʱ�ļ� */
			{
				return false;
			}
		}
		else
		{
			;
		}
	}

	dir.cdUp();            /**< �����ϼ�Ŀ¼����Ϊֻ�з����ϼ�Ŀ¼���ſ���ɾ�����Ŀ¼ */
	if(dir.exists(DirName))
	{
		if(!MiscDeleteFile(DirName))
			return false;
	}
	return true;
} 

//ִ���ⲿEXE
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
		ClientLogger->AddLog(QString::fromLocal8Bit("[%1] ����ʧ�ܣ�������[%2]").arg((char*)strExePath.toLocal8Bit().constData()).arg(dwLastError));
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
		{//��ȡ�˳���ʧ�ܣ��Է���һ��ǿ�ƹرս���
			TerminateProcess(processInfoPC.hProcess, 0);
		}
	}

	return true;
}

//ִ���ⲿEXE
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
		ClientLogger->AddLog(QString::fromLocal8Bit("[%1] ����ʧ�ܣ�������[%2]").arg((char*)strExePath.toLocal8Bit().constData()).arg(dwLastError));
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
		{//��ȡ�˳���ʧ�ܣ��Է���һ��ǿ�ƹرս���
			TerminateProcess(si.hProcess, 0);
		}
	}

	CloseHandle(si.hProcess);
	return true;
}

//��ȡϵͳ�汾��
QString CMisc::GetOSInfo()
{
	return m_strOsVer;
}

bool CMisc::IsOsVerHigherXP()
{
	SYSTEM_INFO info;                                   //��SYSTEM_INFO�ṹ�ж�64λAMD������
	GetSystemInfo(&info);                               //����GetSystemInfo�������ṹ
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX); 
	if(GetVersionEx((OSVERSIONINFO *)&os))                  
	{
		if(os.dwMajorVersion >= 6)
			return true;
		else
			return false;
	}

	return true;//Ĭ��win7����ϵͳ
}

//�ж�ָ�����Ƶ���������Ƿ��Ѱ�װ
bool CMisc::IsAppInstall(const QString& strAppName)
{
	//����ע����ȡ����Ӧ�ó���Ŀ¼
	if(IsSysWin32())
	{//32λϵͳ
		return _IsAppInstall(WIN32_APPLOCATE, strAppName);
	}
	else
	{//64λϵͳ�п�������·��������
		if(_IsAppInstall(WIN32_APPLOCATE, strAppName))
			return true;

		return	_IsAppInstall(WIN64_APPLOCATE, strAppName);
	}

	return false;
}

//�ж�ָ�����Ƶ���������Ƿ��Ѱ�װ
bool CMisc::_IsAppInstall(const char* lpSubKey, const QString& strAppName)
{
	if(NULL == lpSubKey || strAppName.isEmpty()) return false;
	QString strCompareAppName = strAppName;
	strCompareAppName = strCompareAppName.toUpper();

	bool bRet = false;
	//��ע���  
	HKEY hSubKey;   
	REGSAM openMark; 
	if(0 == strcmp(lpSubKey, WIN32_APPLOCATE) && !IsSysWin32())
		openMark = KEY_READ | KEY_WOW64_64KEY;
	else
		openMark = KEY_READ ;

	long lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, openMark, &hSubKey); 

	if(ERROR_SUCCESS == lResult)
	{//�򿪳ɹ�
		DWORD dwIndex = 0;  
		TCHAR szKeyName[256] = {0};   
		DWORD cbName = 256 * sizeof(TCHAR);   

		//��ʼö��ע�����
		lResult = RegEnumKeyEx(hSubKey, dwIndex++, szKeyName, &cbName, 0, NULL, NULL, NULL);       
		while (ERROR_SUCCESS == lResult && ERROR_MORE_DATA != lResult)
		{//�ҵ�һ��Ӧ�ó����ע�����   
			QString strKey = QString::fromLocal8Bit(szKeyName);
			if(-1 != strKey.indexOf(strCompareAppName))
			{
				bRet = true;
				break;
			}

			//��������displayname�ж�
			HKEY hItem;      
			if(ERROR_SUCCESS == RegOpenKeyEx(hSubKey, szKeyName, 0, openMark, &hItem))
			{  
				char  chBuf[256] = {0}; 
				DWORD dwSize = sizeof(chBuf);  
				DWORD dwRegType;

				//�õ�Ӧ�ó�����
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

			//����ö��ע�����
			cbName = 256 * sizeof(TCHAR);   
			lResult = RegEnumKeyEx(hSubKey, dwIndex++, szKeyName, &cbName, 0, NULL, NULL, NULL);   	
		}    
	}   

	//�ر�ע���  
	RegCloseKey(hSubKey);   
	return bRet;   
}

//�ж�ϵͳλ��
bool CMisc::IsSysWin32()
{
	return m_bIsSys32;
}

bool CMisc::IsNeedUpdate(const QString& localVersion, const QString& svrVersion)
{
	if(svrVersion.length() != 8)
	{//���������ڸ�ʽ����ȷ
		return false;
	}

	if(localVersion.length() != 8)
	{//�������ڸ�ʽ����
		return true;
	}

	if(localVersion == svrVersion)
		return false;

	//�Ƚ���
	QString strlocal = localVersion.left(4);
	QString strSvr = svrVersion.left(4);
	int nCompareResult = CompareStringNum(strlocal, strSvr);

	if(nCompareResult < 0)
		return true;
	else if(nCompareResult > 0)
		return false;

	//�Ƚ���
	strlocal = localVersion.mid(4, 2);
	strSvr = svrVersion.mid(4, 2);
	nCompareResult = CompareStringNum(strlocal, strSvr);

	if(nCompareResult < 0)
		return true;
	else if(nCompareResult > 0)
		return false;

	//�Ƚ���
	strlocal = localVersion.mid(6, 2);
	strSvr = svrVersion.mid(6, 2);
	nCompareResult = CompareStringNum(strlocal, strSvr);

	if(nCompareResult < 0)
		return true;
	else if(nCompareResult > 0)
		return false;

	return false;
}

//�Ƚ��������ַ��� ����ֵ�� 0 ��� -1 С�� 1 ���� -2 str1��ʽ���� 2 str2��ʽ����
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

//��ָ���ļ�Ͷ�䵽�����ݷ�ʽ
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

//ͨ����ȡ�汾���ļ����Ӧ�ó���汾��
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
			//�����ļ������
			file.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
			strVersion = file.readAll().constData();
			strVersion = strVersion.trimmed();
			file.close();
		}
	}

	return strVersion;
} 

//��ȡ�ļ�����
bool CMisc::GetFileContent(const QString& strFilePath, QString &strContent)
{
	QFile file;
	file.setFileName(strFilePath);
	if(file.open(QIODevice::ReadOnly))
	{
		//�����ļ������
		file.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
		strContent = file.readAll().constData();
		strContent = strContent.trimmed();
		file.close();
		return true;
	}

	return false;
}

//ͨ������������IP��ַ
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

//��ȡ������CrossLink״̬
QString CMisc::GetCrossLinkRunInstallState()
{
	QSettings settings(HKCU, QSettings::NativeFormat);
	settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
	return settings.value("Local/InstallState", QString()).toString();
}

//���÷������洢�ı���CrossLink��װ״̬
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

//ɾ���ļ�/Ŀ¼
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

//�ж�Ŀ¼�Ƿ�ȫ
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
	//�����ȡ�����򷵻�Ĭ��ͼ��
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