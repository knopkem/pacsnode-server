#include "dicomNode.h"

namespace pacsnode {

DicomNode::DicomNode()
{
    m_port = 0;
    m_fetchable = false;
}

//--------------------------------------------------------------------------------------------

DicomNode::DicomNode( const DicomNode& clone ): m_title(clone.m_title), m_address(clone.m_address),
    m_port(clone.m_port), m_fetchable(clone.m_fetchable)
{

}

//--------------------------------------------------------------------------------------------

DicomNode::DicomNode( const QString& title, const QString& address, int port, bool fetchable )
{
    m_title = title;
    m_address = address;
    m_port = port;
    m_fetchable = fetchable;
}

//--------------------------------------------------------------------------------------------

DicomNode& DicomNode::operator=( const DicomNode& clone )
{
    m_title = clone.m_title;
    m_address = clone.m_address;
    m_port = clone.m_port;
    m_fetchable = clone.m_fetchable;
    return *this;
}

//--------------------------------------------------------------------------------------------

DicomNode::~DicomNode()
{

}

//--------------------------------------------------------------------------------------------

void DicomNode::setTitle( QString title )
{
    m_title = title;
}

//--------------------------------------------------------------------------------------------

void DicomNode::setAddress( QString address )
{
    m_address = address;
}

//--------------------------------------------------------------------------------------------

void DicomNode::setPort( int port )
{
    m_port = port;
}

//--------------------------------------------------------------------------------------------

void DicomNode::setPortAsString( const QString& port )
{
    try  {
        bool ok;
        setPort(port.toInt( & ok, 10 ));
    }
    catch (...) {
        qWarning() << "Could not convert port to int";
    }
}

//--------------------------------------------------------------------------------------------

QString DicomNode::title( void ) const
{
    return m_title;
}

//--------------------------------------------------------------------------------------------

QString DicomNode::address( void ) const
{
    return m_address;
}

//--------------------------------------------------------------------------------------------

int DicomNode::port( void ) const
{
    return m_port;
}

//--------------------------------------------------------------------------------------------

QString DicomNode::portAsString( void ) const
{
    QString portStr;
    portStr.setNum(m_port);
    return portStr;
}

//--------------------------------------------------------------------------------------------

void DicomNode::registerMetaType()
{
    qRegisterMetaType<DicomNode>("DicomNode");
    qRegisterMetaTypeStreamOperators<DicomNode>("DicomNode");
    qRegisterMetaType< QList<DicomNode> >("DicomNodeList");
}

//--------------------------------------------------------------------------------------------

bool DicomNode::isValid()
{
    return ( !m_address.isEmpty() && !m_title.isEmpty() && (m_port != 0) );
}

//--------------------------------------------------------------------------------------------

void DicomNode::setFetchable( bool value )
{
    m_fetchable = value;
}

//--------------------------------------------------------------------------------------------

bool DicomNode::fetchable() const
{
    return m_fetchable;
}

//--------------------------------------------------------------------------------------------

QDataStream & operator << (QDataStream &out, const DicomNode &obj)
{
    out << obj.m_title << obj.m_address << (unsigned int)obj.m_port << obj.m_fetchable;
    return out;
}

//--------------------------------------------------------------------------------------------

QDataStream & operator >> (QDataStream &in, DicomNode &obj)
{
    in >> obj.m_title >> obj.m_address >> obj.m_port >> obj.m_fetchable;
    return in;
}

//--------------------------------------------------------------------------------------------

bool operator<( const DicomNode& node1, const DicomNode& node2 )
{
    return (node1.title()+node1.address()+QString::number(node1.port()) < node2.title()+node2.address()+QString::number(node2.port()));
}

//--------------------------------------------------------------------------------------------

uint qHash( const DicomNode &key )
{
    uint hash = 0;
    hash = hash ^ qHash(key.address());
    hash = hash ^ qHash(key.portAsString());
    hash = hash ^ qHash(key.title());
    return hash;
}

//--------------------------------------------------------------------------------------------

bool operator==( const DicomNode& node1, const DicomNode& node2 )
{
    return ( node1.title() == node2.title() &&
        node1.address() == node2.address() &&
        node1.port() == node2.port() );
}

//--------------------------------------------------------------------------------------------

bool operator!=( const DicomNode& node1, const DicomNode& node2 )
{
    return !(node1 == node2);
}

//--------------------------------------------------------------------------------------------

QList<DicomNode> pacsnode::DicomNode::convert( const QVariant &varStations )
{
    QList<DicomNode> nodeList;
    QVariantList nodeVarList;

    if (varStations.canConvert<QVariantList>()) {
        nodeVarList = varStations.value<QVariantList>();
        foreach(const QVariant& varNode, nodeVarList) {
            DicomNode node = varNode.value<DicomNode>();
            nodeList << node;
        }
    } 
    else {
        qWarning() << "Failed to read settings, list of nodes is invalid.";
    }
    return nodeList;
}

//--------------------------------------------------------------------------------------------
}
