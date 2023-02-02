#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QWidget>
#include <QDialog>

class MyMessageBox : public QDialog
{
    Q_OBJECT
public:
    enum class MsgBoxType
    {
        MsgBoxType_Warn = 0,
        MsgBoxType_Info = 1,
        MsgBoxType_Error = 2
    };
public:
    explicit MyMessageBox(MsgBoxType type,QString text,QWidget *parent = nullptr);
    void initState();

    void initWarn(const QString &text);
    void initError(const QString &text);
    void initInfo(const QString &text);
signals:

public slots:
    void dealbtnSureClicked();
    void dealbtnCancelClicked();

private:
    class QLabel *labPic;
    class QLabel *labNote;
    QPushButton *btnSure;
    QPushButton *btnCancle;
};

#endif // MYMESSAGEBOX_H
