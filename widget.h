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
namespace Ui {
class Widget;
}
QT_END_NAMESPACE


class Widget : public QWidget {
    Q_OBJECT

  public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

  signals:
    void emitProgress(int value);//进度更新信号
    void backChoose();//对应文件/文件夹选择按钮

  protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event)override;

  private:
    void initForm(); //布局设置与初始化
    void initConnection();
    void startEncryptFile();//加密文件
    void startEncryptDir();//加密文件夹
    void startDecryptFile();//解密文件
    void startDecryptDir();//解密文件夹
    bool dirZiper();//压缩文件夹
    bool dirUnZiper();//解压文件夹

  private slots:
    void switchMainPage();//切换主页面
    void getFilePath();//获得文件信息
    void getDirPath();//获取文件夹信息
    void startCryption();//开始加解密

  private:
    int b_state=0; //EMPTY=0,FILE=1,DIR=2
    QVector<QChar> picPath{QChar(0xf067),QChar(0xf016),QChar(0xf114)};
    Ui::Widget *ui;
    QVector<QWidget*> pages; //存放不同页面
    class QStackedWidget* mainStack; // 页面container
    int m_MainPageIndex=0;

    //解决窗体移动问题
    class QPoint m_lastPos;
    bool isPressedWidget;

    //加密对象
    enum class ObjectType {
        FILE_ENCRYPT,DIR_ENCRYPT,FILE_DECRYPT,DIR_DECRYPT,UNKNOWN
    };
    struct obj_file_info {
        QString name;//=raw_name+appendix
        QString raw_name;
        QString path;
        QString appendix;
        obj_file_info()=default;
        ~obj_file_info()=default;
        obj_file_info(const QString& name,const QString& raw_name,const QString& path,const QString&app):name(name),raw_name(raw_name),path(path),appendix(app) {}
        void update(const QString& name,const QString& path) {
            this->name=name;
            this->path=path;
            auto tmpList=this->name.split(".");
            if(tmpList.size()>2) {
                QStringList resList;
                for(int i=0; i<tmpList.size()-1; i++)
                    resList.append(tmpList[i]);
                this->raw_name=resList.join(".");
            } else
                this->raw_name=tmpList[0];
            this->appendix=tmpList.size()==1?"":tmpList[tmpList.size()-1];
        }
    };
    struct obj_dir_info {
        QString name;//=path+raw_name
        QString raw_name;
        QString path;
        obj_dir_info()=default;
        ~obj_dir_info()=default;
        void update(const QString&name,const QString& raw_name,const QString& path) {
            this->name=name;
            this->raw_name=raw_name;
            this->path=path;
        }
    };

    ObjectType m_objType=ObjectType::UNKNOWN;//工作模式：文件/文件夹、加密/解密
    obj_file_info m_fileInfo;//文件信息记录体
    obj_dir_info m_dirInfo;//文件夹信息记录体
    //AES信息记录体
    int AES_Length=0;
    int AES_Mode=0;
    int AES_Padding=0;
    //重要指针信息记录
    class std::array<PasswordEditor*,2> passwdArray;
    QPointer<QLineEdit> hintInput;
    QPointer<QLabel> hintOutput;
    QPointer<ProgressButton> runButton;

    //加密工具
    FileEncryption* m_fileEncryption=nullptr;// 文件加解密对象
    QThread* m_thread=nullptr;// 文件加解密线程

    //zip helper
    class CompreDecompreFileThread *compreDecompreFileThread;
    QThread *compreDecompreFileMainThread;

};
#endif // WIDGET_H
