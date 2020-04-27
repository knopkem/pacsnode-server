#ifndef TableUpdater_h__
#define TableUpdater_h__

#include "tableElement.h"

#include <QHash>
#include <QVariant>

namespace pacsnode {

class TableUpdaterPrivate;
class TableUpdater
{
public:
    TableUpdater();
    ~TableUpdater();

    bool insertValuesIntoDb(const QHash< TableElement, QVariant >& keyValueList);

protected:

    DbIdent insertIntoPatients( const QHash< TableElement, QVariant >& keyValueList);

    DbIdent insertIntoStudies(const QHash< TableElement, QVariant >& keyValueList, DbIdent patientIdent);

    DbIdent insertIntoSeries(const QHash< TableElement, QVariant >& keyValueList, DbIdent studyIdent);

    DbIdent insertIntoImages(const QHash< TableElement, QVariant >& keyValueList, DbIdent seriesIdent);

    qint64 insertAttributesIntoTable(const QHash< TableElement, QVariant >& keyValueList, qint64 id, TABLE_LEVEL level);

    QString fromHash(const QHash< TableElement, QVariant >& keyValueList, DcmTagKey key);

private:
    TableUpdaterPrivate* d;

};

}

#endif // TableUpdater_h__

