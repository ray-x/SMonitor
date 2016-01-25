#pragma once

#include <QObject>
#include <QString>

//HTTP操作参数类型
typedef enum _HTTPTYPE
{
	HTTPCONNECTION = 0, //HTTP连接参数
	HTTPSESSION,        //HTTP会话参数
	HTTPFILE            //HTTP文件参数
}HTTPTYPE;

class CHTTPBase  : public QObject
{
	Q_OBJECT
public:
	CHTTPBase(QObject *parent = 0);
	virtual ~CHTTPBase();

	//设置下载管理器
	void SetRelativeDownloader(class CHTTPDownloader *pRelativeDownloader);
	
	//设置HTTP操作相关参数(需在OpenConnection之后调用, 否则无效)
	void SetOption(HTTPTYPE type, unsigned long dwOption, unsigned long dwValue, unsigned long dwFlags = 0);

	//设置下载参数
	//参数:   dwTransferBlockSize - [in] 每次传输的字节数
	//        dwMaxTransferRetryTimes - [in] 最大重试次数
	//        dwBreakPointRetryInterval - [in] 重试之间的等待时间
	//        lDownloadStartBytes - [in] 文件下载开始位置 
	//        bOverWrite - [in] 该次写入是否擦除文件
	//注意:   此函数可选，如果不调用此函数采用默认值，dwTransferBlockSize - 1024 dwMaxTransferRetryTimes - 2 dwBreakPointRetryInterval - 1000 
	void SetDownloadPara(unsigned long dwTransferBlockSize, unsigned long dwMaxTransferRetryTimes, unsigned long dwBreakPointRetryInterval, unsigned long lDownloadStartBytes, bool bOverWrite);

	//打开连接
	//参数:   lpUrl - [in] 要打开的HTTP url
	//        lpErrMsg - [out] 错误信息，打开成功时返回空
	//        bOpenedType - [in] 打开方式 true - 发送请求/上传文件  false - 下载文件
	//返回值: true - 打开成功,  false - 打开失败
	//执行发送请求，上传文件，下载文件必须先OpenConnection，最后CloseConnection
	bool OpenConnection(const QString& strUrl, QString& lpErrMsg, bool bOpenedType = true, bool bPost = true);

	//添加报文头
	//参数:   lpHeaderName - [in] 报文头
	//        lpHeaderValue - [in] 报文值
	//        lpErrMsg - [in,out] 错误信息
	//返回值: true - 添加成功 false - 添加失败
	//注意: OpenConnection后添加报文头，此函数可选
	bool AddHeader(const QString& strHeaderName, const QString& strHeaderValue, QString& lpErrMsg);

	//添加报文参数
	//参数:   lpParaName - [in] 参数名
	//        lpParaValue - [in] 参数值
	//        lpBoundary - [in] 分隔符,如果设置了分隔符，此时参数为FORM形式,分隔符可以通过GetBoundary获取
	//返回值: true - 添加成功 false - 添加失败
	//注意: OpenConnection后添加发送的参数，此函数可选
	bool AddPara(const QString& lpParaName, const QString& lpParaValue, const QString& lpBoundary = "");

	//添加FORM结尾域
	//注意: 在AddPara中添加了FORM形式的参数，并且使用的是SendRequest，则需要在SendRequest之前调用AddFormEnd
	//      如果为UploadFile则不需要
	bool AddFormEnd();

	//获取分隔符
	//返回值:  返回分隔符
	//注意: OpenConnection之后生成,在这一个周期里，它是不变的，只在OpenConnection之后改变
	QString GetBoundary();
	
	//发送HTTP请求
	//参数:   lpErrMsg - [out]请求成功时返回空，请求失败时返回错误信息
	//返回值: true - 请求成功， false - 请求失败 
	//注意: 必须先OpenConnection
	bool SendRequest(QString& lpErrMsg);

	//下载文件
	//参数:   lpFilePath - [in]文件的本地存储路径,如果路径中指出了文件名，则以此名保存，否则以服务器返回来的名字保存
	//                          如果路径中没有指定文件后缀，则以服务器返回的文件后缀保存
	//        lpErrMsg - [out]文件下载成功时返回空，文件下载失败时返回错误信息
	//返回值: true - 下载成功， false - 下载失败
	//注意: 必须先OpenConnection，一次只下载一个文件，但是必须CloseConnection -> OpenConnection -> DownloadFile
	bool DownloadFile(QString& lpFilePath, QString& lpErrMsg);

	//停止下载
	void StopDownload();

	//获取上传/下载文件当前已处理的字节数
	//参数:   dwOverCount - [out] 当前已处理的字节数
	//        dwTotalCount - [out] 上传/下载总的字节数
	//返回值  true - 获取信息成功 false -获取信息失败
	void GetCount(unsigned long &dwOverCount, unsigned long &dwTotalCount);

	//获取服务器响应报文
	//返回值: 服务器响应的数据
	//注意: SendRequest和UploadFile后调用
	QString GetResponseData();
	
	//关闭连接
	void CloseConnection(bool bInitPara);
	
	//对字符串中的特殊字符进行转码，使之能够通过HTTP传输
	void TransCode(QString &str);

protected:

	//初始化参数
	void Init();
	
	//打开上传链接
	//参数:  lpUrl - [in] 远程服务器的路径
	//       lpErrMsg - [out] 返回错误信息
	bool OpenUploadConnection(const QString& lpUrl, QString& lpErrMsg, bool bPost = true);

	//打开HTTPS上送连接
	//参数:  lpUrl - [in] 远程服务器的路径
	//       lpErrMsg - [out] 返回错误信息
	bool OpenUploadHTTPSConnection(const QString& lpUrl, QString& lpErrMsg, bool bPost = true);

	//打开下载链接
	//参数:  lpUrl - [in] 远程服务器的路径
	//       lpErrMsg - [out] 返回错误信息
	bool OpenDownloadConnection(const QString& lpUrl, QString& lpErrMsg);

	//组织HTTP报文头
	virtual void PrePareHTTPHeaderData() = 0;
	
	//组织参数域信息
	//参数: - strBoundary [in] - 分隔符
	//返回值: 返回文件域前面的域信息
	virtual void PrePareHTTPParaData(const QString& lpBoundary) = 0;

	//获取上传文件时文件域的属性信息
	//参数: - strBoundary [in] - 分隔符
	//返回值: 返回文件域前面的域信息
	virtual QString GetFilePropertyData(int nNum, const QString& lpBoundary, const QString& lpFilePath, const QString& lpNewFileName) = 0;

	//获取结尾域信息
	//参数: - lpBoundary [in] - 分隔符
	//返回值: 返回文件域属性信息
	virtual QString GetEndFileData(const QString& lpBoundary) = 0;

	//下载数据，并存储到文件
	//参数: - lpFilePath - [in] 文件的存储路径
	//		- lpErrMsg [in,out] 错误信息
	//      - dwDownloadPerTime [in] 每次下载的字节数
	//返回值: true - 下载成功 false - 下载失败
	bool DownloadDataToFile(const QString& lpFilePath, QString& lpErrMsg,unsigned long dwDownloadPerTime = 1024);

	//获取服务器响应报文
	//参数: - lpErrMsg [out] 错误信息
	//返回值: true - 获取成功 false - 获取失败
	bool GetResponseData(QString& lpErrMsg);
	
	//随机计算出一个分隔符
	QString RandomMakeBoundary();

	//根据URL的文件信息和用户提供的文件信息组合得到本地存储的绝对路径和文件名
	//参数:  lpUrl - [in] 下载文件的URL地址
	//       strFilePath - [in] 用户指定的本地存储路径(可以是文件夹，也可以是文件)
	//       lpFileName - [in] strFilePath只是路径，lpFileName是本地的文件名
	//       lpErrMsg - [out] 获取成功时返回空，获取失败时返回错误信息 
	//注意：lpUrl，strFilePath，lpFileName会组合使用，strFilePath是一个文件夹时，如果未指定lpFileName，
	//      他会将lpUrl的文件名作为本地文件名，如果lpFileName已制定，会以lpFileName存储，如果lpFileName未指定
	//      后缀，会以lpUrl的后缀存储，如果strFilePath是一个文件绝对路径时，直接以strFilePath存储
	bool GetLocalFilePath(const QString& lpUrl, QString &strFilePath, const QString& lpFileName, QString& lpErrMsg);

	//获取文件名
	//参数: lpFilePath - [in]文件本地路径
	//		strFileName - [out]文件名
	//      strFileExt - [out] 文件后缀
	void GetFileName(const QString& lpFilePath, QString &strFileName, QString &strFileExt);	

	//汇报错误
	void ReportError(const QString& errInfo);

protected:
	//当前HTTP会话的SESSION 
	class CInternetSession *m_pHttpSession;

	//当前HTTP会话中的连接
	class CHttpConnection *m_pHttpCon;

	//当前HTTP会话中的文件指针
	class CHttpFile *m_pHttpFile;

	//已发送/下载的字节数
	unsigned long m_dwTreatOverCount;

	//发送/下载总的字节数
	unsigned long m_dwTotalCount;

	//服务器返回的数据
	QString m_strRecvData;

	//参数域
	QString m_strParaData;

	//远程服务器URL
	QString m_strUrl;

	//分隔符
	QString m_strSeparate;

	//下载时每次传输的字节数
	unsigned long m_dwTransferBlockSize;

	//下载时最大重试次数
	unsigned long m_dwMaxTransferRetryTimes;

	//下载时重试之间的等待时间
	unsigned long m_dwBreakPointRetryInterval;

	//该次下载起始位置
	unsigned long m_lDownloadStartBytes;

	//是否设置了下载参数
	bool m_bSetDownloadPara;

	//标示每一笔HTTP报文的唯一性ID
	unsigned long m_dwHttpID;    

	class CHTTPDownloader* m_pRelativeDownloader;

	bool m_bStop;

	bool m_bOverWrite;
};

//HTTP工具类
class CHTTPComm : public CHTTPBase
{
public:
	CHTTPComm(QObject *parent = 0);
	~CHTTPComm();

protected:
	//组织HTTP报文头
	virtual void PrePareHTTPHeaderData();
	
	//组织参数域信息
	//参数: - strBoundary [in] - 分隔符
	//返回值: 返回文件域前面的域信息
	virtual void PrePareHTTPParaData(const QString& lpBoundary);

	//获取上传文件时文件域的属性信息
	//参数:  nNum - [in] 文件编号
	//       lpBoundary - [in] 分隔符
	//       lpFilePath - [in] 文件路径
	//       lpNewFileName - [in] 新的文件名
	//返回值: 返回文件域的属性信息
	virtual QString GetFilePropertyData(int nNum, const QString& lpBoundary, const QString& lpFilePath, const QString& lpNewFileName);

	//获取结尾域信息
	//参数: - nNum [in] - 文件序号
	//      - strFileName [in] - 文件名
	//返回值: 返回文件域属性信息
	virtual QString GetEndFileData(const QString& lpBoundary);
	
};