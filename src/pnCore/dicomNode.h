#ifndef DicomNode_h__
#define DicomNode_h__

// Qt
#include <QtCore>

#include "pnCore_Export.h"

namespace pacsnode {

/**
 * A DicomNode includes an AET an Address and a Port
 */
class pnCore_EXPORT DicomNode
{

public:

    /**
     * default constructor needed for meta type registration
     */
    DicomNode();

    /**
     * copy constructor
     */
    DicomNode(const DicomNode& clone);

    /**
     * overloaded constructor
     */
    DicomNode(const QString& title, const QString& address, int port, bool fetchable = false);

    /**
     * destructor
     */
    virtual ~DicomNode();

    /**
     * set AET
     */
    void setTitle(QString title);

    /**
     * set alias or IP address
     */
    void setAddress(QString address);

    /**
     * set port
     */
    void setPort(int port);

    /**
     * overloaded for convenience
     */
    void setPortAsString(const QString& port);

    /**
     * returns AET
     */
    QString title(void) const;

    /**
     * returns alias or IP address
     */
    QString address(void) const;

    /**
     * return ports
     */
    int port(void) const;

    /**
     * overload needs to have a different name
     */
    QString portAsString(void) const;

    /**
     * indicates if a node can be used for fetching images (store-scp)
     */
    void setFetchable(bool value);
    bool fetchable() const;

    /**
     * return false at least one value is empty
     */
    bool isValid();

    /**
     * overload assignment operator
     */
    DicomNode& operator=(const DicomNode& clone);

    /**
     * for using in hashes
     */
    //friend bool operator<(const DicomNode& node1, const DicomNode& node2);
    //friend uint qHash(const DicomNode &key);
    friend QDataStream & operator << (QDataStream &out, const DicomNode &obj);
    friend QDataStream & operator >> (QDataStream &in, DicomNode &obj);

    /**
     * register for Qt meta system
     */
    static void registerMetaType();

    /**
     * helper to convert from variant
     */
    static QList<DicomNode> convert(const QVariant &varStations);


private:

    QString         m_title;
    QString         m_address;
    int             m_port;
    bool            m_fetchable;

};

pnCore_EXPORT bool operator==(const DicomNode& node1, const DicomNode& node2);
pnCore_EXPORT bool operator!=(const DicomNode& node1, const DicomNode& node2);
pnCore_EXPORT uint qHash(const DicomNode &key);
pnCore_EXPORT bool operator<(const DicomNode& node1, const DicomNode& node2);

QDataStream & operator << (QDataStream &out, const DicomNode &obj);
QDataStream & operator >> (QDataStream &in, DicomNode &obj);

}

Q_DECLARE_METATYPE(pacsnode::DicomNode)
Q_DECLARE_METATYPE(QList<pacsnode::DicomNode>)

#endif // DicomNode_h__



