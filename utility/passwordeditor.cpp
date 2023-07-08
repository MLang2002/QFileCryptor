#include "passwordeditor.h"
#include <QLabel>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QValidator>
#include <QRegExp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QDebug>

PasswordEditor::PasswordEditor(QWidget *parent)
    : QWidget{parent}
{
    QString bgColor = "#FFFFFF";    //背景颜色
    QString borderColor = "#A6B5B8";//边框颜色

    pngLabel = new QLabel;
    pngLabel->setAlignment(Qt::AlignCenter);
    pngLabel->setPixmap(QPixmap(":/new/prefix1/image/closeEye.png").scaled(26, 26));

    lineEdit = new QLineEdit;
    lineEdit->setObjectName("lineEdit");
    lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);// 禁用输入法


//    QRegExp regx("[0-9a-zA-Z]+$");        // 限制只能输入数字
//    QValidator *validator = new QRegExpValidator(regx, lineEdit);
//    lineEdit->setValidator( validator );
    lineEdit->setMaxLength(20);      // 限制只能输入20位
    lineEdit->setPlaceholderText("Password(<=20 letters)");

    lineEdit->setEchoMode(QLineEdit::Password);
    pngLabel->installEventFilter(this);

    QFrame *frame = new QFrame;
    frame->setObjectName("framePassword");

    QStringList qss;
//    qss.append(QString("QFrame#framePassword{border:1px solid %1;}").arg(borderColor));
    qss.append(QString("QFrame#framePassword{border-radius:3px;border-style:none;}"));
    qss.append(QString("QLabel{min-width:15px;background-color:%1;}").arg(bgColor));
    qss.append(QString("QLineEdit{background-color:%1;border:none;font:25 11pt 'Times New Roman';}").arg(bgColor));
    frame->setStyleSheet(qss.join(""));

    //将控件按照横向布局排列
    QHBoxLayout *layout = new QHBoxLayout(frame);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(lineEdit);
    layout->addWidget(pngLabel);

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setMargin(0);
    verticalLayout->setSpacing(0);
    verticalLayout->addWidget(frame);
}

PasswordEditor::~PasswordEditor()
{

}

bool PasswordEditor::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
        case QEvent::MouseButtonPress:
            if(can_see==false)
            {
                lineEdit->setEchoMode(QLineEdit::Normal);
                pngLabel->setPixmap(QPixmap(":/new/prefix1/image/openEye.png").scaled(26, 26));
                can_see=true;
            }
            else
            {
                lineEdit->setEchoMode(QLineEdit::Password);
                pngLabel->setPixmap(QPixmap(":/new/prefix1/image/closeEye.png").scaled(26, 26));
                can_see=false;
            }
            break;
        default:
            break;
    }
    return QWidget::eventFilter(watched, event);
}
void PasswordEditor::setFont(const QFont &font)
{
    lineEdit->setFont(font);
}

QString PasswordEditor::text() const
{
    return lineEdit->text();
}

void PasswordEditor::setHintTxt(const QString& txt)
{
    lineEdit->setPlaceholderText(txt);
}

QFont PasswordEditor::getFont()const
{
    return lineEdit->font();
}

void PasswordEditor::clear()
{
    this->lineEdit->clear();
}
