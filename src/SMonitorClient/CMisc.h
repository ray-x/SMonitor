#pragma once

#include <QFont>
#include <QString>
#include <QStringList>
#include <QIcon>

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
	static long DecompressFiles(const QString& lpSrcFile, const QString& lpDesDir);

	//ͨ��zlib��һ���ļ���ѹ��ĳ��Ŀ¼��
	static long ZDecompressFiles(const QString& lpSrcFile, const QString& lpDesDir);

	//��ָ��Ŀ¼����Ӧ�ó�������п�ִ���ļ�
	static bool FindExe(const QString& strDirName, QStringList &strFileArray);

	//ɱ�����н���
	static bool KillAllProcessesByTaskKiller(QStringList &strProcessArray);

	//ɱ���Լ�
	static bool KillSelf();

	//ִ��������
	static bool ExcuteCmd(const QString& strApp);

	//���Ŀ��Ŀ¼
	static bool DeleteDirectory(const QString& DirName);

	//ִ���ⲿEXE
	static bool ExcuteExe(const QString& strExePath, bool bSlient, long lWaitTime, bool bTerminateProcess = true);

	//ִ���ⲿEXE
	static bool ShellExcuteExe(const QString& strExePath, bool bSlient, long lWaitTime, bool bTerminateProcess = true);

	//��ȡϵͳ�汾��
	static QString GetOSInfo();

	static bool IsOsVerHigherXP();

	//�ж�ϵͳ�汾�Ƿ����
	static bool IsOSSuit(QString strOS);

	//�ж�ָ�����Ƶ���������Ƿ��Ѱ�װ
	static bool IsAppInstall(const QString& strAppName);

	//�ж�ϵͳλ��
	static bool IsSysWin32();

	//�Ƚϰ汾��
	static bool IsNeedUpdate(const QString& localVersion, const QString& svrVersion);

	//�Ƚ��������ַ��� ����ֵ�� 0 ��� -1 С�� 1 ���� -2 str1��ʽ���� 2 str2��ʽ����
	static int CompareStringNum(const QString& str1, const QString& str2);

	//��ָ���ļ�Ͷ�䵽�����ݷ�ʽ
	static bool LinkToDesktop(const QString& strFilePath, const QString linkName);

	//����һ������
	static QString Encrypt(const QString& src);

	//����һ������
	static QString Decrypt(const QString& src);

	//ͨ���汾���ļ���ȡ�汾��
	static QString GetVersionFileVersion(const QString& dir);

	//��ȡ�ļ�����
	static bool GetFileContent(const QString& strFilePath, QString &strContent);

	//ͨ������������IP��ַ
	static QStringList GetIpByHostName(const QString& strHostName);

	//��ȡ����Ҫ���������Ʒ���б�
	static QStringList BrandsNotNeedPasswd();

	//���ò���Ҫ���������Ʒ��
	static void SetBrandsNotNeedPasswd(const QString& brand);

	//���ò���Ҫ���������Ʒ��
	static void SetBrandsNeedPasswd(const QString& brand);

	//����Ʒ������
	static void SetBrandPasswd(const QString& brand, const QString& passwd);

	//��ȡƷ������
	static QString GetBrandPasswd(const QString& brand);

	//��ȡ�������洢�ı���CrossLink��װ״̬
	static QString GetCrossLinkRunInstallState();

	//���÷������洢�ı���CrossLink��װ״̬
	static void SetCrossLinkRunInstallState(const QString& state);

	//API�ж��ļ��Ƿ����
	static bool IsFileExist(const char* pszNameFile);
	static bool IsFileExist(const QString& strNameFile);

	//ɾ���ļ�/Ŀ¼
	static bool MiscDeleteFile(const QString& strPath);

	//�ж�Ŀ¼�Ƿ�ȫ
	static bool isSafeDirectory(const QString& strPath);

	static unsigned long GetMiscLastError();

	//�����ļ�·����ȡ�ļ�ͼ��
	static QIcon GetFileIcon(const QString& path);

private:

	//�ж�ָ�����Ƶ���������Ƿ��Ѱ�װ
	static bool _IsAppInstall(const char* lpSubKey, const QString& strAppName);

private:
	static bool m_bIsSys32; //ϵͳ�Ƿ�Ϊ32λ
	static QString m_strOsVer;//��ǰϵͳ�汾�ַ���
	static QStringList m_UnSafeList;
};