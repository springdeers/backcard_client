#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString userName = "moniclient";
    QString userPwd = "123433";
    nwthread = new NWThread(this,userName, userPwd);
    connect(nwthread,SIGNAL(signal_login_result(bool,QString)),this,SLOT(slot_loginResult(bool,QString)));
    connect(nwthread,SIGNAL(signal_nw_data_avaliable(QString)),this,SLOT(slot_4G_Data(QString)));
    nwthread->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::slot_loginResult(bool bSuccess, QString strResult)
{
    strResult = strResult;
    qDebug() << "slot_loginResult:" << strResult;
    if(bSuccess)
    {
        ui->label->setText("登录成功");

    }
    else
    {
        ui->label->setText("登录失败");
    }

}



//slot..4G_Data_process
void MainWindow::slot_4G_Data(QString str)
{

    int data_top,data_bottom;
    QString framehead= "$";
    QString frametail= "\r\n";

    data_top=str.indexOf(framehead,0);
    data_bottom=str.indexOf(frametail,0);

    QString net_data=str.mid((data_top),(data_bottom-3));//去掉"*AA\r\n"字段


//    QString data_for_check = str.mid((data_top),(data_bottom));//去掉\r\n字段，进行校验

//    if( !checkSum(data_for_check) )//check string XOR result
//    {
//         qDebug()<<"recve error RDSSdata:"<<str;
//         return;
//    }

    qDebug() << "receive net data: " << net_data;

    // receive initack
    if(net_data.startsWith("$show")){
        data_show(net_data);
    }else{
         qDebug() << "input network data error";
    }
}


void MainWindow::data_show(QString str )
{
     show_info_st show_info;

    QString errmsg = "data is no error!";
    int len = str.indexOf("{");
    QString buf=str.right(str.length()-len);
    QByteArray data = buf.toUtf8();

    QJsonParseError jsonError;//Qt5新类
    QJsonDocument json = QJsonDocument::fromJson(data, &jsonError);//Qt5新类
    if (jsonError.error == QJsonParseError::NoError)
    {
        if (json.isObject())
        {
            QJsonObject rootObj = json.object();

            //是否含有key  stage
            if(rootObj.contains("stage"))
            {
                QJsonValue value = rootObj.value("stage");
                //判断是否是string类型
                if (value.isString()){
                    show_info.stage = value.toString();
                }else{
                    errmsg = "stage is not string!";
                }
            }else{
                errmsg = "stage is not contains";
            }

            //是否含有key  cardid
            if(rootObj.contains("cardid"))
            {
                QJsonValue value = rootObj.value("cardid");
                //判断是否是string类型
                if (value.isString()){
                    show_info.cardid = value.toString();
                }else{
                    errmsg = "cardid is not string!";
                }
            }else{
                errmsg = "cardid is not contains";
            }


            //是否含有key  comid
            if(rootObj.contains("comid"))
            {
                QJsonValue value = rootObj.value("comid");
                //判断是否是string类型
                if (value.isString()){
                    show_info.comid = value.toString();
                }else{
                    errmsg = "comid is not string!";
                }
            }else{
                errmsg = "comid is not contains";
            }


            //是否含有key  comid
            if(rootObj.contains("name"))
            {
                QJsonValue value = rootObj.value("name");
                //判断是否是string类型
                if (value.isString()){
                    show_info.name = value.toString();
                }else{
                    errmsg = "name is not string!";
                }
            }else{
                errmsg = "name is not contains";
            }

        }else{
            errmsg = "json is not a object";
        }
     }else
    {
        errmsg = "json is error";
    }

    qDebug()<<errmsg;
    qDebug()<<show_info.cardid;
    qDebug()<<show_info.name;
    qDebug()<<show_info.stage;

    QStringList msg_list;
    if(msg_list.count()>200)   msg_list.pop_front();
    msg_list.append(QString("卡号：%1  姓名：%2  状态： %3 ").arg(show_info.cardid).arg(show_info.name).arg(show_info.stage));
    if(msg_list.count()>0)
    {
        QString ret = msg_list.last();
        ui->TextEdit->appendPlainText(ret);
        ui->TextEdit->moveCursor(QTextCursor::End);
    }
}



int MainWindow::checkSum( QString & str)
{
    //"$BDBSI,02,03,2,4,3*62"  the str has delate \r\n
    QByteArray data = str.toUtf8();

     char mask = data[0];
     int datalen=data.length()-3;
     for (int i = 1; i <datalen; i++) {
         mask ^= data[i];
     }
     QString maskstr = QString::number(((mask & 0xff) >> 4),16);
     maskstr += QString::number((mask & 0x0f),16);
     maskstr = maskstr.toUpper();

     QString ret = str.right(2);

     if(maskstr == ret)
     {
         return 1;
     }else {
         qDebug() << "checksum Error!";
         return 0;
     }

}
