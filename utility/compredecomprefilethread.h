#ifndef COMPREDECOMPREFILETHREAD_H
#define COMPREDECOMPREFILETHREAD_H

#include <QObject>
#include "QtGui/private/qzipreader_p.h"
#include "QtGui/private/qzipwriter_p.h"
#include <QFileInfoList>
#include <QDir>
#include <QFile>
#include <QThread>
class CompreDecompreFileThread : public QObject
{
    Q_OBJECT
public:
    typedef enum
    {
        success,
        failed,
        running
    }ResultE;
signals:
    void compression_res_signal(ResultE res,quint8 progress,QString remarks = nullptr);
    void decompression_res_signal(ResultE res,quint8 progress,QString remarks = nullptr);
public slots:
    /*
     * 压缩文件
     */
    void compression_file_run(QString srcPath,QString destPath);
    /*
     * 解压文件
     */
    void decompression_file_run(QString srcPath,QString destPath);
public:
    explicit CompreDecompreFileThread(QObject *parent = nullptr);
private:
    /*
     * 遍历压缩文件
     */
    QFileInfoList ergodic_compression_file(QZipWriter *writer,QString rootPath,QString filePath);

private:
    QZipWriter *zipWriter = nullptr;
    QZipReader *zipReader = nullptr;
};

#endif // COMPREDECOMPREFILETHREAD_H
