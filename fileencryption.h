#ifndef FILEENCRYPTION_H
#define FILEENCRYPTION_H

#include <QObject>
#include "qaesencryption.h"

class FileEncryption : public QObject
{
    Q_OBJECT
public:
    explicit FileEncryption(QObject *parent = nullptr);
    void setFile(const QString& strIn, const QString& strOut);
    void setKey(const QByteArray& key);
    void setEncryption(bool flag);
    void setAESParameter(QAESEncryption::Aes aes, QAESEncryption::Mode mode, QAESEncryption::Padding padding);
    void stop();
    void setHintText(const QString& txt);
    QString getHintText(const QString& filename);

    static std::string base64EncryptStr(const std::string& plainText);
    static std::string base64DecryptStr(const std::string& plainText);
    static inline bool is_base64(unsigned char c);
    static QString base64EncryptStr(const QString& plainText);
    static QString base64DecryptStr(const QString& plainText);

signals:
    void start();
    void showLog(QString log);
    void finish(bool hasError);

private:
    void startEncryption();
    void encryption();
    void decrypt();
    bool readFile(const QString& fileName);
    void dataOperation();
    void check();
    bool writeFile(const QString& fileName);
    void clear();

private:
    QString m_strIn;
    QString m_strOut;
    QString m_fileSuffix;
    QByteArray m_key;
    QByteArray m_iv;
    bool m_encryption = true;
    QAESEncryption::Aes m_aes;
    QAESEncryption::Mode m_mode;
    QAESEncryption::Padding m_padding;

    QByteArray m_dataIn;
    QByteArray m_dataOut;
    QByteArray m_md5;

    QByteArray m_head;
};

#endif // FILEENCRYPTION_H
