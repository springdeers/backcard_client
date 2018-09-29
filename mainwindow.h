#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <nwthread.h>
#include <QTextCodec>

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

    void data_show(QString str);
    int checkSum( QString & str);

protected slots:
    void slot_4G_Data(QString str);
    void slot_loginResult(bool bSuccess,QString strResult);

};

#endif // MAINWINDOW_H
