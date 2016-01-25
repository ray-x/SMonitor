#ifndef CMESSAGEMEDIATOR_H
#define CMESSAGEMEDIATOR_H

#include <QObject>
class CMessageEventMediator : public QObject
{
	Q_OBJECT

public:
	CMessageEventMediator(QObject *parent = 0);
	~CMessageEventMediator();

Q_SIGNALS:
	void sigLoginIn();
};



#endif // CMESSAGEMEDIATOR_H
