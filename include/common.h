#pragma once

#ifndef NOMINMAX                       
#  define NOMINMAX
#endif

#include <QUrl>
#include <QString>
#include <QDateTime>
#include <QApplication>

#define VERSIONFILE "SVersionFile.txt"
#define HKCU ("HKEY_CURRENT_USER\\Software\\SMonitor")
#define localSoftFile "/configure.cof"
#define MAXMSGSHOWCOUNT (100)
#define INDEPENDENT "客户端"

//删除非空指针宏
#define DELETE_POINTER(ptr)		\
	if(ptr != nullptr){			\
		delete ptr;				\
		ptr = nullptr;			\
	}

//软件状态枚举
namespace SMonitor{
	enum Type {
		UnInstall,		//未安装
		Normal,			//已安装
		Downloading,	//正在下载
		Downloaded,		//已下载
		Installing,		//正在安装
		Upgrade			//更新
	};
}

//定义一个软件相关的信息结构
typedef struct _SoftData{

	QString		name;			//软件名称
	QString		version;		//软件版本号
	QString		url;			//软件最新版本获取地址
	QString		brand;			//软件品牌
	QString		category;		//软件类型
	QString		location;		//软件在客户端存在的文件夹
	QString     mainExe;        //主EXE
	int			state;			//0:未安装 1:已安装 2:正在下载，3:已下载 4:正在安装 5:更新

	_SoftData()
	{
		name = version = brand = category = location = mainExe = url = "";
		state = 0;
	}

	bool operator==(const _SoftData& right) const
	{
		return		(name == right.name)  
				&&	(version == right.version) 
				&&	(url == right.url) 
				&&	(brand == right.brand) 
				&&	(category == right.category)
				&&	(location == right.location)
				&&  (mainExe == right.mainExe);
	}

}SoftData;

typedef struct _Message{
	QString			content;//消息内容
	QDateTime		datetime;//消息产生的时间
}Message;


//用于表示一个下载任务
typedef struct _Task
{
	QString		key;		//软件标识
	QString		srcPath;	//资源路径
	QString		desPath;	//目标路径
	QString     version;    //软件版本号
	QString     category;   //类别
	QString		tempPath;	//文件下载保存路径
	QString     mainExe;    //主EXE
	int			state;		//0:未安装 1:已安装 2:正在下载，3:已下载 4:正在安装 5:更新
	_Task()
	{
		key = "";
		srcPath = "";
		desPath = "";
		version = "";
		category = "";
		tempPath = QApplication::applicationDirPath() + "/download";
		mainExe = "";
		state   = 0;
	}
}Task;