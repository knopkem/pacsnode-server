#ifndef tableElement_h__
#define tableElement_h__

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdicent.h"

#include <QString>

#include "dicomElement.h"

namespace pacsnode {

static GlobalDcmDataDictionary* DICT = new GlobalDcmDataDictionary();

static inline QString getTagName(const DcmTagKey& tag) {
    return DICT->rdlock().findEntry(tag, NULL)->getTagName();
}


/** enumeration describing the levels of the DICOM Q/R information model
 */
enum TABLE_LEVEL
{
  /// DICOM Q/R patient level
  PATIENT_LEVEL,
  /// DICOM Q/R study level
  STUDY_LEVEL,
  /// DICOM Q/R series level
  SERIES_LEVEL,
  /// DICOM Q/R instance level
  IMAGE_LEVEL
};

/** query models
 */
enum TABLE_QUERY_CLASS
{
    /// patient root Q/R model
    PATIENT_ROOT,
    /// study root Q/R model
    STUDY_ROOT,
    /// patient/study only Q/R model
    PATIENT_STUDY
};

/** types of query keys
 */
enum TABLE_KEY_TYPE
{
    /// unique key
    UNIQUE_KEY,
    /// required key
    REQUIRED_KEY,
    /// optional key
    OPTIONAL_KEY
};

/** types of database keys
 */
enum TABLE_KEY_CLASS
{
    /// a date entry
    DATE_CLASS,
    /// a time entry
    TIME_CLASS,
    /// a UID entry
    UID_CLASS,
    /// a string entry
    STRING_CLASS,
    /// an entry not belonging to any other class
    OTHER_CLASS
};

/** db identifier
 */
struct DbIdent
{
    DbIdent() {
        level = PATIENT_LEVEL;
        primaryKey = -1;
        isNew = true;
        statusGood = true;
    }

    TABLE_LEVEL level;
    qint64 primaryKey;
    bool isNew;
    bool statusGood;
};

/**
 * result object for queries
 */
struct QueryResult {
    qint64 id;
    QList< DicomElement > resultList;
};

static inline QString tableNameForLevel(TABLE_LEVEL level) {
    QString result;
    switch(level) {
    case PATIENT_LEVEL:
        result = "patient";
        break;
    case STUDY_LEVEL:
        result = "study";
        break;
    case SERIES_LEVEL:
        result = "series";
        break;
    case IMAGE_LEVEL:
        result = "image";
        break;
    default:
        qCritical() << "tableName not defined";
        break;
    }

    return result;
}

struct TableElement 
{
    TableElement(const DcmTagKey& t, TABLE_LEVEL l, TABLE_KEY_TYPE kt, TABLE_KEY_CLASS kc) : 
        tag(t), level(l), keyAttr(kt), keyClass(kc) {
            tagName = getTagName(t);
        }

    DcmTagKey tag;
    TABLE_LEVEL level;
    TABLE_KEY_TYPE keyAttr;
    TABLE_KEY_CLASS keyClass;
    QString tagName;

};


inline bool operator==(const TableElement& a, const TableElement& b) {
    return (a.tag == b.tag);
}


inline uint qHash(const TableElement &key)
{
    uint hash = 0;
    hash = hash ^ key.tag.getGroup() ^ key.tag.getElement();
    return hash;
}

}

#endif // tableElement_h__

