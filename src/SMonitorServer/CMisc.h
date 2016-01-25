#pragma once

#include <QFont>
#include <QString>
#include <QStringList>
#include <QDateTime>

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
	static bool CMisc::DecompressFiles(const QString& lpSrcFile, const QString& lpDesDir);

	//加密一个数据
	static QString Encrypt(const QString& src);

	//解密一个数据
	static QString Decrypt(const QString& src);

	//读取INI
	static DWORD ReadIniKeyValue(LPCTSTR lpFileName, LPCTSTR lpSectionName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedStr, DWORD dwSize);

	//读取INI
	static DWORD ReadIniKeyValue(LPCTSTR lpFileName, LPCTSTR lpSectionName, LPCTSTR lpKeyName, DWORD dwDefault);

	//设置品牌密码
	static bool SetBrandPasswd(const QString& brand, const QString& passwd);

	//读取品牌密码
	static QString ReadBrandPasswd(const QString& brand);

	//比较版本号
	static bool IsNeedUpdate(const QString& localVersion, const QString& svrVersion);

	//比较数字型字符串 返回值： 0 相等 -1 小于 1 大于 -2 str1格式错误 2 str2格式错误
	static int CompareStringNum(const QString& str1, const QString& str2);

	//杀掉所有进程
	static bool KillAllProcessesByTaskKiller(QStringList &strProcessArray);

	//执行命令行
	static bool ExcuteCmd(const QString& strApp);
};