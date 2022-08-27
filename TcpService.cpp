#include "TcpService.h"
#include "ui_TcpService.h"

TcpService::TcpService(QWidget *parent):
    QMainWindow(parent),
    ui (new Ui::TcpService)
{
    ui->setupUi(this);
    tcpServer = new QTcpServer(this);
//    QList<QHostAddress> ll = QNetworkInterface().allAddresses();
    ui->edtIP->setText(QNetworkInterface().allAddresses().at(0).toString());
    ui->btnSend->setEnabled(false);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnectSlot()));
}

TcpService::~TcpService()
{
    for(int i = 0; i < tcpClient.length(); i++) { //断开所有连接
        tcpClient[i]->disconnectFromHost();
        //            bool ok = tcpClient[i]->waitForDisconnected(1000);
        //            if(!ok) {
        //                // 处理异常
        //            }
        tcpClient.removeAt(i);  //从保存的客户端列表中取去除
    }
    tcpServer->close();
    delete ui;
}

void TcpService::on_btnSend_clicked()
{
    QString data = ui->edtSendData->toPlainText();
    //全部连接
    if(ui->cboxClientName->currentIndex() == 0) {
        for(int i = 0; i < tcpClient.length(); i++)
            tcpClient[i]->write(data.toLatin1()); //qt5除去了.toAscii()
    }
    //指定连接
//    QString clientIP = ui->cboxClientName->currentText().split(":")[0];
//    int clientPort = ui->cboxClientName->currentText().split(":")[1].toInt();
//    for(int i = 0; i < tcpClient.length(); i++) {
//        if(tcpClient[i]->peerAddress().toString().split("::ffff:")[1] == clientIP\
//                && tcpClient[i]->peerPort() == clientPort) {
//            tcpClient[i]->write(data.toLatin1());
//            return; //ip:port唯一，无需继续检索
//        }
//    }
}

void TcpService::on_btnClear_clicked()
{
    ui->edtSendData->clear();
    ui->edtrRevData->clear();
}

void TcpService::on_btnOpenClientWnd_clicked()
{
    m_tcpClient = new TcpClient(this);
    m_tcpClient->setWindowTitle(QString("客户端:%1").arg(++clientNum));
    m_tcpClient->show();
}

void TcpService::on_btnconnect_clicked()
{
    if (ui->btnconnect->text() == "连接") {
        bool ok = tcpServer->listen(QHostAddress::Any, ui->edtPort->text().toInt());
        if(ok) {
            ui->btnconnect->setText("断开");
            ui->btnSend->setEnabled(true);
        }
    }
    else {
        for(int i = 0; i < tcpClient.length(); i++) { //断开所有连接
            tcpClient[i]->disconnectFromHost();
//            bool ok = tcpClient[i]->waitForDisconnected(1000);
//            if(!ok) {
//                // 处理异常
//            }
            tcpClient.removeAt(i);  //从保存的客户端列表中取去除
        }
        tcpServer->close();
        ui->btnconnect->setText("连接");
        ui->btnSend->setEnabled(false);
    }
}

void TcpService::newConnectSlot()
{
    tcpClient.append(currentClient = tcpServer->nextPendingConnection());
    ui->cboxClientName->addItem(tr("%1:%2").arg(currentClient->peerAddress().toString().split("::ffff:")[1])
                                .arg(currentClient->peerPort()));
    connect(currentClient, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(currentClient, SIGNAL(disconnected()), this, SLOT(disConnectSlot()));
}

void TcpService::disConnectSlot()
{
    //由于disconnected信号并未提供SocketDescriptor，所以需要遍历寻找
    for(int i = 0; i < tcpClient.length(); i++) {
        if(tcpClient[i]->state() == QAbstractSocket::UnconnectedState) {
            // 删除存储在combox中的客户端信息
            ui->cboxClientName->removeItem(ui->cboxClientName->findText(tr("%1:%2")\
                                           .arg(tcpClient[i]->peerAddress().toString().split("::ffff:")[1])\
                                           .arg(tcpClient[i]->peerPort())));
            // 删除存储在tcpClient列表中的客户端信息
            tcpClient[i]->destroyed();
            tcpClient.removeAt(i);
        }
    }
}

void TcpService::readData()
{
    // 客户端数据可读信号，对应的读数据槽函数
    // 由于readyRead信号并未提供SocketDecriptor，所以需要遍历所有客户端
    for(int i = 0; i < tcpClient.length(); i++) {
        QByteArray buffer = tcpClient[i]->readAll();
        if(buffer.isEmpty())
            continue;
        static QString IP_Port, IP_Port_Pre;
        IP_Port = tr("[%1:%2]:").arg(tcpClient[i]->peerAddress().toString().split("::ffff:")[1])\
                  .arg(tcpClient[i]->peerPort());
        // 若此次消息的地址与上次不同，则需显示此次消息的客户端地址
        if(IP_Port != IP_Port_Pre)
            ui->edtrRevData->append(IP_Port);
        ui->edtrRevData->append(buffer);
        tcpClient[i]->write(buffer);
        //更新ip_port
        IP_Port_Pre = IP_Port;
    }
}
