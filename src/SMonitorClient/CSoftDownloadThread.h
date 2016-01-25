#ifndef CSOFTDOWNLOADTHREAD_H
#define CSOFTDOWNLOADTHREAD_H

#include <QThread>

class CHTTPDownloader;

//���̸߳������е��������񣬲�������������з���
class CSoftDownloadThread : public QThread
{
	Q_OBJECT

public:
	CSoftDownloadThread(const QString& key, const QString& srcPath, const QString& desPath, 
		long startBytes, bool truncate, QObject *parent);
	~CSoftDownloadThread();

protected:
	virtual void run();

Q_SIGNALS:
	void downloadFinished(const QString& key);
	void downloadErrorOccured(const QString&key, const QString& msg);
	void downloadPercent(const QString& key, long lDownloadBytes, int totalBytes);

private Q_SLOTS:
	void downloadFinish();
	void downloadErrorOccur(const QString& msg);
	void loadPercent(long lDownLoadByte, int totalBytes);

private:
	CHTTPDownloader*	m_pHTTPDownload;	//
	QString				m_pSrcPath;			//����λ��(http��Դλ��)
	QString				m_pDesPath;			//���ص�Ŀ��λ��
	QString				m_pSoftKey;			//��������ı�ʶ
	long				m_pStartBytes;		//��ʼ�����ļ���λ��
	bool				m_pTruncate;		//�Ƿ���Ҫ�����ļ�
};

#endif // CSOFTWAREDOWNLOADTHREAD_H
