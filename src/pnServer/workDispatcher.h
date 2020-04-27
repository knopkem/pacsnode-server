#ifndef workDispatcher_h__
#define workDispatcher_h__

#include <QObject>

namespace pacsnode {

class SqlDatabase;

class DispatchWorker : public QObject
{
    Q_OBJECT
public:
    DispatchWorker(QObject* parent = NULL);
    ~DispatchWorker();

public Q_SLOTS:

    void start();

    void stop();

    void findWork();

signals:
    void finished();

protected:
    void markImagePending( qlonglong id);

private:
    SqlDatabase* m_sqlDb;
    bool m_cancel;
};

class WorkDispatcherPrivate;
class WorkDispatcher  : public QObject
{
    Q_OBJECT
public:
    WorkDispatcher(QObject* parent = NULL);
    ~WorkDispatcher();

public Q_SLOTS:

    void start();

    void stop();

private:
    WorkDispatcherPrivate* d;
};

}

#endif // workDispatcher_h__
