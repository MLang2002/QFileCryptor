#include "filedirbutton.h"
#include <QString>
#include <QStringList>
#include <QTextCodec>

QString FileDirButton::style_fileDirButton()
{
    QString style="font:25 14pt 'Times New Roman';"
                  "color:white;"
                  "background-color:transparent;"
                  "border-radius:128px;"
                  "border-style:dashed;";
    return style;
}

QString FileDirButton::style_fileButton()
{
    QString style="font:25 14pt 'Times New Roman';"
                  "color:white;"
                  "background-color:transparent;"
                  "border-radius:64px;"
                  "border-style:dashed;";
    return style;
}

QString FileDirButton::style_labelword()
{
    QString style="font:25 10pt 'Times New Roman';"
                  "color:black;"
                  "background-color:transparent;"
                  "border-style:dashed;";
    return style;
}
QString FileDirButton::style_labelhint()
{
    QString style="font:25 8pt 'Times New Roman';"
                  "color:black;"
                  "background-color:transparent;"
                  "border-style:dashed;";
    return style;
}
QString FileDirButton::style_comboBox()
{
    QString style="border:1px solid #242424;"
                  "border-radius:3px;"
                  "padding:2px;"
                  "font:25 10pt 'Times New Roman';"
                  "background-color:#FFFAF0;"
                  "selection-background-color:#484848;"
                  "selection-color:#DCDCDC;";
    return style;
}

QString FileDirButton::style_textbrowser()
{
    QStringList list;
    list.append(QString("QTextBrowser{"
                        "border:1px solid #242424;"
                        "border-radius:2px;"
                        "color: black;"
                        "background-color:#FFFAF0;"
///                        "border-color: rgba(0,156,255,50%);"
///                        "font-size: 12px;"
///                        "font-weight: 400;"
                        "font:25 9pt 'Times New Roman';"
                        "selection-background-color: #4169E1;"
                        "selection-color:#e6e6e6;"
                        "padding-left:6px;"
                        "padding-right:6px;}"));

    list.append(QString("QTextBrowser QScrollBar:vertical{"
                        "border: none;"
                        "background: transparent;"
                        "width: 6px;}"));

    list.append(QString("QTextBrowser QScrollBar::handle:vertical{"
                        "background-color: rgb(13,74,107);"
                        "min-height: 30px;"
                        "border-radius:3px;"
                        "width: 6px;}"));

    list.append(QString("QTextBrowser QScrollBar::add-line:vertical{"
                        "border: none;"
                        "height: 0px;}"));

    list.append(QString("QTextBrowser QScrollBar::sub-line:vertical{"
                        "border: none;"
                        "height: 0px;}"));

    list.append(QString("QTextBrowser QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{"
                        "background:transparent;"
                        "border-radius:3px;}"));
    QString qss=list.join("");
    return qss;
}

std::string UIHelper::qstr2str(const QString& qstr)
{
    QByteArray cdata=qstr.toLocal8Bit();
    return std::string(cdata);
}

QString UIHelper::str2qstr(const std::string& str){
    return QString::fromLocal8Bit(str.data());
}

std::string UIHelper::WString2String(const std::wstring& ws){
    std::string strLocale = setlocale(LC_ALL, "");
    const wchar_t* wchSrc = ws.c_str();
    size_t nDestSize = wcstombs(NULL, wchSrc, 0) + 1;
    char *chDest = new char[nDestSize];
    memset(chDest,0,nDestSize);
    wcstombs(chDest,wchSrc,nDestSize);
    std::string strResult = chDest;
    delete []chDest;
    setlocale(LC_ALL, strLocale.c_str());
    return strResult;
}

std::wstring String2WString(const std::string& s){
    std::string strLocale = setlocale(LC_ALL, "");
    const char* chSrc = s.c_str();
    size_t nDestSize = mbstowcs(NULL, chSrc, 0) + 1;
    wchar_t* wchDest = new wchar_t[nDestSize];
    wmemset(wchDest, 0, nDestSize);
    mbstowcs(wchDest,chSrc,nDestSize);
    std::wstring wstrResult = wchDest;
    delete []wchDest;
    setlocale(LC_ALL, strLocale.c_str());
    return wstrResult;
}

QString UIHelper::TextCodeChanger(QString& words,const QString& oldCodec,const QString& newCodec){
    QTextCodec* old=QTextCodec::codecForName(oldCodec.toStdString().c_str());
    QTextCodec* newer=QTextCodec::codecForName(newCodec.toStdString().c_str());
    QString unicode=old->toUnicode(words.toLocal8Bit().data());
    QByteArray bytes=newer->fromUnicode(unicode);
    return bytes;
}

