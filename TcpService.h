#ifndef TCPSERVICE_H
#define TCPSERVICE_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QNetworkInterface>
#include "tcpclient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class TcpService ;
}
QT_END_NAMESPACE

class TcpService : public QMainWindow {
    Q_OBJECT
public:
    TcpService(QWidget *parent = nullptr);
    ~TcpService();

private slots:
    void on_btnSend_clicked();
    void on_btnClear_clicked();
    void on_btnOpenClientWnd_clicked();
    void on_btnconnect_clicked();
    void newConnectSlot();
private:
    Ui::TcpService *ui;
    QTcpServer *tcpServer;
    QList<QTcpSocket *> tcpClient;
    QTcpSocket *currentClient;
    TcpClient *m_tcpClient;
    unsigned short clientNum = 0;
};
#endif // TCPSERVICE_H test
