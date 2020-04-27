#include "multiFinder.h"

#include "findScu.h"
#include "sqlDatabase.h"
#include "dicomNodeConfig.h"
#include "tableElement.h"

#include <QtConcurrent>
#include <QtConcurrentMap>

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"


namespace pacsnode {

struct mapParam {
    DicomNode host;
    DicomNode peer;
    DicomObject searchParams;
};

QString getMaxInt(const QString& input1, const QString& input2) {
    int i1, i2;
    bool ok1;
    i1 = input1.toInt(&ok1);
    bool ok2;
    i2 = input2.toInt(&ok2);

    int max = 0;
    if (ok1) {
        max = i1;
    }
    if (ok2 && max < i2) {
        max = i2;
    }
    if (max > 0) {
        return QString::number(max);
    }
    return QString();
}

QList< DicomObject > mapFunction( const mapParam& param) {
    FindScu scu;
    return scu.executeFindRequest(param.host, param.peer, param.searchParams);
}


void reduceFunction(QList< DicomObject > &finalResult, const QList< DicomObject > &intermediateResult) {
    // qDebug() << "reduceFunction called" << finalResult.count() << intermediateResult.count();
    foreach(const DicomObject& item, intermediateResult) {

    // foreach item in input list, test if study or series level, get uid
    QSet<DicomElement> itemSet = item.toSet();
    QString studyUid = DicomElement::findValue(itemSet, DCM_StudyInstanceUID);
    QString seriesUid = DicomElement::findValue(itemSet, DCM_SeriesInstanceUID);
    QString imageUid = DicomElement::findValue(itemSet, DCM_SOPInstanceUID);

    QString lookupUid;
    DcmTagKey lookupKey;
    if (!imageUid.isEmpty() && !seriesUid.isEmpty() && !studyUid.isEmpty() ) {
        lookupUid = imageUid;
        lookupKey = DCM_SOPInstanceUID;
    }
    else if (!seriesUid.isEmpty() && !studyUid.isEmpty()) {
        lookupUid = seriesUid;
        lookupKey = DCM_SeriesInstanceUID;
    }
    else if (!studyUid.isEmpty()) {
        lookupUid = studyUid;
        lookupKey = DCM_StudyInstanceUID;
    } 
    else {
        qCritical() << "uid not found, cannot process";
        return;
    }

    // iterate outer list and check if uid can be found: Not: append current inner input list, else: merge
    bool itemFound = false;
    QMutableListIterator< DicomObject >mutIt(finalResult);
    while(mutIt.hasNext()) {
        DicomObject innerElement = mutIt.next();
        if (DicomElement::findValue(innerElement.toSet(), lookupKey) == lookupUid) {
            itemFound = true;
            // merge: try find/cast and max compare: number of series (study) or number of images (series), update
            if (lookupKey == DCM_StudyInstanceUID) {
                QString seriesCount = getMaxInt(DicomElement::findValue(innerElement.toSet(), DCM_NumberOfStudyRelatedSeries), 
                    DicomElement::findValue(itemSet, DCM_NumberOfStudyRelatedSeries));
                innerElement.removeAll(DicomElement(DCM_NumberOfStudyRelatedSeries,""));
                innerElement.append(DicomElement(DCM_NumberOfStudyRelatedSeries,seriesCount));
            }
            else {
                QString imageCount = getMaxInt(DicomElement::findValue(innerElement.toSet(), DCM_NumberOfSeriesRelatedInstances),
                    DicomElement::findValue(itemSet, DCM_NumberOfSeriesRelatedInstances));
                innerElement.removeAll(DicomElement(DCM_NumberOfSeriesRelatedInstances,""));
                innerElement.append(DicomElement(DCM_NumberOfSeriesRelatedInstances,imageCount));
            }
            break;
        }
    }

    // just append if nothing was found
    if (!itemFound) {
        finalResult.append(item);
    }

    }
}


class FinderPrivate {
public:
};

pacsnode::MultiFinder::MultiFinder() : d( new FinderPrivate)
{
}

pacsnode::MultiFinder::~MultiFinder()
{
    delete d;
    d = 0;
}

QList< DicomObject > pacsnode::MultiFinder::doFind( const QList<DicomNode>& nodes, const DicomObject& searchParams )
{
    // get the host
    DicomNodeConfig cfg;
    DicomNode host = cfg.hostNode();

    QList<mapParam> params;
    foreach( const DicomNode& peer, nodes) {
        mapParam param;
        param.host = host;
        param.peer = peer;
        param.searchParams = searchParams;
        params << param;
    }

    // perform threaded lookup then merge
    return QtConcurrent::mappedReduced < QList< DicomObject > >(params, mapFunction, reduceFunction).result();
}

}

