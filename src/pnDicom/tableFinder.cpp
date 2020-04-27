#include "tableFinder.h"

#include <QtSql>

#include "storage.h"
#include "sqlDatabase.h"
#include "tableDefinition.h"
#include "dicomHelper.h"

namespace pacsnode {

class TableFinderPrivate {
public:
    SqlDatabase* sqlDb;
    QList<pacsnode::TableElement> definedTags;
};

//--------------------------------------------------------------------------------------------

TableFinder::TableFinder() : d(new TableFinderPrivate)
{
    d->sqlDb = new SqlDatabase(pacsnode::storageLocation());
    d->definedTags = pacsnode::supportedAttributes();
}

//--------------------------------------------------------------------------------------------

TableFinder::~TableFinder()
{
    delete d->sqlDb;
    delete d;
    d = NULL;
}

//--------------------------------------------------------------------------------------------

QStringList TableFinder::sortedImageUIDs( const QString& studyUID, const QString& seriesUID )
{
    if (studyUID.isEmpty() || seriesUID.isEmpty()) {
        qWarning() << "sortedImageUIDs - uid missing";
    }

    // sort by instance number
    // todo: altern. sort by time
    QMap<int, QString > sortMap;
    QList<sInstanceInfo> infos = instances(studyUID, seriesUID);

    foreach(const sInstanceInfo info, infos) {
        QString uid = info.SOPInstanceUID;
        QString nb = info.InstanceNumber;
        sortMap[nb.toInt()] = uid;
    }

    QStringList pathList;
    QMapIterator<int, QString> iter(sortMap);
    while (iter.hasNext()) {
        iter.next();
        pathList << iter.value();
    }
    return pathList;
}

//--------------------------------------------------------------------------------------------

QString TableFinder::pathForProcessedImage( const QString& studyUID, const QString& seriesUID, const QString& sopInstUID, const QString& suffix )
{
    if (studyUID.isEmpty() || seriesUID.isEmpty() || sopInstUID.isEmpty()) {
        qWarning() << "pathForProcessedImage - uid missing";
        return QString();
    }


    if (suffix.isEmpty() ) {
        qWarning() << "pathForProcessedImage - no suffix given";
        return QString();
    }
    QString path = pathForDicomImage(studyUID, seriesUID, sopInstUID);

    QString result =  DicomHelper::thumbFolder(path) + "/" + sopInstUID + suffix + ".png";
    return result;
}

//--------------------------------------------------------------------------------------------

QString TableFinder::pathForDicomImage( const QString& studyUID, const QString& seriesUID, const QString& sopInstUID )
{
    if (studyUID.isEmpty() || seriesUID.isEmpty() || sopInstUID.isEmpty()) {
        qWarning() << "pathForDicomImage - uid missing";
        return QString();
    }

    sInstanceInfo info = instance(studyUID, seriesUID, sopInstUID);
    return pacsnode::storageLocation() + info.FileName; // combine to get the true path
}

//--------------------------------------------------------------------------------------------

QList<TableFinder::sInstanceInfo> TableFinder::instances(const QString& studyUID, const QString& seriesUID)
{
    Q_UNUSED(studyUID);
    QList<sInstanceInfo> result;

    if (seriesUID.isEmpty()) {
        qWarning() << "instances - uid missing";
        return result;
    }

    QSqlQuery query(d->sqlDb->openConnection());

    QStringList selectClause;
    selectClause << "image." + pacsnode::getTagName(DCM_InstanceNumber);
    selectClause << "image." + pacsnode::getTagName(DCM_PrivateFileName);
    selectClause << "image." + pacsnode::getTagName(DCM_SOPInstanceUID);

    // needs join between study and series
    QString prepare = "SELECT " + selectClause.join(",") + " FROM " + pacsnode::tableNameForLevel(IMAGE_LEVEL)
        + " INNER JOIN " + pacsnode::tableNameForLevel(SERIES_LEVEL) + " ON image.referenceId=series.id"
        + " WHERE series." + pacsnode::getTagName(DCM_SeriesInstanceUID) + "= :refId";
    query.prepare(prepare);
    query.bindValue(":refId", seriesUID);
    EXEC_QUERY(query);

    while (query.next()) {
        sInstanceInfo info;
        int i = 0;
        info.InstanceNumber = query.value(i++).toString();
        info.FileName = query.value(i++).toString();
        info.SOPInstanceUID = query.value(i++).toString();
        result << info;
    }
    if (result.isEmpty()) {
        qWarning() << "instances - no images found";
    }
    return result;
}

//--------------------------------------------------------------------------------------------

TableFinder::sInstanceInfo TableFinder::instance(const QString& studyUID, const QString& seriesUID, const QString& sopInstUID)
{
    Q_UNUSED(studyUID);
    Q_UNUSED(seriesUID);
    sInstanceInfo result;

    if (sopInstUID.isEmpty()) {
        qWarning() << "instance - uid missing";
        return result;
    }

    QSqlQuery query(d->sqlDb->openConnection());

    QStringList selectClause;
    selectClause << pacsnode::getTagName(DCM_SOPInstanceUID);
    selectClause << pacsnode::getTagName(DCM_InstanceNumber);
    selectClause << pacsnode::getTagName(DCM_PrivateFileName);

    // we just search for image uid, no join needed
    QString prepare = "SELECT " + selectClause.join(",") + " FROM "
        + pacsnode::tableNameForLevel(IMAGE_LEVEL) + " WHERE " + pacsnode::getTagName(DCM_SOPInstanceUID) + " = :refId";

    query.prepare(prepare);
    query.bindValue(":refId", sopInstUID);

    EXEC_QUERY(query);

    while (query.next()) {
        sInstanceInfo info;
        info.SOPInstanceUID = query.value(0).toString();
        info.InstanceNumber = query.value(1).toString();
        info.FileName = query.value(2).toString();
        return info;
    }
    qWarning() << "instance - no image found";
    return result;
}

//--------------------------------------------------------------------------------------------

}
