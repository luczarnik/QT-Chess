#include "choosetile.h"
#include "piecewidget.h"
#include "chessboard.h"
#include <QVBoxLayout>

ChooseTile::ChooseTile(QWidget *parent,Chess::PIECE piece,
                       Chess::COLOR color, Chess::ChessBoard* chessboard)
    :pieceType(piece),pieceColor(color),QLabel(parent)
{
    pieceWidget=nullptr;
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setStyleSheet("QLabel { background-color : yellow; }");
    QVBoxLayout* layout= new QVBoxLayout(this);
    pieceWidget = new PieceWidget(piece,color,this);
    layout->addWidget(pieceWidget);

    pieceWidget->show();

    connect(this,&ChooseTile::chosen,chessboard,&Chess::ChessBoard::promote);
}

void ChooseTile::resizeEvent(QResizeEvent *event)
{
    if (pieceType==Chess::PIECE::BLANK) return;
    resize(parentWidget()->size());
    pieceWidget->resizeEvent(event);

}

void ChooseTile::mousePressEvent(QMouseEvent *ev)
{
    emit clicked();
    emit chosen(pieceType);

}
