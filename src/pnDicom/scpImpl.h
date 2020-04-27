#ifndef scpImpl_h__
#define scpImpl_h__

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/scpthrd.h"

#include <QList>
#include "dicomElement.h"
#include "tableElement.h"

namespace pacsnode {

class ScpImplPrivate;
class ScpImpl : public DcmThreadSCP
{
public:
    ScpImpl();
    virtual ~ScpImpl();

protected:

    OFCondition handleIncomingCommand(T_DIMSE_Message *incomingMsg,
                                              const DcmPresentationContextInfo &presInfo);

    Uint16 checkAndProcessSTORERequest(const T_DIMSE_C_StoreRQ &reqMessage,
                                               DcmFileFormat &fileformat);

    OFCondition insertMetaData(DcmDataset* dataset, const OFString & filename);

    OFCondition generateSTORERequestFilename(const T_DIMSE_C_StoreRQ &reqMessage,
                                                     OFString &filename);

    void notifyInstanceStored(const OFString &filename,
                                      const OFString &sopClassUID,
                                      const OFString &sopInstanceUID,
                                      DcmDataset *dataset = NULL) const;

    OFCondition generateDirAndFilename(OFString &filename,
                                               OFString &directoryName,
                                               OFString &sopClassUID,
                                               OFString &sopInstanceUID,
                                               DcmDataset *dataset = NULL);


private:
    ScpImplPrivate* d;
};

}
#endif // scpImpl_h__


