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

#include "stdafx.h"
#include "CMisc.h"

#include <QStringList>
#include <QFontDatabase>
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <QDebug>
#include <QApplication>

#include <windows.h>
#include <WinBase.h>
#include <MMSystem.h>
#include <IPHlpApi.h>
#include <TlHelp32.h>

#pragma comment(lib, "Iphlpapi.lib")

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
bool CMisc::DecompressFiles(const QString& lpSrcFile, const QString& lpDesDir)
{
	if(lpSrcFile.isEmpty() || lpDesDir.isEmpty())
		return false;

	//判断源文件后缀名是否为".zip"或".rar"
	QString strSrcFile = lpSrcFile;
	QFileInfo desFileinfo(lpSrcFile);
	qDebug() << desFileinfo.suffix();
	if(desFileinfo.suffix() != "zip" && desFileinfo.suffix() != "rar")
		return false;

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
		return FALSE;			
	}

	WaitForSingleObject(pi.hProcess, INFINITE); 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return TRUE;
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
	QByteArray byteArray = QByteArray::fromBase64(src.toLatin1());
	QString result = qUncompress(byteArray);
	return result;
}

//读取INI
DWORD CMisc::ReadIniKeyValue(LPCTSTR lpFileName, LPCTSTR lpSectionName, LPCTSTR lpKeyName, DWORD dwDefault)
{
	return GetPrivateProfileInt(lpSectionName, lpKeyName, dwDefault, lpFileName);
}

//读取INI
DWORD CMisc::ReadIniKeyValue(LPCTSTR lpFileName, LPCTSTR lpSectionName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedStr, DWORD dwSize)
{
	if(NULL == lpReturnedStr || 0 == dwSize)
		return 0;

	if(NULL == lpFileName || NULL == lpSectionName || NULL == lpKeyName)
	{
		DWORD dwCopy = 0;
		if(NULL != lpDefault)
		{
			DWORD dwCopy = lstrlen(lpDefault);
			if(dwCopy > dwSize)
				dwCopy = dwSize;

			lstrcpyn(lpReturnedStr, lpDefault, dwCopy);
		}

		return dwCopy;
	}

	return GetPrivateProfileString(lpSectionName, lpKeyName, lpDefault, lpReturnedStr, dwSize, lpFileName);
}

QString CMisc::ReadBrandPasswd(const QString& brand)
{
	QString path = QApplication::applicationDirPath() + "/configure.ini";
	char chPasswd[256] = {0};
	CMisc::ReadIniKeyValue(path.toLocal8Bit().constData(), "Passwd", 
		brand.toLocal8Bit().constData(), "", chPasswd, 256);

	QString passwd = QString(chPasswd);

	return CMisc::Decrypt(passwd);
}

bool CMisc::SetBrandPasswd(const QString& brand, const QString& passwd)
{
	QString path = QApplication::applicationDirPath() + "/configure.ini";
	return WritePrivateProfileString("Passwd", brand.toLocal8Bit().constData(),
				CMisc::Encrypt(passwd).toLocal8Bit().constData(), path.toLocal8Bit().constData());
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