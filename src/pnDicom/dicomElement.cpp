#include "dicomElement.h"

namespace pacsnode {

//--------------------------------------------------------------------------------------

DicomElement::DicomElement()
{

}

//--------------------------------------------------------------------------------------

DicomElement::DicomElement( const DcmTagKey& key, const QString& valueString )
{
    _group = key.getGroup();
    _elem  = key.getElement();
    _valueString = valueString;
}

//--------------------------------------------------------------------------------------

DicomElement::DicomElement( const DicomElement& other )
{
    _group       = other._group;
    _elem        = other._elem;
    _valueString = other._valueString;
}

//--------------------------------------------------------------------------------------

DicomElement& DicomElement::operator=( const DicomElement& other )
{
    _group       = other._group;
    _elem        = other._elem;
    _valueString = other._valueString;
    return *this;
}

//--------------------------------------------------------------------------------------

DcmTagKey DicomElement::XTag() const
{
    return DcmTagKey(_group, _elem);
}

//--------------------------------------------------------------------------------------

QString DicomElement::valueField() const
{
    return _valueString;
}

//--------------------------------------------------------------------------------------

void DicomElement::setValue( const QString& value )
{
    _valueString = value;
}

//--------------------------------------------------------------------------------------

void DicomElement::registerMetaType()
{
    qRegisterMetaType<DicomElement>("DicomElement");
    qRegisterMetaType< QList< DicomElement > >("DicomObject");
    qRegisterMetaType< QList< QList< DicomElement > > >("QList< DicomObject >");
}

//--------------------------------------------------------------------------------------

QString DicomElement::findValue(const QSet<DicomElement>& input, const DcmTagKey& key)
{
    QString result;
    QSet<DicomElement>::const_iterator iter = input.find(DicomElement(key, ""));
    if ( iter != input.end() ) {
        result = iter->valueField();
    }
    return result;
}

//--------------------------------------------------------------------------------------

bool operator==( const DicomElement& e1, const DicomElement& e2 )
{
    return ( e1.XTag() == e2.XTag() );
}

//--------------------------------------------------------------------------------------------

bool operator!=( const DicomElement& e1, const DicomElement& e2 )
{
    return !(e1 == e2);
}

//--------------------------------------------------------------------------------------------

uint qHash( const DicomElement &key )
{
    return key.XTag().getGroup() ^ key.XTag().getElement();
}
//--------------------------------------------------------------------------------------------
}
