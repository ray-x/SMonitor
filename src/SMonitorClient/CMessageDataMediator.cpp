#include "CMessageDataMediator.h"
#include <QtCore/QCoreApplication>
#include <QFile>
#include "AccessHelper.h"
#include "CMisc.h"
#include "CClientLog.h"
#include "NetClass.h"
#include "CHomePageWidget.h"
CMessageDataMediator::CMessageDataMediator(QObject *parent)
	: QObject(qApp)
{
	m_bIsCanConnectedTime = false;
	m_strClientVersion = "20150430";
	m_bClientStartOver = false;
	m_bTextMode = false;
	m_bCrossLinkIsInstalled = CMisc::GetCrossLinkRunInstallState() == QString::fromLocal8Bit("是");
	m_bLocalCrossLinkIsInstalled = false;
}

CMessageDataMediator::~CMessageDataMediator()
{

}

CMessageDataMediator* CMessageDataMediator::m_Instance = NULL;

CMessageDataMediator* CMessageDataMediator::Instance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new CMessageDataMediator();
		Q_ASSERT(m_Instance);
	}

	return m_Instance;
}

//设置连接消息MAP属性值
void CMessageDataMediator::setConnectMsgMapProperty(MsgHead msgHead, bool bRecved)
{
	QMutexLocker locker(&m_ConnectMsgMapMutex);
	if(m_ConnectMsgMap.contains(msgHead))
	{
		m_ConnectMsgMap[msgHead] = bRecved;
	}
}

//初始化连接消息MAP
void CMessageDataMediator::initConnectMsgMap()
{
	QMutexLocker locker(&m_ConnectMsgMapMutex);
	m_ConnectMsgMap.clear();
	m_ConnectMsgMap.insert(EMSG_REQUESTXML, false);
	m_ConnectMsgMap.insert(EMSG_REQUESTMSG, false);
	m_ConnectMsgMap.insert(EMSG_SOFTUPLOAD, false);
}

//添加消息
void CMessageDataMediator::addConnectMsgProperty(MsgHead msgHead, bool bRecved)
{
	QMutexLocker locker(&m_ConnectMsgMapMutex);
	if(m_ConnectMsgMap.contains(msgHead))
	{
		m_ConnectMsgMap[msgHead] = bRecved;
	}
	else
	{
		m_ConnectMsgMap.insert(msgHead, bRecved);
	}
}

//获取消息属性
bool CMessageDataMediator::getConnectMsg(MsgHead msgHead, bool& bRecved)
{
	QMutexLocker locker(&m_ConnectMsgMapMutex);
	if(!m_ConnectMsgMap.contains(msgHead))
		return false;

	bRecved = m_ConnectMsgMap[msgHead];
	return true;
}

//判断所有消息是否全都收到
bool CMessageDataMediator::isAllMsgRecved(bool bNeedHasMsg)
{
	QMutexLocker locker(&m_ConnectMsgMapMutex);
	QMapIterator<MsgHead, bool> iter(m_ConnectMsgMap);
	while(iter.hasNext())
	{
		iter.next();
		if(!iter.value())
			return false;
	}

	if(bNeedHasMsg && m_ConnectMsgMap.count() <= 0)
		return false;

	return true;
}

//清空消息MAP
void CMessageDataMediator::clearConnectMsgMap()
{
	QMutexLocker locker(&m_ConnectMsgMapMutex);
	m_ConnectMsgMap.clear();
}

//查询门店信息
void CMessageDataMediator::QueryShopInfo(bool bRefreshRead)
{
	if(m_bCrossLinkIsInstalled) //软件安装成功已经上报，不再查询
		return;

	if(bRefreshRead || m_strShopId.isEmpty() || m_strUnitName.isEmpty() || m_strUnitNum.isEmpty())
	{
		QString strDataPath = "";
		if(!GetAccessDataPath(strDataPath))
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("access文件不存在"));
			return;
		}

		AccessHelper ss;
		ss.GetShopInfo(strDataPath, "deausbwsm", m_strShopId, m_strUnitNum, m_strUnitName);

		if(m_strShopId.isEmpty())
		{
			ClientLogger->AddLog(QString::fromLocal8Bit("access信息读取失败"));
		}
	}

	ClientLogger->AddLog(QString::fromLocal8Bit("Shopid [%1] unitnum[%2] unit_name[%3]").arg(m_strShopId).arg(m_strUnitNum).arg(m_strUnitName));
}

//是否需要上传CrossLinkRun安装信息
bool CMessageDataMediator::NeedUploadCrossLinkRunState()
{
	//服务器已记录，不需要再上传
	if(m_bCrossLinkIsInstalled)
		return false;

	//读取本地安装记录
	if(!m_bLocalCrossLinkIsInstalled)
	{
		if(NULL != NetClass->m_pHomePageWidget)
			m_bLocalCrossLinkIsInstalled = NetClass->m_pHomePageWidget->isLocalCrossLinkRunInstalled();
	}

	//本地未安装，返回
	if(!m_bLocalCrossLinkIsInstalled)
		return false;

	//判断数据库信息是否存在
	return !MessageDataMediator->m_strShopId.isEmpty();
}

bool CMessageDataMediator::GetAccessDataPath(QString& strDataPath)
{
	QString strPath = "semirgerp\\database\\semirgshop.mdb";
	if(QFile::exists("d:\\" + strPath))
	{
		strDataPath = "d:\\" + strPath;
		return true;
	}

	QStringList driveList = CMisc::GetLogicalDriveStringList();
	for each (auto driver in driveList)
	{
		if(QFile::exists(driver + strPath))
		{
			strDataPath = driver + strPath;
			return true;
		}
	}

	return false;
}