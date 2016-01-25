#include "CDecompressThread.h"
#include "CMisc.h"
#include <afxwin.h>
#include <QFileInfo>
#include <QDebug>
#include "QDir"
#include "CClientLog.h"
#include "CSoftManager.h"

CDecompressThread::CDecompressThread(QObject *parent)
	: QThread(parent)
{
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

CDecompressThread::~CDecompressThread()
{
	qDebug() << "~CDecompressThread()";
	if(isRunning())
	{
		terminate();
	}
}

void CDecompressThread::Start(const Task& task)
{
	m_pCurTask = task;
	start();
}

void CDecompressThread::run()
{
	//����Ƿ�Ϊ��������
	if(selfUpdate())
		return;

	QString fileName = QFileInfo(m_pCurTask.srcPath).fileName();
	QString exePath = m_pCurTask.tempPath + "/" + fileName;
	if(-1 != m_pCurTask.category.indexOf(QString::fromLocal8Bit("����")))
	{//����
		ClientLogger->AddLog(QString::fromLocal8Bit("��װ��������[%1] EXE[%2]").arg(m_pCurTask.key).arg(exePath));
		if(CMisc::ShellExcuteExe(exePath, false, -1)) //�����������û�ѡ�񣬴˴����޵ȴ�
		{
			if(CMisc::IsAppInstall(m_pCurTask.key))
				emit decompressFinished(m_pCurTask.key);
			else
				emit decompressError(m_pCurTask.key, false);
		}
		else
		{
			emit decompressError(m_pCurTask.key, true);
		}
		return;
	}
	else if(exePath.length() >= 4 && exePath.right(4) == ".exe")
	{//exe
		ClientLogger->AddLog(QString::fromLocal8Bit("��װ����[%1] EXE[%2]").arg(m_pCurTask.key).arg(m_pCurTask.srcPath));
		if(CMisc::ShellExcuteExe(m_pCurTask.tempPath + "/" + fileName, false, -1)) //���ȴ�20����
		{
			//��ѹ������Ȼ�����ˣ����ǲ�һ����װ�ɹ������һ��Ŀ���ļ����Ƿ����
			if(!m_pCurTask.mainExe.isEmpty())
			{
				if(!CMisc::IsFileExist(m_pCurTask.desPath + "\\" + m_pCurTask.mainExe))
				{
					ClientLogger->AddLog(QString::fromLocal8Bit("��EXE[%1]������ ��װ����[%2]ʧ�� Ŀ¼[%3]").arg(m_pCurTask.desPath + "\\" + m_pCurTask.mainExe).arg(m_pCurTask.key).arg(m_pCurTask.desPath));
					//CMisc::DeleteDirectory(m_pCurTask.desPath);
					emit decompressError(m_pCurTask.key, false);
					return;
				}
			}
			else
			{
				if(!CMisc::IsFileExist(m_pCurTask.desPath))
				{
					emit decompressError(m_pCurTask.key, false);
					return;
				}
			}

			emit decompressFinished(m_pCurTask.key);
		}
		else
		{
			emit decompressError(m_pCurTask.key, true);
		}
		return;
	}

	ClientLogger->AddLog(QString::fromLocal8Bit("��װ����[%1] ·��[%2]").arg(m_pCurTask.key).arg(m_pCurTask.desPath));

	//1.ǿ�ƹر������еĽ���,���̲�һ�����رգ���360
	//����·�������н���
	if(CMisc::IsFileExist(m_pCurTask.desPath))
	{
		QStringList strExeList;
		if(CMisc::FindExe(m_pCurTask.desPath, strExeList) && strExeList.count() > 0)
		{
			CMisc::KillAllProcessesByTaskKiller(strExeList);
		}
	}

	//2.Ϊ������װ������Ҫ���Ŀ¼ ���Ŀ¼�Ƚ�Σ�գ������Ҫ����Ƿ�Ϊ��ȫ��Ŀ¼
	if(isSafeDirectory(m_pCurTask.desPath))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("��װ����[%1] ���Ŀ¼[%2]").arg(m_pCurTask.key).arg(m_pCurTask.desPath));
		CMisc::MiscDeleteFile(m_pCurTask.desPath);
// 		if(!CMisc::MiscDeleteFile(m_pCurTask.desPath))
// 		{
// 			ClientLogger->AddLog(QString::fromLocal8Bit("��װ����[%1] ·��[%2] ʱ�����Ŀ¼ʧ�� ������[%3]").arg(m_pCurTask.key).arg(m_pCurTask.desPath).arg(CMisc::GetMiscLastError()));
// 			emit decompressError(m_pCurTask.key, false);
// 			return;
// 		}
	}
	
	//3.��ѹ����ǰĿ¼��
	QString path = m_pCurTask.desPath;
	QDir dir;
	if(!dir.mkpath(path))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("��װ����[%1] ·��[%2] ʱ������Ŀ¼ʧ��").arg(m_pCurTask.key).arg(m_pCurTask.desPath));
		emit decompressError(m_pCurTask.key, false);
	}

	long dwRet = CMisc::DecompressFiles(exePath, path);
	if(0 == dwRet)
	{
		//�ȴ�1��
		msleep(1000);
		ClientLogger->AddLog(QString::fromLocal8Bit("����[%1] ��ѹ�ɹ�").arg(m_pCurTask.key));

		//��ѹ������Ȼ�����ˣ����ǲ�һ����װ�ɹ������һ��Ŀ���ļ����Ƿ����
// 		if(!m_pCurTask.mainExe.isEmpty())
// 		{
// 			if(!CMisc::IsFileExist(path + "\\" + m_pCurTask.mainExe))
// 			{
// 				ClientLogger->AddLog(QString::fromLocal8Bit("��EXE[%1]������ ��װ����[%2]ʧ�� Ŀ¼[%3]").arg(path + "\\" + m_pCurTask.mainExe).arg(m_pCurTask.key).arg(m_pCurTask.desPath));
// 				//CMisc::DeleteDirectory(m_pCurTask.desPath);
// 				emit decompressError(m_pCurTask.key, true);
// 				return;
// 			}
// 		}
// 		else
// 		{
// 			if(!CMisc::IsFileExist(path))
// 			{
// 				emit decompressError(m_pCurTask.key, true);
// 				return;
// 			}
// 		}

		if(!m_pCurTask.mainExe.isEmpty())
		{
			 if(!CMisc::IsFileExist(path + "\\" + m_pCurTask.mainExe))
			 {
				 ClientLogger->AddLog(QString::fromLocal8Bit("��װ����[%1] �ļ�[%2]��ʱδ��⵽").arg(m_pCurTask.key).arg(path + "\\" + m_pCurTask.mainExe));
			 }
		}

		//����ݷ�ʽ�ŵ�����
		if(!m_pCurTask.mainExe.isEmpty())
		{
			CMisc::LinkToDesktop(path + "\\" + m_pCurTask.mainExe, m_pCurTask.key);
		}

		emit decompressFinished(m_pCurTask.key);
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("��װ����[%1] ����DecompressFiles ʧ�� Դ·��[%2] Ŀ��·��[%3]").arg(m_pCurTask.tempPath + "/" + fileName).arg(path).arg(m_pCurTask.key));
		//CMisc::DeleteDirectory(m_pCurTask.desPath);
		
		if(dwRet == 5)
		{
			QString strText = QString::fromLocal8Bit("��װ[%1]ʧ�ܣ���װʱ�����ļ�����ֹ����ر�360��ȫ���ӣ�QQ�ܼң���ɽ��ȫ���ӵ������������ԣ�").arg(m_pCurTask.key);
			AfxMessageBox(strText.toLocal8Bit().constData());
		}

		bool bDeletePack = false;
		//��ѹ������Ȼ�����ˣ����ǲ�һ����װ�ɹ������һ��Ŀ���ļ����Ƿ����
		if(!m_pCurTask.mainExe.isEmpty())
		{
			if(!CMisc::IsFileExist(path + "\\" + m_pCurTask.mainExe))
			{
				bDeletePack = true;
			}
		}
		else
		{
			if(!CMisc::IsFileExist(path))
			{
				bDeletePack = true;
			}
		}
		
		emit decompressError(m_pCurTask.key, bDeletePack);
	}

	//quit();
}

bool CDecompressThread::selfUpdate()
{
	if(m_pCurTask.category != INDEPENDENT)
		return false;

	QString strUpdateExe = QApplication::applicationDirPath() + "\\smsetup.exe";
	if(!CMisc::IsFileExist(strUpdateExe))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("Ŀ¼�µ���������smsetup.exe������, �޷����£�"));
		emit decompressError(m_pCurTask.key, false);
		return true;
	}

	if(!CMisc::ShellExcuteExe(strUpdateExe, false, 0, false))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("��������smsetup.exeʧ��, ���Ժ����ԣ�"));
		emit decompressError(m_pCurTask.key, false);
		return true;
	}

	qApp->quit();
	ClientLogger->AddLog(QString::fromLocal8Bit("��ʼ�������"));
	return true;
}

bool CDecompressThread::isSafeDirectory(const QString& strPath)
{
	return CMisc::isSafeDirectory(strPath);
}