#include "progressbutton.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QVariantAnimation>
#include <QDebug>

ProgressButton::ProgressButton(QWidget *parent)
    : QWidget{parent} {
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    setFixedHeight(45);
    setMinimumWidth(140);
    auto font = this->font();
    font.setPixelSize(20);
    setFont(font);

    setMouseTracking(true);

    connect(this,&ProgressButton::startProcessing,this,&ProgressButton::operationProcessing,Qt::QueuedConnection);
    connect(this,&ProgressButton::valueChanged,this,[=](int value) {
        if(can_value_modify == true) {
            progress = value;
            if(value>100)progress=100;
            repaint();
            operationProcessing();
        }
    },Qt::QueuedConnection);
    connect(this,&ProgressButton::startProcessing,this,[=]() {
        can_value_modify = true;
    });
    connect(this,&ProgressButton::closeProcessing,this,[=]() {
        can_value_modify = false;
    });
}

ProgressButton::~ProgressButton() {

}

void ProgressButton::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    auto rect=event->rect();
    if(buttonState == state::NORMAL || buttonState == state::HOVER) {
        QPainterPath path;
        path.addRoundedRect(rect,30,30);
        painter.setClipPath(path);
        painter.fillRect(rect,Qt::white);

        painter.save();
        painter.setBrush(QColor(buttonState == state::NORMAL ? 0x51c582 : 0x37be77));
        painter.setPen(Qt::transparent);
        painter.drawRoundedRect(rect.adjusted(3,3,-3,-3),30,30);
        painter.restore();

        painter.setPen(Qt::white);
        painter.setFont(this->font());
        painter.drawText(rect, Qt::AlignCenter, txt);
    } else if(buttonState == state::FROM_ROUNDED_CORNERS_TO_ROUNDED || buttonState == state::RECOVERY) {
        painter.setBrush(QColor(0x51c582));
        painter.setPen(QPen(QColor(0x45B078),3));
        painter.translate(rect.center());
        painter.drawRoundedRect(QRect(-widthChangeValue,
                                      -(rect.height() / 2 - 3),
                                      widthChangeValue * 2,
                                      rect.height() - 6),30,30);
    } else if(buttonState == state::OPEN_PROGRESS) {
        painter.translate(rect.center());
        auto radiu = (rect.height() - 6) / 2 -3;
        painter.setBrush(QColor(0x51c582));
        painter.setPen(QPen(QColor(0x45B078),3));
        painter.drawEllipse(QPoint(0,0),radiu,radiu);

        painter.setPen(QPen(Qt::white,3));

        QRect rect = QRect(-radiu, -radiu,
                           radiu*2, radiu*2);

        auto angle = progress * 360 / 100;
        painter.drawArc(rect.adjusted(-3,-3,3,3),90 * 16,-static_cast<int>(angle * 16));
    } else if(buttonState == state::CLOSE_PROGRESS) {
        auto radiu = (rect.height() - 6) / 2;
        painter.translate(rect.center());
        painter.setPen(Qt::transparent);
        painter.setBrush(QColor(0, 0, 0, 63));
        painter.drawEllipse(QPoint(0,0),radiu,radiu);

        radiu -= 3;
        painter.setBrush(Qt::white);
        painter.drawEllipse(QPoint(0,0),radiu,radiu);

        switch(this->buttonItem) {
        case symbol_flag::CORRECT:
            painter.setPen(QPen(QColor("#4DAF51"),3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
            painter.drawLine(QPoint(-radiu / 3,0),
                             QPoint(-radiu / 5,radiu / 3));
            painter.drawLine(QPoint(-radiu / 5,radiu / 3),
                             QPoint(radiu / 4,-radiu / 4));
            break;
        case symbol_flag::ERROR:
        default:
            painter.setPen(QPen(QColor("#E3170D"),3,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
            painter.drawLine(QPoint(-radiu / 4,-radiu / 4),
                             QPoint(radiu / 4,radiu / 4));
            painter.drawLine(QPoint(-radiu / 4,radiu / 4),
                             QPoint(radiu / 4,-radiu / 4));
        }
    }
}

void ProgressButton::enterEvent(QEvent* event) {
    if(buttonState == state::NORMAL) {
        buttonState = state::HOVER;
        update();
    }
    QWidget::enterEvent(event);
}

void ProgressButton::leaveEvent(QEvent* event) {
    if(buttonState == state::HOVER) {
        buttonState = state::NORMAL;
        update();
    }
    QWidget::leaveEvent(event);
}

void ProgressButton::mousePressEvent(QMouseEvent* event) {
    if(buttonState == state::HOVER || buttonState == state::NORMAL) {
        buttonState = state::FROM_ROUNDED_CORNERS_TO_ROUNDED;
//        widthChangeValue = (this->width() - 6) / 2;
//        timer.start();
        onTriggering();
        update();
    } else if(buttonState == state::CLOSE_PROGRESS) {
        buttonState = state::RECOVERY;
//        timer.start();
        onTriggering();
        update();
    }
    QWidget::mousePressEvent(event);
}

void ProgressButton::onTriggering() {
    if(buttonState == state::FROM_ROUNDED_CORNERS_TO_ROUNDED) {
        widthChangeValue = this->height();
        buttonState = state::OPEN_PROGRESS;
        progress = 0;
        emit startProcessing();
    } else {
        widthChangeValue = this->width()/2;
        buttonState = state::NORMAL;
        emit closeProcessing();
    }
    update();
}

void ProgressButton::operationProcessing() {
    if(progress == 100) {
        buttonState = state::CLOSE_PROGRESS;
        update();
        auto waterDrop = new WaterDrop();
        waterDrop->move(this->mapToGlobal(this->rect().center()));
        waterDrop->show();
    }
}

void ProgressButton::setTxt(const QString& val) {
    this->txt=val;
}

QString ProgressButton::Txt() {
    return this->txt;
}

void ProgressButton::RequireValueChange(int val) {
    emit valueChanged(val);
}

void ProgressButton::setButtonPattern(const bool flag) {
    if(flag) this->buttonItem=symbol_flag::CORRECT;
    else this->buttonItem=symbol_flag::ERROR;
}

bool ProgressButton::ButtonPattern() const {
    if(this->buttonItem==symbol_flag::CORRECT) return true;
    return false;
}


const int RADIUS = 60;
WaterDrop::WaterDrop(QWidget* parent)
    : QWidget(parent),
      m_waterDropAnimation(nullptr),
      m_animationRadius(0) {

    this->setFixedSize(QSize(RADIUS * 2, RADIUS * 2));
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAttribute(Qt::WA_DeleteOnClose);
    m_waterDropAnimation = new QVariantAnimation(this);
}

WaterDrop::~WaterDrop() {

}

void WaterDrop::move(const QPoint &point) {
    QPoint translatePoint = point - QPoint(RADIUS, RADIUS);
    QWidget::move(translatePoint);
}

void WaterDrop::show() {
    m_waterDropAnimation->setStartValue(0);
    m_waterDropAnimation->setEndValue(RADIUS);
    m_waterDropAnimation->setDuration(350);

    connect(m_waterDropAnimation, &QVariantAnimation::valueChanged, this, &WaterDrop::onRaduisChanged);
    connect(m_waterDropAnimation, &QVariantAnimation::finished, this, &WaterDrop::close);
    m_waterDropAnimation->start();
    QWidget::show();
}

void WaterDrop::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;
    pen.setColor(QColor(0xffffff80));
    pen.setWidth(5);
    painter.setPen(pen);
    painter.drawEllipse(event->rect().center(),m_animationRadius, m_animationRadius);
}

void WaterDrop::onRaduisChanged(QVariant value) {
    m_animationRadius = value.toInt();
    update();
}


