#include "CSoftwareListWidget.h"
#include "CTableWidget.h"

#include <QTabWidget>
#include <QHBoxLayout>

CSoftwareListWidget::CSoftwareListWidget(QWidget *parent)
	: QWidget(parent)
{
	m_pTabWidget = new QTabWidget(this);
	QHBoxLayout* hboxLayout = new QHBoxLayout(this);
	hboxLayout->addWidget(m_pTabWidget);
	connect(CXmlParse::Instance(), &CXmlParse::dataChangedNotify, this, &CSoftwareListWidget::updateUi);
}

CSoftwareListWidget::~CSoftwareListWidget()
{

}

void CSoftwareListWidget::updateUi(const QMap<QString, SoftData>& data)
{
	QMapIterator<QString, SoftData> iter(data);
	while(iter.hasNext())
	{
		iter.next();
		//本地安装程序
		if(iter.value().bUpgrade && m_pTabMap.contains(iter.key()))
		{
			m_pTabMap.value(iter.key())->updateTable(iter.value());
		}
		else if(iter.value().bInstall)
		{
			CTableWidget* table = new CTableWidget(iter.value(), this);
			m_pTabWidget->addTab(table,iter.key());
			m_pTabMap.insert(iter.key(), table);
		}
	}
	m_pTabWidget->setCurrentIndex(0);
}
