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
#define INDEPENDENT "�ͻ���"

//ɾ���ǿ�ָ���
#define DELETE_POINTER(ptr)		\
	if(ptr != nullptr){			\
		delete ptr;				\
		ptr = nullptr;			\
	}

//���״̬ö��
namespace SMonitor{
	enum Type {
		UnInstall,		//δ��װ
		Normal,			//�Ѱ�װ
		Downloading,	//��������
		Downloaded,		//������
		Installing,		//���ڰ�װ
		Upgrade			//����
	};
}

//����һ�������ص���Ϣ�ṹ
typedef struct _SoftData{

	QString		name;			//�������
	QString		version;		//����汾��
	QString		url;			//������°汾��ȡ��ַ
	QString		brand;			//���Ʒ��
	QString		category;		//�������
	QString		location;		//����ڿͻ��˴��ڵ��ļ���
	QString     mainExe;        //��EXE
	int			state;			//0:δ��װ 1:�Ѱ�װ 2:�������أ�3:������ 4:���ڰ�װ 5:����

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
	QString			content;//��Ϣ����
	QDateTime		datetime;//��Ϣ������ʱ��
}Message;


//���ڱ�ʾһ����������
typedef struct _Task
{
	QString		key;		//�����ʶ
	QString		srcPath;	//��Դ·��
	QString		desPath;	//Ŀ��·��
	QString     version;    //����汾��
	QString     category;   //���
	QString		tempPath;	//�ļ����ر���·��
	QString     mainExe;    //��EXE
	int			state;		//0:δ��װ 1:�Ѱ�װ 2:�������أ�3:������ 4:���ڰ�װ 5:����
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