#ifndef dicomElement_h__
#define dicomElement_h__

#include "pnDicom_Export.h"

#include <QtCore>
#include <QUuid>

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctagkey.h"

namespace pacsnode {


/**
 * Class that holds DICOM attribute information
 */
class pnDicom_EXPORT DicomElement
{
public:


    /**
     * Default constructor needed
     */
    DicomElement();

    /**
     * Overload
     */
    DicomElement(const DcmTagKey& key, const QString& valueString);

    /**
     * Copy constructor
     */
    DicomElement(const DicomElement& other);

    /**
     * overrides assignment operator
     */
    DicomElement& operator =( const DicomElement& other );

    /**
     * The attribute without value field
     */
    DcmTagKey XTag() const;

    /**
     * The value as string
     */
    QString valueField() const;

    /**
     * set value
     */
    void setValue(const QString& value);

    /**
     * register meta type for Qt
     */
    static void registerMetaType();

    /**
     * return the value or an empty string in a set
     */
    static QString findValue(const QSet<DicomElement>& input, const DcmTagKey& key);

protected:

    qint32  _group;
    qint32  _elem;
    QString _valueString;

    friend QDataStream& operator <<( QDataStream& stream, const DicomElement& qElm ) {
        return stream << qElm._group << qElm._elem << qElm._valueString;
    }

    friend QDataStream& operator >>( QDataStream& stream, DicomElement& qElm ) {
        return stream >> qElm._group >> qElm._elem >> qElm._valueString;
    }
};

pnDicom_EXPORT bool operator==(const DicomElement& e1, const DicomElement& e2);
pnDicom_EXPORT bool operator!=(const DicomElement& e1, const DicomElement& e2);
pnDicom_EXPORT uint qHash(const DicomElement &key);

typedef QList<DicomElement> DicomObject;

}

Q_DECLARE_METATYPE(pacsnode::DicomElement)

#endif // dicomElement_h__

