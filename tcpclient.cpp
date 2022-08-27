#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QMessageBox>

TcpClient::TcpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    //初始化TCP客户端
    tcpSocketClient = new QTcpSocket(this);   //实例化tcpClient
    tcpSocketClient->abort();                 //取消原有连接
    connect(tcpSocketClient, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(tcpSocketClient, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(ReadError(QAbstractSocket::SocketError)));
    ui->btnSend->setEnabled(false);
    setAttribute(Qt::WA_DeleteOnClose);
}

TcpClient::~TcpClient()
{
    if (tcpSocketClient->state() != QAbstractSocket::UnconnectedState) {
        tcpSocketClient->disconnectFromHost();
        if (tcpSocketClient->state() == QAbstractSocket::UnconnectedState
                || tcpSocketClient->waitForDisconnected(1000)) { //已断开连接则进入if{}
            ui->btnConnect->setText("connect");
            ui->btnSend->setEnabled(false);
        }
    }
    delete ui;
}

bool TcpClient::sendFlag()
{
    return sendData == "123aaa";
}

void TcpClient::readData()
{
    QByteArray buffer = tcpSocketClient->readAll();
    if (!buffer.isEmpty()) {
        ui->edtData->append(buffer);
    }
    sendData = buffer;
    revData=buffer;
    revState=true;
}

void TcpClient::readError(QAbstractSocket::SocketError )
{
    tcpSocketClient->disconnectFromHost();
    ui->btnConnect->setText(tr("连接"));
    QMessageBox msgBox;
    msgBox.setText(tr("failed to connect server because %1").arg(tcpSocketClient->errorString()));
}

void TcpClient::on_btnConnect_clicked()
{
    if(ui->btnConnect->text() == "connect") {
        tcpSocketClient->connectToHost(ui->edtIpAddress->text(), ui->edtPortNum->text().toInt());
        if(tcpSocketClient->waitForConnected(1000)) {
            ui->btnConnect->setText("disconnect");
            ui->btnSend->setEnabled(true);
        }
    }
    else {
        tcpSocketClient->disconnectFromHost();
        if (tcpSocketClient->state() == QAbstractSocket::UnconnectedState
                || tcpSocketClient->waitForDisconnected(1000)) { //已断开连接则进入if{}
            ui->btnConnect->setText("connect");
            ui->btnSend->setEnabled(false);
        }
    }
}

#include <QThread>
void TcpClient::on_btnSend_clicked()
{
//    QString data = ui->edtData->toPlainText();
    QString data = "st_focus\n";
    sendData.clear();
    if (!data.isEmpty())
        tcpSocketClient->write(data.toLatin1());
    bool result;
    result= checkRcvData(data.toLatin1());
}

void TcpClient::on_pushButton_clicked()
{
    ui->edtData->clear();
    bool state = sendFlag();
    QThread::msleep(100);
}

bool TcpClient::checkRcvData(const char *data)
{
    int count=0;
    revState=false;
    while(!revState && count<50){
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        QThread::msleep(20);
        count++;
    }
    if (count>=50)
        return false;
    return true;
}
