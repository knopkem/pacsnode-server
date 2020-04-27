#include "dicomNodeConfig.h"

#include "storage.h"
#include "sqlDatabase.h"

#include <QSqlQuery>

namespace pacsnode {


class DicomNodeConfigPrivate 
{
public:
    QList<DicomNode> nodes;
    SqlDatabase* sqlDb;
};

//--------------------------------------------------------------------------------------------


DicomNodeConfig::DicomNodeConfig() : d (new DicomNodeConfigPrivate)
{
    d->sqlDb = new SqlDatabase(pacsnode::storageLocation());
    fetchNodes();
}

//--------------------------------------------------------------------------------------------

DicomNodeConfig::~DicomNodeConfig()
{
    delete d->sqlDb;
    delete d;
    d = 0;
}

//--------------------------------------------------------------------------------------------

void DicomNodeConfig::clearSettings()
{
    QSqlQuery query(d->sqlDb->openConnection());
    query.exec("DELETE FROM setting");
}

//--------------------------------------------------------------------------------------------

void DicomNodeConfig::setHttpPort(int port)
{
    QSqlQuery query(d->sqlDb->openConnection());

    QString prepare = "INSERT INTO setting ( key, value) VALUES ( :key, :value)";
    query.prepare(prepare);

    query.bindValue(":key", "httpPort");
    query.bindValue(":value", QString::number(port));
    EXEC_QUERY(query);
}

//--------------------------------------------------------------------------------------------

int DicomNodeConfig::httpPort() const
{
    QSqlQuery query(d->sqlDb->openConnection());
    QString prepare = "SELECT value FROM setting WHERE key = :key";
    query.prepare(prepare);
    query.bindValue(":key", "httpPort");
    EXEC_QUERY(query);
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 8080;
}

//--------------------------------------------------------------------------------------------

void DicomNodeConfig::addNode( const DicomNode& node )
{
    if (this->contains(node)) {
        qWarning() << "node already stored" << node.title();
        return;
    }

    QSqlQuery query(d->sqlDb->openConnection());

    QString prepare = "INSERT INTO nodes ( title, address, port, fetchable) VALUES ( :title, :address, :port, :fetchable )";
    query.prepare(prepare);

    query.bindValue(":title", node.title());
    query.bindValue(":address", node.address());
    query.bindValue(":port", node.portAsString());
    query.bindValue(":fetchable", node.fetchable());
    EXEC_QUERY(query);

    fetchNodes();
}

//--------------------------------------------------------------------------------------------

QList<DicomNode> DicomNodeConfig::nodes() const
{
    return d->nodes;
}

//--------------------------------------------------------------------------------------------

bool DicomNodeConfig::contains( const DicomNode& node ) const
{
    return d->nodes.contains(node);
}

//--------------------------------------------------------------------------------------------

void DicomNodeConfig::fetchNodes()
{
    d->nodes.clear();
    QSqlQuery query(d->sqlDb->openConnection());
    QString prepare = "SELECT title, address, port, fetchable FROM nodes";
    query.prepare(prepare);
    EXEC_QUERY(query);
    while( query.next() ) {
        DicomNode node;
        node.setTitle(query.value(0).toString());
        node.setAddress(query.value(1).toString());
        node.setPortAsString(query.value(2).toString());
        node.setFetchable(query.value(3).toBool());
        if (!d->nodes.contains(node)) {
            d->nodes << node;
        }
    }
}

//--------------------------------------------------------------------------------------------

void DicomNodeConfig::setHostNode( const DicomNode& node )
{
    QSqlQuery query(d->sqlDb->openConnection());

    QString prepare = "INSERT INTO setting ( key, value) VALUES ( :key, :value)";
    query.prepare(prepare);

    query.bindValue(":key", "hostPort");
    query.bindValue(":value", QString::number(node.port()));
    EXEC_QUERY(query);
}

//--------------------------------------------------------------------------------------------

pacsnode::DicomNode DicomNodeConfig::hostNode() const
{
    int port = 6789;
    QSqlQuery query(d->sqlDb->openConnection());
    QString prepare = "SELECT value FROM setting WHERE key = :key";
    query.prepare(prepare);
    query.bindValue(":key", "hostPort");
    EXEC_QUERY(query);
    if (query.next()) {
        port = query.value(0).toInt();
    }

    return DicomNode("PACSNODE", "localhost", port, true);
}

//--------------------------------------------------------------------------------------------

void DicomNodeConfig::clear()
{
    d->nodes.clear();
    QSqlQuery query(d->sqlDb->openConnection());
    if (query.exec("DELETE FROM nodes") ) {
        qWarning() << "nodes table cleared";
    }
    else {
        qWarning() << "failed to clear nodes table";
    }
}

//--------------------------------------------------------------------------------------------

}
