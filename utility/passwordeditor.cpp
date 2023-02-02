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
    QString bgColor = "#FFFFFF";
    QString borderColor = "#A6B5B8";

    pngLabel = new QLabel;
    pngLabel->setAlignment(Qt::AlignCenter);
    pngLabel->setPixmap(QPixmap(":/new/prefix1/image/closeEye.png").scaled(26, 26));

    lineEdit = new QLineEdit;
    lineEdit->setObjectName("lineEdit");
    lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    lineEdit->setMaxLength(20);
    lineEdit->setPlaceholderText("Password(<=20 letters)");

    lineEdit->setEchoMode(QLineEdit::Password);
    pngLabel->installEventFilter(this);

    QFrame *frame = new QFrame;
    frame->setObjectName("framePassword");

    QStringList qss;
    qss.append(QString("QFrame#framePassword{border-radius:3px;border-style:none;}"));
    qss.append(QString("QLabel{min-width:15px;background-color:%1;}").arg(bgColor));
    qss.append(QString("QLineEdit{background-color:%1;border:none;font:25 11pt 'Times New Roman';}").arg(bgColor));
    frame->setStyleSheet(qss.join(""));

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
