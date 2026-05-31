#ifndef SORTINGFILES_H
#define SORTINGFILES_H

#include <QObject>

class SortingFiles : public QObject
{
    Q_OBJECT
public:
    explicit SortingFiles(QObject *parent = nullptr);

public slots:
    void sortFiles(QString inputPath, QString outputPath);

signals:
    void progressChanged(int value); // сигнал для смены значения progressBar
    void maxProgressChanged(int value); //сиглал для установки максимального значения progressBar
    void logMessage(QString message); // сиглан для отправки сообщений в TextEdit
    void finished(); // сигнал окончания сортировки

private:
    QDateTime getDateFile(QString itemPath);
};

#endif // SORTINGFILES_H
