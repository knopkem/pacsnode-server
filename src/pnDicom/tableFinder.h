#ifndef TableFinder_h__
#define TableFinder_h__

#include "pnDicom_Export.h"
#include "tableElement.h"
#include "dicomElement.h"

#include <QtCore>

class DcmTagKey;

namespace pacsnode {

class TableFinderPrivate;
class SqlDatabase;
class pnDicom_EXPORT TableFinder
{
public:
    TableFinder();
    ~TableFinder();

    /**
     * returns the list of sopInstanceUIDs for this series
     */
    QStringList sortedImageUIDs(const QString& studyUID, const QString& seriesUID);

    /**
     * returns the PNG DICOM (MSB or LSB part) of this image (suffix a or b)
     */
    QString pathForProcessedImage(const QString& studyUID, const QString& seriesUID, const QString& sopInstUID, const QString& suffix);

    /**
     * returns the path for the dicom image that belongs to the given uids
     */
    QString pathForDicomImage(const QString& studyUID, const QString& seriesUID, const QString& sopInstUID);

protected:
    struct sInstanceInfo {
        QString SOPInstanceUID;
        QString InstanceNumber;
        QString FileName;
    };
    QList<sInstanceInfo> instances(const QString& studyUID, const QString& seriesUID);

    sInstanceInfo instance(const QString& studyUID, const QString& seriesUID, const QString& sopInstUID);

private:
    TableFinderPrivate* d;

};

}

#endif // TableFinder_h__

