#ifndef SqlDatabase_h__
#define SqlDatabase_h__

#include "pnCore_Export.h"

#include <QObject>
#include <QSqlDatabase>


namespace pacsnode {

class ConnectionPool;

#define EXEC_QUERY(q) SqlDatabase::execQuery(q, __FILE__ , __LINE__ )

//! Basic interfaces to Sql databases using QSqlDatabase, allowing one connection
//  per thread.
class pnCore_EXPORT SqlDatabase : public QObject
{
    Q_OBJECT
public:

    SqlDatabase(const QString& dbLocation, QObject* parent = NULL);
    virtual ~SqlDatabase();

    //! Get the database connection for the current thread, opening a new one if necessary.
    //  This function is thread-safe.
    QSqlDatabase openConnection() const;

    //! Close the database connection for the current thread, if it is open.
    //  If the current thread has no connection, nothing is done.
    void closeConnection() const;

    //! perform query
    static bool execQuery(QSqlQuery & query, const char *file, int line);

    //! begin transaction
    void transaction() const;

    //! commit changes, (end transaction)
    void commit() const;

protected:

    static QString printQuery( const QSqlQuery & query);

private:
    //! Called whenever we need to open a new connection
    QSqlDatabase createDatabaseConnection();

    //! Return a string id unique for this thread.
    static QString uniqueId();

    void init(const QSqlDatabase& db);

    //! Return true if the current thread has an open connection to this database.
    bool isConnected() const;

    // this has to be static, to log all connections between threads
    static ConnectionPool* sConnPool;

    // not implemented!, prevent usage
    SqlDatabase & operator= ( const SqlDatabase & );
    SqlDatabase ( const SqlDatabase & );

    // db storage location
    QString m_storageLocation;
};
#endif // SqlDatabase_h__

}
