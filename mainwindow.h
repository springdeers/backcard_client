#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <nwthread.h>
#include <QTextCodec>
#include <QDateTime>
#include <QLabel>
#include <QTimer>

#define MAX_LIST 1000
#define PRINTER_ALL     0
#define PRINTER_ONE     1
#define PRINTER_TWO     2
#define PRINTER_THREE   3
#define PRINTER_FOUR    4


typedef struct
{
    QString stage;
    QString cardid;
    QString comid;
    QString name;
    QString param;
}show_info_st, *show_info_t;



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    NWThread* nwthread;
    show_info_st show_info;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel* connect_state;

    QTimer* timer;
    QTimer* timer_tcp;

    QLabel* current_time;
    QStringList* msg_list_all;
    QStringList* msg_list_1;
    QStringList* msg_list_2;
    QStringList* msg_list_3;
    QStringList* msg_list_4;

    void data_show(QString str);
    void data_refresh();
    int checkSum( QString & str);

protected slots:
    void slot_4G_Data(QString str);
    void slot_loginResult(bool bSuccess,QString strResult);
    void slot_conbox_index_chaged(int index);

    void timer_out();
    void timer_tcp_out();

private slots:
    void on_pushButton_clicked();
    void on_clear_btn_clicked();
};

#endif // MAINWINDOW_H
