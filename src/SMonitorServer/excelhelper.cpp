
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

#include "stdafx.h"
#include "excelhelper.h"

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include "Markup.h"

//�̶�������
static QStringList columnNames = QStringList()
	<< QString::fromLocal8Bit("Ӫ������") << QString::fromLocal8Bit("��֯���") 
	<< QString::fromLocal8Bit("��֯����") << QString::fromLocal8Bit("�ŵ���") 
	<< QString::fromLocal8Bit("�ŵ�����") << QString::fromLocal8Bit("�Ƿ�װ");

ExcelHelper *ExcelHelper::_instance = 0;
ExcelHelper::ExcelHelper(QObject *parent) :
    QObject(parent)
{
}

bool ExcelHelper::ToExcel(QString fileName, QString sheetName, QString title, QString columnNames[], int columnWidths[], int columnCount, QStringList content)
{
    //���ƴ��xml�ַ�,�ٱ���Ϊxls�ļ�
    QStringList html;

    //�̶�ͷ����Ϣ
    html.append("<?xml version=\"1.0\"?>\n");
    html.append("<?mso-application progid=\"Excel.Sheet\"?>\n");
    html.append("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    html.append(" xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n");
    html.append(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n");
    html.append(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    html.append(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n");

    //�ĵ���Ϣ
    html.append(" <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
    html.append(QString::fromLocal8Bit("  <LastAuthor>CompanyName<LastAuthor>\n"));
    html.append(QString("  <Created>%1</Created>\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
    html.append("  <Version>11.6360</Version>\n");
    html.append(" </DocumentProperties>\n");

    html.append(" <OfficeDocumentSettings xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
    html.append("  <AllowPNG/>\n");
    html.append(" </OfficeDocumentSettings>\n");

    html.append(" <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
    html.append("  <WindowHeight>9795</WindowHeight>\n");
    html.append("  <WindowWidth>21435</WindowWidth>\n");
    html.append("  <WindowTopX>120</WindowTopX>\n");
    html.append("  <WindowTopY>75</WindowTopY>\n");
    html.append("  <WindowTopY>75</WindowTopY>\n");
    html.append("  <ProtectWindows>False</ProtectWindows>\n");
    html.append(" </ExcelWorkbook>\n");

    //��ʽ��
    html.append(" <Styles>\n");
    html.append("  <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n");
    html.append("   <Alignment ss:Vertical=\"Center\"/>\n");
    html.append("   <Borders/>\n");
    html.append(QString::fromLocal8Bit("   <Font ss:FontName=\"����\" x:CharSet=\"134\" ss:Size=\"11\" ss:Color=\"#000000\"/>\n"));
    html.append("   <Interior/>\n");
    html.append("   <NumberFormat/>\n");
    html.append("   <Protection/>\n");
    html.append("  </Style>\n");
    //������ʽ
    html.append("  <Style ss:ID=\"s64\">\n");
    html.append("   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n");
    html.append(QString::fromLocal8Bit("   <Font ss:FontName=\"����\" x:CharSet=\"134\" ss:Size=\"14\" ss:Color=\"#000000\" ss:Bold=\"1\"/>\n"));
    html.append("  </Style>\n");
    //���߿���ʽ
    html.append("  <Style ss:ID=\"s91\">\n");
    html.append("  <Borders>\n");
    html.append("   <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    html.append("   <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    html.append("   <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    html.append("   <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    html.append("  </Borders>\n");
    html.append(" </Style>\n");

    html.append(" </Styles>\n");

    //����������
    html.append(QString(" <Worksheet ss:Name=\"%1\">\n").arg(sheetName));
    int rowCount = content.count(); //����������

    //���ʼ
    html.append(QString("  <Table ss:ExpandedColumnCount=\"%1\" ss:ExpandedRowCount=\"%2\" x:FullColumns=\"1\"\n").arg(columnCount).arg(rowCount + 2));
    html.append("   x:FullRows=\"1\" ss:DefaultColumnWidth=\"54\" ss:DefaultRowHeight=\"18\">\n");

    //�����ֶο��
    for (int i = 0; i < columnCount; i++) {
        html.append(QString("   <Column ss:AutoFitWidth=\"0\" ss:Width=\"%1\"/>\n").arg(QString::number(columnWidths[i])));
    }

    //������
    html.append("   <Row ss:AutoFitHeight=\"0\" ss:Height=\"26.25\">\n");
    html.append(QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s64\"><Data ss:Type=\"String\">%2</Data></Cell>\n").arg(columnCount - 1).arg(title));
    html.append("   </Row>");

    //�������ֶ�����
    html.append("   <Row ss:AutoFitHeight=\"0\">\n");
    for (int i = 0; i < columnCount; i++) {
        html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + columnNames[i] + "</Data></Cell>\n");
    }
    html.append("   </Row>\n");

    //����������
    for (int i = 0; i < rowCount; i++) {
        html.append("   <Row ss:AutoFitHeight=\"0\">\n");
        QString temp = content[i];
        QStringList value = temp.split(";");
        foreach (QString str, value) {
            html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + str + "</Data></Cell>\n");
        }
        html.append("   </Row>\n");
    }
    html.append("  </Table>\n");

    //�̶���β��ʽ
    html.append("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
    html.append("   <PageSetup>\n");
    html.append("    <Header x:Margin=\"0.3\"/>\n");
    html.append("    <Footer x:Margin=\"0.3\"/>\n");
    html.append("    <PageMargins x:Bottom=\"0.75\" x:Left=\"0.7\" x:Right=\"0.7\" x:Top=\"0.75\"/>\n");
    html.append("   </PageSetup>\n");
    html.append("   <Unsynced/>\n");
    html.append("   <Selected/>\n");
    html.append("   <ProtectObjects>False</ProtectObjects>\n");
    html.append("   <ProtectScenarios>False</ProtectScenarios>\n");
    html.append("  </WorksheetOptions>\n");
    html.append(" </Worksheet>\n");
    html.append("</Workbook>\n");

    //д���ļ�
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QIODevice::Text)) {
		file.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
        QTextStream text(&file);
        text.setCodec("UTF-8");
        text << html.join("");

		return true;
    }

	return false;
}

bool ExcelHelper::ToExcel(QString fileName, const QHash<int, QStringList>& data)
{
	int rowCount = data.count();
	int columnCount = 6;


	//���ƴ��xml�ַ�,�ٱ���Ϊxls�ļ�
	QStringList html;

	//�̶�ͷ����Ϣ
	html.append("<?xml version=\"1.0\"?>\n");
	html.append("<?mso-application progid=\"Excel.Sheet\"?>\n");
	html.append("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
	html.append(" xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n");
	html.append(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n");
	html.append(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
	html.append(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n");

	//�ĵ���Ϣ
	html.append(" <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
	html.append(QString::fromLocal8Bit("  <LastAuthor>CompanyName</LastAuthor>\n"));
	html.append(QString("  <Created>%1</Created>\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
	html.append("  <Version>11.6360</Version>\n");
	html.append(" </DocumentProperties>\n");

	html.append(" <OfficeDocumentSettings xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
	html.append("  <AllowPNG/>\n");
	html.append(" </OfficeDocumentSettings>\n");

	html.append(" <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
	html.append("  <WindowHeight>9795</WindowHeight>\n");
	html.append("  <WindowWidth>21435</WindowWidth>\n");
	html.append("  <WindowTopX>120</WindowTopX>\n");
	html.append("  <WindowTopY>75</WindowTopY>\n");
	html.append("  <WindowTopY>75</WindowTopY>\n");
	html.append("  <ProtectWindows>False</ProtectWindows>\n");
	html.append(" </ExcelWorkbook>\n");

	//��ʽ��
	html.append(" <Styles>\n");
	html.append("  <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n");
	html.append("   <Alignment ss:Vertical=\"Center\"/>\n");
	html.append("   <Borders/>\n");
	html.append(QString::fromLocal8Bit("   <Font ss:FontName=\"����\" x:CharSet=\"134\" ss:Size=\"11\" ss:Color=\"#000000\"/>\n"));
	html.append("   <Interior/>\n");
	html.append("   <NumberFormat/>\n");
	html.append("   <Protection/>\n");
	html.append("  </Style>\n");
	//������ʽ
	html.append("  <Style ss:ID=\"s64\">\n");
	html.append("   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n");
	html.append(QString::fromLocal8Bit("   <Font ss:FontName=\"����\" x:CharSet=\"134\" ss:Size=\"14\" ss:Color=\"#000000\" ss:Bold=\"1\"/>\n"));
	html.append("  </Style>\n");
	//���߿���ʽ
	html.append("  <Style ss:ID=\"s91\">\n");
	html.append("  <Borders>\n");
	html.append("   <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
	html.append("   <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
	html.append("   <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
	html.append("   <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
	html.append("  </Borders>\n");
	html.append(" </Style>\n");

	html.append(" </Styles>\n");

	//����������
	html.append(QString(" <Worksheet ss:Name=\"%1\">\n").arg("Sheet1"));
	//int rowCount = content.count(); //����������

	//���ʼ
	html.append(QString("  <Table ss:ExpandedColumnCount=\"%1\" ss:ExpandedRowCount=\"%2\" x:FullColumns=\"1\"\n").arg(columnCount).arg(rowCount + 2));
	html.append("   x:FullRows=\"1\" ss:DefaultColumnWidth=\"54\" ss:DefaultRowHeight=\"18\">\n");

	//�����ֶο��
	for (int i = 0; i < columnCount; i++) {
		html.append(QString("   <Column ss:AutoFitWidth=\"0\" ss:Width=\"%1\"/>\n").arg(QString::number(150)));
	}

	//������
	//html.append("   <Row ss:AutoFitHeight=\"0\" ss:Height=\"26.25\">\n");
	//html.append(QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s64\"><Data ss:Type=\"String\">%2</Data></Cell>\n").arg(columnCount - 1).arg(QString::fromLocal8Bit("�ŵ�����")));
	//html.append("   </Row>");

	//�������ֶ�����
	html.append("   <Row ss:AutoFitHeight=\"0\">\n");
	//for (int i = 0; i < columnCount; i++) {
	//	html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + columnNames[i] + "</Data></Cell>\n");
	//}
	//<< QString::fromLocal8Bit("Ӫ������") << QString::fromLocal8Bit("��֯���") 
	//	<< QString::fromLocal8Bit("��֯����") << QString::fromLocal8Bit("�ŵ���") 
	//	<< QString::fromLocal8Bit("�ŵ�����") << QString::fromLocal8Bit("�Ƿ�װ");
	html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + QString::fromLocal8Bit("Ӫ������") + "</Data></Cell>\n");
	html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + QString::fromLocal8Bit("��֯���") + "</Data></Cell>\n");
	html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + QString::fromLocal8Bit("��֯����") + "</Data></Cell>\n");
	html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + QString::fromLocal8Bit("�ŵ���") + "</Data></Cell>\n");
	html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + QString::fromLocal8Bit("�ŵ�����") + "</Data></Cell>\n");
	html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + QString::fromLocal8Bit("�Ƿ�װ") + "</Data></Cell>\n");
	
	html.append("   </Row>\n");

	//����������
	//for (int i = 0; i < rowCount; i++) {
	//	html.append("   <Row ss:AutoFitHeight=\"0\">\n");
	//	QString temp = content[i];
	//	QStringList value = temp.split(";");
	//	foreach (QString str, value) {
	//		html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + str + "</Data></Cell>\n");
	//	}
	//	html.append("   </Row>\n");
	//}

	QHashIterator<int, QStringList> iter(data);
	while(iter.hasNext())
	{
		iter.next();
		html.append("   <Row ss:AutoFitHeight=\"0\">\n");
		for each(auto var in iter.value())
		{
			html.append("    <Cell ss:StyleID=\"s91\"><Data ss:Type=\"String\">" + var + "</Data></Cell>\n");
		}
		html.append("   </Row>\n");
	}

	html.append("  </Table>\n");

	//�̶���β��ʽ
	html.append("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
	html.append("   <PageSetup>\n");
	html.append("    <Header x:Margin=\"0.3\"/>\n");
	html.append("    <Footer x:Margin=\"0.3\"/>\n");
	html.append("    <PageMargins x:Bottom=\"0.75\" x:Left=\"0.7\" x:Right=\"0.7\" x:Top=\"0.75\"/>\n");
	html.append("   </PageSetup>\n");
	html.append("   <Unsynced/>\n");
	html.append("   <Selected/>\n");
	html.append("   <ProtectObjects>False</ProtectObjects>\n");
	html.append("   <ProtectScenarios>False</ProtectScenarios>\n");
	html.append("  </WorksheetOptions>\n");
	html.append(" </Worksheet>\n");
	html.append("</Workbook>\n");

	//д���ļ�
	FixedFileSuffix(fileName);
	QString dirPath = QFileInfo(fileName).absolutePath();
	QDir dir;
	if(!dir.mkpath(dirPath))
	{
		ServerLogger->AddLog("Ŀ¼[%s]����ʧ�ܣ��޷�����excel, ������[%d]", dirPath.toLocal8Bit().constData(), GetLastError());
		return false;
	}
	
	QFile file(fileName);
	if (file.open(QFile::WriteOnly | QIODevice::Text)) {
		file.setPermissions(QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther);
		QTextStream text(&file);
		text.setCodec("UTF-8");
		text << html.join("");

		return true;
	}

	ServerLogger->AddLog("�ļ�[%s]д��ʧ�ܣ��޷�����excel, ������[%d]", fileName.toLocal8Bit().constData(), GetLastError());
	return false;
}

//�����ļ���׺
void ExcelHelper::FixedFileSuffix(QString& fileName)
{
	if(fileName.count() >= 4 && 0 == fileName.right(4).compare(".xml", Qt::CaseInsensitive))
		return;

	QString ext = ".xml";
	int n = fileName.lastIndexOf(".");
	if (n >= 0)
	{
		fileName = fileName.mid(0, n);
	}

	fileName += ext;
}