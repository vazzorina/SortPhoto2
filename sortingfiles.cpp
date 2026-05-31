#include "sortingfiles.h"
#include <QDirIterator>
#include <QDir>
#include <exiv2/exiv2.hpp>
#include <QElapsedTimer>

SortingFiles::SortingFiles(QObject *parent)
    : QObject{parent}
{}

void SortingFiles::writeTotalFilesPath(QString inputPath) {
    totalFiles = 0;
    QStringList formatsFiles = {"*.jpeg", "*.jpg", "*.png"}; // "*.gif", "*.webp", "*.heic", "*.tiff", "*.mp4", "*.mov", "*.avi", "*.mkv", "*.webm", "*.wmv", "*.flv"
    QDirIterator count_it(inputPath, formatsFiles, QDir::Files, QDirIterator::Subdirectories);
    while (count_it.hasNext()) {
        count_it.next();
        totalFiles++;
    }

    emit maxProgressChanged(totalFiles);
    emit logMessage("INFO: В папке \"" + inputPath + "\" находится файлов: " + QString::number(totalFiles));

    totalFilesPath.reserve(totalFiles);

    QDirIterator inputCatalog(inputPath, formatsFiles, QDir::Files, QDirIterator::Subdirectories);
    while (inputCatalog.hasNext()) {
        QString itemPath = inputCatalog.next();
        emit logMessage("INFO: В список всех путей добавлен: " + itemPath);
        totalFilesPath.append(itemPath);
    }

}

void SortingFiles::sortFiles(QString outputPath, long indexStart, long indexEnd, int numThread) {
    QElapsedTimer timer;
    timer.start();
    emit logMessage("START: Запущена сортировка файлов в потоке: " + QString::number(numThread));

    QDir outputCatalog;
    std::set<QString> dateDirs; // хранение годов, для которых уже созданы каталоги
    QString itemPath, itemOutputPath, year, suff;
    QDateTime date;
    currentProgress = 0;

    for (long i = indexStart; i < indexEnd; i++) {
        itemPath = totalFilesPath[i];
        suff = QFileInfo(itemPath).suffix();
        date = getDateFile(itemPath);
        year = QString::number(date.date().year());
        itemOutputPath = outputPath + "/" + year + "/" + date.toString("dd.MM.yyyy HH-mm-ss") + "." + suff;

        if (!date.isNull() and !dateDirs.count(year)) {
            dateDirs.insert(year);
            if (outputCatalog.mkpath(outputPath + "/" + year)) emit logMessage("NEW: Успешно создана папка: " + outputPath + "/" + year);
            else emit logMessage("ERROR: Не удалось создать папку: " + outputPath + "/" + year);
        }

        int cnt = 1;
        while (QFile::exists(itemOutputPath)) {
            itemOutputPath = outputPath + "/" + year + "/" + date.toString("dd.MM.yyyy HH-mm-ss") +
                             " (" + QString::number(cnt)+ ")" + "." + suff;
            cnt++;
            if (cnt == 100) break;
            emit logMessage("WARN: Ошибка при копировании (возможно дубликат): " + itemOutputPath);
        }
        if (QFile::copy(itemPath, itemOutputPath)) emit logMessage("INFO: Файл успешно скопирован и переименован: " + itemOutputPath);
        else emit logMessage("ERROR: Не удалось скопировать файл (проверьте место на диски или права доступа: " + itemOutputPath);

        currentProgress++;
        emit progressChanged(currentProgress);
    }

    emit logMessage("FINISH: Сортировка файлов окончена в потоке: " + QString::number(numThread));
    emit finished();
    elapsedTime += timer.elapsed();
}

QDateTime SortingFiles::getDateFile(QString itemPath) {
    std::string filePath = itemPath.toLocal8Bit().constData();

    try {
        auto image = Exiv2::ImageFactory::open(filePath);
        image->readMetadata();
        Exiv2::ExifData& exifData = image->exifData();
        QFileInfo fileInfo(itemPath);

        // Сначала читаем дату послденего изменения из информации о файле, так как она всегда существует
        std::string dateStr = fileInfo.lastModified().toString("yyyy:MM:dd HH:mm:ss").toStdString();

        // Проверяем тег Exif.Photo.DateTimeOriginal, так как в нем хранится дата съемки
        // Если тег не пустой, то dateStr изменится на дату съемки
        if(!exifData["Exif.Photo.DateTimeOriginal"].toString().empty()){
            dateStr = exifData["Exif.Photo.DateTimeOriginal"].toString();
        }

        // Превращаем строку EXIF-формата в объект QDateTime для удобной сортировки
        return QDateTime::fromString(QString::fromStdString(dateStr), "yyyy:MM:dd HH:mm:ss");

    } catch (Exiv2::Error& e) {
        emit logMessage("ERROR: Не удалось прочитать дату файла: " + itemPath);
        // При ошибке чтения файла возвращаем дату с нулевыми данными, чтобы вызывающий код сохранил файл в отдельной папке
        return QDateTime(QDate(1, 1, 1), QTime(0, 0, 0));
    }

}
