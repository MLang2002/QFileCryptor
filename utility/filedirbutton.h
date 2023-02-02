#ifndef FILEDIRBUTTON_H
#define FILEDIRBUTTON_H
#include <QString>
#include <string>

class FileDirButton
{
public:
    static QString style_fileDirButton();
    static QString style_fileButton();
    static QString style_labelword();
    static QString style_labelhint();
    static QString style_comboBox();
    static QString style_textbrowser();
};

class UIHelper
{
public:
    static std::string qstr2str(const QString& qstr);
    static QString str2qstr(const std::string& str);
    static std::string WString2String(const std::wstring& ws);
    static std::wstring String2WString(const std::string& s);
    static QString TextCodeChanger(QString& words,const QString& oldCodec,const QString& newCodec);
};

#endif // FILEDIRBUTTON_H
