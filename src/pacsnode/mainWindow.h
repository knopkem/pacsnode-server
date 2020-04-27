#ifndef mainWindow_h__
#define mainWindow_h__

#include <QDialog>
#include <QSystemTrayIcon>

namespace pacsnode {

class MainWindowPrivate;

/**
 * The main window of the desktop application
 *
 */
class MainWindow : public QDialog
{
    Q_OBJECT
public:
    MainWindow();
    virtual ~MainWindow(void);

protected slots:
    void onProcessStarted();
    void onNotify(const QString& msg);
    void openBrowser();
    void restartServer();
    void spinBoxChange(int);

private:
    MainWindowPrivate* d;
};

}

#endif // mainWindow_h__
