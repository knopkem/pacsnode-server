#ifndef queryHandler_h__
#define queryHandler_h__


#include <QObject>
#include <abstracthttpserverrequesthandler.h>

namespace Tufao {
class HttpServerRequest;
class HttpServerResponse;
}

namespace pacsnode {

class DicomNode;
class PatientData;
class QueryHandler : public Tufao::AbstractHttpServerRequestHandler
{
    Q_OBJECT
public:
    explicit QueryHandler(QObject *parent = 0);
    virtual ~QueryHandler();

public slots:
    bool handleRequest(Tufao::HttpServerRequest *request,
                       Tufao::HttpServerResponse *response,
                       const QStringList &args = QStringList());

protected:

    bool handleStudyAndSeriesQuery(const QString& resource, Tufao::HttpServerRequest *request,
        Tufao::HttpServerResponse *response);

    bool handleStudyQuery(const QString& resource, Tufao::HttpServerRequest *request,
        Tufao::HttpServerResponse *response);

    bool handleSeriesQuery(const QString& resource, Tufao::HttpServerRequest *request,
        Tufao::HttpServerResponse *response);

    bool handleFullFrame(const QString& resource, Tufao::HttpServerRequest *request,
        Tufao::HttpServerResponse *response);

    bool handleThumbnail(const QString& resource, Tufao::HttpServerRequest *request,
        Tufao::HttpServerResponse *response);

    bool serveBuffer(const QString& filepath, int quality, Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response);

    bool serveHeader(const QString& stdUid, const QString& serUid, const QString& imgUid, 
        Tufao::HttpServerRequest *request, Tufao::HttpServerResponse *response);

    QList<PatientData> getStudyInfo(const QString& patientName, const QString& studyUid, const QList<DicomNode>& nodes);

    QList<PatientData> getSeriesInfo(const QString& studyUid, const QString& seriesUid, const QList<DicomNode>& nodes, bool includeInstances);

    QStringList getSortedSOPInstances(const QString& studyUid, const QString& seriesUid, const QList<DicomNode>& nodes);

    PatientData mergePatientData(const PatientData& a, const PatientData& b);

private:
};

Tufao::AbstractHttpServerRequestHandler* createHandler();

}

#endif // queryHandler_h__

