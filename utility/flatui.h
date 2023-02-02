#ifndef FLATUI_H
#define FLATUI_H

#include <QObject>

class QPushButton;
class QLineEdit;
class QProgressBar;
class QSlider;
class QRadioButton;
class QCheckBox;
class QScrollBar;

#ifdef quc
class Q_DECL_EXPORT FlatUI
#else
class FlatUI
#endif

{
public:
    //设置按钮样式
    static QString setPushButtonQss(QPushButton *btn,
                                    int radius = 5,
                                    int padding = 8,
                                    const QString &normalColor = "#34495E",
                                    const QString &normalTextColor = "#FFFFFF",
                                    const QString &hoverColor = "#4E6D8C",
                                    const QString &hoverTextColor = "#F0F0F0",
                                    const QString &pressedColor = "#2D3E50",
                                    const QString &pressedTextColor = "#B8C6D1");

    //设置文本框样式
    static QString setLineEditQss(QLineEdit *txt,
                                  int radius = 3,
                                  int borderWidth = 2,
                                  const QString &normalColor = "#DCE4EC",
                                  const QString &focusColor = "#34495E");

    //设置进度条样式
    static QString setProgressQss(QProgressBar *bar,
                                  int barHeight = 8,
                                  int barRadius = 5,
                                  int fontSize = 12,
                                  const QString &normalColor = "#E8EDF2",
                                  const QString &chunkColor = "#E74C3C");

    //设置滑块条样式
    static QString setSliderQss(QSlider *slider,
                                int sliderHeight = 8,
                                const QString &normalColor = "#E8EDF2",
                                const QString &grooveColor = "#1ABC9C",
                                const QString &handleBorderColor = "#1ABC9C",
                                const QString &handleColor = "#FFFFFF");

    //设置单选框样式
    static QString setRadioButtonQss(QRadioButton *rbtn,
                                     int indicatorRadius = 8,
                                     const QString &normalColor = "#D7DBDE",
                                     const QString &checkColor = "#34495E");

    //设置滚动条样式
    static QString setScrollBarQss(QWidget *scroll,
                                   int radius = 6,
                                   int min = 120,
                                   int max = 12,
                                   const QString &bgColor = "#606060",
                                   const QString &handleNormalColor = "#34495E",
                                   const QString &handleHoverColor = "#1ABC9C",
                                   const QString &handlePressedColor = "#E74C3C");
};

#endif // FLATUI_H
