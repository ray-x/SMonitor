#pragma once

#include <QFont>
#include <QString>
#include <QStringList>
#include <QDateTime>

class CMisc{
public:
	static QFont FontAwesome(int pointSize = 10);

	//��ȡ������ЧIP��ַ����Ӧ��MAC��ַ
	static bool GetLocalHostAddr(QString& IP, QString& MAC);

	//��ȡ�������з����̷���
	static QStringList GetLogicalDriveStringList();
	//��ȡ�ļ�����MD5
	static bool getFileMD5(const QString &strFile, QString &strMD5);

	//��ȡ�ļ���Ӧ����ʱ�ļ�
	static QString getFileTemporary(const QString& strFile);

	//��һ���ļ���ѹ��ĳ��Ŀ¼��
	static bool CMisc::DecompressFiles(const QString& lpSrcFile, const QString& lpDesDir);

	//����һ������
	static QString Encrypt(const QString& src);

	//����һ������
	static QString Decrypt(const QString& src);

	//��ȡINI
	static DWORD ReadIniKeyValue(LPCTSTR lpFileName, LPCTSTR lpSectionName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedStr, DWORD dwSize);

	//��ȡINI
	static DWORD ReadIniKeyValue(LPCTSTR lpFileName, LPCTSTR lpSectionName, LPCTSTR lpKeyName, DWORD dwDefault);

	//����Ʒ������
	static bool SetBrandPasswd(const QString& brand, const QString& passwd);

	//��ȡƷ������
	static QString ReadBrandPasswd(const QString& brand);

	//�Ƚϰ汾��
	static bool IsNeedUpdate(const QString& localVersion, const QString& svrVersion);

	//�Ƚ��������ַ��� ����ֵ�� 0 ��� -1 С�� 1 ���� -2 str1��ʽ���� 2 str2��ʽ����
	static int CompareStringNum(const QString& str1, const QString& str2);

	//ɱ�����н���
	static bool KillAllProcessesByTaskKiller(QStringList &strProcessArray);

	//ִ��������
	static bool ExcuteCmd(const QString& strApp);
};