#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("退卡显示客户端");

    msg_list_all = new QStringList;
    msg_list_1 = new QStringList;
    msg_list_2 = new QStringList;
    msg_list_3 = new QStringList;
    msg_list_4 = new QStringList;

    // 添加combobox
    ui->comboBox->addItem("全部打印机");
    ui->comboBox->addItem("1号打印机");
    ui->comboBox->addItem("2号打印机");
    ui->comboBox->addItem("3号打印机");
    ui->comboBox->addItem("4号打印机");
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_conbox_index_chaged(int)));


    // 设置状态栏
    connect_state = new QLabel(this);
    ui->statusBar->addWidget(connect_state);
    current_time = new QLabel(this);
    ui->statusBar->addPermanentWidget(current_time);
    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(timer_out()));
    timer->start(1000);


    timer_tcp = new QTimer;
    connect(timer_tcp,SIGNAL(timeout()),this,SLOT(timer_tcp_out()));


    // 连接服务器
    QString userName = "moniclient";
    QString userPwd = "123433";
    nwthread = new NWThread(this,userName, userPwd);
    connect(nwthread,SIGNAL(signal_login_result(bool,QString)),this,SLOT(slot_loginResult(bool,QString)));
    connect(nwthread,SIGNAL(signal_nw_data_avaliable(QString)),this,SLOT(slot_4G_Data(QString)));
    nwthread->start();
    connect_state->setText("正在连接服务器...");
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
        connect_state->setStyleSheet("color: green");
        connect_state->setText("服务器连接成功！");
    }
    else
    {
        connect_state->setStyleSheet("color: red");
        connect_state->setText("服务器连接失败！");
        timer_tcp->start(10*1000);


    }

}

void MainWindow::slot_conbox_index_chaged(int index)
{
    qDebug() << "charged index = " << index;

    int select_index = index;
    QString current_info_show;

    switch (select_index)
    {
        case PRINTER_ALL:
            for(int i=0; i<msg_list_all->count(); i++)
                current_info_show.append(msg_list_all->at(i));
            break;
        case PRINTER_ONE:
            for(int i=0; i<msg_list_1->count(); i++)
                current_info_show.append(msg_list_1->at(i));
            break;
        case PRINTER_TWO:
            for(int i=0; i<msg_list_2->count(); i++)
                current_info_show.append(msg_list_2->at(i));
            break;
        case PRINTER_THREE:
            for(int i=0; i<msg_list_3->count(); i++)
                current_info_show.append(msg_list_3->at(i));
            break;
        case PRINTER_FOUR:
            for(int i=0; i<msg_list_4->count(); i++)
                current_info_show.append(msg_list_4->at(i));
            break;
        default:
            break;
    }

    ui->TextEdit1->setPlainText(current_info_show);
    ui->TextEdit1->moveCursor(QTextCursor::End);
}

void MainWindow::timer_out()
{
    static int i = 1;
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    current_time->setText(time);


    // begin test
    i++;
    if(i>4) i=1;
    QString temp = (QString("%1 卡号：%2  姓名：钢铁侠  状态： 正在打印成绩 \n").arg(time)).arg(i);
    msg_list_all->append(temp);
//    qDebug() <<temp;
    if(i == 1) msg_list_1->append(temp);
    if(i == 2) msg_list_2->append(temp);
    if(i == 3) msg_list_3->append(temp);
    if(i == 4) msg_list_4->append(temp);
    data_refresh();
    // end test
}

void MainWindow::timer_tcp_out()
{
        timer_tcp->stop();
        // 连接服务器
        QString userName = "moniclient";
        QString userPwd = "123433";
        nwthread = new NWThread(this,userName, userPwd);
        connect(nwthread,SIGNAL(signal_login_result(bool,QString)),this,SLOT(slot_loginResult(bool,QString)));
        connect(nwthread,SIGNAL(signal_nw_data_avaliable(QString)),this,SLOT(slot_4G_Data(QString)));
        nwthread->start();
        connect_state->setStyleSheet("color: black");
        connect_state->setText("正在连接服务器...");
}

void MainWindow::slot_4G_Data(QString str)
{

    int data_top,data_bottom;
    QString framehead= "$";
    QString frametail= "\r\n";

    data_top=str.indexOf(framehead,0);
    data_bottom=str.indexOf(frametail,0);

    QString net_data=str.mid((data_top),(data_bottom-3));//去掉"*AA\r\n"字段

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
    //解析数据
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

    //生成待显示数据
    QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString stage = "";
    QString show_info_stage = show_info.stage;

    if(show_info_stage == "querying")
        stage = "正在查询成绩";
    else if(show_info_stage == "queryok")
        stage = "成绩查询成功";
    else if(show_info_stage == "printing")
        stage = "正在你打印成绩";
    else if(show_info_stage == "printok")
        stage = "成绩打印成功";
    else if(show_info_stage == "printerr")
        stage = "成绩打印失败";
    else if(show_info_stage == "backcarding")
        stage = "正在退卡";
    else if(show_info_stage == "backcardok")
        stage = "退卡成功";
    else if (show_info_stage == "backcarderr")
        stage = "退卡失败";
    else
        stage = "异常";
    QString info = (QString("%1 卡号：%2  姓名：%3  状态： %4 \n").arg(current_date).arg(show_info.cardid).arg(show_info.name).arg(stage));


    // 插入待显示数据
    QString comid = show_info.comid;
    if(msg_list_all->count()>MAX_LIST)  msg_list_all->pop_front();
    msg_list_all->append(info);
    if(comid == "101")
    {
       if(msg_list_1->count()>MAX_LIST)  msg_list_1->pop_front();
        msg_list_1->append(info);
    }
    else if(comid == "102")
    {
        if(msg_list_2->count()>MAX_LIST)  msg_list_2->pop_front();
        msg_list_2->append(info);
    }
    else if(comid == "103")
    {
        if(msg_list_3->count()>MAX_LIST)  msg_list_3->pop_front();
       msg_list_3->append(info);
    }
    else if(comid == "104")
    {
        if(msg_list_4->count()>MAX_LIST)  msg_list_4->pop_front();
        msg_list_4->append(info);
    }


    // 显示数据
    int select_index = ui->comboBox->currentIndex();
    QString current_info_show;
    switch (select_index)
    {
        case PRINTER_ALL:
            for(int i=0; i<msg_list_all->count(); i++)
                current_info_show.append(msg_list_all->at(i));
            break;
        case PRINTER_ONE:
            for(int i=0; i<msg_list_1->count(); i++)
                current_info_show.append(msg_list_1->at(i));
            break;
        case PRINTER_TWO:
            for(int i=0; i<msg_list_2->count(); i++)
                current_info_show.append(msg_list_2->at(i));
            break;
        case PRINTER_THREE:
            for(int i=0; i<msg_list_3->count(); i++)
                current_info_show.append(msg_list_3->at(i));
            break;
        case PRINTER_FOUR:
            for(int i=0; i<msg_list_4->count(); i++)
                current_info_show.append(msg_list_4->at(i));
            break;
        default:
            break;
    }

    ui->TextEdit1->setPlainText(current_info_show);
    ui->TextEdit1->moveCursor(QTextCursor::End);
}

void MainWindow::data_refresh()
{
    int select_index = ui->comboBox->currentIndex();
    QString current_info_show;

    switch (select_index)
    {
        case PRINTER_ALL:
            for(int i=0; i<msg_list_all->count(); i++)
                current_info_show.append(msg_list_all->at(i));
            break;
        case PRINTER_ONE:
            for(int i=0; i<msg_list_1->count(); i++)
                current_info_show.append(msg_list_1->at(i));
            break;
        case PRINTER_TWO:
            for(int i=0; i<msg_list_2->count(); i++)
                current_info_show.append(msg_list_2->at(i));
            break;
        case PRINTER_THREE:
            for(int i=0; i<msg_list_3->count(); i++)
                current_info_show.append(msg_list_3->at(i));
            break;
        case PRINTER_FOUR:
            for(int i=0; i<msg_list_4->count(); i++)
                current_info_show.append(msg_list_4->at(i));
            break;
        default:
            break;
    }

    ui->TextEdit1->setPlainText(current_info_show);
    ui->TextEdit1->moveCursor(QTextCursor::End);
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


void MainWindow::on_clear_btn_clicked()
{
    msg_list_all->clear();
    msg_list_1->clear();
    msg_list_2->clear();
    msg_list_3->clear();
    msg_list_4->clear();
    ui->TextEdit1->clear();
}
