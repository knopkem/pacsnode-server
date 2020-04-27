#include "findScu.h"

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dfindscu.h"

#ifdef HAVE_WINDOWS_H
// on Windows, we need Winsock2 for network functions
#include <winsock2.h>
#endif

namespace {
    OFString convertElement(const pacsnode::DicomElement& element) {
        DcmTagKey tag = element.XTag();
        enum { DefaultFieldWidth = 0, Base16 = 16 };
        QString queryStr( QString("%1,%2=%3")
            .arg(tag.getGroup(), DefaultFieldWidth, Base16)
            .arg(tag.getElement(), DefaultFieldWidth, Base16)
            .arg(element.valueField()) );
        return OFString(queryStr.toLatin1().constData());
    }
}

namespace pacsnode {

class FindScuCallback: public DcmFindSCUCallback
{
public:

    FindScuCallback(const DicomObject& rqContainer, QList< DicomObject >* rspContainer);

    ~FindScuCallback() {}

    void callback(
        T_DIMSE_C_FindRQ *request,
        int &responseCount,
        T_DIMSE_C_FindRSP *rsp,
        DcmDataset *responseIdentifiers);

private:

    DicomObject  m_requestContainer;
    QList< DicomObject >* m_responseContainer;
};


FindScuCallback::FindScuCallback( const DicomObject& rqContainer,  QList< DicomObject >* rspContainer ) 
    : m_requestContainer(rqContainer), m_responseContainer(rspContainer)
{

}

void FindScuCallback::callback( T_DIMSE_C_FindRQ *request, int& responseCount, T_DIMSE_C_FindRSP *rsp, DcmDataset *responseIdentifiers )
{
    DicomObject responseItem;
    foreach(const DicomElement& dicomElement, m_requestContainer) {
        OFString value;
        responseIdentifiers->findAndGetOFStringArray(dicomElement.XTag(), value);
        responseItem.append(DicomElement(dicomElement.XTag(), value.c_str()));
    }
    m_responseContainer->append(responseItem);
}


FindScu::FindScu()
{

}

FindScu::~FindScu()
{

}

QList< DicomObject > FindScu::executeFindRequest( const DicomNode& host, const DicomNode& peer, const DicomObject& queryAttributes )
{
    QList< DicomObject > result;

    // fix trailing stuff, our db lookup is currently very sensitive to how the data was stored
    dcmEnableAutomaticInputDataCorrection.set(OFTrue);

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif


    DcmFindSCU scu;

    OFCondition cond = scu.initializeNetwork(30);
    if (cond.bad()) {
        qCritical() << "network initialization failed" << cond.text();
        return result;
    }

    // map attributes
    OFList<OFString> overrideKeys;

    foreach(const DicomElement& element, queryAttributes) {
        OFString key = convertElement(element);
        overrideKeys.push_back(key);
    }

    FindScuCallback callback(queryAttributes, &result);

    // do the main work: negotiate network association, perform C-FIND transaction,
    // process results, and finally tear down the association.
    cond = scu.performQuery(
        peer.address().toLatin1().constData(),
        peer.port(),
        host.title().toUtf8().constData(),
        peer.title().toUtf8().constData(),
        UID_FINDStudyRootQueryRetrieveInformationModel,
        EXS_Unknown,
        DIMSE_NONBLOCKING,
        60,
        ASC_DEFAULTMAXPDU,
        OFFalse,
        OFFalse,
        1,
        DcmFindSCUExtractMode::FEM_none,
        0,
        &overrideKeys,
        &callback,
        NULL,
        NULL);

    // destroy network structure
    cond = scu.dropNetwork();
    if (cond.bad()) {
        qCritical() << "network release failed" << cond.text();
    }

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

    return result;
}

}
