#ifndef PASSWORDEDITOR_H
#define PASSWORDEDITOR_H

#include <QWidget>

class PasswordEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PasswordEditor(QWidget *parent = nullptr);
    ~PasswordEditor();
    QString text() const;
    void setFont(const QFont& font);
    QFont getFont()const;
    void setHintTxt(const QString& txt);
    void clear();

protected:
    bool eventFilter(QObject* watched, QEvent* event);

private:
    class QLineEdit* lineEdit;
    class QLabel *pngLabel;
    bool can_see=false;

signals:

};

#endif // PASSWORDEDITOR_H
