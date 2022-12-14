#ifndef HELP_H
#define HELP_H

#include <QWidget>
#include <QIcon>

namespace Ui {
class Help;
}

class Help : public QWidget
{
    Q_OBJECT

public:
    explicit Help(QWidget *parent = 0);
    ~Help();

private slots:
    void on_githubButton_clicked();

private:
    Ui::Help *ui;
};

#endif // HELP_H
