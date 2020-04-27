#include "tableCreator.h"

#include <QtSql>

#include "storage.h"
#include "sqlDatabase.h"

namespace pacsnode {

class TableCreatorPrivate
{
public:
    QList<TableElement> findList;
    SqlDatabase* db;
};

//--------------------------------------------------------------------------------------------

TableCreator::TableCreator( const QList<TableElement>& findList) : d (new TableCreatorPrivate)
{
    d->findList = findList;
    d->db = new SqlDatabase(pacsnode::storageLocation());
}

//--------------------------------------------------------------------------------------------

TableCreator::~TableCreator()
{
    delete d->db;
    delete d;
    d = NULL;
}

//--------------------------------------------------------------------------------------------

void TableCreator::init()
{
    this->createPatientTable();
    this->createStudyTable();
    this->createSeriesTable();
    this->createImageTable();
    this->createNodesTable();
    this->createSettingTable();
}

//--------------------------------------------------------------------------------------------

bool TableCreator::createPatientTable()
{
    QSqlQuery query(d->db->openConnection());
    QStringList list;

    list << "CREATE TABLE IF NOT EXISTS patient (";
    list << " id                    INTEGER      PRIMARY KEY AUTOINCREMENT,";
    list << " referenceId           INTEGER,"; // UNUSED FOREIGN KEY


    foreach(const TableElement& attr, d->findList) {
        if (attr.level == PATIENT_LEVEL) {
            QString terminator = ",";
            if (attr.keyAttr == UNIQUE_KEY)
                terminator = "UNIQUE,";
            list << attr.tagName + " TEXT " + terminator;
        }
    }
    list << " PlaceHoler TEXT";
    list << ");";

    if (!query.exec(list.join(""))) {
        qWarning() << query.lastError() << " at " << __FILE__ << __LINE__;
        return false;
    }
    return true;
}

//--------------------------------------------------------------------------------------------

bool TableCreator::createStudyTable()
{
    QSqlQuery query(d->db->openConnection());
    QStringList list;

    list << "CREATE TABLE IF NOT EXISTS study (";
    list << " id                    INTEGER      PRIMARY KEY AUTOINCREMENT,";
    list << " referenceId           INTEGER      REFERENCES patient,"; // FOREIGN KEY

    foreach(const TableElement& attr, d->findList) {
        if (attr.level == STUDY_LEVEL) {
            QString terminator = ",";
            if (attr.keyAttr == UNIQUE_KEY)
                terminator = "UNIQUE,";
            list << attr.tagName + " TEXT " + terminator;
        }
    }
    list << " PlaceHoler TEXT";
    list << ");";

    if (!query.exec(list.join(""))) {
        qWarning() << query.lastError() << " at " << __FILE__ << __LINE__;
        return false;
    }

    if (!query.exec("CREATE INDEX IF NOT EXISTS studyIndex ON study(referenceId);")) {
        qWarning() << "ERROR while creating SQL index: " << query.lastError() << " at " << __FILE__ << __LINE__;
        return false;
    }
    return true;
}

//--------------------------------------------------------------------------------------------

bool TableCreator::createSeriesTable()
{
    QSqlQuery query(d->db->openConnection());
    QStringList list;

    list << "CREATE TABLE IF NOT EXISTS series (";
    list << " id                    INTEGER      PRIMARY KEY AUTOINCREMENT,";
    list << " referenceId           INTEGER      REFERENCES study,"; // FOREIGN KEY

    foreach(const TableElement& attr, d->findList) {
        if (attr.level == SERIES_LEVEL) {
            QString terminator = ",";
            if (attr.keyAttr == UNIQUE_KEY)
                terminator = "UNIQUE,";
            list << attr.tagName + " TEXT " + terminator;
        }
    }
    list << " PlaceHoler TEXT";
    list << ");";

    if (!query.exec(list.join(""))) {
        qWarning() << query.lastError() << " at " << __FILE__ << __LINE__;
        return false;
    }

    if (!query.exec("CREATE INDEX IF NOT EXISTS seriesIndex ON series(referenceId);")) {
        qWarning() << "ERROR while creating SQL index: " << query.lastError() << " at " << __FILE__ << __LINE__ ;
        return false;
    }
    return true;
}

//--------------------------------------------------------------------------------------------

bool TableCreator::createImageTable()
{
    QSqlQuery query(d->db->openConnection());
    QStringList list;
    list << "CREATE TABLE IF NOT EXISTS image (";
    list << " id                    INTEGER      PRIMARY KEY AUTOINCREMENT,";
    list << " referenceId           INTEGER      REFERENCES patient,"; // FOREIGN KEY
    foreach(const TableElement& attr, d->findList) {
        if (attr.level == IMAGE_LEVEL) {
            QString terminator = ",";
            if (attr.keyAttr == UNIQUE_KEY)
                terminator = "UNIQUE,";
            list << attr.tagName + " TEXT " + terminator;
        }
    }
    list << " processed TEXT";
    list << ");";

    if (!query.exec(list.join(""))) {
        qWarning() << query.lastError() << " at " << __FILE__ << __LINE__;
        return false;
    }

    if (!query.exec("CREATE INDEX IF NOT EXISTS imageIndex ON image(referenceId);")) {
        qWarning() << "ERROR while creating SQL index: " << query.lastError() << " at " << __FILE__ << __LINE__;
        return false;
    }
    return true;
}

//--------------------------------------------------------------------------------------------

bool TableCreator::createNodesTable()
{
    QSqlQuery query(d->db->openConnection());
    QStringList list;
    list << "CREATE TABLE IF NOT EXISTS nodes (";
    list << " id                    INTEGER      PRIMARY KEY AUTOINCREMENT,";
    list << " title     TEXT,";
    list << " address   TEXT,";
    list << " port      TEXT,";
    list << " fetchable TEXT";
    list << ");";

    if (!query.exec(list.join(""))) {
        qWarning() << query.lastError() << " at " << __FILE__ << __LINE__;
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------

bool TableCreator::createSettingTable()
{
    QSqlQuery query(d->db->openConnection());
    QStringList list;
    list << "CREATE TABLE IF NOT EXISTS setting (";
    list << " id                    INTEGER      PRIMARY KEY AUTOINCREMENT,";
    list << " key     TEXT,";
    list << " value   TEXT";
    list << ");";

    if (!query.exec(list.join(""))) {
        qWarning() << query.lastError() << " at " << __FILE__ << __LINE__;
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------
}
