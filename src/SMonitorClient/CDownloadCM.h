#ifndef CDOWNLOADCM_H
#define CDOWNLOADCM_H

#include <QObject>
#include <QFile>

class CHTTPDownloader;

//���̸߳������е��������񣬲�������������з���
class CDownloadCM : public QObject
{
	Q_OBJECT

public:
	CDownloadCM(QObject *parent = 0);
	~CDownloadCM();

	//���һ����������
	void addDownload(const QString& key, const QString& srcPath, const QString& desPath, const QString& version);

	//�ر�����
	void stopDownload();

protected:
	//virtual void run();

Q_SIGNALS:
	void downloadFinished(const QString key);
	void downloadErrorOccured(const QString&key, const QString& msg);
	void downloadPercent(const QString& key, long lDownloadBytes, int totalBytes);
	void downloadQuit(const QString& key);

private Q_SLOTS:
	void downloadFinish();
	void downloadQuited();
	void downloadErrorOccur(const QString& msg);
	void downloadProcess(long lDownLoadByte, int totalBytes);

private:
	CHTTPDownloader*	m_pHTTPDownload;	//�����߳�	
	QString				m_pSoftKey;
	QString				m_pLocalSaveFile;
	QString				m_pTempFile;
	QString             m_strVersionFile;   //���ļ��İ汾��¼�ļ�
	QFile*				m_pRCFile;			//��¼�����ļ�λ��
	bool				m_bRCFileOpend;		//
};

#endif // CCOMMONDOWNLOAD_H
