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
    QDir dir(filePath);          //遍历各级子目录
    QDir fileDir(filePath);    //遍历子目录中所有文件

    //先遍历文件夹 添加进widget
    QFileInfoList fileList=dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folderList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);   //获取当前所有目录 QDir::Dirs 0x001 列出目录；
    qDebug()<<"Enter "<<__FUNCTION__;
    qDebug()<<"folder size: "<<folderList.size();
    rootPath+="/";
    qDebug()<<"Input RootPath: "<<rootPath;
    qDebug()<<"filePath: "<<filePath;
    for(int i = 0; i != folderList.size(); i++)         //自动递归添加各目录到上一级目录
    {
        QString namepath = folderList.at(i).absoluteFilePath();    //获取路径
        QFileInfo folderinfo= folderList.at(i);
        QString name=folderinfo.fileName();      //获取目录名


        QStringList list = folderinfo.absoluteFilePath().split(rootPath);
        //qDebug()<<"Dir:"<<folderinfo.absoluteFilePath().remove(list.at(0));
        qDebug()<<"folderInfo: "<<folderinfo;
        qDebug()<<"namepath: "<<namepath;
        qDebug()<<"name: "<<name;
        qDebug()<<"Add Dir: "<<folderinfo.absoluteFilePath().remove(list.at(0));
        writer->addDirectory(folderinfo.absoluteFilePath().remove(list.at(0)));

        QFileInfoList child_file_list = ergodic_compression_file(writer,rootPath,namepath);          //进行递归 递归这个文件夹
        fileList.append(child_file_list);
        //  file_list.append(name);

    }
    /*添加path路径文件*/
    qDebug()<<"Adding File";
    fileDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);        //获取当前所有文件
    fileDir.setSorting(QDir::Name);
    QFileInfoList _fileList = fileDir.entryInfoList();
    qDebug()<<"File List Size: "<<_fileList.size();
    for (int i = 0; i < _fileList.size(); ++i) //将当前目录中所有文件添加到treewidget中
    {
        QFileInfo fileInfo = _fileList.at(i);

        QFile file(fileInfo.absoluteFilePath());
        if(file.open(QIODevice::ReadOnly))
        {
//            QStringList list = fileInfo.absoluteFilePath().split(rootPath);

//            qDebug()<<"ABS: "<<fileInfo.absoluteFilePath();
//            qDebug()<<"RemoveStr: "<<list.at(0);
//            qDebug()<<"File:"<<fileInfo.absoluteFilePath().remove(list.at(0));
//            qDebug()<<"Split Symbol: "<<rootPath;

//            writer->addFile(fileInfo.absoluteFilePath().remove(list.at(0)),file.readAll());
//            file.close();
            QStringList list=fileInfo.path().split("/");
            qDebug()<<"fileInfo: "<<fileInfo.path();
            list.pop_back();
            QString fatherDir=list.join("/");
            if(fatherDir.endsWith('/')==false)fatherDir+="/";
            qDebug()<<"fatherDir: "<<fatherDir;
            QString addFilePath=fileInfo.absoluteFilePath().remove(fatherDir);
            qDebug()<<"File: "<<addFilePath;
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
        emit compression_res_signal(ResultE::failed,100,tr("路径为空！"));
        return;
    }
    QFileInfo fileDistInfo(destPath);
    if(fileDistInfo.isFile())   //目的路径不能为文件名
    {
        emit compression_res_signal(ResultE::failed,100,tr("路径不能为文件！"));
        return;
    }


    QFileInfo fileInfo(srcPath);
    if(fileInfo.isFile())//压缩的是一个文件
    {
        qDebug()<<"判断压缩对象为文件";
        QString saveFileName = destPath+"/"+fileInfo.baseName()+".zip";
        qDebug()<<"saveFileName:"<<saveFileName;
        this->zipWriter = new QZipWriter(saveFileName);
        QFile file(srcPath);
        if(!file.open(QIODevice::ReadOnly))
        {
            emit compression_res_signal(ResultE::failed,100,tr("文件打开失败！"));
            return;
        }
        emit compression_res_signal(ResultE::running,50,tr("正在压缩中..."));
        this->zipWriter->addFile(file.fileName(),file.readAll());
        this->zipWriter->close();
        delete this->zipWriter;
        this->zipWriter = nullptr;
        emit compression_res_signal(ResultE::running,100,tr("正在压缩中..."));
        emit compression_res_signal(ResultE::success,100,tr("压缩完成！"));
    }
    else    //压缩的是一个文件夹
    {
        qDebug()<<"判断压缩对象是文件夹";
        QString folderStr = srcPath.mid(srcPath.lastIndexOf("/")).remove("/");
        if(folderStr == nullptr)
        {
            emit compression_res_signal(ResultE::failed,100,tr("压缩文件路径错误！"));
            return;
        }
        emit compression_res_signal(ResultE::running,50,tr("正在压缩中..."));
//        QString saveFileName = destPath+"/"+folderStr+".zip";
        QString saveFileName = destPath+folderStr+".zip";
        qDebug()<<"saveFileName:"<<saveFileName;
        this->zipWriter = new QZipWriter(saveFileName);
        this->zipWriter->addDirectory(folderStr);
        this->ergodic_compression_file(this->zipWriter,folderStr,srcPath);
        delete this->zipWriter;
        this->zipWriter = nullptr;
        emit compression_res_signal(ResultE::running,100,tr("正在压缩中..."));
        emit compression_res_signal(ResultE::success,100,tr("压缩完成！"));
    }

}

//void CompreDecompreFileThread::decompression_file_run(QString srcPathAndName, QString destPath)
//{
//    qDebug()<<QThread::currentThreadId();
//    qDebug()<<"Enter "<<__FUNCTION__;
//    qDebug()<<"srcFile: "<<srcPathAndName;
//    qDebug()<<"dstPath: "<<destPath;
//    if(srcPathAndName == nullptr || destPath == nullptr)
//    {
//        qDebug()<<"路径为空！"<<__FUNCTION__;
//        emit decompression_res_signal(ResultE::failed,100,tr("路径为空！"));
//        return;
//    }
////    QFileInfo destInfo(destPath);
////    if(destInfo.isFile()||!destInfo.exists())
////    {
////        qDebug()<<"目的路径异常！"<<__FUNCTION__;
////        emit decompression_res_signal(ResultE::failed,100,tr("目的路径异常！"));
////        return;
////    }

//    QFileInfo fileInfo(srcPathAndName);
//    qDebug()<<"fileInfo is file: "<<fileInfo.isFile();
//    qDebug()<<"fileINfo suffix: "<<fileInfo.suffix();
////    if(!fileInfo.isFile() || fileInfo.suffix() !="zip")
//    if(fileInfo.suffix() !="zip")
//    {
//        qDebug()<<"压缩文件为空！"<<__FUNCTION__;
//        emit decompression_res_signal(ResultE::failed,100,tr("压缩文件为空！"));
//        return;
//    }
//    emit decompression_res_signal(ResultE::running,50,tr("正在解压缩..."));
//    this->zipReader = new QZipReader(srcPathAndName);
//    bool res = this->zipReader->extractAll(destPath);
//    emit decompression_res_signal(ResultE::running,100,tr("正在解压缩..."));
//    if(res) //解压缩成功
//    {
//        qDebug()<<"解压缩完成！"<<__FUNCTION__;
//        emit decompression_res_signal(ResultE::success,100,tr("解压缩完成！"));
//    }
//    else
//    {
//        qDebug()<<"解压缩失败！"<<__FUNCTION__;
//         emit decompression_res_signal(ResultE::failed,100,tr("解压缩失败！"));
//    }

//    this->zipReader->close();
//    delete this->zipReader;
//    this->zipReader = nullptr;

//}

void CompreDecompreFileThread::decompression_file_run(QString srcPathAndName, QString destPath)
{
    qDebug()<<QThread::currentThreadId();
    qDebug()<<"Enter "<<__FUNCTION__;
    qDebug()<<"srcFile: "<<srcPathAndName;
    qDebug()<<"dstPath: "<<destPath;
    bool result=true;
    if(srcPathAndName.isEmpty() || destPath.isEmpty())
    {
        qDebug()<<"路径为空！"<<__FUNCTION__;
        emit decompression_res_signal(ResultE::failed,100,tr("路径为空！"));
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
        //emit decompression_res_signal(ResultE::failed,100,tr("解压缩失败！"));
        qDebug()<<"解压缩失败！"<<__FUNCTION__;
    else
        qDebug()<<"解压缩完成！"<<__FUNCTION__;
//        emit decompression_res_signal(ResultE::success,100,tr("解压缩完成！"));
    this->zipReader->close();
    delete this->zipReader;
    this->zipReader = nullptr;

}
