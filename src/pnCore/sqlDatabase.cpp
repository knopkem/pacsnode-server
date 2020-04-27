#include "sqlDatabase.h"

#include <QtCore>
#include <QtSql>

namespace pacsnode{
ConnectionPool* SqlDatabase::sConnPool = NULL;

//------------------------------------------------------------------------------


    class ThreadConnection {
    public:
        ThreadConnection();
        ~ThreadConnection();
        QSqlDatabase database;
    };

    ThreadConnection::ThreadConnection()
    {
    }

    ThreadConnection::~ThreadConnection()
    {
        if ( database.isValid() ) {
            const QString connectionName = database.connectionName();
            //qWarning() << "removing" << connectionName;
            if ( database.isOpen() ) {
                database.close();
            }
            database = QSqlDatabase();  // Drop our reference to the DB
            QSqlDatabase::removeDatabase( connectionName );
        }
    }


//------------------------------------------------------------------------------

class ConnectionPool {
public:
    // QThreadStorage takes ownership of items assigned to it.
    QThreadStorage < ThreadConnection* > threadConnections;
};

//------------------------------------------------------------------------------

SqlDatabase::SqlDatabase(const QString& dbLocation, QObject* parent) : m_storageLocation(dbLocation), QObject(parent)
{
    if (sConnPool == NULL)
        sConnPool = new ConnectionPool;
}

//------------------------------------------------------------------------------

SqlDatabase::~SqlDatabase()
{
    closeConnection();
}

//------------------------------------------------------------------------------

QSqlDatabase SqlDatabase::openConnection() const
{
    //qWarning() << "open db for thread" << threadIdString();
    if ( !sConnPool->threadConnections.hasLocalData() ) {
        sConnPool->threadConnections.setLocalData( new ThreadConnection );
    }
    ThreadConnection * databaseConnInfo = sConnPool->threadConnections.localData();
    if ( databaseConnInfo->database.isValid() ) {
        return databaseConnInfo->database;
    }

    databaseConnInfo->database = const_cast<SqlDatabase*>(this)->createDatabaseConnection();

    return databaseConnInfo->database;
}

//------------------------------------------------------------------------------

void SqlDatabase::closeConnection() const
{
    if ( this->isConnected() )
    {
        sConnPool->threadConnections.localData()->database.connectionName();
        sConnPool->threadConnections.localData()->database.close();
    }
    sConnPool->threadConnections.setLocalData( NULL );
}

//------------------------------------------------------------------------------

bool SqlDatabase::isConnected() const
{
    return ( sConnPool->threadConnections.hasLocalData() &&
        sConnPool->threadConnections.localData()->database.isValid() );
}

//------------------------------------------------------------------------------

QString SqlDatabase::uniqueId()
{
    enum { Base_Ten = 10, Base_Hex = 16 };
#ifdef Q_OS_WIN
    // On windows the thread id is cast from GetCurrentThreadId.
    quint64 threadId = (quint64)(QThread::currentThreadId());
    return QString::number( threadId, Base_Ten ) ;
#else
    // Convert current thread pointer to hex.
    quint64 threadId = (quint64)(QThread::currentThread());
    return QString::number( threadId, Base_Hex ) ;
#endif
}

//------------------------------------------------------------------------------

QSqlDatabase SqlDatabase::createDatabaseConnection()
{
    QString connectionName = "pacsnode" + SqlDatabase::uniqueId();

    // assign ourselves using static method from QSqlDatabase
    QSqlDatabase database;

    database = QSqlDatabase::addDatabase("QSQLITE", connectionName );
    database.setDatabaseName(m_storageLocation + "/" + "db.sqlite");

    if (!database.open()) {
        qWarning() << "Cannot open database: Unable to establish a database connection.";
        return QSqlDatabase();
    }

    init(database);

    return database;
}

//------------------------------------------------------------------------------

void SqlDatabase::init(const QSqlDatabase& db)
{
    QSqlQuery query(db);

    if ( !query.exec( "PRAGMA page_size=4096" ) ) {
        qWarning() << "Could not set sqlite page size";
    }
    if ( !query.exec( "PRAGMA synchronous = OFF" ) ) {
        qWarning() << "Could not set sqlite synchronous mode";
    }
    if ( !query.exec( "PRAGMA journal_mode=wal" ) ) {
        qWarning() << "Could not set sqlite journal mode";
    }
    if ( !query.exec( "PRAGMA count_changes=OFF" ) ) {
        qWarning() << "Could not set sqlite count changes";
    }
    if ( !query.exec( "PRAGMA temp_store=MEMORY" ) ) {
        qWarning() << "Could not set sqlite tempory storage";
    }
    if ( !query.exec( "PRAGMA cache_size = 16384" ) ) {
        qWarning() << "Could not set sqlite cache size";
    }
    if ( !query.exec( "PRAGMA locking_mode = NORMAL" ) ) {
        qWarning() << "Could not set sqlite locking mode";
    }
    if ( !query.exec( "PRAGMA legacy_file_format = 0" ) ) {
        qWarning() << "Could not unset sqlite legacy file format";
    }
    if ( !query.exec( "PRAGMA foreign_keys = OFF" ) ) {
        qWarning() << "Could not disable foreign key support";
    }
    if ( !query.exec( "PRAGMA secure_delete = OFF" ) ) {
        qWarning() << "Could not disable secure deletion";
    }
}

//--------------------------------------------------------------------------------------------

bool SqlDatabase::execQuery( QSqlQuery & query, const char *file, int line )
{
    if ( query.exec() ) {
        return true;
    }
    else {
        qWarning() << file << "(" << line << ") :" << query.lastError();
        qWarning() << "    Query was :\n" << printQuery(query);
        return false;
    }
    return true;
}

//--------------------------------------------------------------------------------------------

QString SqlDatabase::printQuery( const QSqlQuery & query )
{
    QString ret = query.lastQuery();

    QStringList boundValues;
    QMapIterator<QString, QVariant> i(query.boundValues());
    while (i.hasNext()) {
        i.next();
        boundValues << QString("%1 = \"%2\"").arg(i.key()).arg(i.value().toString());
    }
    if ( boundValues.isEmpty() ) {
        ret += "\n[No bound values]";
    }
    else {
        ret += "\nBound values : " + boundValues.join(", ");
    }
    return ret;
}

//--------------------------------------------------------------------------------------------

void SqlDatabase::transaction() const
{
    QSqlQuery query(this->openConnection());
    query.prepare("BEGIN IMMEDIATE");
    query.exec();
}

//--------------------------------------------------------------------------------------------

void SqlDatabase::commit() const
{
    this->openConnection().commit();
}

//--------------------------------------------------------------------------------------------
}
