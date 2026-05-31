#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sortingfiles.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbtnStartPath_clicked();
    void on_pbtnFinishPath_clicked();
    void on_spCntManyThreads_valueChanged(int arg1);
    void on_pbtnFinish_clicked();
    void on_pbtnStop_clicked();
    void on_pbtnStart_clicked();
    void on_pbtnFinishThreads_clicked();
    void on_pbtnStopThreads_clicked();
    void on_pbtnStartThreads_clicked();

private:
    Ui::MainWindow *ui;
    QString startPath = "";
    QString finishPath = "";
    int cntThreads = 0;
    SortingFiles *sortingFiles = nullptr;
    void getPath();

};
#endif // MAINWINDOW_H
