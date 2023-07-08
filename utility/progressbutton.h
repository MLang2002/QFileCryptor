#ifndef PROGRESSBUTTON_H
#define PROGRESSBUTTON_H

#include <QWidget>
#include <QTimer>

class ProgressButton : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressButton(QWidget *parent = nullptr);
    ~ProgressButton();
    void setTxt(const QString& txt);
    QString Txt();

    void setButtonPattern(const bool flag);
    bool ButtonPattern() const;

public slots:
    void RequireValueChange(int val);

signals:
    void startProcessing();
    void closeProcessing();
    void valueChanged(int value);

protected:
    void paintEvent(QPaintEvent* event)override;
    void enterEvent(QEvent* event)override;
    void leaveEvent(QEvent* event)override;
    void mousePressEvent(QMouseEvent* event)override;

private:
    void onTriggering();
    void operationProcessing();

private:
    bool can_value_modify=false;
    enum class state{
        NORMAL,
        HOVER,
        FROM_ROUNDED_CORNERS_TO_ROUNDED,
        OPEN_PROGRESS,
        CLOSE_PROGRESS,
        RECOVERY
    };
    state buttonState{state::NORMAL};
    QString txt{"Sure"};
//    QTimer timer;
    int widthChangeValue=0;
    int progress=0;

    enum class symbol_flag{
        CORRECT,
        ERROR
    };
    symbol_flag buttonItem{symbol_flag::CORRECT};
};

class WaterDrop: public QWidget
{
    Q_OBJECT
public:
    explicit WaterDrop(QWidget* parent=nullptr);
    ~WaterDrop();
    void show();
    void move(const QPoint& point);

private:
    void paintEvent(QPaintEvent* event);
    void onRaduisChanged(QVariant value);

private:
    class QVariantAnimation* m_waterDropAnimation;
    int m_animationRadius;
};

#endif // PROGRESSBUTTON_H
