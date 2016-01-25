#include "HTTPComm.h"

#include <AFXINET.H>
#include "CHTTPDownloader.h"
#include <QFileInfo>
#include <QDir>
CHTTPBase::CHTTPBase(QObject *parent)
	: QObject(parent)
{
	m_dwHttpID = 0;
	Init();
}
CHTTPBase::~CHTTPBase()
{
	CloseConnection(true);
}

//��ʼ������
void CHTTPBase::Init()
{
	m_pHttpSession = NULL;
	m_pHttpCon = NULL;
	m_pHttpFile = NULL;
	m_pRelativeDownloader = NULL;
	
	m_strParaData = _T("");
	m_strSeparate = _T("");
	m_strUrl = _T("");
	m_strRecvData = _T("");
	
	m_dwTotalCount = 0;
	m_dwTreatOverCount = 0;
	
	m_dwTransferBlockSize = 1024;
	m_dwMaxTransferRetryTimes = 2;
	m_dwBreakPointRetryInterval = 1000;
	m_lDownloadStartBytes = 0;
	
	m_bSetDownloadPara = false;
	m_bStop = false;
	m_bOverWrite = true;
}

//�������ع�����
void CHTTPBase::SetRelativeDownloader(CHTTPDownloader *pRelativeDownloader)
{
	m_pRelativeDownloader = pRelativeDownloader;
}

//����HTTP������ز���(����OpenConnection֮�����, ������Ч)
void CHTTPBase::SetOption(HTTPTYPE type, unsigned long dwOption, unsigned long dwValue, unsigned long dwFlags /* = 0 */)
{
	if(HTTPCONNECTION == type && m_pHttpCon)
	{//HTTP���Ӳ���
		m_pHttpCon->SetOption(dwOption, dwValue, dwFlags);
	}
	else if(HTTPSESSION == type && m_pHttpSession)
	{//HTTP�Ự����
		m_pHttpSession->SetOption(dwOption, dwValue, dwFlags);
	}
	else if(HTTPFILE == type && m_pHttpFile)
	{//HTTP�ļ�����
		m_pHttpFile->SetOption(dwOption, dwValue, dwFlags);
	}
}

//�������ز���
//����:   dwTransferBlockSize - [in] ÿ�δ�����ֽ���
//        dwMaxTransferRetryTimes - [in] ������Դ���
//        dwBreakPointRetryInterval - [in] ����֮��ĵȴ�ʱ��
//        lDownloadStartBytes - [in] �ļ����ؿ�ʼλ�� 
//        bOverWrite - [in] �ô�д���Ƿ�����ļ�
//ע��:   �˺�����ѡ����������ô˺�������Ĭ��ֵ��dwTransferBlockSize - 1024 dwMaxTransferRetryTimes - 2 dwBreakPointRetryInterval - 1000
void CHTTPBase::SetDownloadPara(unsigned long dwTransferBlockSize, unsigned long dwMaxTransferRetryTimes, unsigned long dwBreakPointRetryInterval, unsigned long lDownloadStartBytes, bool bOverWrite)
{
	m_dwTransferBlockSize = dwTransferBlockSize;
	m_dwMaxTransferRetryTimes = dwMaxTransferRetryTimes;
	m_dwBreakPointRetryInterval = dwBreakPointRetryInterval;
	m_lDownloadStartBytes = lDownloadStartBytes;
	m_bOverWrite = bOverWrite;
	
	//��ǰ���ز����ѱ��޸�
	m_bSetDownloadPara = true;
}

//������
//����:   lpUrl - [in] Ҫ�򿪵�HTTP url
//        lpErrMsg - [out] ������Ϣ���򿪳ɹ�ʱ���ؿ�
//        bOpenedType - [in] �򿪷�ʽ true - ��������/�ϴ��ļ�  false - �����ļ�
//����ֵ: true - �򿪳ɹ�,  false - ��ʧ��
//ִ�з��������ϴ��ļ��������ļ�������OpenConnection�����CloseConnection
bool CHTTPBase::OpenConnection(const QString& strUrl, QString& lpErrMsg, bool bOpenedType/* = true*/, bool bPost/* = true*/)
{
	//ȷ����Դ���ͷ�
	CloseConnection(false);

	if(strUrl.isEmpty())
	{
		//CCommonFunc::WriteLog(lpErrMsg, "�����ַ����Ϊ��!"); yhb log
		ReportError(QString::fromLocal8Bit("CHTTPBase::OpenConnection �����ַ����Ϊ��!"));
		return false;
	}
	
	//���������ַ
	m_strUrl = strUrl;
	
	//��ʼ���ָ���
	m_strSeparate = RandomMakeBoundary();

	CString strServer, strServerFile;
	unsigned short nPort;
	DWORD dwServiceType;
	
	if(!AfxParseURL(strUrl.toLocal8Bit().constData(), dwServiceType, strServer, strServerFile, nPort))
	{
		ReportError(QString::fromLocal8Bit("CHTTPBase::OpenConnection ������ʧ�ܣ�[%1] ������Ч�������ַ").arg(strUrl));
		return false;
	}

	if(dwServiceType == AFX_INET_SERVICE_HTTPS)
	{
		//������
		if(bOpenedType)//��������/�ϴ��ļ�
			return OpenUploadHTTPSConnection(strUrl, lpErrMsg, bPost);
		else//�����ļ�
			return OpenDownloadConnection(strUrl, lpErrMsg);
	}
	else
	{
		//������
		if(bOpenedType)//��������/�ϴ��ļ�
			return OpenUploadConnection(strUrl, lpErrMsg, bPost);
		else//�����ļ�
			return OpenDownloadConnection(strUrl, lpErrMsg);
	}
	
}

//��ӱ���ͷ
//����:   lpHeaderName - [in] ����ͷ
//        lpHeaderValue - [in] ����ֵ
//        lpErrMsg - [in,out] ������Ϣ
//����ֵ: true - ��ӳɹ� false - ���ʧ��
//ע��: OpenConnection����ӱ���ͷ���˺�����ѡ
bool CHTTPBase::AddHeader(const QString& strHeaderName, const QString& strHeaderValue, QString& lpErrMsg)
{
	QString strHeader = QString::fromLocal8Bit("%1: %2").arg(strHeaderName).arg(strHeaderValue);
	if(NULL != m_pHttpFile)
	{
		if(!m_pHttpFile->AddRequestHeaders(strHeader.toLocal8Bit().constData()))
		{
			//CCommonFunc::WriteLog(lpErrMsg, "���ͷ��Ϣ[%s]ʧ��!", strHeader);  yhb log
			ReportError(QString::fromLocal8Bit("���ͷ��Ϣ[%1]ʧ��!").arg(strHeader));
			return false;
		}
		
		return true;
	}
	else
	{
		//CCommonFunc::WriteLog(lpErrMsg, "δ���ӵ�Զ���ļ����޷����ͷ��Ϣ[%s]!", strHeader);  yhb log
		ReportError(QString::fromLocal8Bit("δ���ӵ�Զ���ļ����޷����ͷ��Ϣ[%1]!").arg(strHeader));
		return false;
	}
}

//��ӱ��Ĳ���
//����:   lpParaName - [in] ������
//        lpParaValue - [in] ����ֵ
//        lpBoundary - [in] �ָ���,��������˷ָ�������ʱ����ΪFORM��ʽ,�ָ�������ͨ��GetBoundary��ȡ
//����ֵ: true - ��ӳɹ� false - ���ʧ��
//ע��: OpenConnection����ӷ��͵Ĳ������˺�����ѡ
bool CHTTPBase::AddPara(const QString& lpParaName, const QString& lpParaValue, const QString& lpBoundary /*= NULL*/)
{
	QString strTempPara = _T("");
	if(NULL == lpBoundary)
	{//���ŷָ���ʽ
		if(m_strParaData.isEmpty())
		{
			m_strParaData = QString::fromLocal8Bit("%1=%2").arg(lpParaName).arg(lpParaValue);
		}
		else
		{
			strTempPara = m_strParaData;
			m_strParaData = QString::fromLocal8Bit("%1&%2=%3").arg(strTempPara).arg(lpParaName).arg(lpParaValue);
		}
	}
	else
	{//FORM��ʽ
		strTempPara += m_strParaData;
		if(m_strParaData.isEmpty())
			strTempPara += "--%1\r\n";
		else
			strTempPara += "\r\n--%1\r\n";
		strTempPara += "Content-Disposition: form-data; name=\"%2\"";
		strTempPara += "\r\n\r\n";
		strTempPara += "%3";

		m_strParaData = QString::fromLocal8Bit(strTempPara.toLocal8Bit().constData()).arg(lpBoundary).arg(lpParaName).arg(lpParaValue);
	}
	return true;
}

//���FORM��β��
//ע��: ��AddPara�������FORM��ʽ�Ĳ���������ʹ�õ���SendRequest������Ҫ��SendRequest֮ǰ����AddFormEnd
//      ���ΪUploadFile����Ҫ
bool CHTTPBase::AddFormEnd()
{
	m_strParaData += GetEndFileData(m_strSeparate);
	return true;
}

//��ȡ�ָ���
//����ֵ:  ���طָ���
//ע��: OpenConnection֮������,����һ����������ǲ���ģ�ֻ��OpenConnection֮��ı�
QString CHTTPBase::GetBoundary()
{
	return m_strSeparate;
}

//����HTTP����
//����:   lpErrMsg - [out]����ɹ�ʱ���ؿգ�����ʧ��ʱ���ش�����Ϣ 
//����ֵ: true - ����ɹ��� false - ����ʧ�� 
//ע��: ������OpenConnection
bool CHTTPBase::SendRequest(QString& lpErrMsg)
{
	//���ݳ�ʼ��
	bool bRet = false;
	m_strRecvData.clear();
	
	//׼��HTTP����ͷ
	PrePareHTTPHeaderData();
	
	//��ȡ���͵Ĳ�����
	PrePareHTTPParaData("");
	
	//Ĭ�Ϸ�ʽ
	try
	{
		//����HTTP����
		//CCommonFunc::WriteLogToFile(COMM_HTTP, m_dwHttpID, "ͨ�ŵ�ַ:[" + m_strUrl + "]\r\n" + m_strParaData);  yhb log
		bRet = m_pHttpFile->SendRequest(NULL, 0, (char*)(m_strParaData.toLocal8Bit().constData()), m_strParaData.length()); 
		
		if(bRet)
		{//HTTP���ͳɹ�
			unsigned long dwRetCode;
			
			//�鿴��HTTP���󷵻ص�״̬��
			m_pHttpFile->QueryInfoStatusCode(dwRetCode);
			
			if(200 <= dwRetCode && dwRetCode <= 299)
			{//���״̬��Ϊ200-299��ʾ���͵�HTTP���󱻷���������
				
				//��ȡ��Ӧ����.
				if(!GetResponseData(lpErrMsg))
				{
					//CCommonFunc::WriteLog(lpErrMsg, "��ȡ��Ӧ����ʧ��");  yhb log
					ReportError(QString::fromLocal8Bit("CHTTPBase::SendRequest ��ȡ��Ӧ����ʧ��"));
					bRet = false;
				}
			}
			else
			{	
				//CCommonFunc::WriteLog(lpErrMsg, "������:Ϊ[%d]", dwRetCode);  yhb log
				ReportError(QString::fromLocal8Bit("CHTTPBase::SendRequest ������:Ϊ[%1]").arg(dwRetCode));
				bRet = false;
			}
			
		}
	}
	catch(CInternetException* e)
	{
		char chBuf[256] = {0};
		e->GetErrorMessage(chBuf, 255);
		//CCommonFunc::WriteLog(lpErrMsg, "%s", strErrString);  yhb log
		ReportError(QString::fromLocal8Bit("CHTTPBase::SendRequest ��%1��").arg(chBuf));
		e->Delete();
		bRet = false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "δ֪����");  yhb log
		ReportError(QString::fromLocal8Bit("CHTTPBase::SendRequest δ֪����"));
		bRet = false;
	}
	
	
	//�ر�����
	CloseConnection(false);
	return bRet;
}

//�����ļ�
//����:   lpFilePath - [in]�ļ��ı��ش洢·��,���·����ָ�����ļ��������Դ������棬�����Է����������������ֱ���
//                          ���·����û��ָ���ļ���׺�����Է��������ص��ļ���׺����
//        lpErrMsg - [out]�ļ����سɹ�ʱ���ؿգ��ļ�����ʧ��ʱ���ش�����Ϣ
//����ֵ: true - ���سɹ��� false - ����ʧ��
//ע��: ������OpenConnection��һ��ֻ����һ���ļ������Ǳ���CloseConnection -> OpenConnection -> DownloadFile
bool CHTTPBase::DownloadFile(QString& lpFilePath, QString& lpErrMsg)
{
	//���ݳ�ʼ��
	QString strFilePath = lpFilePath;
	m_strRecvData.clear();
	m_dwTotalCount = 0;
	m_dwTreatOverCount = m_lDownloadStartBytes;
	
	//�鿴�Ƿ�ָ����Զ�������ַ�ͱ��ش洢·��
	if(m_strUrl.isEmpty() || strFilePath.isEmpty())
	{
		//CCommonFunc::WriteLog(lpErrMsg, "�����ַ[%s]�򱾵�·��[%s]û��ָ��", lpFilePath, lpErrMsg);  yhb log
		ReportError(QString::fromLocal8Bit("CHTTPBase::DownloadFile �����ַ[%1]�򱾵�·��[%2]û��ָ��").arg(m_strUrl).arg(strFilePath));
		return false;
	}
	
	//������
	bool bRet = false;
	
	//��ñ����ļ��洢�ľ���·��
	QString strLocalFilePath = strFilePath;
	bRet = GetLocalFilePath(m_strUrl, strLocalFilePath, NULL, lpErrMsg);

	//�����ļ���
	if(strLocalFilePath.length() <= 255)
		lpFilePath = strLocalFilePath;
	
	if(bRet)
	{//��ʼ�����ļ�
		bRet = DownloadDataToFile(strLocalFilePath, lpErrMsg, m_dwTransferBlockSize);
	}
	
	return bRet;
}

//ֹͣ����
void CHTTPBase::StopDownload()
{
	m_bStop = true;
}

//��ȡ�ϴ�/�����ļ���ǰ�Ѵ�����ֽ���
//����:   dwOverCount - [out] ��ǰ�Ѵ�����ֽ���
//        dwTotalCount - [out] �ϴ�/�����ܵ��ֽ���
//����ֵ  true - ��ȡ��Ϣ�ɹ� false -��ȡ��Ϣʧ��
void CHTTPBase::GetCount(unsigned long &dwOverCount, unsigned long &dwTotalCount)
{
	dwOverCount = m_dwTreatOverCount;
	dwTotalCount = m_dwTotalCount;
}

//��ȡ��������Ӧ����
//����ֵ: ��������Ӧ������
//ע��: SendRequest��UploadFile�����
QString CHTTPBase::GetResponseData()
{
	return m_strRecvData;
}

//�ر�����
void CHTTPBase::CloseConnection(bool bInitPara)
{
	if(NULL != m_pHttpFile)
	{
		m_pHttpFile->Close();
		delete m_pHttpFile;
		m_pHttpFile = NULL;
	}
	if(NULL != m_pHttpCon)
	{
		m_pHttpCon->Close();
		delete m_pHttpCon;
		m_pHttpCon = NULL;
	}
	if(NULL != m_pHttpSession)
	{
		m_pHttpSession->Close();
		delete m_pHttpSession;
		m_pHttpSession = NULL;
	}
	
	if(bInitPara)
	{//��ʼ������
		Init();
	}
}

//���ַ����е������ַ�����ת�룬ʹ֮�ܹ�ͨ��HTTP����
void CHTTPBase::TransCode(QString &str)
{
	//'&'���'&'�ַ�������ַ�ȫ���ض�,��ת��
	str.replace("&", "%26");

	//'+'���ֱ��ʹ�û��ʾΪ�ո�
	str.replace("+", "%2B");

	str.replace(" ", "+");
}

//��HTTPS��������
//����:  lpUrl - [in] Զ�̷�������·��
//       lpErrMsg - [out] ���ش�����Ϣ
bool CHTTPBase::OpenUploadHTTPSConnection(const QString& lpUrl, QString& lpErrMsg, bool bPost/* = true*/)
{
	unsigned long dwServiceType;
	CString strServer, strServerFile;
	unsigned short nPort;
	try
	{
		if(!AfxParseURL(lpUrl.toLocal8Bit().constData(), dwServiceType, strServer, strServerFile, nPort) || AFX_INET_SERVICE_HTTPS != dwServiceType)
		{//����URL
			//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ�[%s] ������Ч��https�����ַ", lpUrl);  yhb log
			return false;
		}
		
		//����SESSION
		m_pHttpSession = new CInternetSession();
		if(NULL == m_pHttpSession)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ��Ự����ʧ��");  yhb log
			return false;
		}

		m_pHttpSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 60000); 
		
		//ͨ��SESSION�õ�CONNECTION
		nPort = INTERNET_DEFAULT_HTTPS_PORT;
		m_pHttpCon = m_pHttpSession->GetHttpConnection(strServer, nPort); 
		
		if(NULL == m_pHttpCon)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ������������������ʧ��");  yhb log
			return false;
		}
		else
		{
			int nVerbType = bPost ? CHttpConnection::HTTP_VERB_POST : CHttpConnection::HTTP_VERB_GET;
			m_pHttpFile= m_pHttpCon->OpenRequest(nVerbType, strServerFile, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE ); 
			if(NULL == m_pHttpFile)
			{
				//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ��Ҳ��������ַ[%s]", lpUrl);  yhb log
				return false;
			}
		}
	}
	catch(CInternetException* e)
	{
		char chBuf[256] = {0};
		e->GetErrorMessage(chBuf, 255);
		//strErrString.TrimRight("\r\n"); 
		//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ�����ԭ��:[%s]", strErrString);  yhb log
		e->Delete();
		return false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ�δ֪����");  yhb log
		return false;
	}
	return true;
}

//����������
//����:  lpUrl - [in] Զ�̷�������·��
//       lpErrMsg - [out] ���ش�����Ϣ
bool CHTTPBase::OpenUploadConnection(const QString& lpUrl, QString& lpErrMsg, bool bPost/* = true*/)
{
	unsigned long dwServiceType;
	CString strServer, strServerFile;
	unsigned short nPort;
	try
	{
		if(!AfxParseURL(lpUrl.toLocal8Bit().constData(), dwServiceType, strServer, strServerFile, nPort))
		{//����URL
			//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ�[%s] ������Ч�������ַ", lpUrl);  yhb log
			return false;
		}
		
		//����SESSION
		m_pHttpSession = new CInternetSession();
		if(NULL == m_pHttpSession)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ��Ự����ʧ��");  yhb log
			return false;
		}
		
		//ͨ��SESSION�õ�CONNECTION
		m_pHttpCon = m_pHttpSession->GetHttpConnection(strServer, nPort); 
		
		if(NULL == m_pHttpCon)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ������������������ʧ��");  yhb log
			return false;
		}
		else
		{
			int nVerbType = bPost ? CHttpConnection::HTTP_VERB_POST : CHttpConnection::HTTP_VERB_GET;
			m_pHttpFile= m_pHttpCon->OpenRequest(nVerbType,strServerFile, NULL, 1, NULL, NULL, INTERNET_FLAG_NO_COOKIES|INTERNET_FLAG_EXISTING_CONNECT); 
			if(NULL == m_pHttpFile)
			{
				//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ��Ҳ��������ַ[%s]", lpUrl);  yhb log
				return false;
			}
		}
	}
	catch(CInternetException* e)
	{
		char chBuf[256] = {0};
		e->GetErrorMessage(chBuf, 255);
		//strErrString.TrimRight("\r\n");
		//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ�����ԭ��:[%s]", strErrString);  yhb log
		e->Delete();
		return false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ�δ֪����");  yhb log
		return false;
	}
	return true;
}

//����������
//����:  lpUrl - [in] Զ�̷�������·��
//       lpErrMsg - [out] ���ش�����Ϣ
bool CHTTPBase::OpenDownloadConnection(const QString& lpUrl, QString& lpErrMsg)
{
	QString strUrl = lpUrl;
	m_bStop = false;
	QString strTemp;
	try
	{
		//�鿴�����ʽ�Ƿ���ȷ
		unsigned long dwType;
		CString strServer, strServerFile;
		INTERNET_PORT wPort;
		if(!AfxParseURL(strUrl.toLocal8Bit().constData(), dwType, strServer, strServerFile, wPort))
		{
			//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ�[%s] ������Ч�������ַ", lpUrl);  yhb log
			ReportError(QString::fromLocal8Bit("CHTTPBase::DownloadFile ������ʧ�ܣ�[%1] ������Ч�������ַ").arg(strUrl));
			return false;
		}
		
		//����SESSION
		m_pHttpSession = new CInternetSession();
		if(NULL == m_pHttpSession)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ��Ự����ʧ��");  yhb log
			ReportError(QString::fromLocal8Bit("CHTTPBase::DownloadFile ������ʧ�ܣ��Ự����ʧ��"));
			return false;
		}
		
		//��ǰ���ز����Ƿ��޸Ĺ�
		if(!m_bSetDownloadPara)
		{//��ǰ���ز���δ�����ã�ʹ��Ĭ�����ز���
			m_dwBreakPointRetryInterval = 1000;
			m_dwMaxTransferRetryTimes = 2;
			m_dwTransferBlockSize = 1024;
			m_lDownloadStartBytes = 0;
			m_bOverWrite = true;
		}
		
		//���ò���
		m_bSetDownloadPara = false;
		m_pHttpSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, m_dwBreakPointRetryInterval); //����֮��ĵȴ���ʱ
		m_pHttpSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, m_dwMaxTransferRetryTimes); //���Դ���
		
		//ͨ��SESSION�õ�CONNECTION
		m_pHttpCon = m_pHttpSession->GetHttpConnection(strServer, wPort); 
		
		if(NULL == m_pHttpCon)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ������������������ʧ��");  yhb log
			ReportError(QString::fromLocal8Bit("CHTTPBase::OpenDownloadConnection ������ʧ�ܣ������������������ʧ��"));
			return false;

		}
		
		//������
		m_pHttpFile = m_pHttpCon->OpenRequest(CHttpConnection::HTTP_VERB_GET, strServerFile);
		
		if(NULL == m_pHttpFile)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ��Ҳ��������ַ[%s]", lpUrl);  yhb log
			ReportError(QString::fromLocal8Bit("������ʧ�ܣ��Ҳ��������ַ[%1]").arg(strUrl));
			return false;
		}

		if(m_lDownloadStartBytes > 0)
		{//׷�Ӷϵ�����ͷ
			if(!AddHeader("RANGE", QString::fromLocal8Bit("bytes=%1-").arg(m_lDownloadStartBytes), lpErrMsg))
			{
				return false;
			}
		}



		AddHeader("Accept", "*/*", lpErrMsg);
		//AddHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36", lpErrMsg);
		AddHeader("pragma", "no-cache", lpErrMsg);
		AddHeader("cache-control", "no-cache", lpErrMsg);

		//AddHeader("If-Modified-Since", "Sat, 01 Jan 2000 00:00:00 GMT", lpErrMsg);
		//AddHeader("If-Modified-Since", "-1", lpErrMsg);
		AddHeader("If-Modified-Since", "Sat, 01 Jan 2000 00:00:00 GMT", lpErrMsg);

		//if(!m_pHttpFile->SendRequest(NULL, 0, (char*)(m_strParaData.toStdString().c_str()), m_strParaData.length()))
		if(!m_pHttpFile->SendRequest())
		{//��������ʧ��
			//CCommonFunc::WriteLog(lpErrMsg, "��[%s]��������ʧ��", lpUrl);  yhb log
			ReportError(QString::fromLocal8Bit("��[%1]��������ʧ��").arg(strUrl));
			return false;
		}
		
		unsigned long dwStateCode;
		m_pHttpFile->QueryInfoStatusCode(dwStateCode);
		
		if(dwStateCode == HTTP_STATUS_OK || dwStateCode == HTTP_STATUS_PARTIAL_CONTENT)
		{
			return true;
		}
		else
		{
			//CCommonFunc::WriteLog(lpErrMsg, "���͵�����δ������������");  yhb log
			ReportError(QString::fromLocal8Bit("[%1]���͵�����δ������������ ��Ӧ��[%2]").arg(strUrl).arg(dwStateCode));

			CString strContextText;
			m_pHttpFile->QueryInfo(HTTP_QUERY_STATUS_TEXT, strContextText); 
			ReportError(QString::fromLocal8Bit("������Ϣ[%1]").arg(strContextText.GetBuffer(0)));
			strContextText.ReleaseBuffer();

			return false;
		}
		
	}
	catch(CInternetException* e)
	{
		if(m_bStop)
			return false;

		char chBuf[256] = {0};
		e->GetErrorMessage(chBuf, 256);
		//strErrString.TrimRight("\r\n");
		//CCommonFunc::WriteLog(lpErrMsg, "������ʧ�ܣ�����ԭ��:[%s]", strErrString);  yhb log
		ReportError(QString::fromLocal8Bit("CHTTPBase::OpenDownloadConnection ������[%1]ʧ�ܣ�����ԭ��:[%2]").arg(strUrl).arg(QString::fromLocal8Bit(chBuf)));
		e->Delete();
		return false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "δ֪����");  yhb log
		ReportError(QString::fromLocal8Bit("δ֪����"));
		return false;
	}	
	return false;
}

//�������ݣ����洢���ļ�
//����: - lpFilePath - [in] �ļ��Ĵ洢·��
//		- lpErrMsg [in,out] ������Ϣ
//      - dwDownloadPerTime [in] ÿ�����ص��ֽ���
//����ֵ: true - ���سɹ� false - ����ʧ��
bool CHTTPBase::DownloadDataToFile(const QString& lpFilePath, QString& lpErrMsg,unsigned long dwDownloadPerTime/* = 1024*/)
{
	if(NULL == m_pHttpFile)
	{
		ReportError(QString::fromLocal8Bit("m_pHttpFile == null"));
		return false;
	}

	char* pszBuffer = NULL;
	try
	{
		//��ȡ�ļ���С
		char szInfoBuffer[1024]; 
		unsigned long dwInfoBufferSize = sizeof(szInfoBuffer);
		if(m_pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, (void*)szInfoBuffer, &dwInfoBufferSize, NULL))
		{
			m_dwTotalCount = atoi(szInfoBuffer);	
		}
		else
		{
			//CCommonFunc::WriteLog(lpErrMsg, "��ȡ�ļ���Ϣʧ�ܣ�����ȡ��");  yhb log
			ReportError(QString::fromLocal8Bit("�޷���ȡ�ļ���С��Ϣ"));
			return false;
		}
		
		//��ȡ���ش洢·��
		QString strFilePath = lpFilePath;
		if(!GetLocalFilePath(m_strUrl, strFilePath, NULL, lpErrMsg))
		{
			return false;
		}

		QString strDirPath = QFileInfo(strFilePath).absolutePath();
		QDir dir;
		if(!dir.mkpath(strDirPath))
		{
			if(m_bOverWrite)
			{
				ReportError(QString::fromLocal8Bit("���������ļ�[%1]ʧ��:������ȡ�� ������[%2]").arg(lpFilePath).arg(GetLastError()));
			}
			else
			{
				//������Ϊ0
				emit m_pRelativeDownloader->sig_DownPercent(0, m_dwTotalCount + m_lDownloadStartBytes);
				ReportError(QString::fromLocal8Bit("�����ļ�[%1]��ʧ������������������").arg(lpFilePath));
			}
			return false;
		}

		UINT openMode = m_bOverWrite ? CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone : CFile::modeReadWrite | CFile::shareDenyNone;

		//�򿪱����ļ�
		CFile file;
		if(!file.Open(lpFilePath.toLocal8Bit().constData(), openMode))
		{
			//CCommonFunc::WriteLog(lpErrMsg, "���������ļ�[%s]ʧ��:������ȡ��", lpFilePath);  yhb log

			if(m_bOverWrite)
			{
				ReportError(QString::fromLocal8Bit("���������ļ�[%1]ʧ��:������ȡ�� ������[%2]").arg(lpFilePath).arg(GetLastError()));
			}
			else
			{
				//������Ϊ0
				emit m_pRelativeDownloader->sig_DownPercent(0, m_dwTotalCount + m_lDownloadStartBytes);
				ReportError(QString::fromLocal8Bit("�����ļ�[%1]��ʧ������������������").arg(lpFilePath));
			}
			return false;
		}

		if(!m_bOverWrite)
		{
			file.SeekToEnd();
		}

		//���������ٶ�
		DWORD dwExpectDownloadPerTime = dwDownloadPerTime;

		//�������ػ���������ٶ�
		pszBuffer = new char[dwDownloadPerTime + 1];
		memset(pszBuffer, 0, dwDownloadPerTime + 1);
		
		//��ȡ������������
		dwDownloadPerTime = m_pHttpFile->Read(pszBuffer, dwDownloadPerTime); 
		
		while(dwDownloadPerTime > 0 && !m_bStop)
		{
			//д�������ļ�
			file.Write(pszBuffer, dwDownloadPerTime);
			
			//�����������ֽ���
			m_dwTreatOverCount += dwDownloadPerTime;

			emit m_pRelativeDownloader->sig_DownPercent(m_dwTreatOverCount, m_dwTotalCount + m_lDownloadStartBytes);

			//���������ٶ�
			dwDownloadPerTime = m_pHttpFile->Read(pszBuffer, dwExpectDownloadPerTime);
		}
		
		file.Close();
		delete[]pszBuffer;
		pszBuffer = NULL;

		if(m_dwTreatOverCount >= m_dwTotalCount + m_lDownloadStartBytes)
			emit m_pRelativeDownloader->sig_DownFinished(); 
		else
			emit m_pRelativeDownloader->sig_DownQuit(); 

		return true;
	}
	catch(CInternetException* e)
	{
		char chBuf[256] = {0};
		e->GetErrorMessage(chBuf, 256);
		//strErrString.TrimRight("\r\n");
		//CCommonFunc::WriteLog(lpErrMsg, "�����ļ�ʧ�ܣ�����ԭ��:[%s]", strErrString);  yhb log
		ReportError(QString::fromLocal8Bit("�����ļ�ʧ�ܣ�����ԭ��:[%1]").arg(QString::fromLocal8Bit(chBuf)));
		e->Delete();
		delete[]pszBuffer;
		pszBuffer = NULL;
		return false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "δ֪����");  yhb log
		ReportError(QString::fromLocal8Bit("δ֪����"));
		delete[]pszBuffer;
		pszBuffer = NULL;
		return false;
	}	
}

//��ȡ��������Ӧ����
//����: - lpErrMsg [out] ������Ϣ
//����ֵ: true - ��ȡ�ɹ� false - ��ȡʧ��
bool CHTTPBase::GetResponseData(QString& lpErrMsg)
{
	//���建��
	char * chBuf = NULL;
	
	try
	{
 		unsigned long dwDownloadCount = m_pHttpFile->GetLength();
 		
 		//��ȡ����
 		while (0 != dwDownloadCount)
 		{
 			//���建��
 			chBuf = new char[dwDownloadCount + 1];
 			memset(chBuf, 0 , dwDownloadCount + 1);
 			
 			//�ӷ������ļ��ϻ�ȡ����
 			m_pHttpFile->Read(chBuf, dwDownloadCount);
 			
 			//��¼��Ӧ������
 			m_strRecvData += chBuf;
 			
 			dwDownloadCount = m_pHttpFile->GetLength();
 			
 			if(NULL != chBuf)
 			{
 				delete [] chBuf;
 				chBuf = NULL;
 			}
 		}
		
		//CCommonFunc::WriteLogToFile(COMM_HTTP, ++m_dwHttpID > 99999 ? 0 : m_dwHttpID, "ͨ�ŵ�ַ:[" + m_strUrl + "]\r\n" + m_strRecvData);  yhb log
// 		QString strData;
// 		m_pHttpFile->ReadString(m_strRecvData);
// 
// 		do 
// 		{
// 			m_strRecvData += strData;
// 		} while(m_pHttpFile->ReadString(strData));

		//CCommonFunc::ChangeMultiToUnicode(m_strRecvData);
	}
	catch(CInternetException *e)
	{
		TCHAR tszErrString[256] = {0};
		e->GetErrorMessage(tszErrString, 256);
		//CCommonFunc::WriteLog(lpErrMsg, "��ȡ��Ӧ����ʧ�ܣ�����ԭ��:[%s]", tszErrString);  yhb log
		e->Delete();
		if(NULL != chBuf)
		{
			delete [] chBuf;
			chBuf = NULL;
		}
		return false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "δ֪����");  yhb log
		if(NULL != chBuf)
		{
			delete [] chBuf;
			chBuf = NULL;
		}
		return false;
	}
	
	return true;	
}

//����ó�һ���ָ���
QString CHTTPBase::RandomMakeBoundary()
{
	QString strSeparate;
	CString strTemp;
	
	//��ȡ��ǰʱ��
	CTime t = CTime::GetCurrentTime();
	strSeparate = t.Format("%Y%m%d%H%M%S");
	
	//�õ�һ�����ֵ
	int nLength = strSeparate.length();
	strTemp.Format("%d", rand()%nLength);
	
	//�ڵ�ǰ�ָ��������λ�ò���һ�����ֵ
	strSeparate.insert(rand()%nLength, strTemp.GetBuffer(0));
	
	//����ָ���
	strSeparate = "----------" + strSeparate;
	
	//���طָ���
	return strSeparate;
}

//����URL���ļ���Ϣ���û��ṩ���ļ���Ϣ��ϵõ����ش洢�ľ���·�����ļ���
//����:  lpUrl - [in] �����ļ���URL��ַ
//       strFilePath - [in] �û�ָ���ı��ش洢·��(�������ļ��У�Ҳ�������ļ�)
//       lpFileName - [in] strFilePathֻ��·����lpFileName�Ǳ��ص��ļ���
//       lpErrMsg - [out] ��ȡ�ɹ�ʱ���ؿգ���ȡʧ��ʱ���ش�����Ϣ 
//ע�⣺lpUrl��strFilePath��lpFileName�����ʹ�ã�strFilePath��һ���ļ���ʱ�����δָ��lpFileName��
//      ���ὫlpUrl���ļ�����Ϊ�����ļ��������lpFileName���ƶ�������lpFileName�洢�����lpFileNameδָ��
//      ��׺������lpUrl�ĺ�׺�洢�����strFilePath��һ���ļ�����·��ʱ��ֱ����strFilePath�洢
bool CHTTPBase::GetLocalFilePath(const QString& lpUrl, QString &strFilePath, const QString& lpFileName, QString& lpErrMsg)
{
	if(strFilePath.isEmpty())
	{
		//CCommonFunc::WriteLog(lpErrMsg, "û��ָ������·��������ȡ��!");  yhb log
		ReportError(QString::fromLocal8Bit("û��ָ������·��������ȡ��"));
		return false;
	}

	QString strServerFileName, strLocalFileName, strServerFileExt, strLocalFileExt;
	
	//�õ����������������ļ���
	GetFileName(lpUrl, strServerFileName, strServerFileExt);
	
	if(0 == strServerFileName.length())
	{
		//CCommonFunc::WriteLog(lpErrMsg, "û����URL��ַ��ָ��Ҫ���ص��ļ�������ȡ��!");  yhb log
		ReportError(QString::fromLocal8Bit("û����URL��ַ��ָ��Ҫ���ص��ļ�������ȡ��!"));
		return false;
	}
	
	//�鿴·�����Ƿ�ָ���˱����ļ���
	GetFileName(strFilePath, strLocalFileName, strLocalFileExt);

	//����·����ָ���˾���·���������ļ���
	if(!strLocalFileExt.isEmpty())
		return true;
	
	//��ȡ�ļ�����Ϣ
	GetFileName(lpFileName, strLocalFileName, strLocalFileExt);

	if(!strLocalFileName.isEmpty())
	{//ָ�����ļ���
		
		//�鿴�ļ����Ƿ��к�׺
		if(strLocalFileExt.isEmpty())
		{//û�к�׺���Է��������صĺ�׺��Ϊ�ļ���׺
			strLocalFileExt = strServerFileExt;
		}
		
		if(!strLocalFileName.isEmpty())
			strFilePath += "\\" + strLocalFileName + "." +  strLocalFileExt;
		else
			strFilePath += "." + strLocalFileExt;
	}
	else
	{
		//û��ָ�������ļ������Է��������������ļ����洢
		strFilePath = strFilePath + "\\" + strServerFileName + "." + strServerFileExt;
	}
	
	return true;
}

//��ȡ�ļ���
//����: lpFilePath - [in] �ļ�����·��
//		strFileName - [out] �ļ���
//      strFileExt - [out] �ļ���׺
void CHTTPBase::GetFileName(const QString& lpFilePath, QString &strFileName, QString &strFileExt)
{
	TCHAR szDrive[_MAX_DRIVE] = {0};
	TCHAR szDir[_MAX_DIR] = {0};
	TCHAR szFName[_MAX_FNAME] = {0};
	TCHAR szExt[_MAX_EXT] = {0};
	
	_splitpath(lpFilePath.toLocal8Bit().constData(), szDrive, szDir, szFName, szExt);
	
	strFileName = szFName;
	
	strFileExt = szExt;
	
	if(1 < strFileExt.length())
		strFileExt = strFileExt.mid(1, strFileExt.length() - 1);
}

//�㱨����
void CHTTPBase::ReportError(const QString& errInfo)
{
	if(NULL != m_pRelativeDownloader)
	{
		emit m_pRelativeDownloader->sig_ErrorOccur(errInfo);
		m_pRelativeDownloader->m_bOccurError = true;
	}
}

CHTTPComm::CHTTPComm(QObject *parent)
	: CHTTPBase(parent)
{
	
}

CHTTPComm::~CHTTPComm()
{
	
}

//��֯HTTP����ͷ
void CHTTPComm::PrePareHTTPHeaderData()
{
	//AddHeader
}

//��֯��������Ϣ
//����: - strBoundary [in] - �ָ���
void CHTTPComm::PrePareHTTPParaData(const QString& lpBoundary)
{
	//AddPara
}

//��ȡ�ϴ��ļ�ʱ�ļ����������Ϣ
//����:  nNum - [in] �ļ����
//       lpBoundary - [in] �ָ���
//       lpFilePath - [in] �ļ�·��
//       lpNewFileName - [in] �µ��ļ���
//����ֵ: �����ļ����������Ϣ
QString CHTTPComm::GetFilePropertyData(int nNum, const QString& lpBoundary, const QString& lpFilePath, const QString& lpNewFileName)
{
	QString strFileName, strFileExt, strFileType;
	
	//�õ��ļ������ļ���׺
	GetFileName(lpNewFileName, strFileName, strFileExt);
	
	if(strFileExt.isEmpty())
	{//����µ��ļ���û��ָ����׺
		GetFileName(lpFilePath, strFileExt, strFileExt);
	}
	strFileName += "." + strFileExt;
	
	//��ȡ�ļ�����
	if(0 == strFileExt.compare("jpg", Qt::CaseInsensitive))
	{
		strFileType = "image/pjpeg";
	}
	else if(0 == strFileExt.compare("png", Qt::CaseInsensitive))
	{
		strFileType = "image/x-png";
	}
	else if(0 == strFileExt.compare("bmp", Qt::CaseInsensitive))
	{
		strFileType = "image/bmp";
	}
	else if(0 == strFileExt.compare("tif", Qt::CaseInsensitive))
	{
		strFileType = "image/tiff";
	}
	else if(0 == strFileExt.compare("gif", Qt::CaseInsensitive))
	{
		strFileType = "image/gif";
	}
	else if(0 == strFileExt.compare("zip", Qt::CaseInsensitive))
	{
		strFileType = "application/x-zip-compressed";
	}
	else if(0 == strFileExt.compare("rar", Qt::CaseInsensitive) || 0 == strFileExt.compare("7z", Qt::CaseInsensitive) || 0 == strFileExt.compare("exe", Qt::CaseInsensitive) ||
		0 == strFileExt.compare("ini", Qt::CaseInsensitive) || 0 == strFileExt.compare("dll", Qt::CaseInsensitive) || 0 == strFileExt.compare("flv", Qt::CaseInsensitive))
	{
		strFileType = "application/octet-stream";
	}
	else if(0 == strFileExt.compare("avi", Qt::CaseInsensitive))
	{
		strFileType = "video/avi";
	}
	else if(0 == strFileExt.compare("rmvb", Qt::CaseInsensitive))
	{
		strFileType = "application/vnd.rn-realmedia-vbr";
	}
	else if(0 == strFileExt.compare("mp3", Qt::CaseInsensitive))
	{
		strFileType = "audio/mpeg";
	}
	else if(0 == strFileExt.compare("wav", Qt::CaseInsensitive))
	{
		strFileType = "audio/wav";
	}
	else if(0 == strFileExt.compare("txt", Qt::CaseInsensitive))
	{
		strFileType = "text/plain";
	}
	else if(0 == strFileExt.compare("doc", Qt::CaseInsensitive))
	{
		strFileType = "application/msword";
	}
	else if(0 == strFileExt.compare("mdb", Qt::CaseInsensitive))
	{
		strFileType = "application/msaccess";
	}
	else if(0 == strFileExt.compare("html", Qt::CaseInsensitive) || strFileExt.compare("htm", Qt::CaseInsensitive))
	{
		strFileType = "text/html";
	}
	else if(0 == strFileExt.compare("xml", Qt::CaseInsensitive))
	{
		strFileType = "text/xml";
	}
	
    QString strFormat;
    QString strData;

	strFormat = _T("\r\n");
    strFormat += _T("--%1");
    strFormat += _T("\r\n");
    strFormat += _T("Content-Disposition: form-data; name=\"File%2\"; filename=\"%3\"");
    strFormat += _T("\r\n");
    strFormat += _T("Content-Type: %4");
    strFormat += _T("\r\n\r\n");
	
    strData == QString::fromLocal8Bit(strFormat.toLocal8Bit().constData()).arg(lpBoundary).arg(nNum).arg(strFileName).arg(strFileType);
    return strData;
}

//��ȡ��β����Ϣ
//����: - lpBoundary [in] - �ָ���
//����ֵ: ���ؽ�β��������Ϣ
QString CHTTPComm::GetEndFileData(const QString& lpBoundary)
{
	QString strFormat;
    QString strData;
	
	strFormat = _T("\r\n");
    strFormat += _T("--%1");
    strFormat += _T("\r\n\r\n");
	//  strFormat += _T("Content-Disposition: form-data; name=\"submitted\"");
	//     strFormat += _T("\r\n\r\n");
	//     strFormat += _T("hello");
	//     strFormat += _T("\r\n");
	strFormat += _T("--%2--");
	strFormat += _T("\r\n");
	
    strData == QString::fromLocal8Bit(strFormat.toLocal8Bit().constData()).arg(lpBoundary).arg(lpBoundary);
    return strData;
}
