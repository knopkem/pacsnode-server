#include "queryHandler.h"

#include <QtCore>
#include <QImage>
#include <QSqlQuery>

// tufao
#include <HttpFileServer>
#include <HttpServerRequest>
#include <Headers>
#include <Url>
#include <NotFoundHandler>

#include "util.h"
#include "sqlDatabase.h"

// dicom
#include "multiFinder.h"
#include "dicomElement.h"
#include "dicomNode.h"
#include "dicomNodeConfig.h"
#include "patientData.h"
#include "tableFinder.h"
#include "storage.h"
#include "dicomHelper.h"
#include "moveScu.h"
#include "commonHelper.h"
#include "moveRequestor.h"

namespace pacsnode {


//--------------------------------------------------------------------------------------

QueryHandler::QueryHandler(QObject *parent /*= 0*/ )
{
}

//--------------------------------------------------------------------------------------

QueryHandler::~QueryHandler()
{
}

//--------------------------------------------------------------------------------------

bool QueryHandler::handleRequest( Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response, 
    const QStringList &args /*= QStringList()*/ )
{
    QString resource(QByteArray::fromPercentEncoding(Tufao::Url(request->url())
        .path().toUtf8()));

    // this handles study information lookup
    if (resource.contains("/queryStudyInfo")) {
        return handleStudyQuery(resource, request, response);
    }

    // this handles series information lookup
    if (resource.contains("/querySeriesInfo")) {
        return handleSeriesQuery(resource, request, response);
    }

    // this handles study and series information lookup
    if (resource.contains("/queryStudyAndSeriesInfo")) {
        return handleStudyAndSeriesQuery(resource, request, response);
    }

    // full frame dicom request including (header or buffer)
    if (resource.contains("/full")) {
        return handleFullFrame(resource, request, response);
    }

    // thumbnail image request
    if (resource.contains("/queryThumbnail")) {
        return handleThumbnail(resource, request, response);
    }

    // default static file
    Tufao::HttpFileServer fileServer(QCoreApplication::applicationDirPath() + "/public");
    if (fileServer.handleRequest(request, response)) {
        return true;
    }

    // if all fails return error
    Tufao::NotFoundHandler notFound;
    return notFound.handleRequest(request, response);
}

//--------------------------------------------------------------------------------------

bool QueryHandler::handleStudyAndSeriesQuery( const QString& resource, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response )
{
    QString url = QString(request->url()).remove(resource + "?");
    Util::ParameterMap paramLookup = Util::parseFromUrlEncoded(url);

    QString stdUID = paramLookup.value("stdUID");
    QString serUID = paramLookup.value("serUID");

    DicomNodeConfig cfg;
    QList<DicomNode> nodes;
    // nodes.append(cfg.hostNode());
    nodes.append(cfg.nodes());

    QList<PatientData> studyInfoList = this->getStudyInfo("", stdUID, nodes);
    QList<PatientData> seriesInfoList = this->getSeriesInfo(stdUID, serUID, nodes, true);

    if (studyInfoList.isEmpty() || seriesInfoList.isEmpty()) {
        qWarning() << "nothing found for" << stdUID << "and" << serUID;
        return false;
    }

    PatientData merged = mergePatientData(studyInfoList.first(), seriesInfoList.first());

    // create JSON out of response, see patientData
    QJsonObject root;
    // iterate studies
    root.insert(merged.DD_DCM_SeriesInstanceUID, merged.toJSON());

    QJsonDocument doc(root);
    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().replace("Content-Type", "application/json");
    response->end(doc.toJson());
    return true;
}

//--------------------------------------------------------------------------------------

bool QueryHandler::handleStudyQuery( const QString& resource, Tufao::HttpServerRequest *request, 
    Tufao::HttpServerResponse *response )
{
    QString url = QString(request->url()).remove(resource + "?");
    Util::ParameterMap paramLookup = Util::parseFromUrlEncoded(url);

    QString patName = paramLookup.value("patName");

    DicomNodeConfig cfg;
    QList<DicomNode> nodes;
    // nodes.append(cfg.hostNode());
    nodes.append(cfg.nodes());

    QJsonObject root;
    // iterate studies
    foreach(const PatientData& study, this->getStudyInfo(patName, "", nodes)) {
        root.insert(study.DD_DCM_StudyInstanceUID, study.toJSON());
    }

    QJsonDocument doc(root);
    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().replace("Content-Type", "application/json");
    response->end(doc.toJson());

    return true;
}

//--------------------------------------------------------------------------------------

bool QueryHandler::handleSeriesQuery( const QString& resource, Tufao::HttpServerRequest *request, 
    Tufao::HttpServerResponse *response )
{
    QString url = QString(request->url()).remove(resource + "?");
    Util::ParameterMap paramLookup = Util::parseFromUrlEncoded(url);

    QString stdUID = paramLookup.value("stdUID");

    if (stdUID.isEmpty()) {
        qWarning() << "studyInstanceUID not provided, cannot server request";
    }

    DicomNodeConfig cfg;
    QList<DicomNode> nodes;
    // nodes.append(cfg.hostNode());
    nodes.append(cfg.nodes());

    // create JSON out of response, see patientData
    QJsonObject root;
    // iterate studies
    foreach(const PatientData& series, this->getSeriesInfo(stdUID, "", nodes, false)) {
        root.insert(series.DD_DCM_SeriesInstanceUID, series.toJSON());
    }

    QJsonDocument doc(root);
    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().replace("Content-Type", "application/json");
    response->end(doc.toJson());

    return true;

}

//--------------------------------------------------------------------------------------

bool QueryHandler::handleFullFrame( const QString& resource, Tufao::HttpServerRequest *request, 
    Tufao::HttpServerResponse *response )
{
    QString url = QString(request->url()).remove(resource + "?");
    Util::ParameterMap paramLookup = Util::parseFromUrlEncoded(url);

    QString stdUID = paramLookup.value("stdUID");
    QString serUID = paramLookup.value("serUID");;
    QString imgUID = paramLookup.value("imgUID");
    QString typeID = paramLookup.value("type");
    QString quality = paramLookup.value("quality");

    QString filePath;

    // we need all
    if (stdUID.isEmpty() || serUID.isEmpty() || imgUID.isEmpty() || typeID.isEmpty()) {
        qWarning() << "uid not provided, cannot server request";
        return false;
    }

    if (typeID == "header") {
        serveHeader(stdUID, serUID, imgUID, request, response);
    } 
    else {
        TableFinder finder;
        filePath = finder.pathForProcessedImage(stdUID, serUID, imgUID, typeID);
        this->serveBuffer(filePath, quality.toInt(), request, response);
    }
    return true;
}

//--------------------------------------------------------------------------------------

bool QueryHandler::handleThumbnail( const QString& resource, Tufao::HttpServerRequest *request, 
    Tufao::HttpServerResponse *response )
{
    QString url = QString(request->url()).remove(resource + "?");
    Util::ParameterMap paramLookup = Util::parseFromUrlEncoded(url);

    QString stdUID = paramLookup.value("stdUID");
    QString serUID = paramLookup.value("serUID");

    // we need all
    if (stdUID.isEmpty() || serUID.isEmpty() ) {
        qWarning() << "uid not provided, cannot server request";
        return false;
    }

    // get the middle slice of the series
    TableFinder finder;
    QStringList sops = finder.sortedImageUIDs(stdUID, serUID);
    QString thumbnailPath = QCoreApplication::applicationDirPath() + "/public/stylesheets/images/nopreview.png";

    if (!sops.isEmpty()) {
        QString thumbnailSop = sops.at(qFloor(sops.count()/2));
        QString filePath = finder.pathForDicomImage(stdUID, serUID, thumbnailSop);

        // generate and serve PNG
        QString tPath = DicomHelper::createThumbnail(filePath, serUID, DicomHelper::thumbFolder(filePath));
    
        QFileInfo info(tPath);
        if (info.exists()) {
            thumbnailPath = tPath;
        }

    }
    Tufao::HttpFileServer fileServer("");
    fileServer.serveFile(thumbnailPath, request, response);
    return true;
}

//--------------------------------------------------------------------------------------

bool QueryHandler::serveBuffer( const QString& filepath, int quality,Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response )
{
    if (filepath.isEmpty()) {
        return false;
    }

    QString url = QString(request->url());
    Util::ParameterMap paramLookup = Util::parseFromUrlEncoded(url);

    QString imgUID = paramLookup.value("imgUID");


    QFileInfo info;

    if (quality < 5) {
        qWarning() << "Quality parameter below threshold (5%)";
        quality = 5;
    }

    SqlDatabase sql(pacsnode::storageLocation());
    while(1) {
        QSqlQuery query(sql.openConnection());
        QString prepare = "SELECT processed FROM image WHERE SOPInstanceUID = :imgUid";
        query.prepare(prepare);
        query.bindValue(":imgUid", imgUID );
        EXEC_QUERY(query);
        if (query.next() && query.value(0).toString() == "true") {
            break;
        }
        // qDebug() << "waiting for image to be processed";
        CommonHelper::msleep(100);
    }

    if (quality == 100) {
        Tufao::HttpFileServer fileServer("");
        fileServer.serveFile(filepath, request, response);
        return true;
    }

    QImage image;
    image.load(filepath, "PNG");

    // scaling file
    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().replace("Content-Type", "image/png");


    QImage scaled;
    int sizeW = image.width() * quality / 100;
    int sizeH = image.height() * quality / 100;
    scaled = image.scaled(QSize(sizeW, sizeH),Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QByteArray ba;              // Construct a QByteArray object
    QBuffer buffer(&ba);        // Construct a QBuffer object using the QbyteArray
    scaled.save(&buffer, "PNG",-1); // Save the QImage data into the QBuffer
    response->end(ba);

    return true;
}

//--------------------------------------------------------------------------------------

bool QueryHandler::serveHeader( const QString& stdUid, const QString& serUid, const QString& imgUid, 
    Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response )
{
    SqlDatabase sql(pacsnode::storageLocation());
    {
        // check if the image is local
        QSqlQuery query(sql.openConnection());
        QString prepare = "SELECT processed FROM image WHERE SOPInstanceUID = :imgUid";
        query.prepare(prepare);
        query.bindValue(":imgUid", imgUid);
        EXEC_QUERY(query);
        if (!query.next()) {
            qWarning() << "Series not local, requesting from PACS";
            MoveRequestor::instance()->requestMove(stdUid, serUid, imgUid);
            MoveRequestor::instance()->requestMove(stdUid, serUid, ""); // also requesting series
            response->writeHead(Tufao::HttpServerResponse::NO_CONTENT);
            response->end();
            return true;
        }
    }

    QSqlQuery query(sql.openConnection());
    QStringList selection;
    selection << "Columns";
    selection << "InstanceNumber";
    selection << "PhotometricInterpretation";
    selection << "PixelSpacing";
    selection << "RescaleIntercept";
    selection << "RescaleSlope";
    selection << "Rows";
    selection << "WindowCenter";
    selection << "WindowWidth";

    QString prepare = "SELECT " + selection.join(",") + " FROM image WHERE SOPInstanceUID = :imgUid";
    query.prepare(prepare);
    query.bindValue(":imgUid", imgUid);
    EXEC_QUERY(query);
    QJsonObject root;
    if (query.next()) {
        int i = 0;
        root.insert("DCM_Columns", query.value(i++).toString());
        root.insert("DCM_InstanceNumber", query.value(i++).toString());
        root.insert("DCM_PhotoMetricInterpretation", query.value(i++).toString());
        root.insert("DCM_PixelSpacing", query.value(i++).toString());
        root.insert("DCM_RescaleIntercept", query.value(i++).toString());
        root.insert("DCM_RescaleSlope", query.value(i++).toString());
        root.insert("DCM_Rows", query.value(i++).toString());
        root.insert("DCM_WindowCenter", query.value(i++).toString());
        root.insert("DCM_WindowWidth", query.value(i++).toString());
    }
 
    QJsonDocument doc(root);
    response->writeHead(Tufao::HttpServerResponse::OK);
    response->headers().replace("Content-Type", "application/json");
    response->end(doc.toJson());
    return true;
}

//--------------------------------------------------------------------------------------

QList<PatientData> QueryHandler::getStudyInfo( const QString& patientName, const QString& studyUid, const QList<DicomNode>& nodes )
{
    QList<PatientData> result;

    DicomObject attributes;
    attributes.append(DicomElement(DCM_QueryRetrieveLevel, "STUDY"));
    attributes.append(DicomElement(DCM_PatientName, patientName));
    attributes.append(DicomElement(DCM_StudyInstanceUID, studyUid));
    attributes.append(DicomElement(DCM_PatientID, ""));
    attributes.append(DicomElement(DCM_PatientBirthDate, ""));
    attributes.append(DicomElement(DCM_PatientSex, ""));
    attributes.append(DicomElement(DCM_ReferringPhysicianName, ""));
    attributes.append(DicomElement(DCM_AccessionNumber, ""));
    attributes.append(DicomElement(DCM_StudyDescription, ""));
    attributes.append(DicomElement(DCM_NumberOfStudyRelatedSeries, ""));
    attributes.append(DicomElement(DCM_ModalitiesInStudy,""));
    attributes.append(DicomElement(DCM_StudyDate,""));
    attributes.append(DicomElement(DCM_StudyTime,""));

    MultiFinder find;
    QList<DicomObject> queryResult = find.doFind(nodes, attributes);
    // qDebug() << "Number of (merged) studies found:" << queryResult.count();

    // iterate studies
    foreach(const DicomObject study, queryResult) {
        PatientData studyData;
        studyData.DD_DCM_PatientName            = DicomElement::findValue(study.toSet(), DCM_PatientName);
        studyData.DD_DCM_StudyInstanceUID       = DicomElement::findValue(study.toSet(), DCM_StudyInstanceUID);
        studyData.DD_DCM_PatientID              = DicomElement::findValue(study.toSet(), DCM_PatientID);
        studyData.DD_DCM_PatientsBirthDate      = DicomHelper::convertDate(DicomElement::findValue(study.toSet(), DCM_PatientBirthDate));
        studyData.DD_DCM_PatientSex             = DicomElement::findValue(study.toSet(), DCM_PatientSex);
        studyData.DD_DCM_ReferringPhysicianName = DicomElement::findValue(study.toSet(), DCM_ReferringPhysicianName);
        studyData.DD_DCM_AccessionNumber        = DicomElement::findValue(study.toSet(), DCM_AccessionNumber);
        studyData.DD_DCM_StudyDescription       = DicomElement::findValue(study.toSet(), DCM_StudyDescription);
        studyData.DD_DCM_StudyRelatedSeries     = DicomElement::findValue(study.toSet(), DCM_NumberOfStudyRelatedSeries);
        studyData.DD_DCM_ModalitiesInStudy      = DicomElement::findValue(study.toSet(), DCM_ModalitiesInStudy);
        studyData.DD_DCM_StudyDate              = DicomHelper::convertDate(DicomElement::findValue(study.toSet(), DCM_StudyDate));
        studyData.DD_DCM_StudyTime              = DicomHelper::convertTime(DicomElement::findValue(study.toSet(), DCM_StudyTime));
        result << studyData;
    }
    return result;
}

//--------------------------------------------------------------------------------------

QList<PatientData> QueryHandler::getSeriesInfo( const QString& studyUid, const QString& seriesUid, const QList<DicomNode>& nodes, bool includeInstances )
{
    QList<PatientData> result;
    DicomObject attributes;
    attributes.append(DicomElement(DCM_QueryRetrieveLevel, "SERIES"));
    attributes.append(DicomElement(DCM_StudyInstanceUID, studyUid));
    attributes.append(DicomElement(DCM_SeriesInstanceUID, seriesUid));
    attributes.append(DicomElement(DCM_SeriesDescription, ""));
    attributes.append(DicomElement(DCM_Modality, ""));
    attributes.append(DicomElement(DCM_SeriesNumber, ""));
    attributes.append(DicomElement(DCM_BodyPartExamined, ""));
    attributes.append(DicomElement(DCM_PatientPosition, ""));
    attributes.append(DicomElement(DCM_SeriesDate, ""));
    attributes.append(DicomElement(DCM_SeriesTime, ""));
    attributes.append(DicomElement(DCM_ProtocolName, ""));
    attributes.append(DicomElement(DCM_NumberOfSeriesRelatedInstances, ""));

    MultiFinder find;
    QList<DicomObject> queryResult = find.doFind(nodes, attributes);
    // qDebug() << "Number of (merged) series found:" << queryResult.count();

    foreach(const DicomObject series, queryResult) {
        PatientData seriesData;
        seriesData.DD_DCM_StudyInstanceUID       = DicomElement::findValue(series.toSet(), DCM_StudyInstanceUID);
        seriesData.DD_DCM_SeriesInstanceUID      = DicomElement::findValue(series.toSet(), DCM_SeriesInstanceUID);
        seriesData.DD_DCM_SeriesDescription      = DicomElement::findValue(series.toSet(), DCM_SeriesDescription);
        seriesData.DD_DCM_Modality               = DicomElement::findValue(series.toSet(), DCM_Modality);
        seriesData.DD_DCM_SeriesNumber           = DicomElement::findValue(series.toSet(), DCM_SeriesNumber);
        seriesData.DD_DCM_BodyPartExamined       = DicomElement::findValue(series.toSet(), DCM_BodyPartExamined);
        seriesData.DD_DCM_PatientPosition        = DicomElement::findValue(series.toSet(), DCM_PatientPosition);
        seriesData.DD_DCM_SeriesDate             = DicomHelper::convertDate(DicomElement::findValue(series.toSet(), DCM_SeriesDate));
        seriesData.DD_DCM_SeriesTime             = DicomHelper::convertTime(DicomElement::findValue(series.toSet(), DCM_SeriesTime));
        seriesData.DD_DCM_ProtocolName           = DicomElement::findValue(series.toSet(), DCM_ProtocolName);
        seriesData.DD_DCM_SeriesRelatedInstances = DicomElement::findValue(series.toSet(), DCM_NumberOfSeriesRelatedInstances);

        // add list of sop-instances
        if (includeInstances) {
            TableFinder finder;
            QStringList localSOPs = finder.sortedImageUIDs(seriesData.DD_DCM_StudyInstanceUID, seriesData.DD_DCM_SeriesInstanceUID);
            QStringList remoteSOPs = getSortedSOPInstances(studyUid, seriesUid, nodes);

            // if local matches remote we prefer local (instance number supported)
            if (localSOPs.count() == remoteSOPs.count()) {
                seriesData.DD_LocalInstances = localSOPs.join("#");
            }
            else {
                seriesData.DD_LocalInstances = remoteSOPs.join("#");
            }
        }

        result << seriesData;
    }
    return result;
}

//--------------------------------------------------------------------------------------

QStringList QueryHandler::getSortedSOPInstances( const QString& studyUid, const QString& seriesUid, const QList<DicomNode>& nodes)
{
    DicomObject attributes;
    attributes.append(DicomElement(DCM_QueryRetrieveLevel, "IMAGE"));
    attributes.append(DicomElement(DCM_StudyInstanceUID, studyUid));
    attributes.append(DicomElement(DCM_SeriesInstanceUID, seriesUid));
    attributes.append(DicomElement(DCM_SOPInstanceUID, ""));
    attributes.append(DicomElement(DCM_InstanceNumber, ""));

    MultiFinder find;
    QList<DicomObject> queryResult = find.doFind(nodes, attributes);

    QMap<int, QString > sortMap;
    foreach(const DicomObject series, queryResult) {
        QString uid = DicomElement::findValue(series.toSet(), DCM_SOPInstanceUID);
        QString nb  = DicomElement::findValue(series.toSet(), DCM_InstanceNumber);
        sortMap[nb.toInt()] = uid;
    }

    QStringList result;
    QMapIterator<int, QString> iter(sortMap);
    while (iter.hasNext()) {
        iter.next();
        result << iter.value();
    }

    return result;
}

//--------------------------------------------------------------------------------------

pacsnode::PatientData QueryHandler::mergePatientData( const PatientData& a, const PatientData& b )
{
    QStringList aList = a.toList();
    QStringList bList = b.toList();
    QStringList merged;
    for (int i=0; i < aList.count(); i++) {
        QString attr = aList.at(i);
        QString bAttr = bList.at(i);
        if (!bAttr.isEmpty()) {
            attr = bAttr;
        }
        merged << attr;
    }

    return PatientData::fromList(merged);
}

//--------------------------------------------------------------------------------------

Tufao::AbstractHttpServerRequestHandler* createHandler()
{
    return new QueryHandler();
}

//--------------------------------------------------------------------------------------


}
