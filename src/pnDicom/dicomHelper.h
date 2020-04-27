#ifndef DicomHelper_h__
#define DicomHelper_h__

#include "pnDicom_Export.h"

#include <QList>

class DcmDataset;
class QImage;
class QString;

namespace pacsnode {

/**
 * static helper class to do stateless operations using dcmtk
 */
class pnDicom_EXPORT DicomHelper 
{

public:

    /**
     *  creates thumbnail and returns path to it
     */
    static QString createThumbnail(const QString& path, const QString& serUID, const QString& proposedPath, unsigned long size = 60);

    /**
     * create two PNG (in case of 16bit) that represent the buffer (MSB and LSB parts)
     */
    static QString dcm2png(const QString& path, const QString& proposedPath);

    static bool dcm2pngImages(const QString& path, QImage& aImage, QImage& bImage);

    /**
     * returns an iso formatted date string
     */
    static QString convertDate(const QString& input );

    /**
     * returns an iso formatted time string
     */
    static QString convertTime(const QString& input );

    /**
     * returns a dedicated thumbnail path in the folder of the image path given
     */
    static QString thumbFolder(const QString imageFilePath);

protected:

    /**
     * checks if this dataset can be used for generating images
     */
    static bool isSupportedImageType(DcmDataset* dataset);

     /**
     * checks if this modality is known not to have pixel data returns true otherwise
     */
    static bool isImageModality(const QString& modality);

    /**
     * helper to write the files from buffer
     */
    static QImage composePng(const QString& header, unsigned char* byteArray, int arrayLength);


    static bool compareString(const QString& input, const QString& value);

};

}
#endif // DicomHelper_h__


