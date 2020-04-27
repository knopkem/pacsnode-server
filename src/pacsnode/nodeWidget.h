#ifndef nodeWidget_h__
#define nodeWidget_h__

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>

namespace pacsnode {

/**
 * A widget that shows/edit properties of a DICOM node
 */
class NodeWidget : public QWidget
{
    Q_OBJECT
public:
    NodeWidget(QString aet, QString address, QString port, bool showButtons, QWidget *parent = NULL);

    QString uid();

    QString aet() {
        return _aetLE.text();
    }
    QString address() {
        return _addLE.text();
    }
    QString port() {
        return _portLE.text();
    }

Q_SIGNALS:
    void updateRequest();
    void echo(QString aet, QString address, QString port, QString uid);
    void remove(QString uid);

    public Q_SLOTS:
        void onRemove();
        void onEcho();
        void commitRequest();

protected:

    QString     _uid;
    QLineEdit   _aetLE;
    QLineEdit   _addLE;
    QLineEdit   _portLE;
    QPushButton _removeButton;
    QPushButton _echoButton;
    QHBoxLayout _hbox;
};

}

#endif // nodeWidget_h__
