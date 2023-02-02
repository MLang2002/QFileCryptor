#include "mymessagebox.h"
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QString>

MyMessageBox::MyMessageBox(MsgBoxType type, QString text,QWidget *parent)
    : QDialog{parent}
{
    initState();

    if(type == MsgBoxType::MsgBoxType_Info)
    {
        initInfo(text);
    }
    else if(type == MsgBoxType::MsgBoxType_Warn)
    {
        initWarn(text);
    }
    else
    {
        initError(text);
    }
}

void MyMessageBox::initState()
{
    this->resize(240,160);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    this->setStyleSheet("background-color:transparent");

    labPic = new QLabel(this);
    labNote = new QLabel(this);
    btnSure = new QPushButton("Yes",this);
    btnCancle = new QPushButton("Cancel",this);

    connect(btnSure,&QPushButton::clicked,this,&MyMessageBox::dealbtnSureClicked);
    connect(btnCancle,&QPushButton::clicked,this,&MyMessageBox::dealbtnCancelClicked);
}

void MyMessageBox::initWarn(const QString &text)
{
    int width = this->width();

    labPic->setStyleSheet("image:url(:/new/prefix1/image/msg_info.png)");
    labPic->setGeometry(width*0.5-20,10,40,40);

    labNote->setStyleSheet("color:white");
    labNote->setAlignment(Qt::AlignCenter);
    labNote->setGeometry(0,70,width,20);
    labNote->setText(text);

    btnSure->setGeometry(width*0.2-15,110,80,30);
    btnSure->setStyleSheet("QPushButton{color:white; border-radius: 5px; background-color:transparent}"
                           "QPushButton:hover{background-color:blue}"
                           "QPushButton:pressed{background-color:blue}");

    btnCancle->setGeometry(width*0.6,110,80,30);
    btnCancle->setStyleSheet("QPushButton{color:white; border-radius: 5px; background-color:transparent}"
                             "QPushButton:hover{background-color:blue}"
                             "QPushButton:pressed{background-color:blue}");

}

void MyMessageBox::initError(const QString &text)
{
    int width = this->width();

    labPic->setStyleSheet("image:url(:/new/prefix1/image/msg_error.png)");
    labPic->setGeometry(width*0.5-20,10,40,40);

    labNote->setStyleSheet("color:white");
    labNote->setAlignment(Qt::AlignCenter);
    labNote->setGeometry(0,70,width,20);
    labNote->setText(text);

    btnSure->setGeometry(width*0.5-40,110,80,30);
    btnSure->setStyleSheet("QPushButton{color:white; border-radius: 5px; background-color:transparent}"
                           "QPushButton:hover{background-color:blue}"
                           "QPushButton:pressed{background-color:blue}");

    btnCancle->hide();
}

void MyMessageBox::initInfo(const QString &text)
{
    int width = this->width();

    labPic->setStyleSheet("image:url(:/new/prefix1/image/msg_correct.png)");
    labPic->setGeometry(width*0.5-20,10,40,40);

    labNote->setStyleSheet("color:white");
    labNote->setAlignment(Qt::AlignCenter);
    labNote->setGeometry(0,70,width,20);
    labNote->setText(text);

    btnSure->setGeometry(width*0.5-40,110,80,30);
    btnSure->setStyleSheet("QPushButton{color:white; border-radius: 5px; background-color:transparent}"
                           "QPushButton:hover{background-color:blue}"
                           "QPushButton:pressed{background-color:blue}");

    btnCancle->hide();
}

void MyMessageBox::dealbtnSureClicked()
{
    this->accept();
}

void MyMessageBox::dealbtnCancelClicked()
{
    this->reject();
}
