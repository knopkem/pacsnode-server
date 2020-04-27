#ifndef nodeEditWidget_h__
#define nodeEditWidget_h__

#include <QWidget>

namespace pacsnode {

class NodeEditWidgetPrivate;

/**
 * Widget to add, remove or edit Dicom Node information
 */
class NodeEditWidget : public QWidget
{
    Q_OBJECT
public: 
    NodeEditWidget(QWidget* parent= NULL);
    virtual ~NodeEditWidget();

    QString aeTitle (void) const;
    QString address (void) const;
    QString port    (void) const;

Q_SIGNALS:
    void deleteNode(void);
    void cancel(void);
    void save(void);

public Q_SLOTS:
    void setAETitle  (const QString &value);
    void setAddress  (const QString &value);
    void setPort     (const QString &value);
private:

    NodeEditWidgetPrivate* d;
};

}
#endif // nodeEditWidget_h__

