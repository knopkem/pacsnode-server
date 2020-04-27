#include "dicomServer.h"

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmnet/scppool.h"
#include "dcmtk/dcmnet/scu.h"

#include <dcmtk/dcmjpeg/djdecode.h>     /* for dcmjpeg decoders */
#include <dcmtk/dcmjpeg/djencode.h>     /* for dcmjpeg encoder*/
#include <dcmtk/dcmdata/dcrledrg.h>     /* for DcmRLEDecoderRegistration */
#include <dcmtk/dcmdata/dcrleerg.h>     /* for DcmRLEEncoderRegistration */
#include <dcmtk/dcmjpls/djdecode.h>     /* for dcmjpls decoder */
#include <dcmtk/dcmjpls/djencode.h>     /* for dcmjpls encoder */

#include <QtCore>

#include "scpImpl.h"
#include "tableCreator.h"
#include "tableDefinition.h"
#include "sqlDatabase.h"
#include "storage.h"
#include "dicomNode.h"
#include "dicomNodeConfig.h"


namespace pacsnode {

struct SCPPoolImpl : DcmSCPPool<pacsnode::ScpImpl>, OFThread
{
    OFCondition result;
protected:
    void run()
    {
        qDebug() << "DICOM server (" << getConfig().getAETitle().c_str() << ") listening on port:" <<getConfig().getPort();
        result = listen();
    }
};


class DicomServerPrivate
{
public:
    SCPPoolImpl* pool;
};

DicomServer::DicomServer( QObject* parent /*= NULL*/ ) 
    : QObject(parent), d (new DicomServerPrivate)
{

    OFLog::configure(OFLogger::WARN_LOG_LEVEL);

    DcmRLEDecoderRegistration::registerCodecs();
    DJDecoderRegistration::registerCodecs();
    DJLSDecoderRegistration::registerCodecs();

    DcmRLEEncoderRegistration::registerCodecs();
    DJEncoderRegistration::registerCodecs();
    DJLSEncoderRegistration::registerCodecs();

    QList<pacsnode::TableElement> findAttributes = pacsnode::supportedAttributes();

    // first create table structure
    TableCreator creator(findAttributes);
    creator.init();

    // initialize sample node
    DicomNodeConfig nodeCfg;
    DicomNode host = nodeCfg.hostNode();

    d->pool = new SCPPoolImpl;
    d->pool->setMaxThreads(50);
    DcmSCPConfig& config = d->pool->getConfig();

    // Dead time during which the pool is unable to respond to
    // stopAfterCurrentAssociations().
    config.setConnectionTimeout(1);

    config.setAETitle(host.title().toLatin1().constData());
    config.setPort(host.port());
    config.setConnectionBlockingMode(DUL_NOBLOCK);

    OFList<OFString> xfers;
    xfers.push_back(UID_LittleEndianExplicitTransferSyntax);
    xfers.push_back(UID_LittleEndianImplicitTransferSyntax);
    config.addPresentationContext(UID_VerificationSOPClass, xfers);
    config.addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, xfers);
    config.addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, xfers);

    for (int i=0; i < numberOfDcmAllStorageSOPClassUIDs; i++) {
        config.addPresentationContext(dcmAllStorageSOPClassUIDs[i], xfers);
    }
}

DicomServer::~DicomServer()
{
    delete d->pool;
    delete d;
}

void DicomServer::start()
{
    qDebug() << "starting server...";
    d->pool->start();
}

void DicomServer::stop()
{
    qDebug() << "stopping server...";
    d->pool->stopAfterCurrentAssociations();
    d->pool->join();
}

}

