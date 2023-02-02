#ifndef WIDGET_H
#define WIDGET_H

#include "fileencryption.h"
#include "utility/progressbutton.h"
#include "utility/passwordeditor.h"
#include <QIcon>
#include <QWidget>
#include <QThread>
#include <QPointer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
    void emitProgress(int value);
    void backChoose();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event)override;

private:
    void initForm();
    void initConnection();
    void startEncryptFile();
    void startEncryptDir();
    void startDecryptFile();
    void startDecryptDir();
    bool dirZiper();
    bool dirUnZiper();

private slots:
    void switchMainPage();
    void getFilePath();
    void getDirPath();
    void startCryption();

private:
    int b_state=0; //EMPTY=0,FILE=1,DIR=2
    QVector<QIcon> picPath{QIcon(":/new/prefix1/image/Adding.png"),
                           QIcon(":/new/prefix1/image/fileicon.png"),
                           QIcon(":/new/prefix1/image/foldericon.png")};
    Ui::Widget *ui;
    QVector<QWidget*> pages;
    class QStackedWidget* mainStack;
    int m_MainPageIndex=0;

    //Window Move Member
    class QPoint m_lastPos;
    bool isPressedWidget;

    enum class ObjectType{
        FILE_ENCRYPT,DIR_ENCRYPT,FILE_DECRYPT,DIR_DECRYPT,UNKNOWN
    };
    struct obj_file_info{
        QString name;//=raw_name+appendix
        QString raw_name;
        QString path;
        QString appendix;
        obj_file_info()=default;
        ~obj_file_info()=default;
        obj_file_info(const QString& name,const QString& raw_name,const QString& path,const QString&app):name(name),raw_name(raw_name),path(path),appendix(app){}
        void update(const QString& name,const QString& path)
        {
            this->name=name;
            this->path=path;
            auto tmpList=this->name.split(".");
            if(tmpList.size()>2)
            {
                QStringList resList;
                for(int i=0;i<tmpList.size()-1;i++)
                    resList.append(tmpList[i]);
                this->raw_name=resList.join(".");
            }
            else
                this->raw_name=tmpList[0];
            this->appendix=tmpList.size()==1?"":tmpList[tmpList.size()-1];
        }
    };
    struct obj_dir_info{
        QString name;//=path+raw_name
        QString raw_name;
        QString path;
        obj_dir_info()=default;
        ~obj_dir_info()=default;
        void update(const QString&name,const QString& raw_name,const QString& path)
            {this->name=name;this->raw_name=raw_name;this->path=path;}
    };

    ObjectType m_objType=ObjectType::UNKNOWN;//Mode: File/Dir Encryption/Decryption
    obj_file_info m_fileInfo;//Record File Infomation
    obj_dir_info m_dirInfo;//Record Directory Infomation
    //Record Information of AES
    int AES_Length=0;
    int AES_Mode=0;
    int AES_Padding=0;
    //Record important pointers
    class std::array<PasswordEditor*,2> passwdArray;
    QPointer<QLineEdit> hintInput;
    QPointer<QLabel> hintOutput;
    QPointer<ProgressButton> runButton;

    FileEncryption* m_fileEncryption=nullptr;
    QThread* m_thread=nullptr;

    //zip helper
    class CompreDecompreFileThread *compreDecompreFileThread;
    QThread *compreDecompreFileMainThread;

};
#endif // WIDGET_H
