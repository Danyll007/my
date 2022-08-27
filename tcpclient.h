#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>

namespace Ui {
    class TcpClient;
}

class TcpClient : public QMainWindow {
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    bool sendFlag();
private:
    bool checkRcvData(const char *data);
private slots:
    void readData();
    void readError(QAbstractSocket::SocketError);
    void on_btnConnect_clicked();
    void on_btnSend_clicked();
    void on_pushButton_clicked();
private:
    Ui::TcpClient *ui;
    QTcpSocket *tcpSocketClient;
    QString sendData,revData;
    bool revState;
};

#endif // TCPCLIENT_H
