#ifndef SORTINGFILES_H
#define SORTINGFILES_H

#include <QObject>
#include <atomic>

class SortingFiles : public QObject
{
    Q_OBJECT
public:
    explicit SortingFiles(QObject *parent = nullptr);
    long totalFiles = 0;
    QStringList totalFilesPath; // список путей всех файлов из исходного католога
    std::atomic<long> currentProgress = 0; // атомарная переменная, которую можно изменять сразу из нескольких потоков
    std::atomic<qint64> elapsedTime;

public slots:
    void sortFiles(QString outputPath, long indexStart, long indexEnd, int numThread);
    void writeTotalFilesPath(QString inputPath);

signals:
    void progressChanged(int value); // сигнал для смены значения progressBar
    void maxProgressChanged(int value); //сиглал для установки максимального значения progressBar
    void logMessage(QString message); // сиглан для отправки сообщений в TextEdit
    void finished(); // сигнал окончания сортировки

private:
    QDateTime getDateFile(QString itemPath);
};

#endif // SORTINGFILES_H
