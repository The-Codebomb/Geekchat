#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <string>

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QMainWindow
{
    Q_OBJECT
    Ui::ChatWindow *ui;
    
public:
    explicit ChatWindow(QWidget *parent = 0);
    void newChatter(std::string &nick);
    void chatterLeft(std::string &nick);
    void appendMessage(std::string &nick, std::string &message, std::string &color);
    ~ChatWindow();

private slots:
    void newOwnMessage();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // CHATWINDOW_H
