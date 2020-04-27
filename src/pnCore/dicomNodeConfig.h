#ifndef dicomNodeConfig_h__
#define dicomNodeConfig_h__

#include "dicomNode.h"

#include "pnCore_Export.h"

namespace pacsnode {

class DicomNodeConfigPrivate;

class pnCore_EXPORT DicomNodeConfig
{
public:
    DicomNodeConfig();
    ~DicomNodeConfig();

    void clearSettings();
    void setHttpPort(int port);
    int httpPort() const;

    void setHostNode(const DicomNode& node);
    DicomNode hostNode() const;

    QList<DicomNode> nodes() const;

    void addNode(const DicomNode& node);

    void clear();

    bool contains(const DicomNode& node) const;

private:

    void fetchNodes();

    DicomNodeConfigPrivate* d;
};

}
#endif // dicomNodeConfig_h__
