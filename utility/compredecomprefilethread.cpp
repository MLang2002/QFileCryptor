#include "compredecomprefilethread.h"
#include <QDebug>
#include <QIODevice>

CompreDecompreFileThread::CompreDecompreFileThread(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<ResultE>("ResultE");
}

QFileInfoList CompreDecompreFileThread::ergodic_compression_file(QZipWriter *writer, QString rootPath, QString filePath)
{
    QDir dir(filePath);
    QDir fileDir(filePath);

    //Add Directory
    QFileInfoList fileList=dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folderList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);   //QDir::Dirs 0x001
    qDebug()<<"Enter "<<__FUNCTION__;
    qDebug()<<"folder size: "<<folderList.size();
    rootPath+="/";
    qDebug()<<"Input RootPath: "<<rootPath;
    qDebug()<<"filePath: "<<filePath;
    for(int i = 0; i != folderList.size(); i++)
    {
        QString namepath = folderList.at(i).absoluteFilePath();
        QFileInfo folderinfo= folderList.at(i);
        QString name=folderinfo.fileName();


        QStringList list = folderinfo.absoluteFilePath().split(rootPath);
        writer->addDirectory(folderinfo.absoluteFilePath().remove(list.at(0)));

        QFileInfoList child_file_list = ergodic_compression_file(writer,rootPath,namepath);
        fileList.append(child_file_list);

    }
    //Add Files
    qDebug()<<"Adding File";
    fileDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    fileDir.setSorting(QDir::Name);
    QFileInfoList _fileList = fileDir.entryInfoList();
    qDebug()<<"File List Size: "<<_fileList.size();
    for (int i = 0; i < _fileList.size(); ++i)
    {
        QFileInfo fileInfo = _fileList.at(i);

        QFile file(fileInfo.absoluteFilePath());
        if(file.open(QIODevice::ReadOnly))
        {
            QStringList list=fileInfo.path().split("/");
            list.pop_back();
            QString fatherDir=list.join("/");
            if(fatherDir.endsWith('/')==false)fatherDir+="/";
            QString addFilePath=fileInfo.absoluteFilePath().remove(fatherDir);
            writer->addFile(addFilePath,file.readAll());
            file.close();
        }

    }
    return fileList;
}
void CompreDecompreFileThread::compression_file_run(QString srcPath, QString destPath)
{
    qDebug()<<QThread::currentThreadId();
    if(srcPath == nullptr || destPath == nullptr)
    {
        emit compression_res_signal(ResultE::failed,100,tr("Empty Path"));
        return;
    }
    QFileInfo fileDistInfo(destPath);
    if(fileDistInfo.isFile())
    {
        emit compression_res_signal(ResultE::failed,100,tr("Path! Not File!"));
        return;
    }


    QFileInfo fileInfo(srcPath);
    if(fileInfo.isFile())
    {
        QString saveFileName = destPath+"/"+fileInfo.baseName()+".zip";
        qDebug()<<"saveFileName:"<<saveFileName;
        this->zipWriter = new QZipWriter(saveFileName);
        QFile file(srcPath);
        if(!file.open(QIODevice::ReadOnly))
        {
            emit compression_res_signal(ResultE::failed,100,tr("Open File Failure!"));
            return;
        }
        emit compression_res_signal(ResultE::running,50,tr("Compressing..."));
        this->zipWriter->addFile(file.fileName(),file.readAll());
        this->zipWriter->close();
        delete this->zipWriter;
        this->zipWriter = nullptr;
        emit compression_res_signal(ResultE::running,100,tr("Compressing..."));
        emit compression_res_signal(ResultE::success,100,tr("Finish Compressing!"));
    }
    else
    {
        QString folderStr = srcPath.mid(srcPath.lastIndexOf("/")).remove("/");
        if(folderStr == nullptr)
        {
            emit compression_res_signal(ResultE::failed,100,tr("Path error!"));
            return;
        }
        emit compression_res_signal(ResultE::running,50,tr("Compressing..."));
        QString saveFileName = destPath+folderStr+".zip";
        qDebug()<<"saveFileName:"<<saveFileName;
        this->zipWriter = new QZipWriter(saveFileName);
        this->zipWriter->addDirectory(folderStr);
        this->ergodic_compression_file(this->zipWriter,folderStr,srcPath);
        delete this->zipWriter;
        this->zipWriter = nullptr;
        emit compression_res_signal(ResultE::running,100,tr("Compressing..."));
        emit compression_res_signal(ResultE::success,100,tr("Finish Compressing!"));
    }

}

void CompreDecompreFileThread::decompression_file_run(QString srcPathAndName, QString destPath)
{
    qDebug()<<QThread::currentThreadId();
    qDebug()<<"Enter "<<__FUNCTION__;
    qDebug()<<"srcFile: "<<srcPathAndName;
    qDebug()<<"dstPath: "<<destPath;
    bool result=true;
    if(srcPathAndName.isEmpty() || destPath.isEmpty())
    {
        emit decompression_res_signal(ResultE::failed,100,tr("Empty Path!"));
        return;
    }
    QString zipDir=destPath;
    QString zipPath=srcPathAndName;
    this->zipReader=new QZipReader(zipPath);
    QDir baseDir(zipDir);

    const QVector<QZipReader::FileInfo>allFiles=zipReader->fileInfoList();

    for(QZipReader::FileInfo fi:allFiles)
    {
        const QString absPath=QDir::toNativeSeparators(QString("%1/%2").arg(zipDir,fi.filePath));
        baseDir.mkpath(QFileInfo(absPath).absoluteDir().path());
        if(fi.isSymLink)
        {
            QString destination=QFile::decodeName(zipReader->fileData(fi.filePath));
            if(destination.isEmpty())
            {
                result=false;
                break;
            }
            QFileInfo linkFi(absPath);
            if(!QFile::exists(linkFi.absolutePath()))
                QDir::root().mkpath(linkFi.absolutePath());
            if(!QFile::link(destination,absPath))
            {
                result=false;
                break;
            }
        }
        else if(fi.isFile)
        {
            QFile f(absPath);
            if(!f.isOpen())
                f.open(QIODevice::WriteOnly);
            f.write(zipReader->fileData(fi.filePath));
            f.setPermissions(fi.permissions);
            f.close();
        }
    }
    if(result==false)
        qDebug()<<"Fail to decompress!"<<__FUNCTION__;
    else
        qDebug()<<"Success in decompressing!"<<__FUNCTION__;
    this->zipReader->close();
    delete this->zipReader;
    this->zipReader = nullptr;

}
