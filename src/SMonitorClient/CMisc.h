#pragma once

#include <QFont>
#include <QString>
#include <QStringList>
#include <QIcon>

class CMisc{
public:
	static QFont FontAwesome(int pointSize = 10);

	//获取本地有效IP地址及对应的MAC地址
	static bool GetLocalHostAddr(QString& IP, QString& MAC);

	//获取本地所有分区盘符号
	static QStringList GetLogicalDriveStringList();
	//获取文件内容MD5
	static bool getFileMD5(const QString &strFile, QString &strMD5);

	//获取文件对应的零时文件
	static QString getFileTemporary(const QString& strFile);

	//将一个文件解压至某个目录下
	static long DecompressFiles(const QString& lpSrcFile, const QString& lpDesDir);

	//通过zlib将一个文件解压至某个目录下
	static long ZDecompressFiles(const QString& lpSrcFile, const QString& lpDesDir);

	//在指定目录查找应用程序的所有可执行文件
	static bool FindExe(const QString& strDirName, QStringList &strFileArray);

	//杀掉所有进程
	static bool KillAllProcessesByTaskKiller(QStringList &strProcessArray);

	//杀掉自己
	static bool KillSelf();

	//执行命令行
	static bool ExcuteCmd(const QString& strApp);

	//清空目标目录
	static bool DeleteDirectory(const QString& DirName);

	//执行外部EXE
	static bool ExcuteExe(const QString& strExePath, bool bSlient, long lWaitTime, bool bTerminateProcess = true);

	//执行外部EXE
	static bool ShellExcuteExe(const QString& strExePath, bool bSlient, long lWaitTime, bool bTerminateProcess = true);

	//获取系统版本号
	static QString GetOSInfo();

	static bool IsOsVerHigherXP();

	//判断系统版本是否兼容
	static bool IsOSSuit(QString strOS);

	//判断指定名称的软件本机是否已安装
	static bool IsAppInstall(const QString& strAppName);

	//判断系统位数
	static bool IsSysWin32();

	//比较版本号
	static bool IsNeedUpdate(const QString& localVersion, const QString& svrVersion);

	//比较数字型字符串 返回值： 0 相等 -1 小于 1 大于 -2 str1格式错误 2 str2格式错误
	static int CompareStringNum(const QString& str1, const QString& str2);

	//将指定文件投射到桌面快捷方式
	static bool LinkToDesktop(const QString& strFilePath, const QString linkName);

	//加密一个数据
	static QString Encrypt(const QString& src);

	//解密一个数据
	static QString Decrypt(const QString& src);

	//通过版本号文件获取版本号
	static QString GetVersionFileVersion(const QString& dir);

	//获取文件内容
	static bool GetFileContent(const QString& strFilePath, QString &strContent);

	//通过域名解析出IP地址
	static QStringList GetIpByHostName(const QString& strHostName);

	//读取不需要输入密码的品牌列表
	static QStringList BrandsNotNeedPasswd();

	//设置不需要输入密码的品牌
	static void SetBrandsNotNeedPasswd(const QString& brand);

	//设置不需要输入密码的品牌
	static void SetBrandsNeedPasswd(const QString& brand);

	//设置品牌密码
	static void SetBrandPasswd(const QString& brand, const QString& passwd);

	//获取品牌密码
	static QString GetBrandPasswd(const QString& brand);

	//读取服务器存储的本地CrossLink安装状态
	static QString GetCrossLinkRunInstallState();

	//设置服务器存储的本地CrossLink安装状态
	static void SetCrossLinkRunInstallState(const QString& state);

	//API判断文件是否存在
	static bool IsFileExist(const char* pszNameFile);
	static bool IsFileExist(const QString& strNameFile);

	//删除文件/目录
	static bool MiscDeleteFile(const QString& strPath);

	//判断目录是否安全
	static bool isSafeDirectory(const QString& strPath);

	static unsigned long GetMiscLastError();

	//根据文件路径获取文件图标
	static QIcon GetFileIcon(const QString& path);

private:

	//判断指定名称的软件本机是否已安装
	static bool _IsAppInstall(const char* lpSubKey, const QString& strAppName);

private:
	static bool m_bIsSys32; //系统是否为32位
	static QString m_strOsVer;//当前系统版本字符串
	static QStringList m_UnSafeList;
};