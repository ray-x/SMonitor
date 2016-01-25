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
	//检测是否为自我升级
	if(selfUpdate())
		return;

	QString fileName = QFileInfo(m_pCurTask.srcPath).fileName();
	QString exePath = m_pCurTask.tempPath + "/" + fileName;
	if(-1 != m_pCurTask.category.indexOf(QString::fromLocal8Bit("驱动")))
	{//驱动
		ClientLogger->AddLog(QString::fromLocal8Bit("安装驱动程序[%1] EXE[%2]").arg(m_pCurTask.key).arg(exePath));
		if(CMisc::ShellExcuteExe(exePath, false, -1)) //弹出窗口让用户选择，此处无限等待
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
		ClientLogger->AddLog(QString::fromLocal8Bit("安装程序[%1] EXE[%2]").arg(m_pCurTask.key).arg(m_pCurTask.srcPath));
		if(CMisc::ShellExcuteExe(m_pCurTask.tempPath + "/" + fileName, false, -1)) //最多等待20分钟
		{
			//解压命令虽然发出了，但是不一定安装成功，检测一下目标文件夹是否存在
			if(!m_pCurTask.mainExe.isEmpty())
			{
				if(!CMisc::IsFileExist(m_pCurTask.desPath + "\\" + m_pCurTask.mainExe))
				{
					ClientLogger->AddLog(QString::fromLocal8Bit("主EXE[%1]不存在 安装程序[%2]失败 目录[%3]").arg(m_pCurTask.desPath + "\\" + m_pCurTask.mainExe).arg(m_pCurTask.key).arg(m_pCurTask.desPath));
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

	ClientLogger->AddLog(QString::fromLocal8Bit("安装程序[%1] 路径[%2]").arg(m_pCurTask.key).arg(m_pCurTask.desPath));

	//1.强制关闭其所有的进程,进程不一定被关闭，如360
	//查找路径下所有进程
	if(CMisc::IsFileExist(m_pCurTask.desPath))
	{
		QStringList strExeList;
		if(CMisc::FindExe(m_pCurTask.desPath, strExeList) && strExeList.count() > 0)
		{
			CMisc::KillAllProcessesByTaskKiller(strExeList);
		}
	}

	//2.为完整安装包，需要清空目录 清空目录比较危险，因此需要检测是否为安全的目录
	if(isSafeDirectory(m_pCurTask.desPath))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("安装程序[%1] 清空目录[%2]").arg(m_pCurTask.key).arg(m_pCurTask.desPath));
		CMisc::MiscDeleteFile(m_pCurTask.desPath);
// 		if(!CMisc::MiscDeleteFile(m_pCurTask.desPath))
// 		{
// 			ClientLogger->AddLog(QString::fromLocal8Bit("安装程序[%1] 路径[%2] 时，清空目录失败 错误码[%3]").arg(m_pCurTask.key).arg(m_pCurTask.desPath).arg(CMisc::GetMiscLastError()));
// 			emit decompressError(m_pCurTask.key, false);
// 			return;
// 		}
	}
	
	//3.解压至当前目录下
	QString path = m_pCurTask.desPath;
	QDir dir;
	if(!dir.mkpath(path))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("安装程序[%1] 路径[%2] 时，创建目录失败").arg(m_pCurTask.key).arg(m_pCurTask.desPath));
		emit decompressError(m_pCurTask.key, false);
	}

	long dwRet = CMisc::DecompressFiles(exePath, path);
	if(0 == dwRet)
	{
		//等待1秒
		msleep(1000);
		ClientLogger->AddLog(QString::fromLocal8Bit("程序[%1] 解压成功").arg(m_pCurTask.key));

		//解压命令虽然发出了，但是不一定安装成功，检测一下目标文件夹是否存在
// 		if(!m_pCurTask.mainExe.isEmpty())
// 		{
// 			if(!CMisc::IsFileExist(path + "\\" + m_pCurTask.mainExe))
// 			{
// 				ClientLogger->AddLog(QString::fromLocal8Bit("主EXE[%1]不存在 安装程序[%2]失败 目录[%3]").arg(path + "\\" + m_pCurTask.mainExe).arg(m_pCurTask.key).arg(m_pCurTask.desPath));
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
				 ClientLogger->AddLog(QString::fromLocal8Bit("安装程序[%1] 文件[%2]暂时未检测到").arg(m_pCurTask.key).arg(path + "\\" + m_pCurTask.mainExe));
			 }
		}

		//将快捷方式放到桌面
		if(!m_pCurTask.mainExe.isEmpty())
		{
			CMisc::LinkToDesktop(path + "\\" + m_pCurTask.mainExe, m_pCurTask.key);
		}

		emit decompressFinished(m_pCurTask.key);
	}
	else
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("安装程序[%1] 调用DecompressFiles 失败 源路径[%2] 目标路径[%3]").arg(m_pCurTask.tempPath + "/" + fileName).arg(path).arg(m_pCurTask.key));
		//CMisc::DeleteDirectory(m_pCurTask.desPath);
		
		if(dwRet == 5)
		{
			QString strText = QString::fromLocal8Bit("安装[%1]失败，安装时部分文件被阻止，请关闭360安全卫视，QQ管家，金山安全卫视等相关软件后再试！").arg(m_pCurTask.key);
			AfxMessageBox(strText.toLocal8Bit().constData());
		}

		bool bDeletePack = false;
		//解压命令虽然发出了，但是不一定安装成功，检测一下目标文件夹是否存在
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
		ClientLogger->AddLog(QString::fromLocal8Bit("目录下的升级程序smsetup.exe不存在, 无法更新！"));
		emit decompressError(m_pCurTask.key, false);
		return true;
	}

	if(!CMisc::ShellExcuteExe(strUpdateExe, false, 0, false))
	{
		ClientLogger->AddLog(QString::fromLocal8Bit("启动程序smsetup.exe失败, 请稍后再试！"));
		emit decompressError(m_pCurTask.key, false);
		return true;
	}

	qApp->quit();
	ClientLogger->AddLog(QString::fromLocal8Bit("开始软件更新"));
	return true;
}

bool CDecompressThread::isSafeDirectory(const QString& strPath)
{
	return CMisc::isSafeDirectory(strPath);
}