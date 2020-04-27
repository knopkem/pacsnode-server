#include "workDispatcher.h"

#include "sqlDatabase.h"
#include "imageWorker.h"
#include "storage.h"

#include <QtSql>

namespace pacsnode {

class WorkDispatcherPrivate {
public:
    QThread* workerThread;
    DispatchWorker* worker;
};

WorkDispatcher::WorkDispatcher(QObject* parent /*= NULL*/) 
    : QObject(parent), d(new WorkDispatcherPrivate)
{
    d->workerThread = NULL;
    d->worker = new DispatchWorker();
}

WorkDispatcher::~WorkDispatcher()
{
    if (d->workerThread ) {
        if (d->workerThread->isRunning()) {
            d->workerThread->quit();
            d->workerThread->wait(10000);
        }
    delete d->workerThread;
    }
    delete d->worker;
    delete d;
    d = 0;
}

void WorkDispatcher::start()
{
    if (!d->workerThread) {
        d->workerThread = new QThread;
        d->worker->moveToThread(d->workerThread);
        connect(d->workerThread, SIGNAL(started()), d->worker, SLOT(start()));
    }

    if (!d->workerThread->isRunning()) {
        d->workerThread->start();
    }
}

void WorkDispatcher::stop()
{
    d->worker->stop();
    QThreadPool::globalInstance()->waitForDone();
}

DispatchWorker::DispatchWorker(QObject* parent ) : QObject(parent)
{
    m_sqlDb = new SqlDatabase(pacsnode::storageLocation());
    m_cancel = false;
}

DispatchWorker::~DispatchWorker()
{
    delete m_sqlDb;
}

void DispatchWorker::findWork()
{
    // search image table for unprocessed files
    QSqlQuery query(m_sqlDb->openConnection());
    QString prepare = "SELECT PrivateCreator, id FROM image WHERE processed IS NULL group by id";

    // print warning
    if ( !query.prepare(prepare) ) {
        qWarning() << prepare;
        qCritical() << query.lastError();
    }

    // query exec
    EXEC_QUERY(query);

    // create qRunnables for each file
    while( query.next() ) {
        qlonglong id = query.value(1).toLongLong();
        QString filePath = pacsnode::storageLocation() + query.value(0).toString(); // combine to get the true path
        this->markImagePending(id);
        ImageWorker* worker = new ImageWorker(filePath, id);
        QThreadPool *threadPool = QThreadPool::globalInstance();
        threadPool->start(worker);
    }
    if (!m_cancel) {
        QTimer::singleShot(150, this, SLOT(findWork()));
    }
    else {
        emit finished();
    }
}

void DispatchWorker::stop()
{
    // qDebug() << "stopping dispatcher";
    m_cancel = true;
}

void DispatchWorker::start()
{
    // qDebug() << "starting dispatcher";
    m_cancel = false;
    findWork();
}

void DispatchWorker::markImagePending( qlonglong id )
{
    // query prepare
    QSqlQuery query(m_sqlDb->openConnection());
    QString prepare = "UPDATE image SET processed = 'pending' WHERE id = " + QString::number(id);

    // print warning
    if (!query.prepare(prepare)) {
        qWarning() << prepare;
        qWarning() << query.lastError();
    }

    // query exec
    EXEC_QUERY(query);
}

}
