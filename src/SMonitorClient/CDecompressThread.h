#ifndef CDECOMPRESSTHREAD_H
#define CDECOMPRESSTHREAD_H

#include <QThread>
#include "common.h"

class CDecompressThread : public QThread
{
	Q_OBJECT

public:
	CDecompressThread(QObject *parent = 0);
	~CDecompressThread();

	virtual void run();

	void Start(const Task& task);

Q_SIGNALS:
	void decompressStarted(const QString& key);
	void decompressFinished(const QString& key);
	void decompressError(const QString& key, bool bDeleteFile);
	void quitAppliction();

protected:

	bool selfUpdate();
	bool isSafeDirectory(const QString& strPath);

private:
	Task m_pCurTask;
};

#endif // CDECOMPRESSTHREAD_H
