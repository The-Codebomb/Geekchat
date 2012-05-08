#include "chatwindow.h"
#include "ui_chatwindow.h"

std::string nick = "Unknown";
std::string color = "#000000";

QTextCharFormat nameFormat;
QTextCursor cursor;

ChatWindow::ChatWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);

    ui->textBrowser->setFocusPolicy(Qt::NoFocus);
    ui->plainTextEdit->setFocusPolicy(Qt::StrongFocus);
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->textBrowser->setReadOnly(true);
    ui->plainTextEdit->installEventFilter(this);

    nameFormat.setFontPointSize(10);
    connect(ui->pushButton, SIGNAL(clicked()), this,
            SLOT(newOwnMessage()));

}

void ChatWindow::appendMessage(std::string &nick, std::string &message, std::string &color)
{
    if(message == "")
        return;
    if(color == "")
        color = "#000000";

    QTextCursor cursor(ui->textBrowser->textCursor());
    QString msg = QString::fromStdString(message);
    QString name = QString::fromStdString(nick);
    const char *clr = color.c_str();

    cursor.movePosition(QTextCursor::End);
    nameFormat.setForeground(QColor(clr));
    cursor.insertText("<" + name + "> ", nameFormat);
    cursor.insertHtml(msg);
    cursor.insertText("\n");
}

void ChatWindow::newOwnMessage()
{
    QString myMessage = ui->plainTextEdit->toPlainText();

    if (myMessage.isEmpty())
    {
        ui->plainTextEdit->setFocus();
        return;
    }
    ui->plainTextEdit->setPlainText("");
    ui->plainTextEdit->setFocus();
    std::string msg = myMessage.toStdString();
    appendMessage(nick,msg, color);
}

bool ChatWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress && obj ==
            ui->plainTextEdit) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_Return)
        {
            newOwnMessage();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void ChatWindow::newChatter(std::string &nick)
{
    if(nick == "")
        return;

    QString name = QString::fromStdString(nick);

    ui->listWidget->addItem(name);
}

void ChatWindow::chatterLeft(std::string &nick)
{
    if(nick == "")
        return;

    QString name = QString::fromStdString(nick);

    QList<QListWidgetItem *> items = ui->listWidget->
            findItems(name,Qt::MatchExactly);
    if (items.isEmpty())
        return;

   delete items.at(0);
}

ChatWindow::~ChatWindow()
{
    delete ui;
}
