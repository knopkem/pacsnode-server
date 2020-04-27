#include "dicomHelper.h"

// Qt
#include <QtCore>
#include <QImage>

// Own
#include "storage.h"
#include <version_config.h>

// dcmtk
#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmjpeg/djdecode.h"     /* for dcmjpeg decoders */
#include "dcmtk/dcmdata/dcrledrg.h"     /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmjpls/djdecode.h"     /* for dcmjpls decoder */
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcistrmf.h"
#include "dcmtk/ofstd/ofstdinc.h"


namespace pacsnode {

//--------------------------------------------------------------------------------------------

QString DicomHelper::createThumbnail( const QString& path, const QString& serUID, const QString& proposedPath, unsigned long size)
{
    QString storePath = proposedPath;

    storePath += "/" + serUID + ".png";

    if (QFile::exists(storePath)) {
        qDebug() << "thumbnail image at" << storePath;
        return storePath;
    }

    DcmFileFormat dicomFile;
    DcmDataset *dataset = NULL;

    // try to load the file
    try {
        OFCondition cond = dicomFile.loadFile(OFString(path.toLatin1()));
        if (!cond.good()) {
            qWarning() << "could not load file:" << path;
            return QString();
        }
    }
    catch (std::exception e) {
        qCritical("%s", e.what());
        return QString();
    }

    // check if we support this
    dataset = dicomFile.getDataset();
    if ( !DicomHelper::isSupportedImageType(dataset)) {
        return QString();
    }

    // try to decompress
    try {
        // decompress
        dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);

        // check if everything went well
        if (!dataset->canWriteXfer(EXS_LittleEndianExplicit)) {
            qWarning() << "createThumbnail: unable to decompress, file:" << path;
            return QString();
        }
    }
    catch (std::exception e) {
        qCritical("%s", e.what());
        return QString();
    }

        //E_TransferSyntax xfer = dataset->getOriginalXfer();
        QScopedPointer<DicomImage> dcmImage(new DicomImage(dataset, EXS_LittleEndianExplicit));

    try {
        // Check whether we have a valid image
        EI_Status result = dcmImage->getStatus();
        if (result != EIS_Normal) {
            qWarning() << (QString("Thumbnail generation failed: ") + DicomImage::getString(result)) << ", file:" << path;
            return QString();
        }

        // Select first window defined in image. If none, compute min/max window as best guess.
        // Only relevant for monochrome.
        if (dcmImage->isMonochrome()) {
            if (dcmImage->getWindowCount() > 0) {
                dcmImage->setWindow(0);
            }
            else {
                dcmImage->setMinMaxWindow(OFTrue /* ignore extreme values */);
            }
        }

        /* get image extension and prepare image header */
        const unsigned long width = dcmImage->getWidth();
        const unsigned long height = dcmImage->getHeight();
        unsigned long offset = 0;
        unsigned long length = 0;
        QString header;

        if (size == 0) {
            size = width;
            if (size < height) {
                size = height;
            }
        }

        if (dcmImage->isMonochrome()) {
            // write PGM header (binary monochrome image format)
            header = QString("P5 %1 %2 255\n").arg(width).arg(height);
            offset = header.length();
            length = width * height + offset;
        }
        else {
            // write PPM header (binary color image format)
            header = QString("P6 %1 %2 255\n").arg(width).arg(height);
            offset = header.length();
            length = width * height * 3 /* RGB */ + offset;
        }

        /* create output buffer for DicomImage class */
        QByteArray buffer;
        /* copy header to output buffer and resize it for pixel data */
        buffer.append(header);
        buffer.resize(length);

        /* render pixel data to buffer */
        QImage image;
        if (dcmImage->getOutputData(static_cast<void *>(buffer.data() + offset), length - offset, 8, 0)) {
            if (!image.loadFromData( buffer )) {
                qWarning() << "buffer invalid, file:" << path;
                return QString();
            }
        }

        int wSize = width;
        int hSize = height;

        if ( size > 0 ) {
            wSize = size;
            hSize = size;
        }

        // store image
        qDebug() << "storing thumbnail image" << storePath;
        image.scaled(wSize, hSize, Qt::KeepAspectRatio, Qt::SmoothTransformation).save(storePath,"PNG");

    }
    catch (std::exception e) {
        qCritical("%s", e.what());
    }
    return storePath;
}

//--------------------------------------------------------------------------------------------

QString DicomHelper::dcm2png( const QString& path, const QString& proposedPath /*= QString()*/ )
{
    QString result;

    QString storePath = proposedPath;

    QImage aImage;
    QImage bImage;

    // add sopInstUID
    QFileInfo info(path);
    storePath += "/" + info.fileName();
    result = storePath;

    if (!DicomHelper::dcm2pngImages(path, aImage, bImage) ) {
        qCritical() << "image creation failed" << path;
        return QString();
    }

    if (!aImage.isNull() && !aImage.save( storePath + "a.png", "PNG") ) {
        qCritical() << "could not save PNG" << storePath + "a.png";
        return QString();
    }

    if (!bImage.isNull() && !bImage.save( storePath + "b.png", "PNG") ) {
        qCritical() << "could not save PNG" << storePath + "b.png";
        return QString();
    }

    return result;
}

//--------------------------------------------------------------------------------------------

bool DicomHelper::dcm2pngImages( const QString& path, QImage& aImage, QImage& bImage )
{

    // declarations
    DcmFileFormat dicomFile;
    DcmDataset *dataset = NULL;
    QString header;
    Uint16 width = 0;
    Uint16 height = 0;
    const Uint16* pixelBuffer;
    OFCondition cond;

    try {
        // load the file
        cond = dicomFile.loadFile(QDir::toNativeSeparators(path).toStdString().c_str());
        if (cond.bad()) {
            qWarning() << "could not load DICOM file " << cond.text() << ", file:" << path;
            return false;
        }
    }
    catch (std::exception e) {
        qCritical("%s", e.what());
        return false;
    }

    dataset = dicomFile.getDataset();
    if (!dataset) {
        qWarning() << "invalid dataset, file:" << path;
        return false;
    }

    if ( !DicomHelper::isSupportedImageType(dataset) )
        return false;

    try {
        // decompress
        dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);

        // check if everything went well
        if (!dataset->canWriteXfer(EXS_LittleEndianExplicit)) {
            qWarning()<< "dcm2png:: unable to decompress, file:" << path;
            return false;
        }
    }
    catch (std::exception e) {
        qCritical("%s", e.what());
        return false;
    }

    //E_TransferSyntax xfer = dataset->getOriginalXfer();
    QScopedPointer<DicomImage> dcmImage(new DicomImage(dataset, EXS_LittleEndianExplicit));

    // Check whether we have a valid image
    EI_Status status = dcmImage->getStatus();
    unsigned long pixelCount = 0;


    try {

        if (status != EIS_Normal) {
            qWarning() << "PNG creation failed:" << DicomImage::getString(status) << path;
            return false;
        }

        // get the buffer (16bit)
        cond = dataset->findAndGetUint16Array(DCM_PixelData, pixelBuffer, &pixelCount);

        if (cond.bad()) {
            qWarning() << "could not get pixel data " << cond.text() << ", file:" << path;
            return false;
        }
        if (!pixelBuffer) {
            qWarning() << "pixel buffer invalid, file:" << path;
            return false;
        }
        width = dcmImage->getWidth();
        height = dcmImage->getHeight();
    }
    catch (std::exception e) {
        qCritical("%s", e.what());
        return false;
    }

    if (height == 0 || width == 0) {
        qWarning() << "could not get size of image, file:" << path;
        return false;
    }

    if (pixelCount != width*height) {
        qWarning() << "rows and cols do not match pixel data, file:" << path;
        return false;
    }

    // create buffers
    Uint8* msbArray = new Uint8[width * height];
    Uint8* lsbArray = new Uint8[width * height];

    try {
        // fill by splitting 16bit to 2 x 8bit
        for (unsigned long i=0 ; i < pixelCount; ++i) {
            unsigned char nDataMSB = pixelBuffer[i] >> 8 ;
            unsigned char nDataLSB = pixelBuffer[i];
            msbArray[i] = nDataMSB;
            lsbArray[i] = nDataLSB;
        }

        // write PGM header (binary monochrome image format)
        header = QString("P5 %1 %2 255\n").arg(width).arg(height);

        // create pngs each
        int arrayLength = width * height;
        aImage = DicomHelper::composePng(header, msbArray, arrayLength);
        bImage = DicomHelper::composePng(header, lsbArray, arrayLength);
    }
    catch (std::exception e) {
        qCritical("%s", e.what());
    }

    delete [] msbArray;
    delete [] lsbArray;

    return true;
}

//--------------------------------------------------------------------------------------------

QString DicomHelper::convertDate( const QString& input )
{
    OFString dateString;
    OFDate dateVal;
    DcmDate dcmDate(DCM_StudyDate);
    dcmDate.putString(input.toLatin1().constData());
    QString formattedDate;
    if (dcmDate.getOFDate(dateVal).good()) {
        dateVal.getISOFormattedDate(dateString);
        formattedDate = dateString.c_str();
    }
    else {
        formattedDate = input;
    }
    return formattedDate;
}

//--------------------------------------------------------------------------------------

QString DicomHelper::convertTime( const QString& input )
{
    OFString timeString;
    OFTime timeVal;
    DcmTime dcmTime(DCM_StudyTime);
    dcmTime.putString(input.toLatin1().constData());
    QString formattedTime;
    if (dcmTime.getOFTime(timeVal).good()) {
        timeVal.getISOFormattedTime(timeString);
        formattedTime = timeString.c_str();
    }
    else {
        formattedTime = input;
    }
    return formattedTime;
}

//--------------------------------------------------------------------------------------

bool DicomHelper::isSupportedImageType( DcmDataset* dataset )
{
    OFString valueString;
    dataset->findAndGetOFString(DCM_Modality, valueString);
    bool isImage = isImageModality(valueString.c_str());
    if (!isImage) {
        qWarning() << "not supported or not an image modality:" << valueString.c_str();
    }
    return isImage;
}

//--------------------------------------------------------------------------------------

bool DicomHelper::isImageModality( const QString& modality )
{
    // known supported types
    if (compareString(modality, "CT") || compareString(modality, "MR") || compareString(modality, "CR") || 
        compareString(modality, "MG") || compareString(modality, "NM") || compareString(modality, "OT") ||
        compareString(modality, "PT") || compareString(modality, "XA") || compareString(modality, "US") || 
        compareString(modality, "DX") || compareString(modality, "SC"))
        return true;

    return false;
}

//--------------------------------------------------------------------------------------

QImage DicomHelper::composePng( const QString& header, unsigned char* byteArray, int arrayLength )
{
    QImage resultImage;
    try {
        // create output buffer for DicomImage class
        QByteArray bufferA;
        // copy header to output buffer and resize it for pixel data
        bufferA.append(header);

        // convert int buffer to qbytearray
        for (int i = 0; i < arrayLength; ++i) {
            bufferA.append((const char*)(byteArray + i), sizeof(Uint8));
        }

        // load the image data from array and save it
        if (!resultImage.loadFromData( bufferA )) {
            qWarning() << ("loading buffer failed");
            return resultImage;
        }

    }
    catch(std::exception e) {
        qCritical("%s", e.what());
    }

    return resultImage;
}

//--------------------------------------------------------------------------------------

bool DicomHelper::compareString( const QString& input, const QString& value )
{
    return (input == value);
}

//--------------------------------------------------------------------------------------

QString DicomHelper::thumbFolder( const QString imageFilePath )
{
    // removing image filename and adding thumbnail folder
    int idx = imageFilePath.lastIndexOf("/");
    return imageFilePath.left(idx) + "/xt";
}

//--------------------------------------------------------------------------------------
}
