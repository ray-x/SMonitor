#ifndef CHTTPDOWNLOADER_H
#define CHTTPDOWNLOADER_H

#include <QThread>
class CHTTPBase;
class CHTTPDownloader : public QThread
{
	Q_OBJECT

public:
	friend class CHTTPBase;
	CHTTPDownloader(QObject *parent = 0);
	~CHTTPDownloader();
	bool       m_bStop;   // ����֪ͨ�߳�ϵͳ�˳����¼���Ϊ���ܹ����õ��˳��߳�

	//�����ļ�
	//strFileUrl - �ļ�URL strLocalSavePath - �ļ����ش洢λ�� lDownloadStartBytes - �ļ����ؿ�ʼλ�� dwTransferBlockSize - ÿ�δ���������ٶ� bOverWrite - �ô�д���Ƿ�����ļ�
	void downloadFileSync(const QString strFileUrl, const QString strLocalSavePath, long lDownloadStartBytes, bool bOverWrite, long lTransferBlockSize = 51200);

	//ֹͣ����
	void stopDownload();

Q_SIGNALS:
	void sig_DownPercent(long lDownloadBytes, int totalBytes);
	void sig_DownFinished();
	void sig_DownQuit();
	void sig_ErrorOccur(const QString& errMsg); //�������� ���ػ������жϣ�������Ҫ��ʾ

public slots:
	void errorOccur(const QString& errMsg);

public:
	void OpenConnection();

protected:
	virtual void run();

	QString    m_strFileUrl;
	QString    m_strLocalSacePath;
	long       m_lDownLoadStartBytes;
	long       m_lTransferBlockSize;
	bool       m_bOverWrite;
	CHTTPBase* m_pHttpbase;
	QString    m_strErrMsg;
	bool       m_bOccurError;
};

#endif // CHTTPDOWNLOADER_H
