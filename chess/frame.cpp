#include "frame.h"
#include "ui_frame.h"
#include "utils.h"
#include "chesswidget.h"

Frame::Frame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Frame)
{
    ui->setupUi(this);
}

Frame::~Frame()
{
    delete ui;
}

void Frame::on_startButton_clicked()
{
    using namespace Chess;
    COLOR color = ui->colorCombo->currentText()=="White" ? WHITE:BLACK;
    bool singlePlayer = ui->botCombo->currentText()==QString("Single Player")? true:false;
    qInfo()<<singlePlayer;

    ChessWidget* chesswidget = new ChessWidget(color,singlePlayer);
    chesswidget->show();
    deleteLater();
}

