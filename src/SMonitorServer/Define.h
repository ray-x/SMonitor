#pragma once
#include <QString>
#include <QMap>
#include <QVector>
#include "CNetMessage.h"

const int MaxDataMotionWriteToDataBase = 500; 

#define MAXMESGCOUNT (5)

typedef struct _ClientInfo
{
	QString id;           //门店编号 
	QString version;      //客户端版本号
}ClientInfo, *PClientInfo;

typedef struct _SoftwareInfo
{
	QString name;
	QString version;
	QString url;
	QString brand;
	QString category;
	QString location;
	QString mainExe; 
	unsigned long installNum;
	unsigned long uninstalledNum;
	int     packType;

	_SoftwareInfo()
	{
		installNum = 0;
		uninstalledNum = 0;
		packType = 0; //此处作为保留字段供其他需求使用
	}

	bool compare(const _SoftwareInfo &software)
	{
		return (this->name == software.name) &&
			(this->version == software.version) &&
			(this->url == software.url) &&
			(this->brand == software.brand) &&
			(this->category == software.category) &&
			(this->location == software.location) &&
			(this->mainExe == software.mainExe);
	}

}SoftwareInfo, *PSoftwareInfo;

typedef QVector<SoftwareInfo> SoftwareinfoVector;
typedef QMap<QString, SoftwareInfo> SoftwareinfoMap;

typedef struct _SoftwareInstallInfo
{
	QString name;
	QString version;
	unsigned long installNum;
	unsigned long uninstalledNum;
	unsigned long addInstallNum;
	unsigned long addUninstalledNum;

	_SoftwareInstallInfo()
	{
		installNum        = 0;
		uninstalledNum    = 0;
		uninstalledNum    = 0;
		addUninstalledNum = 0;
	}

	void add()
	{
		installNum += addInstallNum;
		uninstalledNum += addUninstalledNum;
	}

	void clearAdd()
	{
		addInstallNum = addUninstalledNum = 0;
	}

}SoftwareInstallInfo, *PSoftwareInstallInfo;

typedef QMap<QString, SoftwareInstallInfo> SoftwareInstallInfoMap;

typedef struct _DataMotion
{
	MsgHead            msg;
	SoftwareInfo       softwareInfo;
	SoftwareinfoVector softwareInfoVector;
	ClientInfo         clientInfo;
	QMap<QString, QString> userInfoMap;
}DataMotion, *PDataMotion;

typedef QVector<DataMotion> DataMotionVector;

typedef QMap<QString, QString> HelpInfoMap;

typedef QMap<QDateTime, QString> MessageMap;

