#include "scpImpl.h"

#include <dcmtk/dcmnet/diutil.h>

#include <QDebug>
#include <QDir>

#include "storage.h"
#include "sqlDatabase.h"
#include "tableDefinition.h"
#include "tableUpdater.h"
// #include "tableFinder.h"
// #include "moveProvider.h"

namespace {
    const char* StandardSubdirectory  = "data";
    const char* UndefinedSubdirectory = "undef";
    const char* FilenameExtension     = "";

    const char* PATIENT_LEVEL_STRING = "PATIENT";
    const char* STUDY_LEVEL_STRING = "STUDY";
    const char* SERIES_LEVEL_STRING = "SERIES";
    const char* IMAGE_LEVEL_STRING = "IMAGE";
}

namespace pacsnode {

class ScpImplPrivate 
{
public:
    OFString outputDirectory;
    TableUpdater* tableUpdater;
    QList<TableElement> tableEntries;
    QList<DcmTagKey> supportedTags;
    T_ASC_Parameters params;
};


//---------------------------------------------------------------------------

ScpImpl::ScpImpl() : DcmThreadSCP(), d(new ScpImplPrivate)
{
    d->tableEntries = pacsnode::supportedAttributes();
    d->outputDirectory= pacsnode::storageLocation().toLatin1().constData();
    d->tableUpdater = new TableUpdater();

    // for fast traversal of supported tags
    foreach(const TableElement& tableElement, d->tableEntries) {
        d->supportedTags << tableElement.tag;
    }
}

//---------------------------------------------------------------------------

ScpImpl::~ScpImpl()
{
    delete d->tableUpdater;
    delete d;
    d = NULL;
}

//---------------------------------------------------------------------------

OFCondition ScpImpl::handleIncomingCommand( T_DIMSE_Message *incomingMsg, const DcmPresentationContextInfo &presInfo )
{

    // qDebug() << "handle incoming command";
    OFCondition status = EC_IllegalParameter;
    if (incomingMsg == NULL) {
        return status;
    }

    // check whether we've received a supported command
    if (incomingMsg->CommandField == DIMSE_C_ECHO_RQ) {
        // handle incoming C-ECHO request
        status = handleECHORequest(incomingMsg->msg.CEchoRQ, presInfo.presentationContextID);
    }
    else if (incomingMsg->CommandField == DIMSE_C_STORE_RQ) {
        // handle incoming C-STORE request
        T_DIMSE_C_StoreRQ &storeReq = incomingMsg->msg.CStoreRQ;
        Uint16 rspStatusCode = STATUS_STORE_Error_CannotUnderstand;
        DcmFileFormat fileformat;
        DcmDataset *reqDataset = fileformat.getDataset();
        // receive dataset in memory
        status = receiveSTORERequest(storeReq, presInfo.presentationContextID, reqDataset);
        if (status.good()) {
            // check and process C-STORE request
            rspStatusCode = checkAndProcessSTORERequest(storeReq, fileformat);
        }
        // send C-STORE response (with DIMSE status code)
        if (status.good()) {
            status = sendSTOREResponse(presInfo.presentationContextID, storeReq, rspStatusCode);
        } 
        else if (status == DIMSE_OUTOFRESOURCES) {
            // do not overwrite the previous error status
            sendSTOREResponse(presInfo.presentationContextID, storeReq, STATUS_STORE_Refused_OutOfResources);
        }
    }
    else {
        status = DIMSE_BADCOMMANDTYPE;
    }
    return status;
}

//---------------------------------------------------------------------------

Uint16 ScpImpl::checkAndProcessSTORERequest( const T_DIMSE_C_StoreRQ &reqMessage, DcmFileFormat &fileformat )
{
    // qDebug() << "checking and processing C-STORE request";
    Uint16 statusCode = STATUS_STORE_Error_CannotUnderstand;
    DcmDataset *dataset = fileformat.getDataset();
    // perform some basic checks on the request dataset
    if ((dataset == NULL) || dataset->isEmpty()) {
        return statusCode;
    }

    OFString filename;
    OFString directoryName;
    OFString sopClassUID = reqMessage.AffectedSOPClassUID;
    OFString sopInstanceUID = reqMessage.AffectedSOPInstanceUID;
    // generate filename with full path
    OFCondition status = generateDirAndFilename(filename, directoryName, sopClassUID, sopInstanceUID, dataset);

    if (status.good()) {
        // qDebug() << "generated filename for received object: " << QString(filename.c_str());
        // create the output directory (if needed)
        status = OFStandard::createDirectory(directoryName, d->outputDirectory /* rootDir */);
        if (status.good()) {
            if (OFStandard::fileExists(filename))
            qWarning() << "file already exists, overwriting: " << QString(filename.c_str());
            // store the received dataset to file (with default settings)
            status = fileformat.saveFile(filename);
            if (status.good()) {

                // store meta data
                this->insertMetaData(dataset, filename);

                // call the notification handler (default implementation outputs to the logger)
                notifyInstanceStored(filename, sopClassUID, sopInstanceUID, dataset);
                statusCode = STATUS_Success;
            } 
            else {
                qCritical() << "cannot store received object: " << QString(filename.c_str()) << ": " << status.text();
                statusCode = STATUS_STORE_Refused_OutOfResources;
            }
        } 
        else {
            qCritical() << "cannot create directory for received object: " << QString(directoryName.c_str()) << ": " << status.text();
            statusCode = STATUS_STORE_Refused_OutOfResources;
        }
    }
    return statusCode;
}

//---------------------------------------------------------------------------

OFCondition ScpImpl::generateSTORERequestFilename( const T_DIMSE_C_StoreRQ &reqMessage, OFString &filename )
{
    OFString directoryName;
    OFString sopClassUID = reqMessage.AffectedSOPClassUID;
    OFString sopInstanceUID = reqMessage.AffectedSOPInstanceUID;
    // generate filename (with full path)
    OFCondition status = generateDirAndFilename(filename, directoryName, sopClassUID, sopInstanceUID);
    if (status.good()) {
        // qDebug() << "generated filename for object to be received: " << QString(filename.c_str());
        // create the output directory (if needed)
        status = OFStandard::createDirectory(directoryName, d->outputDirectory /* rootDir */);
        if (status.bad())
            qCritical() << "cannot create directory for object to be received: " << QString(directoryName.c_str()) << ": " << status.text();
    } 
    else
        qCritical() << "cannot generate directory or file name for object to be received: " << status.text();
    return status;
}

//---------------------------------------------------------------------------

void ScpImpl::notifyInstanceStored( const OFString &filename, const OFString &sopClassUID, 
    const OFString &sopInstanceUID, DcmDataset *dataset /*= NULL*/ ) const
{
    // by default, output some useful information
    qDebug() << "Stored received object to file: " << QString(filename.c_str());
}

//---------------------------------------------------------------------------

OFCondition ScpImpl::generateDirAndFilename( OFString &filename, OFString &directoryName, 
    OFString &sopClassUID, OFString &sopInstanceUID, DcmDataset *dataset /*= NULL*/ )
{
    if (dataset == NULL) {
        return EC_CouldNotGenerateDirectoryName;
    }

    OFCondition status = EC_Normal;
    // get SOP class and instance UID (if not yet known from the command set)
    if (sopClassUID.empty())
        dataset->findAndGetOFString(DCM_SOPClassUID, sopClassUID);
    if (sopInstanceUID.empty())
        dataset->findAndGetOFString(DCM_SOPInstanceUID, sopInstanceUID);

    // generate directory name
    OFString generatedDirName;

    OFString seriesDate;
    DcmElement *element = NULL;
    // try to get the series date from the dataset
    if (dataset->findAndGetElement(DCM_SeriesDate, element).good() && (element->ident() == EVR_DA)) {
        OFString dateValue;
        DcmDate *dateElement = OFstatic_cast(DcmDate *, element);
        // output ISO format is: YYYY-MM-DD
        if (dateElement->getISOFormattedDate(dateValue).good() && (dateValue.length() == 10)) {
            OFOStringStream stream;
            stream << StandardSubdirectory << PATH_SEPARATOR
                << dateValue.substr(0, 4) << PATH_SEPARATOR
                << dateValue.substr(5 ,2) << PATH_SEPARATOR
                << dateValue.substr(8, 2) << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(stream, tmpString)
                generatedDirName = tmpString;
            OFSTRINGSTREAM_FREESTR(tmpString);
        }
    }
    // alternatively, if that fails, use the current system date
    if (generatedDirName.empty()) {
        OFString currentDate;
        status = DcmDate::getCurrentDate(currentDate);
        if (status.good()) {
            OFOStringStream stream;
            stream << UndefinedSubdirectory << PATH_SEPARATOR
                << currentDate << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(stream, tmpString)
                generatedDirName = tmpString;
            OFSTRINGSTREAM_FREESTR(tmpString);
        }
    }


    if (status.good()) {

        // combine the generated directory name with the output directory
        OFStandard::combineDirAndFilename(directoryName, d->outputDirectory, generatedDirName);
        // generate filename
        OFString generatedFileName;
        if (sopClassUID.empty()) {
            status = NET_EC_InvalidSOPClassUID;
        } 
        else if (sopInstanceUID.empty()) {
            status = NET_EC_InvalidSOPInstanceUID;
        } 
        else {
            OFOStringStream stream;
            stream << sopInstanceUID << FilenameExtension << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(stream, tmpString)
                generatedFileName = tmpString;
            OFSTRINGSTREAM_FREESTR(tmpString);
            // combine the generated file name with the directory name
            OFStandard::combineDirAndFilename(filename, directoryName, generatedFileName);
        }
    }
    return status;
}

//---------------------------------------------------------------------------

OFCondition ScpImpl::insertMetaData( DcmDataset* dataset, const OFString & filename )
{
    OFCondition status = EC_Normal;

    dataset->convertToUTF8();
    DcmXfer original_xfer (dataset->getOriginalXfer());

    // parse attributes and store them in hashMap
    QHash< TableElement, QVariant > insertMap;
    for(int i=0; i < d->tableEntries.count(); i++) {

        OFCondition ec = EC_Normal;
        const char *strPtr = NULL;
        Uint16 intPtr = 0;
        ec = dataset->findAndGetString(d->tableEntries.at(i).tag, strPtr);
        if ((ec == EC_Normal) && (strPtr != NULL)) {
            insertMap[d->tableEntries.at(i)] = QVariant(strPtr);
        } 
        else {
            ec = dataset->findAndGetUint16(d->tableEntries.at(i).tag, intPtr);
            if ((ec == EC_Normal) && (intPtr != 0)) {
                insertMap[d->tableEntries.at(i)] = QVariant(intPtr);
            }
        }
    }

    // add filename to private field
    QString formattedPath = QString::fromUtf8(filename.c_str());
    formattedPath.replace("\\", "/");
    formattedPath.remove(pacsnode::storageLocation()); // storing only the relative path
    insertMap[TableElement( DCM_PrivateFileName,IMAGE_LEVEL,OPTIONAL_KEY,STRING_CLASS)] = QVariant(formattedPath);

    if (!d->tableUpdater->insertValuesIntoDb(insertMap)) {
        status = EC_IllegalParameter;
    }

    return status;
}

//---------------------------------------------------------------------------

}

