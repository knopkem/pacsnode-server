#include "moveScu.h"


#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmnet/scu.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */
#include "dcmtk/dcmdata/dcpath.h"

#include <QDebug>

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

    
static void prepareTS(E_TransferSyntax ts,
                      OFList<OFString>& syntaxes)
{
  /*
  ** We prefer to use Explicitly encoded transfer syntaxes.
  ** If we are running on a Little Endian machine we prefer
  ** LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
  ** Some SCP implementations will just select the first transfer
  ** syntax they support (this is not part of the standard) so
  ** organize the proposed transfer syntaxes to take advantage
  ** of such behavior.
  **
  ** The presentation contexts proposed here are only used for
  ** C-FIND and C-MOVE, so there is no need to support compressed
  ** transmission.
  */

  switch (ts)
  {
    case EXS_LittleEndianImplicit:
      /* we only support Little Endian Implicit */
      syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      break;
    case EXS_LittleEndianExplicit:
      /* we prefer Little Endian Explicit */
      syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
    case EXS_BigEndianExplicit:
      /* we prefer Big Endian Explicit */
      syntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
#ifdef WITH_ZLIB
    case EXS_DeflatedLittleEndianExplicit:
      /* we prefer Deflated Little Endian Explicit */
      syntaxes.push_back(UID_DeflatedExplicitVRLittleEndianTransferSyntax);
      syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
#endif
    default:
      DcmXfer xfer(ts);
      if (xfer.isEncapsulated())
      {
        syntaxes.push_back(xfer.getXferID());
      }
      /* We prefer explicit transfer syntaxes.
       * If we are running on a Little Endian machine we prefer
       * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
       */
      if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
      {
        syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
        syntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      } else
      {
        syntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
        syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      }
      syntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
  }
}

static void applyOverrideKeys(DcmDataset *dataset,  OFList<OFString> overrideKeys )
{
    /* replace specific keys by those in overrideKeys */
    OFListConstIterator(OFString) path = overrideKeys.begin();
    OFListConstIterator(OFString) endOfList = overrideKeys.end();
    DcmPathProcessor proc;
    proc.setItemWildcardSupport(OFFalse);
    proc.checkPrivateReservations(OFFalse);
    OFCondition cond;
    while (path != endOfList)
    {
        cond = proc.applyPathWithValue(dataset, *path);
        if (cond.bad()) {
            qCritical() << "Bad override key" << cond.text();
        }
        path++;
    }
}
}


namespace pacsnode {

class MoveScuPrivate
{
public:
    DcmDataset* overrideKeys;
};


MoveScu::MoveScu() : d(new MoveScuPrivate)
{
    d->overrideKeys = new DcmDataset;
}

MoveScu::~MoveScu()
{
    delete d->overrideKeys;
    delete d;
    d = 0;
}

bool MoveScu::executeMoveRequest(const DicomNode& host, const DicomNode& peer, const DicomObject& queryAttributes)
{

    // map attributes
    OFList<OFString> overrideKeys;

    foreach(const DicomElement& element, queryAttributes) {
        OFString key = convertElement(element);
        overrideKeys.push_back(key);
    }

    // setup SCU
    OFList<OFString> syntaxes;
    prepareTS(EXS_Unknown, syntaxes);
    DcmSCU scu;
    scu.setMaxReceivePDULength(ASC_DEFAULTMAXPDU);
    scu.setACSETimeout(60);
    scu.setDIMSEBlockingMode(DIMSE_NONBLOCKING);
    scu.setDIMSETimeout(60);
    scu.setAETitle(host.title().toLatin1().constData());
    scu.setPeerHostName(peer.address().toLatin1().constData());
    scu.setPeerPort(OFstatic_cast(Uint16, peer.port()));
    scu.setPeerAETitle(peer.title().toLatin1().constData());

    scu.addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, syntaxes);

    /* initialize network and negotiate association */
    OFCondition cond = scu.initNetwork();
    if (cond.bad()) {
    qCritical() << "network initialization failed";
    return false;
    }

    cond = scu.negotiateAssociation();
    if (cond.bad()) {
        qCritical() << "association negotiation failed";
        return false;
    }

    cond = EC_Normal;
    T_ASC_PresentationContextID pcid = scu.findPresentationContextID(UID_MOVEStudyRootQueryRetrieveInformationModel, "");
    if (pcid == 0) {
    qCritical() << "No adequate Presentation Contexts for sending C-MOVE";
    return false;
    }

    // do the real work, i.e. send C-MOVE requests and receive response
    DcmDataset *dset = new DcmDataset;
    applyOverrideKeys(dset, overrideKeys);

    OFList<RetrieveResponse*> responses;
    cond = scu.sendMOVERequest(pcid, host.title().toLatin1().constData(), dset, &responses);
    if (cond.bad()) {
        qCritical() << "sending move request failed";
        return false;
    }

    if (!responses.empty()) {

        /* output final status report */
        qDebug() << "Final status report from last C-MOVE message:";
        RetrieveResponse* rsp = (*(--responses.end()));
        qDebug() << "Number of Remaining sub-operations : " << rsp->m_numberOfRemainingSubops;
        qDebug() << "Number of Completed sub-operations : " << rsp->m_numberOfCompletedSubops;
        qDebug() << "Number of Failed sub-operations    : " << rsp->m_numberOfFailedSubops;
        qDebug() << "Number of Warning sub-operations   : " << rsp->m_numberOfWarningSubops;

        /* delete responses */
        OFListIterator(RetrieveResponse*) iter = responses.begin();
        OFListConstIterator(RetrieveResponse*) last = responses.end();
        while (iter != last) {
        delete (*iter);
        iter = responses.erase(iter);
        }
    }

    // tear down association
    if (cond == EC_Normal) { 
        scu.releaseAssociation();
    }
    else {
    if (cond == DUL_PEERREQUESTEDRELEASE) {
        scu.closeAssociation(DCMSCU_PEER_REQUESTED_RELEASE);
    } 
    else if (cond == DUL_PEERABORTEDASSOCIATION) {
        scu.closeAssociation(DCMSCU_PEER_ABORTED_ASSOCIATION);
    } 
    else {
        qCritical() << "MOVE SCU Failed";
        scu.abortAssociation();
        return false;
    }
    }
    return true;
}

}
