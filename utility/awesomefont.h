#ifndef AWESOMEFONT_H
#define AWESOMEFONT_H

#include <QObject>
#include <QFont>
#include <QFontDatabase>
#include <QMutex>
#include <QLabel>
#include <type_traits>


class AwesomeFont : public QObject {
  private:
    explicit AwesomeFont(QObject *parent = nullptr);
    QFont iconFont;
    static AwesomeFont* m_instance;

    template<bool flag, typename T=void>
    struct user_enable_if {};
    template<typename T>
    struct user_enable_if<true,T> {
        typedef void type;
    };

  public:
    static AwesomeFont* Instance() {
        static QMutex mutex;
        if(!m_instance) {
            QMutexLocker locker(&mutex);
            if(!m_instance)
                m_instance=new AwesomeFont;
        }
        return m_instance;
    }

    template<typename T>
    typename user_enable_if<std::is_pointer<T>::value,T>::type setIcon(T& widget,QChar ch, int size) {
        iconFont.setPointSize(size);
        widget->setFont(iconFont);
        widget->setText(ch);
    }
    template<typename T>
    typename user_enable_if<!std::is_pointer<T>::value,T>::type setIcon(T& widget, QChar ch, int size) {
        iconFont.setPointSize(size);
        widget.setFont(iconFont);
        widget.setText(ch);
    }



};

#endif // AWESOMEFONT_H
