/*
This file is part of SMonitor.
Copyright  2015~2016  by: rayx  email rayx.cn@gmail.com
SMonitor is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SMonitor is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CHomePageWidget.h"
#include "CMisc.h"
#include "CSoftItemWidget.h"
#include "CFlowLayout.h"
#include "NetClass.h"
#include "NetClt.h"
#include "CListWidget.h"
#include "CNoFocusDelegate.h"
#include "CSoftManager.h"
#include "Markup.h"
#include "CClientLog.h"
#include "CMessageEventMediator.h"
#include "NetClass.h"
#include "CCategoryFactory.h"
#include "CMisc.h"
#include "Markup.h"
#include "CMessageDataMediator.h"
#include "CBrandList.h"

#include <QFontDataBase>
#include <QListWidgetItem>
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QDateTime>
#include <QScrollBar>
#include <QMutexLocker>

CHomePageWidget::CHomePageWidget(QWidget *parent)
	: QWidget(parent)
	, m_bMsgScrolled(false)
{
	setupUi(this);
	//serverStatus();
	//startTimer(1000); ȡ��������״̬��⣬��Ϊ����ֻ����4�η�����

	tbtn_msgMore->setFont(CMisc::FontAwesome());
	tbtn_msgMore->setText(QChar(0xf103));
	tbtn_msgMore->setCheckable(true);

	//���ȱ�ֻ֤����ʾ������¼
	m_msgListWidget = new CListWidget(this);
	m_msgListWidget->setSortingEnabled(true);
	m_msgListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_msgListWidget->verticalScrollBar()->setSliderPosition(0);
	//m_msgListWidget->setItemDelegate(new CNoFocusDelegate(this));

	gridLayout->addWidget(m_msgListWidget);

	QString IP, MAC;
	CMisc::GetLocalHostAddr(IP, MAC);
	label_IP->setText(QString::fromLocal8Bit("����IP: %1").arg(IP));

	connect(CategoryFactory, &CCategoryFactory::dataChangedNotify, this, &CHomePageWidget::setSoftwareListItem);
	connect(CategoryFactory, &CCategoryFactory::MessageNotify, this, &CHomePageWidget::recvMessage);
	connect(CategoryFactory, &CCategoryFactory::SoftwareInstalled, this, &CHomePageWidget::installFinished);
	connect(SoftManager, &CSoftManager::redownloadErrorOccered, this, &CHomePageWidget::installErrorOccur);
	connect(SoftManager, &CSoftManager::SoftWareInstall, this, &CHomePageWidget::InstallSoftware);
	connect(tbtn_msgMore, &QToolButton::clicked, this, &CHomePageWidget::showTotalMessage);
	connect(tbtn_onekeyApp, &QToolButton::clicked, this, &CHomePageWidget::upgradeAllItems);
	connect(NetClass->m_pMessageEventMediator, &CMessageEventMediator::sigGetDistributeMsg,
		this, &CHomePageWidget::getDistributeMsg);
	connect(NetClass->m_pMessageEventMediator, &CMessageEventMediator::sigServerDistributeMsg,
		this, &CHomePageWidget::recvDistributeMsg);
	connect(SoftManager, &CSoftManager::downloadQuit, this, &CHomePageWidget::downloadQuit);
	connect(SoftManager, &CSoftManager::InstallCancled, this, &CHomePageWidget::downloadQuit);

	QWidget* scrollWidget = new QWidget(this);
	scrollWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	scrollWidget->setContentsMargins(6, 6, 6, 6);
	m_scrollLayout = new CFlowLayout(scrollWidget);
	m_scrollLayout->setAlignment(Qt::AlignHCenter);
	scrollArea->setWidget(scrollWidget);

	m_StorageThread = new CStorageMsgThread(this);
	m_StorageThread->Start();

	init();
}

CHomePageWidget::~CHomePageWidget()
{
	if(m_StorageThread)
	{
		delete m_StorageThread;
		m_StorageThread = nullptr;
	}
}

void CHomePageWidget::init()
{
	CMarkup xml;
	QString strPath = QApplication::applicationDirPath() + "/Message.list";
	{
		QMutexLocker qMutexLocker(&m_qMutexForMessage);
		if(xml.Load(strPath.toLocal8Bit().constData()))
		{
			if(xml.FindChildElem("MessageList"))
			{
				xml.IntoElem();
				while(xml.FindChildElem("Message"))
				{
					xml.IntoElem();
					QString time = QString::fromStdString(xml.GetAttrib("time"));
					QString msg = QString::fromStdString(xml.GetAttrib("content"));
					m_MsgMap.insert(QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss zzz"), msg);
					xml.OutOfElem();
				}
				xml.OutOfElem();
			}
		}
	}

	//ɾ���Ѿ����ڵĶ������Ϣ
	while(m_MsgMap.count() >= MAXMSGSHOWCOUNT)
	{
		QDateTime time = m_MsgMap.firstKey();
		m_MsgMap.remove(time);
		removeMessage(time.toString("yyyy-MM-dd hh:mm:ss zzz"));
	}

	QMapIterator<QDateTime, QString> iter(m_MsgMap);
	while(iter.hasNext())
	{
		iter.next();
		appendMessage(iter.key(), iter.value());
	}

	
}

void CHomePageWidget::removeMessage(const QString& time)
{
	CMarkup xml;
	QString strPath = QApplication::applicationDirPath() + "/Message.list";
	QMutexLocker qMutexLocker(&m_qMutexForMessage);
	if(xml.Load(strPath.toLocal8Bit().constData()))
	{
		::SetFileAttributes(strPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_NORMAL); 

		if(xml.FindChildElem("MessageList"))
		{
			xml.IntoElem();
			bool bExist = false;
			while(xml.FindChildElem("Message"))
			{
				xml.IntoElem();
				QString xtime = QString::fromStdString(xml.GetAttrib("time"));
				if(xtime == time){
					xml.RemoveElem();
					bExist = true;
					break;
				}
				/*m_MsgMap.insert(QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss zzz"), msg);*/
				xml.OutOfElem();
			}
			xml.OutOfElem();

			xml.Save(strPath.toLocal8Bit().constData());
		}
	}
	::SetFileAttributes(strPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN); 
}

bool CHomePageWidget::getInstallSoftXml(CMarkup *xml, bool& bLocalCrossInstall)
{
	QMutexLocker locker(&m_WidgetHashMutex);
	xml->SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	xml->AddElem("Message");
	xml->AddChildElem("Header");
	xml->IntoElem();
	xml->AddAttrib("MsgType", "EMSG_SOFTUPLOAD_REQ");
	xml->OutOfElem();
	xml->AddChildElem("InfoList");
	xml->IntoElem();
	xml->AddAttrib("Id", MessageDataMediator->m_strNewId.toStdString().c_str());
	xml->OutOfElem();
	xml->IntoElem();

	if(!MessageDataMediator->m_bCrossLinkIsInstalled && MessageDataMediator->m_strShopId.isEmpty())
	{//����û�л�ȡ���ŵ���Ϣ�����»�ȡһ��
		MessageDataMediator->QueryShopInfo(true);
	}

	bool bNeedUploadCrossLinRunInfo = !MessageDataMediator->m_strShopId.isEmpty() && !MessageDataMediator->m_bCrossLinkIsInstalled;
	ClientLogger->AddLog(QString::fromLocal8Bit("��������־[%1]").arg(MessageDataMediator->m_bCrossLinkIsInstalled ? "1" : "0"));

	bLocalCrossInstall = false;

	int nSoftNum = 0;
	QMapIterator<QString, CSoftItemWidget*> iter(m_subWidgetHash);
	while(iter.hasNext())
	{
		iter.next();
		CSoftItemWidget* pSoftItemWidget = iter.value();

		if(NULL != pSoftItemWidget)
		{
			const SoftData& softData = pSoftItemWidget->getData();
			if(softData.category != QString::fromLocal8Bit("��ҳ") && softData.category != INDEPENDENT)
			{
				if(softData.state == SMonitor::Normal || softData.state == SMonitor::Upgrade)
				{
					xml->AddChildElem("Software");
					xml->IntoElem();
					xml->AddAttrib("Name", softData.name.toStdString().c_str());
					xml->AddAttrib("Version", softData.version.toStdString().c_str());
					xml->AddAttrib("Category", softData.category.toStdString().c_str());
					xml->OutOfElem();
					nSoftNum++;

					if(bNeedUploadCrossLinRunInfo && !bLocalCrossInstall)
					{
						bLocalCrossInstall = softData.location.contains("CrosslinkRun", Qt::CaseInsensitive);
					}
				}
			}
		}
	}

	ClientLogger->AddLog(QString::fromLocal8Bit("����crosslinkrun[%1] �Ƿ���Ҫ�ϴ�[%2]").arg(bLocalCrossInstall ? "1" : "0").arg(bNeedUploadCrossLinRunInfo ? "1" : "0"));

	if(0 == nSoftNum)
	{//����û�а�װ�κ����
		MessageDataMediator->setConnectMsgMapProperty(EMSG_SOFTUPLOAD, true);
		return false;
	}

	xml->OutOfElem();

	if(bLocalCrossInstall && bNeedUploadCrossLinRunInfo)
	{
		//������Ϣ
		xml->AddChildElem("ExtInfo");
		xml->IntoElem();
		xml->AddAttrib("ShopId", MessageDataMediator->m_strShopId.toStdString().c_str());
		xml->AddAttrib("UnitNum", MessageDataMediator->m_strUnitNum.toStdString().c_str());
		xml->AddAttrib("UnitName", MessageDataMediator->m_strUnitName.toStdString().c_str());
		xml->AddAttrib("CrosslinkRunInstalledState", bLocalCrossInstall ? QString::fromLocal8Bit("��").toStdString().c_str() :QString::fromLocal8Bit("��").toStdString().c_str());
		xml->OutOfElem();
	}
	
	return true;
}

bool CHomePageWidget::isItemEmpty()
{
	return m_subWidgetHash.isEmpty();
}

bool CHomePageWidget::isLocalCrossLinkRunInstalled()
{
	if(m_subWidgetHash.size() == 0)
		return false;

	bool bLocalCrossInstall = false;
	QMapIterator<QString, CSoftItemWidget*> iter(m_subWidgetHash);
	while(iter.hasNext())
	{
		iter.next();
		CSoftItemWidget* pSoftItemWidget = iter.value();

		if(NULL != pSoftItemWidget)
		{
			const SoftData& softData = pSoftItemWidget->getData();
			if(softData.category != QString::fromLocal8Bit("��ҳ") && softData.category != INDEPENDENT)
			{
				if(softData.state == SMonitor::Normal || softData.state == SMonitor::Upgrade)
				{
					bLocalCrossInstall = softData.location.contains("CrosslinkRun", Qt::CaseInsensitive);
					if(bLocalCrossInstall)
						break;
				}
			}
		}
	}

	return bLocalCrossInstall;
}

void CHomePageWidget::installFinished(const QString& key)
{
	QMutexLocker locker(&m_WidgetHashMutex);
	if(m_subWidgetHash.contains(key))
	{
		m_subWidgetHash.value(key)->installFinished();
		recvMessage(QDateTime::currentDateTime(), 
			QString::fromLocal8Bit("���[%1]��װ�ɹ���").arg(key));
	}

	//ˢ������б����
	updateSoftWidget();
}

void CHomePageWidget::downloadQuit(const QString& key)
{
	QMutexLocker locker(&m_WidgetHashMutex);
	if(m_subWidgetHash.contains(key))
	{
		m_subWidgetHash.value(key)->downloadQuit();
		recvMessage(QDateTime::currentDateTime(), 
			QString::fromLocal8Bit("���[%1]ȡ�����ذ�װ����").arg(key));
	}
}

void CHomePageWidget::installErrorOccur(const QString& key, int state)
{
	QMutexLocker locker(&m_WidgetHashMutex);
	if(m_subWidgetHash.contains(key))
	{
		m_subWidgetHash.value(key)->installErrorOccur();
		recvMessage(QDateTime::currentDateTime(), 
			QString::fromLocal8Bit("���[%1]��װʧ�ܣ����Ժ����ԡ�").arg(key));
	}
}

void CHomePageWidget::showTotalMessage(bool checked)
{
	if(checked)
		m_msgListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	else{
		m_msgListWidget->verticalScrollBar()->setSliderPosition(0);
		m_msgListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}
}

void CHomePageWidget::setSoftwareListItem(const QMap<QString, QMap<QString, SoftData> >& data)
{
	QMutexLocker locker(&m_WidgetHashMutex);
	qDebug() << "Reset Data";
	//int count = m_scrollLayout->count();
	////������������Ѵ��ڵ���
	//for(int i = 0; i < count; i++){

	//	m_scrollLayout->removeItem(m_scrollLayout->itemAt(i));
	//}

	bool bContainWeb = data.contains(QString::fromLocal8Bit("��ҳ"));

	QMapIterator<QString, CSoftItemWidget*> iteretor(m_subWidgetHash);
	while(iteretor.hasNext())
	{
		iteretor.next();
		const SoftData& sData = iteretor.value()->getData();
		if(bContainWeb && sData.category == QString::fromLocal8Bit("��ҳ"))
		{
			delete iteretor.value();
			m_subWidgetHash.remove(iteretor.key());
		}
		else if(sData.state == SMonitor::UnInstall)
		{//δ����װ����ɾ��
			delete iteretor.value();
			m_subWidgetHash.remove(iteretor.key());
		}
	}

	//���������ҳ������Ҫ��֮ǰ��������ҳȫ�����
// 	if(data.contains(QString::fromLocal8Bit("��ҳ")))
// 	{
// 		QMapIterator<QString, CSoftItemWidget*> iter(m_subWidgetHash);
// 		while(iter.hasNext())
// 		{
// 			iter.next();
// 			const SoftData& sData = iter.value()->getData();
// 			if(sData.category == QString::fromLocal8Bit("��ҳ"))
// 			{
// 				delete iter.value();
// 				m_subWidgetHash.remove(iter.key());
// 			}
// 		}
// 	}

	//��������б�
	QMapIterator<QString, QMap<QString, SoftData> > iter(data);
	while(iter.hasNext())
	{
		iter.next();
		QMapIterator<QString, SoftData> iter2(iter.value());
		while(iter2.hasNext())
		{
			iter2.next();
			QString key = iter2.key().split("##").first();
			SoftData value = iter2.value();
			if(m_subWidgetHash.contains(key))
			{
				CSoftItemWidget* item = m_subWidgetHash.value(key);
				item->updateState(value);
			}
			else
			{
				CSoftItemWidget* item = new CSoftItemWidget(value, this);
				m_subWidgetHash.insert(key, item);
				connect(item, &CSoftItemWidget::NotifyOperation, this, &CHomePageWidget::NotifyOperation);
				qDebug() << key;
				m_scrollLayout->addWidget(item);
			}
		}
	}
}

void CHomePageWidget::recvDistributeMsg(const QString& msg, const QString& time)
{
	if(time.isEmpty()) return ;
	QDateTime dtime = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss zzz");

	if(!m_MsgMap.contains(dtime)){

		/*if(m_msgListWidget->count() >= MAXMSGSHOWCOUNT)
		delete m_msgListWidget->takeItem(m_msgListWidget->count() - 1);

		if(m_MsgMap.count() > MAXMSGSHOWCOUNT){
		QDateTime dTime = m_MsgMap.firstKey();
		m_MsgMap.remove(dTime);
		removeMessage(dTime.toString("yyyy-MM-dd hh:mm:ss zzz"));
		}*/

		recvMessage(dtime, msg);
	}
}

void CHomePageWidget::getDistributeMsg(const QString& msg, const QString& time)
{
	if(time.isEmpty()) return ;
	QDateTime dtime = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss zzz");
	recvMessage(dtime, msg);
}

void CHomePageWidget::recvMessage(const QDateTime& time, const QString& content)
{
	if(time.isNull()) return ;
	if(!m_MsgMap.contains(time))
	{
		if(m_msgListWidget->count() >= MAXMSGSHOWCOUNT)
			delete m_msgListWidget->takeItem(m_msgListWidget->count() - 1);

		
		if(m_MsgMap.count() >= MAXMSGSHOWCOUNT){
			QDateTime dTime = m_MsgMap.firstKey();
			m_MsgMap.remove(dTime);
			removeMessage(dTime.toString("yyyy-MM-dd hh:mm:ss zzz"));
		}
		//ִ����Ϣ�������
		m_StorageThread->AddMessage(time, content);
		m_MsgMap.insert(time, content);
		
		appendMessage(time, content);
	}
}

void CHomePageWidget::appendMessage(const QDateTime& time, const QString& content)
{
	QListWidgetItem* msg = new QListWidgetItem();
	QString strContent = QString("[%1] %2").arg(time.toString("MM-dd hh:mm:ss")).arg(content);
	//str.replace(QRegExp("(((?:https?|ftp)://|www\\.)[\\.A-Za-z0-9\-]+\\.[a-zA-Z]{2,4}"), "<a href=\"\\1\">\\1</a>");
	//ƥ���ַ����п��ܴ��ڵ����Ӳ��Գ����ӵ���ʽ��ʾ
	QRegExp regExp("((([A-Za-z]{3,9}:(?:\\/\\/)?)(?:[\\-;:&=\\+\\$,\\w]+@)?[A-Za-z0-9\\.\\-]+|(?:www\\.|[\\-;:&=\\+\\$,\\w]+@)[A-Za-z0-9\\.\\-]+)((?:\\/[\\+~%\\/\\.\\w\\-]*)?\\??(?:[\\-\\+=&;%@\\.\\w]*)#?(?:[\\.\\!\\/\\\\\\w]*))?)");
	strContent.replace(regExp, "<a href=\"\\1\">\\1</a>");
	CLabel* label = new CLabel(this);
	label->setFixedHeight(21);
	label->setText(strContent);
	label->setOpenExternalLinks(true);
	label->setCursor(Qt::IBeamCursor);
	label->setTextFormat(Qt::RichText);
	label->setFont(QFont("Microsoft yahei UI", 9));
	label->setTextInteractionFlags(Qt::TextBrowserInteraction);
	label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	msg->setToolTip(content);
	msg->setFlags(Qt::ItemIsSelectable);
	msg->setSizeHint(label->sizeHint());
	msg->setForeground(Qt::white);
	msg->setData(Qt::DisplayRole, time.toString("MM-dd hh:mm:ss zzz"));

	QFontMetrics fontMrtrics(label->font());
	int width = fontMrtrics.width(content);
	if(width > 475)
		m_msgListWidget->setFixedHeight(124);

	/*int index = m_MsgMap.keys().indexOf(time);*/
	/*qDebug() << QString::fromLocal8Bit("��Ϣλ��Ϊ") << index;*/
	m_msgListWidget->insertItem(0/*m_MsgMap.count() - 1 - index*/, msg);
	m_msgListWidget->setItemWidget(msg, label);
	m_msgListWidget->sortItems(Qt::DescendingOrder);
}

//��ʱɨ���������������״̬���Ѹ��¿ͻ��˽�����ʾ
void CHomePageWidget::timerEvent(QTimerEvent *event)
{
	serverStatus();
}

void CHomePageWidget::serverStatus()
{
	if(NetClass->m_pNetClt->m_bIsOnLine)
		label_Svr->setText(QString::fromLocal8Bit("����������: ����"));	
	else
		label_Svr->setText(QString::fromLocal8Bit("����������: �Ͽ�"));
}



void CHomePageWidget::upgradeAllItems()
{
	//һ��������ʱ����Ҫ����������Ȳ鿴�����ǲ����Ѿ���װ��ĳ��Ʒ�ƣ�
	//���ĳ��Ʒ���Ѿ�����װ�ˣ���ôֻ��Ҫ���������Ǹ�Ʒ�Ƶ����
	QString brand = CategoryFactory->GetLocalBrand();
	QMultiMap<QString, CSoftItemWidget*> BrandSoftMap; //��Ҫ����Ʒ�ư�װ�����
	QMap<QString, CSoftItemWidget*> UnBrandSoftMap;
	QMutexLocker locker(&m_WidgetHashMutex);
	QMapIterator<QString, CSoftItemWidget*> iter(m_subWidgetHash);

	QVector<CSoftItemWidget*> selfUpdateItemVec;
	while(iter.hasNext())
	{
		iter.next();
		//��ȡ��ǰͬƷ�ƿ�������װ���
		const SoftData& data = iter.value()->getData();

		if(data.category == QString::fromLocal8Bit("��ҳ"))
			continue;

		if(data.category == INDEPENDENT)
		{
			selfUpdateItemVec.push_back(iter.value());
			continue;
		}

		if(data.category == QString::fromLocal8Bit("����")){
			UnBrandSoftMap.insert(data.brand, iter.value());
			continue;
		}

		if(data.state == SMonitor::Upgrade || data.state == SMonitor::UnInstall)
			BrandSoftMap.insert(data.brand, iter.value());
	}

	if(BrandSoftMap.contains(brand))
	{//�б��а��������Ѿ���װ���������ôֻ�谲װ�Ѿ�������Ʒ��
		QList<CSoftItemWidget*> items = BrandSoftMap.values(brand);
		foreach(auto item, items)
		{
			item->setOperation();
		}
	}
	else
	{
		if(BrandSoftMap.count() > 1)
		{
			//�����û���Ҫѡ��Ʒ��
			CBrandList brandList(this);
			brandList.SetBrands(BrandSoftMap.keys());
			if(QDialog::Accepted == brandList.exec())
			{
				QString selBrand = brandList.GetCurBrand();
				QList<CSoftItemWidget*> items = BrandSoftMap.values(selBrand);
				foreach(auto item, items){
					item->setOperation();
				}
			}
			else
			{
				return ;
			}
		}
		else if(BrandSoftMap.count() == 1)
		{
			//ֻ��һ��Ʒ�ƣ�ֱ�Ӱ�װ
			QList<CSoftItemWidget*> items = BrandSoftMap.values(brand);
			foreach(auto item, items)
			{
				item->setOperation();
			}
		}
	}

	QMapIterator<QString, CSoftItemWidget*> iter2(UnBrandSoftMap);
	while(iter2.hasNext())
	{
		iter2.next();
		iter2.value()->setOperation();
	}

	for (QVector<CSoftItemWidget*>::iterator iter = selfUpdateItemVec.begin(); iter != selfUpdateItemVec.end(); iter++)
	{
		(*iter)->setOperation();
	}
}

void CHomePageWidget::updateSoftWidget()
{
	QString brand = CategoryFactory->GetLocalBrand();
	if(!brand.isEmpty())
	{
		QMap<QString, CSoftItemWidget*> dataHash = m_subWidgetHash;
		QMapIterator<QString, CSoftItemWidget*> iter(dataHash);
		while(iter.hasNext())
		{
			iter.next();

			CSoftItemWidget* item = iter.value();
			const SoftData& data = item->getData();
			if((data.category == QString::fromLocal8Bit("ERP") ||
				data.category == QString::fromLocal8Bit("����")) && 
				data.brand != brand)
			{
				delete item;
				item = nullptr;
				m_subWidgetHash.remove(iter.key());
			}
			else if(data.category == QString::fromLocal8Bit("��ҳ"))
			{
				if(data.brand != "" && data.brand != brand)
				{
					delete item;
					item = nullptr;
					m_subWidgetHash.remove(iter.key());
				}
			}
		}
	}
}

void CHomePageWidget::showIndependentItem(QString strKey, QString version, bool bShow)
{
	if(bShow)
	{
		if(!m_subWidgetHash.contains(strKey))
		{
			SoftData data;
			data.name = strKey;
			data.version = version;
			data.state = SMonitor::Upgrade;
			data.category = INDEPENDENT;
			CSoftItemWidget* item = new CSoftItemWidget(data, this);
			m_subWidgetHash.insert(strKey, item);
			connect(item, &CSoftItemWidget::NotifyOperation, this, &CHomePageWidget::NotifyOperation);
			m_scrollLayout->addWidget(item);

			recvMessage(QDateTime::currentDateTime(), 
				QString::fromLocal8Bit("�ͻ������ [%1] �ɸ������汾 [%2]").arg(strKey).arg(version));
		}
	}
	else
	{
		if(m_subWidgetHash.contains(strKey))
		{
			delete m_subWidgetHash.value(strKey);
			m_subWidgetHash.remove(strKey);
		}
	}	
}

void CHomePageWidget::InstallSoftware(const QString& key)
{
	if(m_subWidgetHash.contains(key))
	{
		m_subWidgetHash.value(key)->setOperation();
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("��ҳ�в��������:%1").arg(key));
	}
}

CStorageMsgThread::CStorageMsgThread(QObject* parent /* = 0 */)
	: QThread(parent)
	, m_bThreadExit(true)
{
	m_strMsgPath = QApplication::applicationDirPath() + "/Message.list";
}

CStorageMsgThread::~CStorageMsgThread()
{
	this->terminate();
}

void CStorageMsgThread::Start()
{
	CMarkup xml;

	if(!CMisc::IsFileExist(m_strMsgPath))
	{
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		xml.AddElem("Root");
		xml.AddChildElem("MessageList");
		xml.Save(m_strMsgPath.toLocal8Bit().constData());

		::SetFileAttributes(m_strMsgPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN); 
	}

	start();
	m_bThreadExit = false;
}

void CStorageMsgThread::AddMessage(const QDateTime& date, const QString& msg)
{
	if(date.isNull() || msg.isEmpty())
		return;
	QMutexLocker qMutexLocker(&m_qMutex);

	if(!m_MsgMap.contains(date))
	{
		m_MsgMap.insert(date, msg);
	}
}

void CStorageMsgThread::run()
{
	while(!m_bThreadExit)
	{
		if(CheckMsgList())
		{
			StorgeMessage();
		}

		msleep(5000);
	}
}

bool CStorageMsgThread::CheckMsgList()
{
	if(m_MsgMap.count()  <= 0) return false;
	
	QMutexLocker qMutexLocker(&m_qMutex);
	m_MsgList.clear();
	QMapIterator<QDateTime, QString> iter(m_MsgMap);
	while(iter.hasNext())
	{
		iter.next();
		m_MsgList.append(qMakePair(iter.key().toString("yyyy-MM-dd hh:mm:ss zzz"), iter.value()));
	}
	m_MsgMap.clear();

	return !m_MsgList.isEmpty();
}

void CStorageMsgThread::StorgeMessage()
{
	CMarkup xml;
	if(!xml.Load(m_strMsgPath.toLocal8Bit().constData()))
	{
		xml.SetDoc("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
		xml.AddElem("Root");
		xml.AddChildElem("MessageList");
		xml.ResetMainPos();
	}
	else
	{
		::SetFileAttributes(m_strMsgPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_NORMAL); 
	}

	if(xml.FindChildElem("MessageList"))
	{
		xml.IntoElem();

		foreach(auto qPair, m_MsgList){
			xml.AddChildElem("Message");
			xml.IntoElem();
			xml.AddAttrib("time", qPair.first.toStdString());
			xml.AddAttrib("content", qPair.second.toStdString());
			xml.OutOfElem();
		}
		xml.OutOfElem();
	}

	xml.Save(m_strMsgPath.toLocal8Bit().constData());
	::SetFileAttributes(m_strMsgPath.toLocal8Bit().constData(), FILE_ATTRIBUTE_HIDDEN); 
	m_MsgList.clear();
}