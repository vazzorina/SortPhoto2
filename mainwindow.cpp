#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>
#include <thread>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int maxThreads = std::thread::hardware_concurrency() > 1 ? std::thread::hardware_concurrency() - 1 : 1; // вычисляем максимальное количество потоков с учетом текущего
    ui->spCntManyThreads->setMaximum(maxThreads);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbtnStartPath_clicked()
{
    startPath = "";
    getPath();
}


void MainWindow::on_pbtnFinishPath_clicked()
{
    finishPath = "";
    getPath();
}

void MainWindow::getPath() {
    QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (startPath.isEmpty()) {
        startPath = QFileDialog::getExistingDirectory(this, "Выберите нужный каталог", path);
        finishPath = "";
        ui->leFinishPath->setText(finishPath);
        ui->leStartPath->setText(startPath);
    }
    else if (finishPath.isEmpty()){
        finishPath = QFileDialog::getExistingDirectory(this, "Выберите нужный каталог", path);
        while(startPath == finishPath) {
            QMessageBox::warning(this, "Ошибка!", "Невозможно сохранить фотографии в тот же каталог, "
                                                  "в котором они расположены изначально! "
                                                  "Выберите другой каталог.");
            finishPath = QFileDialog::getExistingDirectory(this, "Выберите нужный каталог", path);
        }
        ui->leFinishPath->setText(finishPath);
    }
}

void MainWindow::on_spCntManyThreads_valueChanged(int arg1)
{
    cntThreads = arg1;
}


void MainWindow::on_pbtnFinish_clicked()
{
    ui->pbtnStartThreads->setEnabled(true);
    ui->pbtnStart->setEnabled(true);

    ui->pbtnFinish->setEnabled(false);
    ui->pbtnStop->setEnabled(false);

    // здесь будет функция прерывания потока
}


void MainWindow::on_pbtnStop_clicked()
{
    // здесь будет функция остановки потока
}


void MainWindow::on_pbtnStart_clicked()
{
    if (startPath.isEmpty() and finishPath.isEmpty()) QMessageBox::warning(this, "Ошикба!", "Не указаны пути каталогов!");
    else {
        ui->pbtnStartThreads->setEnabled(false);
        ui->pbtnFinishThreads->setEnabled(false);
        ui->pbtnStopThreads->setEnabled(false);

        ui->pbtnStart->setEnabled(false);
        ui->pbtnFinish->setEnabled(true);
        ui->pbtnStop->setEnabled(true);

        sortingFiles = new SortingFiles();

        connect(sortingFiles, &SortingFiles::logMessage, ui->teOneThread, &QTextEdit::append);
        connect(sortingFiles, &SortingFiles::progressChanged, ui->pBarOneThread, &QProgressBar::setValue);
        connect(sortingFiles, &SortingFiles::maxProgressChanged, ui->pBarOneThread, &QProgressBar::setMaximum);

        sortingFiles->writeTotalFilesPath(startPath);
        sortingFiles->elapsedTime = 0;
        //sortingFiles->sortFiles(finishPath, 0, sortingFiles->totalFiles, 1);

        //sortingFiles->sortFiles(startPath, finishPath);
        std::thread t(&SortingFiles::sortFiles, sortingFiles, finishPath, 0, sortingFiles->totalFiles, 1);
        t.detach();

        cnt = 0;

        connect(sortingFiles, &SortingFiles::finished, this, [=]() {
            cnt++;
            if (cnt == 1) ui->teOneThread->append("RESULT: Время сортировки равно: " + QString::number(sortingFiles->elapsedTime));
        });
    }
}


void MainWindow::on_pbtnFinishThreads_clicked()
{
    ui->pbtnStartThreads->setEnabled(true);
    ui->pbtnStart->setEnabled(true);

    ui->pbtnFinishThreads->setEnabled(false);
    ui->pbtnStopThreads->setEnabled(false);

    // здесь будет функция прерывания потоков
}


void MainWindow::on_pbtnStopThreads_clicked()
{
    // здесь будет функция остановки потоков
}


void MainWindow::on_pbtnStartThreads_clicked()
{
    if (startPath.isEmpty() and finishPath.isEmpty()) QMessageBox::warning(this, "Ошикба!", "Не указаны пути каталогов!");
    else {
        ui->pbtnStart->setEnabled(false);
        ui->pbtnFinish->setEnabled(false);
        ui->pbtnStop->setEnabled(false);

        ui->pbtnStartThreads->setEnabled(false);
        ui->pbtnFinishThreads->setEnabled(true);
        ui->pbtnStopThreads->setEnabled(true);

        sortingFiles = new SortingFiles();

        connect(sortingFiles, &SortingFiles::logMessage, ui->teManyThreads, &QTextEdit::append);
        connect(sortingFiles, &SortingFiles::progressChanged, ui->pBarManyThreads, &QProgressBar::setValue);
        connect(sortingFiles, &SortingFiles::maxProgressChanged, ui->pBarManyThreads, &QProgressBar::setMaximum);

        sortingFiles->writeTotalFilesPath(startPath);
        sortingFiles->elapsedTime = 0;

        long sizeBlock = sortingFiles->totalFiles/cntThreads + 1;
        std::vector<std::thread> allThreads(cntThreads);
        long startIndex = 0, endIndex = sizeBlock;
        for (int i = 1; i <= cntThreads; i++) {
            if(endIndex >= sortingFiles->totalFiles) endIndex = sortingFiles->totalFiles;
            allThreads[i-1] = std::thread(&SortingFiles::sortFiles, sortingFiles, finishPath, startIndex, endIndex, i);
            allThreads[i-1].detach();
            startIndex = endIndex;
            endIndex += sizeBlock;
        }
        cnt = 0;
        connect(sortingFiles, &SortingFiles::finished, this, [=]() {
            cnt++;
            if (cnt == cntThreads) ui->teManyThreads->append("RESULT: Время сортировки равно: " + QString::number(sortingFiles->elapsedTime));
        });

    }

    // здесб будет функция запсуков потоков
}

