#include "fileencryption.h"
#include "utility/filedirbutton.h"
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <qcryptographichash.h>
#include <qdebug.h>
#include <qfile.h>
#include <qthread.h>

FileEncryption::FileEncryption(QObject *parent) : QObject(parent)
{
    connect(this, &FileEncryption::start, this, &FileEncryption::startEncryption);
    for(int i = 0; i < 16; i++)
    {
        m_iv.append((uchar)i);
    }
}

/**
 * @brief         设置输入输出文件路径
 * @param strIn   输入文件路径
 * @param strOut  输出文件路径
 */
void FileEncryption::setFile(const QString& strIn, const QString& strOut)
{
    if(!strIn.isEmpty() && !strOut.isEmpty())
    {
        this->m_strIn = strIn;
        this->m_strOut = strOut;

        QFileInfo info(strIn);
        m_fileSuffix = info.suffix();
    }
}

/**
 * @brief      设置使用的密钥
 * @param key
 */
void FileEncryption::setKey(const QByteArray &key)
{
    this->m_key = key;
}

/**
 * @brief        设置加密或者解密
 * @param flag   true：加密 false：解密
 */
void FileEncryption::setEncryption(bool flag)
{
    this->m_encryption = flag;
}

/**
 * @brief           设置AES加解密参数
 * @param aes
 * @param mode
 * @param padding
 */
void FileEncryption::setAESParameter(QAESEncryption::Aes aes, QAESEncryption::Mode mode, QAESEncryption::Padding padding)
{
    this->m_aes = aes;
    this->m_mode = mode;
    this->m_padding = padding;
}

/**
 * @brief 加解密操作
 */
void FileEncryption::startEncryption()
{
    clear();
    emit showLog("开始输入原文件！");
    if(readFile(m_strIn))
    {
        emit showLog("文件读取完成！");
        if(m_encryption)
        {
            encryption();
        }
        else
        {
            decrypt();
        }
        if(writeFile(m_strOut))
        {
            emit showLog("数据写入成功！");
        }
        clear();
    }
    else
    {
//        emit showLog("输入文件读取失败！");
        emit finish(true);
    }
    emit finish(false);
}

/**
 * @brief 加密
 */
void FileEncryption::encryption()
{
    QAESEncryption encryption(m_aes, m_mode, m_padding);
    emit showLog("开始加密！");

    if(m_mode == QAESEncryption::ECB)
    {
        m_dataOut.append(encryption.encode(m_dataIn, m_key));
    }
    else
    {
         m_dataOut.append(encryption.encode(m_dataIn, m_key, m_iv));
    }
    m_head=FileEncryption::base64EncryptStr(QString(m_head)).toUtf8();
    qDebug()<<"Original m_head size: "<<m_head.size()<<__FUNCTION__;
    while(m_head.size()<1024)
        m_head.append('\n');
    qDebug()<<"After m_head size: "<<m_head.size()<<__FUNCTION__;
    m_dataOut.insert(0, m_head.mid(0,1024));

    m_dataOut.append(m_md5);
    qDebug()<<"m_md5 size: "<<m_md5.size()<<__FUNCTION__;
    emit showLog("加密完成，开始写入！");

}

/**
 * @brief  解密
 */
void FileEncryption::decrypt()
{
    QAESEncryption encryption(m_aes, m_mode, m_padding);
    emit showLog("开始解密！");

    if(m_mode == QAESEncryption::ECB)
    {
        m_dataOut.append(encryption.decode(m_dataIn, m_key));
    }
    else
    {
         m_dataOut.append(encryption.decode(m_dataIn, m_key, m_iv));
    }
    m_dataOut = encryption.removePadding(m_dataOut);        // 移除填充数据

    emit showLog("解密完成，开始写入！");
    check();
}

/**
 * @brief           读取文件内容
 * @param fileName
 */
bool FileEncryption::readFile(const QString &fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        m_dataIn = file.readAll();
        file.close();
        dataOperation();
        return true;
    }
    else
    {
        emit showLog(QString("%1打开失败！").arg(fileName));
        return false;
    }
}

/**
 * @brief 操作数据内容
 */
void FileEncryption::dataOperation()
{
    // 获取md5值
    if(m_encryption)
    {
        m_md5 = QCryptographicHash::hash(m_dataIn, QCryptographicHash::Md5).toHex();
    }
    else                  // 解密时读取md5值
    {
        m_md5 = m_dataIn.mid(m_dataIn.count() - 32, 32);
        m_dataIn.remove(m_dataIn.count() - 32, 32);
        //删除Hint信息
//        m_head = m_dataIn.mid(0,1024);
        m_dataIn.remove(0,1024);
    }

//    // 读取bmo文件头信息
//    if(m_fileSuffix.compare("bmp", Qt::CaseInsensitive) == 0)
//    {
//        m_head = m_dataIn.mid(0, 54);        // bmp文件头：共14字节；位图信息头：共40字节；
//        m_dataIn.remove(0, 54);
//    }
}

/**
 * @brief  使用md5校验文件是否损毁
 */
void FileEncryption::check()
{
    QByteArray arr = QCryptographicHash::hash(m_dataOut, QCryptographicHash::Md5).toHex();
    if(arr == m_md5)
    {
        emit showLog("文件未损毁！");
    }
    else
    {
        emit showLog("文件存在不同，可能已损毁！");
    }
}


/**
 * @brief           写文件内容
 * @param fileName
 * @param data
 */
bool FileEncryption::writeFile(const QString &fileName)
{
    QFileInfo info(fileName);
    QString filePath = info.absolutePath();
    QDir dir;
    if(!dir.exists(filePath))
    {
        dir.mkpath(filePath);
    }

    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(m_dataOut);
        file.close();
        return true;
    }
    else
    {
        emit showLog(QString("%1打开失败！").arg(fileName));
        return false;
    }
}

/**
 * @brief 清空数据
 */
void FileEncryption::clear()
{
    m_dataIn.clear();
    m_dataOut.clear();
//    m_head.clear();
    m_md5.clear();
}

bool FileEncryption::is_base64(unsigned char c){
    return (std::isalnum(c) || (c=='+') || (c=='/'));
}

std::string FileEncryption::base64EncryptStr(const std::string& plainText)
{
    unsigned char const* bytes_to_encode=reinterpret_cast<const unsigned char*>(plainText.c_str());
    unsigned int in_len=plainText.length();
    const std::string base64_chars =
               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
               "abcdefghijklmnopqrstuvwxyz"
               "0123456789+/";
    std::string ret;
    int i=0,j=0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    while(in_len--){
       char_array_3[i++]=*(bytes_to_encode++);
       if(i==3){
           char_array_4[0]=(char_array_3[0] & 0xfc) >> 2;
           char_array_4[1]=((char_array_3[0] & 0x03)<<4)+((char_array_3[1] & 0xf0)>>4);
           char_array_4[2]=((char_array_3[1] & 0x0f)<<2)+((char_array_3[2] & 0xc0)>>6);
           char_array_4[3]=char_array_3[2] & 0x3f;
           for(i=0;i<4;i++)
               ret+=base64_chars[char_array_4[i]];
           i=0;
       }
    }
    if(i){
       for(j=i;j<3;j++)
           char_array_3[j]='\0';
       char_array_4[0]=(char_array_3[0] & 0xfc)>>2;
       char_array_4[1]=((char_array_3[0] & 0x03)<<4)+((char_array_3[1] & 0xf0)>>4);
       char_array_4[2]=((char_array_3[1] & 0x0f)<<2)+((char_array_3[2] & 0xc0)>>6);
       char_array_4[3]=char_array_3[2] & 0x3f;
       for(j=0;j<i+1;j++)
           ret+=base64_chars[char_array_4[j]];
       while(i++<3)
           ret+='=';
    }
    return ret;
}

QString FileEncryption::base64EncryptStr(const QString& plainText)
{
    auto res=FileEncryption::base64EncryptStr(UIHelper::qstr2str(plainText));
    return UIHelper::str2qstr(res);
}

std::string FileEncryption::base64DecryptStr(const std::string& encoded_string)
{
    const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";
    int in_len=encoded_string.size();
    int i=0,j=0,in_=0;
    unsigned char char_array_4[4];
    unsigned char char_array_3[3];
    std::string ret;
    while(in_len-- && (encoded_string[in_]!='=')&&is_base64(encoded_string[in_])){
        char_array_4[i++]=encoded_string[in_];
        in_++;
        if(i==4){
            for(i=0;i<4;i++)
                char_array_4[i]=base64_chars.find(char_array_4[i]);
            char_array_3[0]=(char_array_4[0]<<2)+((char_array_4[1] & 0x30)>>4);
            char_array_3[1]=((char_array_4[1] & 0xf)<<4)+((char_array_4[2] & 0x3c)>>2);
            char_array_3[2]=((char_array_4[2] & 0x3)<<6)+char_array_4[3];
            for(i=0;i<3;i++)
                ret+=char_array_3[i];
            i=0;
        }
    }
    if(i){
        for(j=i;j<4;j++)
            char_array_4[j]=0;
        for(j=0;j<4;j++)
            char_array_4[j]=base64_chars.find(char_array_4[j]);
        char_array_3[0]=(char_array_4[0]<<2)+((char_array_4[1] & 0x30)>>4);
        char_array_3[1]=((char_array_4[1] & 0xf)<<4)+((char_array_4[2] & 0x3c)>>2);
        char_array_3[2]=((char_array_4[2] & 0x3)<<6)+char_array_4[3];
        for(j=0;j<i-1;j++)
            ret+=char_array_3[j];
    }
    return ret;
}

QString FileEncryption::base64DecryptStr(const QString& plainText)
{
    auto res=FileEncryption::base64DecryptStr(UIHelper::qstr2str(plainText));
    return UIHelper::str2qstr(res);
}

void FileEncryption::setHintText(const QString& txt)
{
    this->m_head=txt.toUtf8();
}

QString FileEncryption::getHintText(const QString& filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        auto tmpData=file.readAll();
        file.close();
        auto raw_hint=tmpData.mid(0,1024);
        while(raw_hint.endsWith('\n'))
            raw_hint.chop(1);
        auto de_hint=FileEncryption::base64DecryptStr(QString(raw_hint)).toUtf8();
        QString str_hint(de_hint);
        return str_hint;
    }
    return "404 Error: in FileEncryption::getHintText";
}
