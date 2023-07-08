#include "widget.h"
#include "ui_widget.h"
#include "utility/filedirbutton.h"
#include "utility/flatui.h"
#include "utility/compredecomprefilethread.h"
#include "utility/mymessagebox.h"
#include "utility/awesomefont.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <qcryptographichash.h>
#include <QIcon>
#include <QStackedWidget>
#include <QToolButton>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QComboBox>
#include <QTextBrowser>
#include <QPushButton>
#include <array>

const int E_BIGICON_SIZE=160;
const static QString E_FILE_APPENDIX="fencrypt";
const static QString E_DIR_APPENDIX="dencrypt";
const static QString PROJECT_INFORMATION="<style>"
        "#demo {"
        "  text-align:center"
        "}"
        "</style>"
        "<div id=\"demo\">"
        "<p><strong>Project: QFileCryptor/FileCryptor</strong></p>"
        "</div>"
        "<div id=\"demo\">"
        "<p><strong>Author: MLang2002</strong></p>"
        "</div>"

        "<div id=\"demo\">"
        "<p><strong>Tutorials in Chinese&nbsp;&nbsp;中文使用说明</strong></p>"
        "</div>"
        "*******************************************<br/>"

        "1.本软件用于对文件/文件夹进行便携式简易加解密，无任何后门，不记录无关信息。<br/>"
        "2.状态说明<br/>"
        "  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;加密：在选择完文件/文件夹后填写密码(必填)和提示(可选)，点击按钮即可完成操作并确认。加密文件会存在于加密对象所在目录中，"
        "并且不会删除原文件。文件后缀名为“.fencrypt”(针对文件)或“.dencrypt”(针对文件夹)。<br/>"
        "  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;解密：在选择完文件/文件夹后填写密码(必填)，点击按钮即可完成操作并确认。解密内容与原文件同目录，不会删除原文件。如果加密时"
        "填写有提示信息，文件/文件夹选择完后会显示出来，供用户回忆密码。<br/>"
        "  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;关于按钮状态：如果点击按钮后出现√，说明未出现异常；如果出现×或弹窗，则需检查操作是否正确。<br/>"
        "<div id=\"demo\"><p><strong>特别说明</strong></p></div>"
        "  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;文件夹加密选择“文件夹”图标，文件加解密、文件夹解密均选择“文件”图标!<br/>"
        "  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;文件夹加解密请选择ISO或PKCS7填充模式！默认模式是ISO模式！<br/>"

        "<div id=\"demo\">"
        "<p><strong>Toturials in English</strong></p>"
        "</div>"

        "*******************************************<br/>"

        "1.This software is used for encryption and decryption of files/folders conveniently, without any back door and records.<br/>"
        "2.Illustration on States<br/>"
        "  &nbsp;&nbsp;&nbsp;&nbsp;Encryption: After selecting the file/folder, fill in the password (required) and hint (optional),"
        " and click the button to complete the operation and confirm. The encrypted file will exist in the directory of the encrypted object, and the original file will not be deleted."
        " The file suffix is \". fencrypt\" (for files) or \". encrypt\" (for folders).<br/>"
        "  &nbsp;&nbsp;&nbsp;&nbsp;Descryption: After selecting the file/folder, fill in the password (required), click the button to complete the operation and confirm."
        " The decrypted file/folder is in the same directory as the original file, and the original file will not be deleted."
        " If a prompt message is filled in during encryption, it will be displayed after selection for the user to recall the password<br/>"
        "  &nbsp;&nbsp;&nbsp;&nbsp;About Buttons' State: If there is a √(tick) after pressing, it means all operations are correct; else, a ×(crossing) or pop-up notification will show up. Please check your operation!<br/>"
        "<div id=\"demo\"><p><strong>Special Instructions</strong></p></div>"
        "  &nbsp;&nbsp;&nbsp;&nbsp;Select the \"folder\" icon for folder encryption. Otherwise, \"file\" icon for file encryption and folder decryption!<br/>"
        "  &nbsp;&nbsp;&nbsp;&nbsp;Please select ISO or PKCS7 filling mode for folder encryption and decryption! The default mode is ISO!<br/>";

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget) {
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowIcon(QIcon(":/new/prefix1/image/Safe.ico"));

    //解决无法在构造函数中正确获得控件大小问题
    this->setAttribute(Qt::WA_DontShowOnScreen);
    this->show();
    this->hide();
    this->setAttribute(Qt::WA_DontShowOnScreen,false);

    m_fileEncryption = new FileEncryption();
    m_thread = new QThread();
    m_fileEncryption->moveToThread(m_thread);
    m_thread->start();

    compreDecompreFileThread = new CompreDecompreFileThread;
    compreDecompreFileMainThread = new QThread;
    compreDecompreFileThread->moveToThread(compreDecompreFileMainThread);
    compreDecompreFileMainThread->start();

    this->initForm();
    this->initConnection();
}

Widget::~Widget() {
    m_thread->requestInterruption();
    m_thread->quit();
    m_thread->wait();
    delete m_thread;
    m_thread=nullptr;
    compreDecompreFileMainThread->requestInterruption();
    compreDecompreFileMainThread->quit();
    compreDecompreFileMainThread->wait();
    delete compreDecompreFileMainThread;
    compreDecompreFileMainThread=nullptr;

    delete compreDecompreFileThread;
    delete m_fileEncryption;
    delete ui;
}

void Widget::initForm() {
    // Set MainWindow's Background Color
    QPalette palette(this->palette());
    palette.setColor(QPalette::Window,QColor("#F0FFFF"));
    this->setPalette(palette);

    ui->WindowTitle->setToolTip(QString("Free encryption/decryption software: QFileCryptor"));

    this->mainStack=new QStackedWidget();
    QWidget* app_page=new QWidget;
    QWidget* setting_page=new QWidget;
    const auto show_page_geometry=ui->ShowWidget->geometry();
    const auto show_page_size=ui->ShowWidget->size();
    app_page->setGeometry(show_page_geometry);
    setting_page->setGeometry(show_page_geometry);
    this->pages.push_back(app_page);
    this->pages.push_back(setting_page);
    this->mainStack->addWidget(app_page);
    this->mainStack->addWidget(setting_page);
    ui->show_vertical->addWidget(this->mainStack);
    AwesomeFont::Instance()->setIcon(ui->closeButton,QChar(0xf00d),10);
    AwesomeFont::Instance()->setIcon(ui->minusButton,QChar(0xf068),10);
    AwesomeFont::Instance()->setIcon(ui->settingButton,QChar(0xf013),10);
    FlatUI::setPushButtonQss(ui->closeButton,0,0,"#F0FFFF","#000000","#FF6A6A","#1C1C1C","#FF0000","#FFFFFF");
    FlatUI::setPushButtonQss(ui->minusButton,0,0,"#F0FFFF","#000000","#FF6A6A","#1C1C1C","#FF0000","#FFFFFF");
    FlatUI::setPushButtonQss(ui->settingButton,0,0,"#F0FFFF","#000000","#FF6A6A","#1C1C1C","#FF0000","#FFFFFF");
    ui->closeButton->setToolTip(QString("Close application."));
    ui->minusButton->setToolTip(QString("Hide application."));
    ui->settingButton->setToolTip(QString("Change settings of software or look up for helps!\nClick again to switch!"));

    /// application pages
    // title part
    auto app_vertical=new QVBoxLayout();
    QLabel* app_title=new QLabel();
    app_title->setText("Select a File/Directory to Encrypt/Descrypt");
    app_title->setStyleSheet(FileDirButton::style_labelword());
    app_title->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    app_title->resize(QSize(show_page_size.width(),26*2));
    app_title->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    app_title->setFrameShape(QFrame::Box);

    // choose part
    QWidget* buttonLayout=new QWidget();
    auto inner_horizental=new QHBoxLayout();
    auto innerButtonStack=new QStackedWidget;
    auto button_page_1=new QWidget;
    auto button_page_2=new QWidget;
    auto button_page_horizental_1=new QHBoxLayout;
    auto button_page_horizental_2=new QHBoxLayout;
    auto app_choose=new QPushButton();
    app_choose->setToolTip(QString("Select target for encryption or decryption!"));
    auto app_choose_file=new QPushButton();
    app_choose_file->setToolTip(QString("Only for single file per operation!"));
    auto app_choose_dir=new QPushButton();
    app_choose_dir->setToolTip(QString("Only for single folder per operation!"));
    button_page_horizental_1->addWidget(app_choose);
    button_page_1->setLayout(button_page_horizental_1);
    button_page_horizental_2->addWidget(app_choose_file);
    button_page_horizental_2->addWidget(app_choose_dir);
    button_page_2->setLayout(button_page_horizental_2);
    innerButtonStack->addWidget(button_page_1);
    innerButtonStack->addWidget(button_page_2);
    inner_horizental->addWidget(innerButtonStack);
    buttonLayout->setLayout(inner_horizental);

    AwesomeFont::Instance()->setIcon(app_choose,this->picPath[b_state],E_BIGICON_SIZE/2);
    FlatUI::setPushButtonQss(app_choose,120,0,"#F0FFFF","#000000","#D3D3D3","#1C1C1C","#BEBEBE","#FFFFFF");
    app_choose->setFixedSize(QSize(240,240));
    AwesomeFont::Instance()->setIcon(app_choose_file,this->picPath[1],E_BIGICON_SIZE/4);
    FlatUI::setPushButtonQss(app_choose_file,5,0,"#F0FFFF","#000000","#D3D3D3","#1C1C1C","#BEBEBE","#FFFFFF");
    app_choose_file->setFixedSize(QSize(240/2,240/2));

    AwesomeFont::Instance()->setIcon(app_choose_dir,this->picPath[2],E_BIGICON_SIZE/4);
    FlatUI::setPushButtonQss(app_choose_dir,5,0,"#F0FFFF","#000000","#D3D3D3","#1C1C1C","#BEBEBE","#FFFFFF");
    app_choose_dir->setFixedSize(QSize(240/2,240/2));

    // File/Dir Name Show
    auto nameLabel=new QLabel();
    nameLabel->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    nameLabel->setStyleSheet(FileDirButton::style_labelword());
    nameLabel->setText("Add File/Directory");

    // input part
    QWidget* inputLayout=new QWidget();
    auto Hint_Box=new QWidget();
    auto input_vertical=new QVBoxLayout();
    auto input_stacked=new QStackedWidget();
    auto input_page_1=new QWidget();
    auto input_page_2=new QWidget();
    input_stacked->addWidget(input_page_1);
    input_stacked->addWidget(input_page_2);
    input_vertical->addWidget(input_stacked);
    //hint when input
    auto hint_input=new QLineEdit();
    hint_input->setPlaceholderText("Hint(Option)");
    hint_input->setAttribute(Qt::WA_InputMethodEnabled, false);
    hint_input->setToolTip(QString("To make sure that hint text stays correctly, only English words and special symbols on keyboard are supported!"
                                   "\nAttention:\n        MaxLength of input is 128 letters!"));
//    QRegExp rx("^[A-Za-z0-9`~!@#$%^&*()_-+=<>,.\\]+$");
//    QRegExpValidator *latitude = new QRegExpValidator(rx, this);
//    hint_input->setValidator(latitude);
    hint_input->setMaxLength(128);
    hint_input->setStyleSheet("background-color:white;border:none;font:25 11pt 'Times New Roman';");
    // hint when output
    auto hint_title=new QLabel();
    hint_title->setText("HINT");
    hint_title->setFont(QFont("Microsoft YaHei", 15, 75));
    hint_title->setStyleSheet(FileDirButton::style_labelword());
    hint_title->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    auto hint_txt=new QLabel();
    hint_txt->setStyleSheet(FileDirButton::style_labelhint());
    hint_txt->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    hint_txt->setWordWrap(true);
    hint_txt->setText("This is test words, you can ignore it completely!"
                      " This is test words, you can ignore it completely!"
                      " This is test words, you can ignore it completely!"); //debug


    auto passwd_input_1=new PasswordEditor();
    auto passwd_input_2=new PasswordEditor();
    passwd_input_1->setToolTip(QString("Password no more than 20 letters!"));
    passwd_input_2->setToolTip(QString("Password no more than 20 letters!"));
    passwd_input_1->resize(QSize(show_page_size.width(),26));
    passwd_input_2->resize(QSize(show_page_size.width(),26));
    passwd_input_1->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    passwd_input_2->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

    //page-1: input page
    auto input_inner_vertical_1=new QVBoxLayout();
    input_inner_vertical_1->addWidget(hint_input);
    input_inner_vertical_1->addWidget(passwd_input_1);
    input_inner_vertical_1->setAlignment(Qt::AlignCenter);
    input_page_1->setLayout(input_inner_vertical_1);
    //page-2: output page
    auto input_inner_vertical_2=new QVBoxLayout();
    input_inner_vertical_2->addWidget(hint_title);
    input_inner_vertical_2->addWidget(hint_txt);
    input_inner_vertical_2->addWidget(passwd_input_2);
    input_inner_vertical_2->setAlignment(Qt::AlignCenter);
    input_page_2->setLayout(input_inner_vertical_2);

    Hint_Box->setLayout(input_vertical);

    auto check_button=new ProgressButton();
    check_button->setToolTip(QString("Check validation of current operation! After checking, click again to switch to main page."));
    check_button->setButtonPattern(false);
    connect(check_button,&ProgressButton::startProcessing,this,[=]() {
        check_button->RequireValueChange(100);
    }); // Debug

    auto big_vertical=new QVBoxLayout();
    big_vertical->addStretch();
    big_vertical->addWidget(Hint_Box);
    big_vertical->addWidget(check_button);
    inputLayout->setLayout(big_vertical);

    //combined
    app_vertical->addWidget(app_title);
    app_vertical->addWidget(buttonLayout);
    app_vertical->addStretch();
    app_vertical->addWidget(nameLabel);
    app_vertical->addWidget(inputLayout);
    app_vertical->setAlignment(Qt::AlignCenter);
    app_page->setLayout(app_vertical);

    //************************** Application Page Connection**********************************//
    // 保存重要的指针信息
    this->passwdArray[0]=passwd_input_1;
    this->passwdArray[1]=passwd_input_2;
    this->hintInput=hint_input;
    this->hintOutput=hint_txt;
    this->runButton=check_button;

    // 点击添加按钮，显示选择文件或文件夹
    connect(app_choose,&QToolButton::clicked,this,[=]() {
        innerButtonStack->setCurrentIndex(1);
    });
    // 选择文件/文件夹完毕后，界面切换
    connect(this,&Widget::backChoose,this,[=]() {
        innerButtonStack->setCurrentIndex(0);
        switch(m_objType) {
        case ObjectType::FILE_ENCRYPT:
            b_state=1;
            check_button->setTxt("File Encrypt");
            check_button->setButtonPattern(true);
            input_stacked->setCurrentIndex(0);  //切换到加密输入框
            nameLabel->setText(QString("File(selected): %1").arg(m_fileInfo.name));
            break;
        case ObjectType::FILE_DECRYPT:
            b_state=1;
            check_button->setTxt("File Decrypt");
            check_button->setButtonPattern(true);
            input_stacked->setCurrentIndex(1);  //切换到解密输入框
            nameLabel->setText(QString("File(selected): %1").arg(m_fileInfo.name));
            break;
        case ObjectType::DIR_DECRYPT:
            b_state=2;
            check_button->setTxt("Directory Decrypt");
            check_button->setButtonPattern(true);
            input_stacked->setCurrentIndex(1);//切换到解密输入框
            nameLabel->setText(QString("Directory(selected): %1").arg(m_dirInfo.raw_name));
            break;
        case ObjectType::DIR_ENCRYPT:
            b_state=2;
            check_button->setTxt("Directory Encrypt");
            check_button->setButtonPattern(true);
            input_stacked->setCurrentIndex(0);  //切换到加密输入框
            nameLabel->setText(QString("Directory(selected): %1").arg(m_dirInfo.raw_name));
            break;
        case ObjectType::UNKNOWN:
        default:
            check_button->setTxt("Sure");
            check_button->setButtonPattern(false);
            input_stacked->setCurrentIndex(0);  //切换到加密输入框
            b_state=0;
            nameLabel->setText("Add File/Directory");
            break;
        }
//        app_choose->setIcon(this->picPath[b_state]);
        AwesomeFont::Instance()->setIcon(app_choose,this->picPath[b_state],E_BIGICON_SIZE/2);
        FlatUI::setPushButtonQss(app_choose,120,0,"#F0FFFF","#000000","#D3D3D3","#1C1C1C","#BEBEBE","#FFFFFF");
        app_choose->setFixedSize(QSize(240,240));
    });
    // 选择文件/文件夹并获得有关信息
    connect(app_choose_file,&QToolButton::clicked,this,&Widget::getFilePath);
    connect(app_choose_dir,&QToolButton::clicked,this,&Widget::getDirPath);
    // 加解密完成后，恢复原始界面状态
    connect(check_button,&ProgressButton::closeProcessing,this,[=]() {
        m_objType=ObjectType::UNKNOWN;
        this->hintInput->clear();
        this->passwdArray[0]->clear();
        this->passwdArray[1]->clear();
        emit backChoose();
    });

//***************************************Setting Page Design******************************************************//
    /// Setting page
    // key_length part
    auto keyLengthWidget=new QWidget;
    auto keyLengthHorizental=new QHBoxLayout;
    auto keyLengthLabel=new QLabel;
    auto keyLengthBox=new QComboBox;
    keyLengthBox->setToolTip(QString("Strength of cryption."));
    keyLengthLabel->setStyleSheet(FileDirButton::style_labelword());
    keyLengthLabel->setText("Key Length");
    keyLengthLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    keyLengthBox->addItems({"AES-128","AES-192","AES-256"});
    keyLengthBox->setStyleSheet(FileDirButton::style_comboBox());
    keyLengthHorizental->addWidget(keyLengthLabel);
    keyLengthHorizental->addWidget(keyLengthBox);
    keyLengthWidget->setLayout(keyLengthHorizental);

    // key_mode part
    auto keyModeWidget=new QWidget;
    auto keyModeHorizental=new QHBoxLayout;
    auto keyModeLabel=new QLabel;
    auto keyModeBox=new QComboBox;
    keyModeBox->setToolTip(QString("Mode of work flow."));
    keyModeLabel->setStyleSheet(FileDirButton::style_labelword());
    keyModeLabel->setText("Mode");
    keyModeLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    keyModeBox->addItems({"ECB","CBC","CFB","OFB"});
    keyModeBox->setStyleSheet(FileDirButton::style_comboBox());
    keyModeHorizental->addWidget(keyModeLabel);
    keyModeHorizental->addWidget(keyModeBox);
    keyModeWidget->setLayout(keyModeHorizental);

    // key_padding part
    auto keyPadWidget=new QWidget;
    auto keyPadHorizental=new QHBoxLayout;
    auto keyPadLabel=new QLabel;
    auto keyPadBox=new QComboBox;
    keyPadBox->setToolTip(QString("Padding algorithm for short password."));
    keyPadLabel->setStyleSheet(FileDirButton::style_labelword());
    keyPadLabel->setText("Padding");
    keyPadLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    keyPadBox->addItems({"ISO","PKCS7","ZERO"});
    keyPadBox->setStyleSheet(FileDirButton::style_comboBox());

    keyPadHorizental->addWidget(keyPadLabel);
    keyPadHorizental->addWidget(keyPadBox);
    keyPadWidget->setLayout(keyPadHorizental);

    // unite: key_length, Key_mode, Key_padding
    auto keyChooseWidget=new QWidget;
    auto keyChooseVertical=new QVBoxLayout;
    keyChooseVertical->addStretch();
    keyChooseVertical->addWidget(keyLengthWidget);
    keyChooseVertical->addWidget(keyModeWidget);
    keyChooseVertical->addWidget(keyPadWidget);
    keyChooseWidget->setLayout(keyChooseVertical);

    // Infomation Part
    auto infoWidget=new QWidget;
    auto infoVertical=new QVBoxLayout;
    auto infoLabel=new QLabel;
    auto infoBrowser=new QTextBrowser;
    infoBrowser->setToolTip(QString("Some help from author."));
    infoLabel->setStyleSheet(FileDirButton::style_labelword());
    infoLabel->setText("INFOMATION");
    infoLabel->setFont(QFont("Microsoft YaHei", 15, 75));
    infoLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    infoBrowser->setStyleSheet(FileDirButton::style_textbrowser());
    infoBrowser->setText(PROJECT_INFORMATION);
    infoVertical->addWidget(infoLabel);
    infoVertical->addWidget(infoBrowser);
    infoWidget->setLayout(infoVertical);

    // Button Part
    auto btnWidget=new QWidget;
    auto btnHorizental=new QHBoxLayout;
    auto yesButton=new QPushButton;
    auto cancelButton=new QPushButton;
    yesButton->setToolTip(QString("Save current state of setting and leave this page."));
    cancelButton->setToolTip(QString("Recover to default setting without leaving."));
    FlatUI::setPushButtonQss(yesButton,5,8,"#7CFC00","#FFFFFF","#32CD32","#F0FFF0","#3D9140","#B8C6D1");
    FlatUI::setPushButtonQss(cancelButton,5,8,"#FF6347","#FFFFFF","#FF4500","#F0FFF0","#E3170D","#B8C6D1");
    yesButton->setText("Yes");
    cancelButton->setText("Default");
    btnHorizental->addWidget(yesButton);
    btnHorizental->addWidget(cancelButton);
    btnWidget->setLayout(btnHorizental);

    // combine
    auto setting_vertical=new QVBoxLayout;
    setting_vertical->addWidget(keyChooseWidget);
    setting_vertical->addWidget(infoWidget);
    setting_vertical->addWidget(btnWidget);
    setting_page->setLayout(setting_vertical);

    //******************************Setting Page Connection**********************************************//
    connect(yesButton,&QPushButton::clicked,this,[=]() {
        this->AES_Length=keyLengthBox->currentIndex();
        this->AES_Mode=keyModeBox->currentIndex();
        this->AES_Padding=keyPadBox->currentIndex();
        ui->settingButton->click();
    });
    connect(cancelButton,&QPushButton::clicked,this,[=]() {
        this->AES_Length=this->AES_Mode=this->AES_Padding=0;
        keyLengthBox->setCurrentIndex(0);
        keyModeBox->setCurrentIndex(0);
        keyPadBox->setCurrentIndex(0);
    });

//**********************************Global Connection of Pages******************************************************//
    /// Global Page Connection


}

void Widget::initConnection() {
    //窗体按钮
    connect(ui->closeButton,&QToolButton::clicked,this,&Widget::close);
    connect(ui->minusButton,&QToolButton::clicked,this,&Widget::showMinimized);
    connect(ui->settingButton,&QToolButton::clicked,this,&Widget::switchMainPage);

    //当加解密完成时状态定义与显示，同时清除多余文件
    connect(m_fileEncryption,&FileEncryption::finish,this,[=](bool hasError) {
        if(hasError)
            runButton->setButtonPattern(false);
        else
            runButton->setButtonPattern(true);
        if(m_objType==ObjectType::DIR_ENCRYPT||m_objType==ObjectType::DIR_DECRYPT)
            if(m_fileInfo.name.contains("zip"))
                QFile::remove(m_fileInfo.path+m_fileInfo.name);
        runButton->RequireValueChange(100);
    });
    //点击按钮，开始进行加解密
    connect(this->runButton,&ProgressButton::startProcessing,this,&Widget::startCryption);
}

void Widget::switchMainPage() {
    int nCount=this->mainStack->count();
    this->m_MainPageIndex=(this->m_MainPageIndex+1)%nCount;
    this->mainStack->setCurrentIndex(this->m_MainPageIndex);
//    if(this->m_MainPageIndex==1)
//        ui->settingButton->setStyleSheet("color:white;background-color:#FFFAF0;");
//    else
//        ui->settingButton->setStyleSheet("color:white;background-color:transparent;");
}

void Widget::mousePressEvent(QMouseEvent *event) {
    m_lastPos = event->globalPos();
    isPressedWidget = true; // 当前鼠标按下的即是QWidget而非界面上布局的其它控件
}

void Widget::mouseMoveEvent(QMouseEvent *event) {
    if (isPressedWidget) {
        this->move(this->x() + (event->globalX() - m_lastPos.x()),
                   this->y() + (event->globalY() - m_lastPos.y()));
        m_lastPos = event->globalPos();
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event) {
    // 其实这里的mouseReleaseEvent函数可以不用重写
    m_lastPos = event->globalPos();
    isPressedWidget = false; // 鼠标松开时，置为false
}

void Widget::getFilePath() {
    QString fileName = QFileDialog::getOpenFileName(this, "选择输入文件", "./", "输入文件(*)");
    if(!fileName.isEmpty()) {
        QFileInfo info(fileName);
        QString name = info.fileName();
        QString path = info.canonicalPath();
        path+="/";
        this->m_fileInfo.update(name,path);

        if(m_fileInfo.appendix==E_FILE_APPENDIX)
            m_objType=ObjectType::FILE_DECRYPT;
        else if(m_fileInfo.appendix==E_DIR_APPENDIX)
            m_objType=ObjectType::DIR_DECRYPT;
        else
            m_objType=ObjectType::FILE_ENCRYPT;
        if(m_objType==ObjectType::FILE_DECRYPT||m_objType==ObjectType::DIR_DECRYPT) {
            auto hint_text=m_fileEncryption->getHintText(fileName);
            if(hint_text.isEmpty())hint_text="(Info: None)";
            this->hintOutput->setText(hint_text);
        }
        emit backChoose();
    } else {
        m_objType=ObjectType::UNKNOWN;
        emit backChoose();
    }
}

void Widget::getDirPath() {
    QString dirName=QFileDialog::getExistingDirectory(this,"选择输入文件夹","./");
    if(!dirName.isEmpty()) {
        QDir dir(dirName);
        QString name=dir.dirName();
        auto pathElements=dirName.split("/");
        QString path="";
        for(int i=0; i<pathElements.size()-1; i++)
            path+=QString("%1/").arg(pathElements[i]);
        this->m_dirInfo.update(dirName,name,path);
        m_objType=ObjectType::DIR_ENCRYPT;
        emit backChoose();
    } else {
        m_objType=ObjectType::UNKNOWN;
        emit backChoose();
    }
}

void Widget::startCryption() {
    switch(m_objType) {
    case ObjectType::FILE_ENCRYPT:
        startEncryptFile();
        break;
    case ObjectType::FILE_DECRYPT:
        startDecryptFile();
        break;
    case ObjectType::DIR_ENCRYPT:
        startEncryptDir();
        break;
    case ObjectType::DIR_DECRYPT:
        startDecryptDir();
        break;
    default:
        break;
    }
}

void Widget::startEncryptFile() {
    qDebug()<<"Enter "<<__FUNCTION__<<" : "<<m_fileInfo.path+m_fileInfo.name;
    if(m_objType!=ObjectType::FILE_ENCRYPT&&m_objType!=ObjectType::DIR_ENCRYPT)
        return;
    QString inFile=m_fileInfo.path+m_fileInfo.name;
    QString outFile=m_fileInfo.path+m_fileInfo.raw_name;
    if(m_objType==ObjectType::FILE_ENCRYPT)
        outFile+=QString(".%1.%2").arg(m_fileInfo.appendix,E_FILE_APPENDIX);
    else
        outFile+=QString(".%1").arg(E_DIR_APPENDIX);
    m_fileEncryption->setFile(inFile,outFile);
    m_fileEncryption->setEncryption(true);
    this->runButton->RequireValueChange(30);
    QByteArray arrkey(this->passwdArray[0]->text().trimmed().toUtf8());
    if(arrkey.isEmpty()) {
//        QMessageBox::about(this,"Attention!","Please input key!");
        MyMessageBox msg_box(MyMessageBox::MsgBoxType::MsgBoxType_Error,"Please input key!",this);
        msg_box.exec();
        this->runButton->setButtonPattern(false);
        this->runButton->RequireValueChange(100);
        return;
    }
    arrkey=QCryptographicHash::hash(arrkey,QCryptographicHash::Md5).toHex();
    m_fileEncryption->setKey(arrkey);
    m_fileEncryption->setHintText(this->hintInput->text().trimmed());
    m_fileEncryption->setAESParameter((QAESEncryption::Aes)this->AES_Length,
                                      (QAESEncryption::Mode)this->AES_Mode,
                                      (QAESEncryption::Padding)this->AES_Padding);
    this->runButton->RequireValueChange(75);
    emit m_fileEncryption->start();
}

void Widget::startDecryptFile() {
    if(m_objType!=ObjectType::FILE_DECRYPT&&m_objType!=ObjectType::DIR_DECRYPT)
        return;
    QString inFile=m_fileInfo.path+m_fileInfo.name;
    QString outFile=m_fileInfo.path+m_fileInfo.raw_name;
    if(m_objType==ObjectType::DIR_DECRYPT)
        if(outFile.endsWith(".zip")==false) {
            outFile+=".zip";
            m_fileInfo.name=m_fileInfo.raw_name+".zip";
            m_fileInfo.appendix=".zip";
        }
    qDebug()<<__FUNCTION__;
    qDebug()<<"inFile: "<<inFile;
    qDebug()<<"outFile: "<<outFile;
    m_fileEncryption->setFile(inFile,outFile);
    m_fileEncryption->setEncryption(false);
    this->runButton->RequireValueChange(30);
    QByteArray arrkey(this->passwdArray[1]->text().trimmed().toUtf8());
    if(arrkey.isEmpty()) {
//        QMessageBox::about(this,"Attention!","Please input key!");
        MyMessageBox msg_box(MyMessageBox::MsgBoxType::MsgBoxType_Error,"Please input key!",this);
        msg_box.exec();
        this->runButton->setButtonPattern(false);
        this->runButton->RequireValueChange(100);
        return;
    }
    arrkey=QCryptographicHash::hash(arrkey,QCryptographicHash::Md5).toHex();
    m_fileEncryption->setKey(arrkey);
    m_fileEncryption->setHintText(this->hintInput->text().trimmed());
    m_fileEncryption->setAESParameter((QAESEncryption::Aes)this->AES_Length,
                                      (QAESEncryption::Mode)this->AES_Mode,
                                      (QAESEncryption::Padding)this->AES_Padding);
    this->runButton->RequireValueChange(75);
    emit m_fileEncryption->start();
}

void Widget::startEncryptDir() {
    if(m_objType!=ObjectType::DIR_ENCRYPT)
        return;
    if(!dirZiper())return;
    startEncryptFile();
}

void Widget::startDecryptDir() {
    if(m_objType!=ObjectType::DIR_DECRYPT)
        return;
    startDecryptFile();
    if(QByteArray(this->passwdArray[1]->text().trimmed().toUtf8()).isEmpty())
        return;
    dirUnZiper();
}

bool Widget::dirZiper() {
    if(m_dirInfo.name.isEmpty()||m_dirInfo.raw_name.isEmpty()||m_dirInfo.path.isEmpty())
        return false;
    compreDecompreFileThread->compression_file_run(m_dirInfo.name,m_dirInfo.path);
    qDebug()<<__FUNCTION__;
    qDebug()<<"Name: "<<m_dirInfo.name;
    qDebug()<<"Path: "<<m_dirInfo.path;
    m_fileInfo.update(m_dirInfo.raw_name+".zip",m_dirInfo.path);
    return true;
}

bool Widget::dirUnZiper() {
    if(m_fileInfo.name.isEmpty()||m_fileInfo.raw_name.isEmpty()||
            m_fileInfo.path.isEmpty()||m_fileInfo.appendix.isEmpty())
        return false;
    auto srcFullFile=m_fileInfo.path+m_fileInfo.raw_name+m_fileInfo.appendix;
    auto dstPath=m_fileInfo.path+m_fileInfo.raw_name+"/";
    while(QFile::exists(srcFullFile)==false)
        qDebug()<<"Don't detect zip file!"<<__FUNCTION__;
    if(QDir().exists(dstPath)==false)
        QDir().mkpath(dstPath);
    else {
//        QMessageBox::about(this,"Infomation","文件夹已存在，已终止程序运行");
        MyMessageBox msg_box(MyMessageBox::MsgBoxType::MsgBoxType_Info,"Directory is already existed! Program stopped!",this);
        msg_box.exec();
        return false;
    }
    dstPath.chop(1);
    compreDecompreFileThread->decompression_file_run(srcFullFile,dstPath);
    qDebug()<<__FUNCTION__;
    qDebug()<<"srcFile: "<<srcFullFile;
    qDebug()<<"dstPath: "<<dstPath;
    qDebug()<<m_fileInfo.path;
    return true;
}
