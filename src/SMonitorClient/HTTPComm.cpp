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

//初始化参数
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

//设置下载管理器
void CHTTPBase::SetRelativeDownloader(CHTTPDownloader *pRelativeDownloader)
{
	m_pRelativeDownloader = pRelativeDownloader;
}

//设置HTTP操作相关参数(需在OpenConnection之后调用, 否则无效)
void CHTTPBase::SetOption(HTTPTYPE type, unsigned long dwOption, unsigned long dwValue, unsigned long dwFlags /* = 0 */)
{
	if(HTTPCONNECTION == type && m_pHttpCon)
	{//HTTP连接参数
		m_pHttpCon->SetOption(dwOption, dwValue, dwFlags);
	}
	else if(HTTPSESSION == type && m_pHttpSession)
	{//HTTP会话参数
		m_pHttpSession->SetOption(dwOption, dwValue, dwFlags);
	}
	else if(HTTPFILE == type && m_pHttpFile)
	{//HTTP文件参数
		m_pHttpFile->SetOption(dwOption, dwValue, dwFlags);
	}
}

//设置下载参数
//参数:   dwTransferBlockSize - [in] 每次传输的字节数
//        dwMaxTransferRetryTimes - [in] 最大重试次数
//        dwBreakPointRetryInterval - [in] 重试之间的等待时间
//        lDownloadStartBytes - [in] 文件下载开始位置 
//        bOverWrite - [in] 该次写入是否擦除文件
//注意:   此函数可选，如果不调用此函数采用默认值，dwTransferBlockSize - 1024 dwMaxTransferRetryTimes - 2 dwBreakPointRetryInterval - 1000
void CHTTPBase::SetDownloadPara(unsigned long dwTransferBlockSize, unsigned long dwMaxTransferRetryTimes, unsigned long dwBreakPointRetryInterval, unsigned long lDownloadStartBytes, bool bOverWrite)
{
	m_dwTransferBlockSize = dwTransferBlockSize;
	m_dwMaxTransferRetryTimes = dwMaxTransferRetryTimes;
	m_dwBreakPointRetryInterval = dwBreakPointRetryInterval;
	m_lDownloadStartBytes = lDownloadStartBytes;
	m_bOverWrite = bOverWrite;
	
	//当前下载参数已被修改
	m_bSetDownloadPara = true;
}

//打开连接
//参数:   lpUrl - [in] 要打开的HTTP url
//        lpErrMsg - [out] 错误信息，打开成功时返回空
//        bOpenedType - [in] 打开方式 true - 发送请求/上传文件  false - 下载文件
//返回值: true - 打开成功,  false - 打开失败
//执行发送请求，上传文件，下载文件必须先OpenConnection，最后CloseConnection
bool CHTTPBase::OpenConnection(const QString& strUrl, QString& lpErrMsg, bool bOpenedType/* = true*/, bool bPost/* = true*/)
{
	//确保资源被释放
	CloseConnection(false);

	if(strUrl.isEmpty())
	{
		//CCommonFunc::WriteLog(lpErrMsg, "网络地址不能为空!"); yhb log
		ReportError(QString::fromLocal8Bit("CHTTPBase::OpenConnection 网络地址不能为空!"));
		return false;
	}
	
	//保留网络地址
	m_strUrl = strUrl;
	
	//初始化分隔符
	m_strSeparate = RandomMakeBoundary();

	CString strServer, strServerFile;
	unsigned short nPort;
	DWORD dwServiceType;
	
	if(!AfxParseURL(strUrl.toLocal8Bit().constData(), dwServiceType, strServer, strServerFile, nPort))
	{
		ReportError(QString::fromLocal8Bit("CHTTPBase::OpenConnection 打开连接失败，[%1] 不是有效的网络地址").arg(strUrl));
		return false;
	}

	if(dwServiceType == AFX_INET_SERVICE_HTTPS)
	{
		//打开连接
		if(bOpenedType)//发送请求/上传文件
			return OpenUploadHTTPSConnection(strUrl, lpErrMsg, bPost);
		else//下载文件
			return OpenDownloadConnection(strUrl, lpErrMsg);
	}
	else
	{
		//打开连接
		if(bOpenedType)//发送请求/上传文件
			return OpenUploadConnection(strUrl, lpErrMsg, bPost);
		else//下载文件
			return OpenDownloadConnection(strUrl, lpErrMsg);
	}
	
}

//添加报文头
//参数:   lpHeaderName - [in] 报文头
//        lpHeaderValue - [in] 报文值
//        lpErrMsg - [in,out] 错误信息
//返回值: true - 添加成功 false - 添加失败
//注意: OpenConnection后添加报文头，此函数可选
bool CHTTPBase::AddHeader(const QString& strHeaderName, const QString& strHeaderValue, QString& lpErrMsg)
{
	QString strHeader = QString::fromLocal8Bit("%1: %2").arg(strHeaderName).arg(strHeaderValue);
	if(NULL != m_pHttpFile)
	{
		if(!m_pHttpFile->AddRequestHeaders(strHeader.toLocal8Bit().constData()))
		{
			//CCommonFunc::WriteLog(lpErrMsg, "添加头信息[%s]失败!", strHeader);  yhb log
			ReportError(QString::fromLocal8Bit("添加头信息[%1]失败!").arg(strHeader));
			return false;
		}
		
		return true;
	}
	else
	{
		//CCommonFunc::WriteLog(lpErrMsg, "未连接到远程文件，无法添加头信息[%s]!", strHeader);  yhb log
		ReportError(QString::fromLocal8Bit("未连接到远程文件，无法添加头信息[%1]!").arg(strHeader));
		return false;
	}
}

//添加报文参数
//参数:   lpParaName - [in] 参数名
//        lpParaValue - [in] 参数值
//        lpBoundary - [in] 分隔符,如果设置了分隔符，此时参数为FORM形式,分隔符可以通过GetBoundary获取
//返回值: true - 添加成功 false - 添加失败
//注意: OpenConnection后添加发送的参数，此函数可选
bool CHTTPBase::AddPara(const QString& lpParaName, const QString& lpParaValue, const QString& lpBoundary /*= NULL*/)
{
	QString strTempPara = _T("");
	if(NULL == lpBoundary)
	{//逗号分隔形式
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
	{//FORM形式
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

//添加FORM结尾域
//注意: 在AddPara中添加了FORM形式的参数，并且使用的是SendRequest，则需要在SendRequest之前调用AddFormEnd
//      如果为UploadFile则不需要
bool CHTTPBase::AddFormEnd()
{
	m_strParaData += GetEndFileData(m_strSeparate);
	return true;
}

//获取分隔符
//返回值:  返回分隔符
//注意: OpenConnection之后生成,在这一个周期里，它是不变的，只在OpenConnection之后改变
QString CHTTPBase::GetBoundary()
{
	return m_strSeparate;
}

//发送HTTP请求
//参数:   lpErrMsg - [out]请求成功时返回空，请求失败时返回错误信息 
//返回值: true - 请求成功， false - 请求失败 
//注意: 必须先OpenConnection
bool CHTTPBase::SendRequest(QString& lpErrMsg)
{
	//数据初始化
	bool bRet = false;
	m_strRecvData.clear();
	
	//准备HTTP报文头
	PrePareHTTPHeaderData();
	
	//获取发送的参数域
	PrePareHTTPParaData("");
	
	//默认方式
	try
	{
		//发送HTTP请求
		//CCommonFunc::WriteLogToFile(COMM_HTTP, m_dwHttpID, "通信地址:[" + m_strUrl + "]\r\n" + m_strParaData);  yhb log
		bRet = m_pHttpFile->SendRequest(NULL, 0, (char*)(m_strParaData.toLocal8Bit().constData()), m_strParaData.length()); 
		
		if(bRet)
		{//HTTP发送成功
			unsigned long dwRetCode;
			
			//查看此HTTP请求返回的状态码
			m_pHttpFile->QueryInfoStatusCode(dwRetCode);
			
			if(200 <= dwRetCode && dwRetCode <= 299)
			{//如果状态码为200-299表示发送的HTTP请求被服务器接受
				
				//获取响应数据.
				if(!GetResponseData(lpErrMsg))
				{
					//CCommonFunc::WriteLog(lpErrMsg, "获取响应数据失败");  yhb log
					ReportError(QString::fromLocal8Bit("CHTTPBase::SendRequest 获取响应数据失败"));
					bRet = false;
				}
			}
			else
			{	
				//CCommonFunc::WriteLog(lpErrMsg, "错误码:为[%d]", dwRetCode);  yhb log
				ReportError(QString::fromLocal8Bit("CHTTPBase::SendRequest 错误码:为[%1]").arg(dwRetCode));
				bRet = false;
			}
			
		}
	}
	catch(CInternetException* e)
	{
		char chBuf[256] = {0};
		e->GetErrorMessage(chBuf, 255);
		//CCommonFunc::WriteLog(lpErrMsg, "%s", strErrString);  yhb log
		ReportError(QString::fromLocal8Bit("CHTTPBase::SendRequest 【%1】").arg(chBuf));
		e->Delete();
		bRet = false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "未知错误");  yhb log
		ReportError(QString::fromLocal8Bit("CHTTPBase::SendRequest 未知错误"));
		bRet = false;
	}
	
	
	//关闭连接
	CloseConnection(false);
	return bRet;
}

//下载文件
//参数:   lpFilePath - [in]文件的本地存储路径,如果路径中指出了文件名，则以此名保存，否则以服务器返回来的名字保存
//                          如果路径中没有指定文件后缀，则以服务器返回的文件后缀保存
//        lpErrMsg - [out]文件下载成功时返回空，文件下载失败时返回错误信息
//返回值: true - 下载成功， false - 下载失败
//注意: 必须先OpenConnection，一次只下载一个文件，但是必须CloseConnection -> OpenConnection -> DownloadFile
bool CHTTPBase::DownloadFile(QString& lpFilePath, QString& lpErrMsg)
{
	//数据初始化
	QString strFilePath = lpFilePath;
	m_strRecvData.clear();
	m_dwTotalCount = 0;
	m_dwTreatOverCount = m_lDownloadStartBytes;
	
	//查看是否指定了远程网络地址和本地存储路径
	if(m_strUrl.isEmpty() || strFilePath.isEmpty())
	{
		//CCommonFunc::WriteLog(lpErrMsg, "网络地址[%s]或本地路径[%s]没有指定", lpFilePath, lpErrMsg);  yhb log
		ReportError(QString::fromLocal8Bit("CHTTPBase::DownloadFile 网络地址[%1]或本地路径[%2]没有指定").arg(m_strUrl).arg(strFilePath));
		return false;
	}
	
	//返回码
	bool bRet = false;
	
	//获得本地文件存储的绝对路径
	QString strLocalFilePath = strFilePath;
	bRet = GetLocalFilePath(m_strUrl, strLocalFilePath, NULL, lpErrMsg);

	//返回文件名
	if(strLocalFilePath.length() <= 255)
		lpFilePath = strLocalFilePath;
	
	if(bRet)
	{//开始下载文件
		bRet = DownloadDataToFile(strLocalFilePath, lpErrMsg, m_dwTransferBlockSize);
	}
	
	return bRet;
}

//停止下载
void CHTTPBase::StopDownload()
{
	m_bStop = true;
}

//获取上传/下载文件当前已处理的字节数
//参数:   dwOverCount - [out] 当前已处理的字节数
//        dwTotalCount - [out] 上传/下载总的字节数
//返回值  true - 获取信息成功 false -获取信息失败
void CHTTPBase::GetCount(unsigned long &dwOverCount, unsigned long &dwTotalCount)
{
	dwOverCount = m_dwTreatOverCount;
	dwTotalCount = m_dwTotalCount;
}

//获取服务器响应报文
//返回值: 服务器响应的数据
//注意: SendRequest和UploadFile后调用
QString CHTTPBase::GetResponseData()
{
	return m_strRecvData;
}

//关闭连接
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
	{//初始化参数
		Init();
	}
}

//对字符串中的特殊字符进行转码，使之能够通过HTTP传输
void CHTTPBase::TransCode(QString &str)
{
	//'&'会把'&'字符后面的字符全部截断,需转码
	str.replace("&", "%26");

	//'+'如果直接使用会表示为空格
	str.replace("+", "%2B");

	str.replace(" ", "+");
}

//打开HTTPS上送连接
//参数:  lpUrl - [in] 远程服务器的路径
//       lpErrMsg - [out] 返回错误信息
bool CHTTPBase::OpenUploadHTTPSConnection(const QString& lpUrl, QString& lpErrMsg, bool bPost/* = true*/)
{
	unsigned long dwServiceType;
	CString strServer, strServerFile;
	unsigned short nPort;
	try
	{
		if(!AfxParseURL(lpUrl.toLocal8Bit().constData(), dwServiceType, strServer, strServerFile, nPort) || AFX_INET_SERVICE_HTTPS != dwServiceType)
		{//解析URL
			//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，[%s] 不是有效的https网络地址", lpUrl);  yhb log
			return false;
		}
		
		//创建SESSION
		m_pHttpSession = new CInternetSession();
		if(NULL == m_pHttpSession)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，会话创建失败");  yhb log
			return false;
		}

		m_pHttpSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 60000); 
		
		//通过SESSION得到CONNECTION
		nPort = INTERNET_DEFAULT_HTTPS_PORT;
		m_pHttpCon = m_pHttpSession->GetHttpConnection(strServer, nPort); 
		
		if(NULL == m_pHttpCon)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，创建与服务器的连接失败");  yhb log
			return false;
		}
		else
		{
			int nVerbType = bPost ? CHttpConnection::HTTP_VERB_POST : CHttpConnection::HTTP_VERB_GET;
			m_pHttpFile= m_pHttpCon->OpenRequest(nVerbType, strServerFile, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE ); 
			if(NULL == m_pHttpFile)
			{
				//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，找不到网络地址[%s]", lpUrl);  yhb log
				return false;
			}
		}
	}
	catch(CInternetException* e)
	{
		char chBuf[256] = {0};
		e->GetErrorMessage(chBuf, 255);
		//strErrString.TrimRight("\r\n"); 
		//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，错误原因:[%s]", strErrString);  yhb log
		e->Delete();
		return false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，未知错误");  yhb log
		return false;
	}
	return true;
}

//打开上送连接
//参数:  lpUrl - [in] 远程服务器的路径
//       lpErrMsg - [out] 返回错误信息
bool CHTTPBase::OpenUploadConnection(const QString& lpUrl, QString& lpErrMsg, bool bPost/* = true*/)
{
	unsigned long dwServiceType;
	CString strServer, strServerFile;
	unsigned short nPort;
	try
	{
		if(!AfxParseURL(lpUrl.toLocal8Bit().constData(), dwServiceType, strServer, strServerFile, nPort))
		{//解析URL
			//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，[%s] 不是有效的网络地址", lpUrl);  yhb log
			return false;
		}
		
		//创建SESSION
		m_pHttpSession = new CInternetSession();
		if(NULL == m_pHttpSession)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，会话创建失败");  yhb log
			return false;
		}
		
		//通过SESSION得到CONNECTION
		m_pHttpCon = m_pHttpSession->GetHttpConnection(strServer, nPort); 
		
		if(NULL == m_pHttpCon)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，创建与服务器的连接失败");  yhb log
			return false;
		}
		else
		{
			int nVerbType = bPost ? CHttpConnection::HTTP_VERB_POST : CHttpConnection::HTTP_VERB_GET;
			m_pHttpFile= m_pHttpCon->OpenRequest(nVerbType,strServerFile, NULL, 1, NULL, NULL, INTERNET_FLAG_NO_COOKIES|INTERNET_FLAG_EXISTING_CONNECT); 
			if(NULL == m_pHttpFile)
			{
				//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，找不到网络地址[%s]", lpUrl);  yhb log
				return false;
			}
		}
	}
	catch(CInternetException* e)
	{
		char chBuf[256] = {0};
		e->GetErrorMessage(chBuf, 255);
		//strErrString.TrimRight("\r\n");
		//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，错误原因:[%s]", strErrString);  yhb log
		e->Delete();
		return false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，未知错误");  yhb log
		return false;
	}
	return true;
}

//打开下载链接
//参数:  lpUrl - [in] 远程服务器的路径
//       lpErrMsg - [out] 返回错误信息
bool CHTTPBase::OpenDownloadConnection(const QString& lpUrl, QString& lpErrMsg)
{
	QString strUrl = lpUrl;
	m_bStop = false;
	QString strTemp;
	try
	{
		//查看网络格式是否正确
		unsigned long dwType;
		CString strServer, strServerFile;
		INTERNET_PORT wPort;
		if(!AfxParseURL(strUrl.toLocal8Bit().constData(), dwType, strServer, strServerFile, wPort))
		{
			//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，[%s] 不是有效的网络地址", lpUrl);  yhb log
			ReportError(QString::fromLocal8Bit("CHTTPBase::DownloadFile 打开连接失败，[%1] 不是有效的网络地址").arg(strUrl));
			return false;
		}
		
		//创建SESSION
		m_pHttpSession = new CInternetSession();
		if(NULL == m_pHttpSession)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，会话创建失败");  yhb log
			ReportError(QString::fromLocal8Bit("CHTTPBase::DownloadFile 打开连接失败，会话创建失败"));
			return false;
		}
		
		//当前下载参数是否被修改过
		if(!m_bSetDownloadPara)
		{//当前下载参数未被设置，使用默认下载参数
			m_dwBreakPointRetryInterval = 1000;
			m_dwMaxTransferRetryTimes = 2;
			m_dwTransferBlockSize = 1024;
			m_lDownloadStartBytes = 0;
			m_bOverWrite = true;
		}
		
		//设置参数
		m_bSetDownloadPara = false;
		m_pHttpSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, m_dwBreakPointRetryInterval); //重试之间的等待延时
		m_pHttpSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, m_dwMaxTransferRetryTimes); //重试次数
		
		//通过SESSION得到CONNECTION
		m_pHttpCon = m_pHttpSession->GetHttpConnection(strServer, wPort); 
		
		if(NULL == m_pHttpCon)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，创建与服务器的连接失败");  yhb log
			ReportError(QString::fromLocal8Bit("CHTTPBase::OpenDownloadConnection 打开连接失败，创建与服务器的连接失败"));
			return false;

		}
		
		//打开连接
		m_pHttpFile = m_pHttpCon->OpenRequest(CHttpConnection::HTTP_VERB_GET, strServerFile);
		
		if(NULL == m_pHttpFile)
		{
			//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，找不到网络地址[%s]", lpUrl);  yhb log
			ReportError(QString::fromLocal8Bit("打开连接失败，找不到网络地址[%1]").arg(strUrl));
			return false;
		}

		if(m_lDownloadStartBytes > 0)
		{//追加断点续传头
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
		{//发送请求失败
			//CCommonFunc::WriteLog(lpErrMsg, "向[%s]发送请求失败", lpUrl);  yhb log
			ReportError(QString::fromLocal8Bit("向[%1]发送请求失败").arg(strUrl));
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
			//CCommonFunc::WriteLog(lpErrMsg, "发送的请求未被服务器受理");  yhb log
			ReportError(QString::fromLocal8Bit("[%1]发送的请求未被服务器受理 响应码[%2]").arg(strUrl).arg(dwStateCode));

			CString strContextText;
			m_pHttpFile->QueryInfo(HTTP_QUERY_STATUS_TEXT, strContextText); 
			ReportError(QString::fromLocal8Bit("错误信息[%1]").arg(strContextText.GetBuffer(0)));
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
		//CCommonFunc::WriteLog(lpErrMsg, "打开连接失败，错误原因:[%s]", strErrString);  yhb log
		ReportError(QString::fromLocal8Bit("CHTTPBase::OpenDownloadConnection 打开连接[%1]失败，错误原因:[%2]").arg(strUrl).arg(QString::fromLocal8Bit(chBuf)));
		e->Delete();
		return false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "未知错误");  yhb log
		ReportError(QString::fromLocal8Bit("未知错误"));
		return false;
	}	
	return false;
}

//下载数据，并存储到文件
//参数: - lpFilePath - [in] 文件的存储路径
//		- lpErrMsg [in,out] 错误信息
//      - dwDownloadPerTime [in] 每次下载的字节数
//返回值: true - 下载成功 false - 下载失败
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
		//获取文件大小
		char szInfoBuffer[1024]; 
		unsigned long dwInfoBufferSize = sizeof(szInfoBuffer);
		if(m_pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, (void*)szInfoBuffer, &dwInfoBufferSize, NULL))
		{
			m_dwTotalCount = atoi(szInfoBuffer);	
		}
		else
		{
			//CCommonFunc::WriteLog(lpErrMsg, "获取文件信息失败，下载取消");  yhb log
			ReportError(QString::fromLocal8Bit("无法获取文件大小信息"));
			return false;
		}
		
		//获取本地存储路径
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
				ReportError(QString::fromLocal8Bit("创建本地文件[%1]失败:，下载取消 错误码[%2]").arg(lpFilePath).arg(GetLastError()));
			}
			else
			{
				//进度置为0
				emit m_pRelativeDownloader->sig_DownPercent(0, m_dwTotalCount + m_lDownloadStartBytes);
				ReportError(QString::fromLocal8Bit("本地文件[%1]丢失，请点击重试重新下载").arg(lpFilePath));
			}
			return false;
		}

		UINT openMode = m_bOverWrite ? CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyNone : CFile::modeReadWrite | CFile::shareDenyNone;

		//打开本地文件
		CFile file;
		if(!file.Open(lpFilePath.toLocal8Bit().constData(), openMode))
		{
			//CCommonFunc::WriteLog(lpErrMsg, "创建本地文件[%s]失败:，下载取消", lpFilePath);  yhb log

			if(m_bOverWrite)
			{
				ReportError(QString::fromLocal8Bit("创建本地文件[%1]失败:，下载取消 错误码[%2]").arg(lpFilePath).arg(GetLastError()));
			}
			else
			{
				//进度置为0
				emit m_pRelativeDownloader->sig_DownPercent(0, m_dwTotalCount + m_lDownloadStartBytes);
				ReportError(QString::fromLocal8Bit("本地文件[%1]丢失，请点击重试重新下载").arg(lpFilePath));
			}
			return false;
		}

		if(!m_bOverWrite)
		{
			file.SeekToEnd();
		}

		//期望下载速度
		DWORD dwExpectDownloadPerTime = dwDownloadPerTime;

		//定义下载缓存和下载速度
		pszBuffer = new char[dwDownloadPerTime + 1];
		memset(pszBuffer, 0, dwDownloadPerTime + 1);
		
		//读取服务器上数据
		dwDownloadPerTime = m_pHttpFile->Read(pszBuffer, dwDownloadPerTime); 
		
		while(dwDownloadPerTime > 0 && !m_bStop)
		{
			//写到本地文件
			file.Write(pszBuffer, dwDownloadPerTime);
			
			//保存已下载字节数
			m_dwTreatOverCount += dwDownloadPerTime;

			emit m_pRelativeDownloader->sig_DownPercent(m_dwTreatOverCount, m_dwTotalCount + m_lDownloadStartBytes);

			//更新下载速度
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
		//CCommonFunc::WriteLog(lpErrMsg, "下载文件失败，错误原因:[%s]", strErrString);  yhb log
		ReportError(QString::fromLocal8Bit("下载文件失败，错误原因:[%1]").arg(QString::fromLocal8Bit(chBuf)));
		e->Delete();
		delete[]pszBuffer;
		pszBuffer = NULL;
		return false;
	}
	catch(...)
	{
		//CCommonFunc::WriteLog(lpErrMsg, "未知错误");  yhb log
		ReportError(QString::fromLocal8Bit("未知错误"));
		delete[]pszBuffer;
		pszBuffer = NULL;
		return false;
	}	
}

//获取服务器响应报文
//参数: - lpErrMsg [out] 错误信息
//返回值: true - 获取成功 false - 获取失败
bool CHTTPBase::GetResponseData(QString& lpErrMsg)
{
	//定义缓存
	char * chBuf = NULL;
	
	try
	{
 		unsigned long dwDownloadCount = m_pHttpFile->GetLength();
 		
 		//获取数据
 		while (0 != dwDownloadCount)
 		{
 			//定义缓存
 			chBuf = new char[dwDownloadCount + 1];
 			memset(chBuf, 0 , dwDownloadCount + 1);
 			
 			//从服务器文件上获取数据
 			m_pHttpFile->Read(chBuf, dwDownloadCount);
 			
 			//记录响应的数据
 			m_strRecvData += chBuf;
 			
 			dwDownloadCount = m_pHttpFile->GetLength();
 			
 			if(NULL != chBuf)
 			{
 				delete [] chBuf;
 				chBuf = NULL;
 			}
 		}
		
		//CCommonFunc::WriteLogToFile(COMM_HTTP, ++m_dwHttpID > 99999 ? 0 : m_dwHttpID, "通信地址:[" + m_strUrl + "]\r\n" + m_strRecvData);  yhb log
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
		//CCommonFunc::WriteLog(lpErrMsg, "获取响应数据失败，错误原因:[%s]", tszErrString);  yhb log
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
		//CCommonFunc::WriteLog(lpErrMsg, "未知错误");  yhb log
		if(NULL != chBuf)
		{
			delete [] chBuf;
			chBuf = NULL;
		}
		return false;
	}
	
	return true;	
}

//随机得出一个分隔符
QString CHTTPBase::RandomMakeBoundary()
{
	QString strSeparate;
	CString strTemp;
	
	//获取当前时间
	CTime t = CTime::GetCurrentTime();
	strSeparate = t.Format("%Y%m%d%H%M%S");
	
	//得到一个随机值
	int nLength = strSeparate.length();
	strTemp.Format("%d", rand()%nLength);
	
	//在当前分隔符的随机位置插入一个随机值
	strSeparate.insert(rand()%nLength, strTemp.GetBuffer(0));
	
	//加入分隔线
	strSeparate = "----------" + strSeparate;
	
	//返回分隔符
	return strSeparate;
}

//根据URL的文件信息和用户提供的文件信息组合得到本地存储的绝对路径和文件名
//参数:  lpUrl - [in] 下载文件的URL地址
//       strFilePath - [in] 用户指定的本地存储路径(可以是文件夹，也可以是文件)
//       lpFileName - [in] strFilePath只是路径，lpFileName是本地的文件名
//       lpErrMsg - [out] 获取成功时返回空，获取失败时返回错误信息 
//注意：lpUrl，strFilePath，lpFileName会组合使用，strFilePath是一个文件夹时，如果未指定lpFileName，
//      他会将lpUrl的文件名作为本地文件名，如果lpFileName已制定，会以lpFileName存储，如果lpFileName未指定
//      后缀，会以lpUrl的后缀存储，如果strFilePath是一个文件绝对路径时，直接以strFilePath存储
bool CHTTPBase::GetLocalFilePath(const QString& lpUrl, QString &strFilePath, const QString& lpFileName, QString& lpErrMsg)
{
	if(strFilePath.isEmpty())
	{
		//CCommonFunc::WriteLog(lpErrMsg, "没有指出保存路径，下载取消!");  yhb log
		ReportError(QString::fromLocal8Bit("没有指出保存路径，下载取消"));
		return false;
	}

	QString strServerFileName, strLocalFileName, strServerFileExt, strLocalFileExt;
	
	//得到服务器返回来的文件名
	GetFileName(lpUrl, strServerFileName, strServerFileExt);
	
	if(0 == strServerFileName.length())
	{
		//CCommonFunc::WriteLog(lpErrMsg, "没有在URL地址中指出要下载的文件，下载取消!");  yhb log
		ReportError(QString::fromLocal8Bit("没有在URL地址中指出要下载的文件，下载取消!"));
		return false;
	}
	
	//查看路径中是否指定了本地文件名
	GetFileName(strFilePath, strLocalFileName, strLocalFileExt);

	//本地路径已指出了绝对路径，包括文件名
	if(!strLocalFileExt.isEmpty())
		return true;
	
	//获取文件名信息
	GetFileName(lpFileName, strLocalFileName, strLocalFileExt);

	if(!strLocalFileName.isEmpty())
	{//指定了文件名
		
		//查看文件名是否有后缀
		if(strLocalFileExt.isEmpty())
		{//没有后缀，以服务器返回的后缀作为文件后缀
			strLocalFileExt = strServerFileExt;
		}
		
		if(!strLocalFileName.isEmpty())
			strFilePath += "\\" + strLocalFileName + "." +  strLocalFileExt;
		else
			strFilePath += "." + strLocalFileExt;
	}
	else
	{
		//没有指定本地文件名，以服务器返回来的文件名存储
		strFilePath = strFilePath + "\\" + strServerFileName + "." + strServerFileExt;
	}
	
	return true;
}

//获取文件名
//参数: lpFilePath - [in] 文件本地路径
//		strFileName - [out] 文件名
//      strFileExt - [out] 文件后缀
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

//汇报错误
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

//组织HTTP报文头
void CHTTPComm::PrePareHTTPHeaderData()
{
	//AddHeader
}

//组织参数域信息
//参数: - strBoundary [in] - 分隔符
void CHTTPComm::PrePareHTTPParaData(const QString& lpBoundary)
{
	//AddPara
}

//获取上传文件时文件域的属性信息
//参数:  nNum - [in] 文件编号
//       lpBoundary - [in] 分隔符
//       lpFilePath - [in] 文件路径
//       lpNewFileName - [in] 新的文件名
//返回值: 返回文件域的属性信息
QString CHTTPComm::GetFilePropertyData(int nNum, const QString& lpBoundary, const QString& lpFilePath, const QString& lpNewFileName)
{
	QString strFileName, strFileExt, strFileType;
	
	//得到文件名和文件后缀
	GetFileName(lpNewFileName, strFileName, strFileExt);
	
	if(strFileExt.isEmpty())
	{//如果新的文件名没有指定后缀
		GetFileName(lpFilePath, strFileExt, strFileExt);
	}
	strFileName += "." + strFileExt;
	
	//获取文件属性
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

//获取结尾域信息
//参数: - lpBoundary [in] - 分隔符
//返回值: 返回结尾域属性信息
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
