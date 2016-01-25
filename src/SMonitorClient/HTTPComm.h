#pragma once

#include <QObject>
#include <QString>

//HTTP������������
typedef enum _HTTPTYPE
{
	HTTPCONNECTION = 0, //HTTP���Ӳ���
	HTTPSESSION,        //HTTP�Ự����
	HTTPFILE            //HTTP�ļ�����
}HTTPTYPE;

class CHTTPBase  : public QObject
{
	Q_OBJECT
public:
	CHTTPBase(QObject *parent = 0);
	virtual ~CHTTPBase();

	//�������ع�����
	void SetRelativeDownloader(class CHTTPDownloader *pRelativeDownloader);
	
	//����HTTP������ز���(����OpenConnection֮�����, ������Ч)
	void SetOption(HTTPTYPE type, unsigned long dwOption, unsigned long dwValue, unsigned long dwFlags = 0);

	//�������ز���
	//����:   dwTransferBlockSize - [in] ÿ�δ�����ֽ���
	//        dwMaxTransferRetryTimes - [in] ������Դ���
	//        dwBreakPointRetryInterval - [in] ����֮��ĵȴ�ʱ��
	//        lDownloadStartBytes - [in] �ļ����ؿ�ʼλ�� 
	//        bOverWrite - [in] �ô�д���Ƿ�����ļ�
	//ע��:   �˺�����ѡ����������ô˺�������Ĭ��ֵ��dwTransferBlockSize - 1024 dwMaxTransferRetryTimes - 2 dwBreakPointRetryInterval - 1000 
	void SetDownloadPara(unsigned long dwTransferBlockSize, unsigned long dwMaxTransferRetryTimes, unsigned long dwBreakPointRetryInterval, unsigned long lDownloadStartBytes, bool bOverWrite);

	//������
	//����:   lpUrl - [in] Ҫ�򿪵�HTTP url
	//        lpErrMsg - [out] ������Ϣ���򿪳ɹ�ʱ���ؿ�
	//        bOpenedType - [in] �򿪷�ʽ true - ��������/�ϴ��ļ�  false - �����ļ�
	//����ֵ: true - �򿪳ɹ�,  false - ��ʧ��
	//ִ�з��������ϴ��ļ��������ļ�������OpenConnection�����CloseConnection
	bool OpenConnection(const QString& strUrl, QString& lpErrMsg, bool bOpenedType = true, bool bPost = true);

	//��ӱ���ͷ
	//����:   lpHeaderName - [in] ����ͷ
	//        lpHeaderValue - [in] ����ֵ
	//        lpErrMsg - [in,out] ������Ϣ
	//����ֵ: true - ��ӳɹ� false - ���ʧ��
	//ע��: OpenConnection����ӱ���ͷ���˺�����ѡ
	bool AddHeader(const QString& strHeaderName, const QString& strHeaderValue, QString& lpErrMsg);

	//��ӱ��Ĳ���
	//����:   lpParaName - [in] ������
	//        lpParaValue - [in] ����ֵ
	//        lpBoundary - [in] �ָ���,��������˷ָ�������ʱ����ΪFORM��ʽ,�ָ�������ͨ��GetBoundary��ȡ
	//����ֵ: true - ��ӳɹ� false - ���ʧ��
	//ע��: OpenConnection����ӷ��͵Ĳ������˺�����ѡ
	bool AddPara(const QString& lpParaName, const QString& lpParaValue, const QString& lpBoundary = "");

	//���FORM��β��
	//ע��: ��AddPara�������FORM��ʽ�Ĳ���������ʹ�õ���SendRequest������Ҫ��SendRequest֮ǰ����AddFormEnd
	//      ���ΪUploadFile����Ҫ
	bool AddFormEnd();

	//��ȡ�ָ���
	//����ֵ:  ���طָ���
	//ע��: OpenConnection֮������,����һ����������ǲ���ģ�ֻ��OpenConnection֮��ı�
	QString GetBoundary();
	
	//����HTTP����
	//����:   lpErrMsg - [out]����ɹ�ʱ���ؿգ�����ʧ��ʱ���ش�����Ϣ
	//����ֵ: true - ����ɹ��� false - ����ʧ�� 
	//ע��: ������OpenConnection
	bool SendRequest(QString& lpErrMsg);

	//�����ļ�
	//����:   lpFilePath - [in]�ļ��ı��ش洢·��,���·����ָ�����ļ��������Դ������棬�����Է����������������ֱ���
	//                          ���·����û��ָ���ļ���׺�����Է��������ص��ļ���׺����
	//        lpErrMsg - [out]�ļ����سɹ�ʱ���ؿգ��ļ�����ʧ��ʱ���ش�����Ϣ
	//����ֵ: true - ���سɹ��� false - ����ʧ��
	//ע��: ������OpenConnection��һ��ֻ����һ���ļ������Ǳ���CloseConnection -> OpenConnection -> DownloadFile
	bool DownloadFile(QString& lpFilePath, QString& lpErrMsg);

	//ֹͣ����
	void StopDownload();

	//��ȡ�ϴ�/�����ļ���ǰ�Ѵ�����ֽ���
	//����:   dwOverCount - [out] ��ǰ�Ѵ�����ֽ���
	//        dwTotalCount - [out] �ϴ�/�����ܵ��ֽ���
	//����ֵ  true - ��ȡ��Ϣ�ɹ� false -��ȡ��Ϣʧ��
	void GetCount(unsigned long &dwOverCount, unsigned long &dwTotalCount);

	//��ȡ��������Ӧ����
	//����ֵ: ��������Ӧ������
	//ע��: SendRequest��UploadFile�����
	QString GetResponseData();
	
	//�ر�����
	void CloseConnection(bool bInitPara);
	
	//���ַ����е������ַ�����ת�룬ʹ֮�ܹ�ͨ��HTTP����
	void TransCode(QString &str);

protected:

	//��ʼ������
	void Init();
	
	//���ϴ�����
	//����:  lpUrl - [in] Զ�̷�������·��
	//       lpErrMsg - [out] ���ش�����Ϣ
	bool OpenUploadConnection(const QString& lpUrl, QString& lpErrMsg, bool bPost = true);

	//��HTTPS��������
	//����:  lpUrl - [in] Զ�̷�������·��
	//       lpErrMsg - [out] ���ش�����Ϣ
	bool OpenUploadHTTPSConnection(const QString& lpUrl, QString& lpErrMsg, bool bPost = true);

	//����������
	//����:  lpUrl - [in] Զ�̷�������·��
	//       lpErrMsg - [out] ���ش�����Ϣ
	bool OpenDownloadConnection(const QString& lpUrl, QString& lpErrMsg);

	//��֯HTTP����ͷ
	virtual void PrePareHTTPHeaderData() = 0;
	
	//��֯��������Ϣ
	//����: - strBoundary [in] - �ָ���
	//����ֵ: �����ļ���ǰ�������Ϣ
	virtual void PrePareHTTPParaData(const QString& lpBoundary) = 0;

	//��ȡ�ϴ��ļ�ʱ�ļ����������Ϣ
	//����: - strBoundary [in] - �ָ���
	//����ֵ: �����ļ���ǰ�������Ϣ
	virtual QString GetFilePropertyData(int nNum, const QString& lpBoundary, const QString& lpFilePath, const QString& lpNewFileName) = 0;

	//��ȡ��β����Ϣ
	//����: - lpBoundary [in] - �ָ���
	//����ֵ: �����ļ���������Ϣ
	virtual QString GetEndFileData(const QString& lpBoundary) = 0;

	//�������ݣ����洢���ļ�
	//����: - lpFilePath - [in] �ļ��Ĵ洢·��
	//		- lpErrMsg [in,out] ������Ϣ
	//      - dwDownloadPerTime [in] ÿ�����ص��ֽ���
	//����ֵ: true - ���سɹ� false - ����ʧ��
	bool DownloadDataToFile(const QString& lpFilePath, QString& lpErrMsg,unsigned long dwDownloadPerTime = 1024);

	//��ȡ��������Ӧ����
	//����: - lpErrMsg [out] ������Ϣ
	//����ֵ: true - ��ȡ�ɹ� false - ��ȡʧ��
	bool GetResponseData(QString& lpErrMsg);
	
	//��������һ���ָ���
	QString RandomMakeBoundary();

	//����URL���ļ���Ϣ���û��ṩ���ļ���Ϣ��ϵõ����ش洢�ľ���·�����ļ���
	//����:  lpUrl - [in] �����ļ���URL��ַ
	//       strFilePath - [in] �û�ָ���ı��ش洢·��(�������ļ��У�Ҳ�������ļ�)
	//       lpFileName - [in] strFilePathֻ��·����lpFileName�Ǳ��ص��ļ���
	//       lpErrMsg - [out] ��ȡ�ɹ�ʱ���ؿգ���ȡʧ��ʱ���ش�����Ϣ 
	//ע�⣺lpUrl��strFilePath��lpFileName�����ʹ�ã�strFilePath��һ���ļ���ʱ�����δָ��lpFileName��
	//      ���ὫlpUrl���ļ�����Ϊ�����ļ��������lpFileName���ƶ�������lpFileName�洢�����lpFileNameδָ��
	//      ��׺������lpUrl�ĺ�׺�洢�����strFilePath��һ���ļ�����·��ʱ��ֱ����strFilePath�洢
	bool GetLocalFilePath(const QString& lpUrl, QString &strFilePath, const QString& lpFileName, QString& lpErrMsg);

	//��ȡ�ļ���
	//����: lpFilePath - [in]�ļ�����·��
	//		strFileName - [out]�ļ���
	//      strFileExt - [out] �ļ���׺
	void GetFileName(const QString& lpFilePath, QString &strFileName, QString &strFileExt);	

	//�㱨����
	void ReportError(const QString& errInfo);

protected:
	//��ǰHTTP�Ự��SESSION 
	class CInternetSession *m_pHttpSession;

	//��ǰHTTP�Ự�е�����
	class CHttpConnection *m_pHttpCon;

	//��ǰHTTP�Ự�е��ļ�ָ��
	class CHttpFile *m_pHttpFile;

	//�ѷ���/���ص��ֽ���
	unsigned long m_dwTreatOverCount;

	//����/�����ܵ��ֽ���
	unsigned long m_dwTotalCount;

	//���������ص�����
	QString m_strRecvData;

	//������
	QString m_strParaData;

	//Զ�̷�����URL
	QString m_strUrl;

	//�ָ���
	QString m_strSeparate;

	//����ʱÿ�δ�����ֽ���
	unsigned long m_dwTransferBlockSize;

	//����ʱ������Դ���
	unsigned long m_dwMaxTransferRetryTimes;

	//����ʱ����֮��ĵȴ�ʱ��
	unsigned long m_dwBreakPointRetryInterval;

	//�ô�������ʼλ��
	unsigned long m_lDownloadStartBytes;

	//�Ƿ����������ز���
	bool m_bSetDownloadPara;

	//��ʾÿһ��HTTP���ĵ�Ψһ��ID
	unsigned long m_dwHttpID;    

	class CHTTPDownloader* m_pRelativeDownloader;

	bool m_bStop;

	bool m_bOverWrite;
};

//HTTP������
class CHTTPComm : public CHTTPBase
{
public:
	CHTTPComm(QObject *parent = 0);
	~CHTTPComm();

protected:
	//��֯HTTP����ͷ
	virtual void PrePareHTTPHeaderData();
	
	//��֯��������Ϣ
	//����: - strBoundary [in] - �ָ���
	//����ֵ: �����ļ���ǰ�������Ϣ
	virtual void PrePareHTTPParaData(const QString& lpBoundary);

	//��ȡ�ϴ��ļ�ʱ�ļ����������Ϣ
	//����:  nNum - [in] �ļ����
	//       lpBoundary - [in] �ָ���
	//       lpFilePath - [in] �ļ�·��
	//       lpNewFileName - [in] �µ��ļ���
	//����ֵ: �����ļ����������Ϣ
	virtual QString GetFilePropertyData(int nNum, const QString& lpBoundary, const QString& lpFilePath, const QString& lpNewFileName);

	//��ȡ��β����Ϣ
	//����: - nNum [in] - �ļ����
	//      - strFileName [in] - �ļ���
	//����ֵ: �����ļ���������Ϣ
	virtual QString GetEndFileData(const QString& lpBoundary);
	
};