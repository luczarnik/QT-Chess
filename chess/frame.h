#ifndef FRAME_H
#define FRAME_H

#include <QFrame>

namespace Ui {
class Frame;
}

class Frame : public QFrame
{
    Q_OBJECT

public:
    explicit Frame(QWidget *parent = nullptr);
    ~Frame();

private slots:
    void on_startButton_clicked();

private:
    Ui::Frame *ui;
};

#endif // FRAME_H
