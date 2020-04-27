#include "tableUpdater.h"

#include "sqlDatabase.h"
#include "storage.h"

#include <QtSql>

namespace pacsnode {

class TableUpdaterPrivate {
public:
    SqlDatabase* sqlDb;

};

//--------------------------------------------------------------------------------------------

TableUpdater::TableUpdater() : d(new TableUpdaterPrivate)
{
    d->sqlDb = new SqlDatabase(pacsnode::storageLocation());
}

//--------------------------------------------------------------------------------------------

TableUpdater::~TableUpdater()
{
    delete d->sqlDb;
    delete d;
    d = NULL;
}

//--------------------------------------------------------------------------------------------

pacsnode::DbIdent TableUpdater::insertIntoPatients( const QHash< TableElement, QVariant >& keyValueList )
{
    DbIdent ident;
    qint64 id(0);
    bool isNew = true;
    QSqlQuery query(d->sqlDb->openConnection());

    // try to find the patient with Name + IPP
    QString patientId = fromHash(keyValueList, DCM_PatientID);
    QString patientName = fromHash(keyValueList, DCM_PatientName);

    // if patient id is empty, try to find the patient first
    if (patientId.isEmpty()) {
        query.prepare("SELECT " + getTagName(DCM_PatientID) + " FROM patient WHERE "
            + getTagName(DCM_PatientName) + "= :patName");
        query.bindValue(":patName", patientName);
        EXEC_QUERY(query);

        if(query.first()) {
            patientId = query.value(0).toString();
        }
    }

    // if still empty, create patient id
    if (patientId.isEmpty()) {
        patientId = QUuid::createUuid().toString();
    }

    query.prepare("SELECT id FROM patient WHERE " + getTagName(DCM_PatientID) + "= :patId AND "
        + getTagName(DCM_PatientName) + "= :patName");
    query.bindValue(":patId", patientId);
    query.bindValue(":patName", patientName);
    EXEC_QUERY(query);

    if(query.first()) {
        id = query.value(0).toLongLong();
        isNew = false;
    }

    query.finish();

    // new patient
    if(isNew) {

        QHash< TableElement, QVariant > modifiedKeyValueList(keyValueList);
        modifiedKeyValueList.insert(TableElement( DCM_PatientID,PATIENT_LEVEL,REQUIRED_KEY,UID_CLASS), QVariant(patientId) );
        id = insertAttributesIntoTable(modifiedKeyValueList, 0, PATIENT_LEVEL);
    }
    ident.level = PATIENT_LEVEL;
    ident.primaryKey = id;
    ident.isNew = isNew;
    return ident;
}

//--------------------------------------------------------------------------------------------

pacsnode::DbIdent TableUpdater::insertIntoStudies( const QHash< TableElement, QVariant >& keyValueList, DbIdent patientIdent )
{
    DbIdent ident;
    qint64 id(0);
    bool isNew = true;

    if (!patientIdent.statusGood)
        return patientIdent;

    QString studyInstUID = fromHash(keyValueList, DCM_StudyInstanceUID);

    // try to find the patient with Name + IPP
    QSqlQuery query(d->sqlDb->openConnection());
    query.prepare("SELECT id FROM study WHERE " + getTagName(DCM_StudyInstanceUID) + "= :stdUid");
    query.bindValue(":stdUid", studyInstUID);
    EXEC_QUERY(query);

    if(query.first()) {
        id = query.value(0).toLongLong();
        isNew = false;
    }

    query.finish();

    // new study
    if(isNew) {
        id = insertAttributesIntoTable(keyValueList, patientIdent.primaryKey, STUDY_LEVEL);
    }
    ident.level = STUDY_LEVEL;
    ident.primaryKey = id;
    ident.isNew = isNew;
    return ident;
}

//--------------------------------------------------------------------------------------------

pacsnode::DbIdent TableUpdater::insertIntoSeries( const QHash< TableElement, QVariant >& keyValueList, DbIdent studyIdent )
{
    DbIdent ident;
    qint64 id(0);
    bool isNew = true;

    if (!studyIdent.statusGood)
        return studyIdent;

    QString seriesInstUID = fromHash(keyValueList, DCM_SeriesInstanceUID);

    // try to find the patient with Name + IPP
    QSqlQuery query(d->sqlDb->openConnection());
    query.prepare("SELECT id FROM series WHERE " + getTagName(DCM_SeriesInstanceUID) + "= :serUid");
    query.bindValue(":serUid", seriesInstUID);
    EXEC_QUERY(query);

    if(query.first()) {
        id = query.value(0).toLongLong();
        isNew = false;
    }

    query.finish();

    // new series
    if(isNew) {
        id = insertAttributesIntoTable(keyValueList, studyIdent.primaryKey, SERIES_LEVEL);
    }
    ident.level = SERIES_LEVEL;
    ident.primaryKey = id;
    ident.isNew = isNew;
    return ident;
}

//--------------------------------------------------------------------------------------------

pacsnode::DbIdent TableUpdater::insertIntoImages( const QHash< TableElement, QVariant >& keyValueList, DbIdent seriesIdent )
{
    DbIdent ident;
    qint64 id(0);
    bool isNew = true;

    if (!seriesIdent.statusGood)
        return seriesIdent;

    QString sopInstUID = fromHash(keyValueList, DCM_SOPInstanceUID);

    // try to find the patient with Name + IPP
    QSqlQuery query(d->sqlDb->openConnection());
    query.prepare("SELECT id FROM image WHERE " + getTagName(DCM_SOPInstanceUID) + "= :sopInstUid");
    query.bindValue(":sopInstUid", sopInstUID);
    EXEC_QUERY(query);

    if(query.first()) {
        id = query.value(0).toLongLong();
        isNew = false;
    }

    // new image
    if(isNew) {
        id = insertAttributesIntoTable(keyValueList, seriesIdent.primaryKey, IMAGE_LEVEL);
    }
    ident.level = IMAGE_LEVEL;
    ident.primaryKey = id;
    ident.isNew = isNew;
    return ident;
}

//--------------------------------------------------------------------------------------------

qint64 TableUpdater::insertAttributesIntoTable( const QHash< TableElement, QVariant >& keyValueList, qint64 id, TABLE_LEVEL level )
{
    QSqlQuery query(d->sqlDb->openConnection());
    QStringList attributs;
    QStringList values;
    QHashIterator<TableElement, QVariant > i(keyValueList);
    while (i.hasNext()) {
        i.next();
        if (i.key().level == level) {
            QString attr = getTagName(i.key().tag);
            attributs << attr;
            values << i.value().toString();
        }

        if ( (level == IMAGE_LEVEL) && (i.key().tag == DCM_PrivateCreator) )
            qCritical() << "Missing filename" << i.value().toString();
    }

    QString postgresExtension;
    if (d->sqlDb->openConnection().driverName() == "QPSQL")
        postgresExtension = "RETURNING id";

    QString prepare = "INSERT INTO " + pacsnode::tableNameForLevel(level) + " ( " + attributs.join(" , ")
        + ", referenceId ) VALUES ( :"+ attributs.join(", :") +", :referenceId ) " + postgresExtension;
    query.prepare(prepare);

    for(int u = 0; u < attributs.count(); u++) {
        query.bindValue(":"+attributs.at(u), values.at(u));
    }
    query.bindValue(":referenceId",id);
    EXEC_QUERY(query);

    if (query.next())
        return query.value(0).toLongLong();
    else
        return query.lastInsertId().toLongLong();

    return 0;
}

//--------------------------------------------------------------------------------------------

QString TableUpdater::fromHash( const QHash< TableElement, QVariant >& keyValueList, DcmTagKey key )
{
    QString result;
    QHash< TableElement, QVariant >::const_iterator iter;
    iter = keyValueList.find( TableElement(key, PATIENT_LEVEL, UNIQUE_KEY, STRING_CLASS) );
    if (iter != keyValueList.constEnd()) {
        result = iter.value().toString();
    }
    return result;
}

//--------------------------------------------------------------------------------------------

bool TableUpdater::insertValuesIntoDb( const QHash< TableElement, QVariant >& keyValueList )
{
    bool result = true;

    d->sqlDb->transaction();
    DbIdent patIdent = insertIntoPatients(keyValueList);
    DbIdent stdIdent = insertIntoStudies(keyValueList, patIdent);
    DbIdent serIdent = insertIntoSeries(keyValueList, stdIdent);
    DbIdent imgIdent = insertIntoImages(keyValueList, serIdent);

    if (!imgIdent.isNew) {
        qWarning() << "instance already registered";
        result = false;
    }
    d->sqlDb->commit();

    return result;
}

//--------------------------------------------------------------------------------------------

}
