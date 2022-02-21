#include "choosetile.h"
#include "piecewidget.h"
#include "chessboard.h"
#include <QVBoxLayout>

ChooseTile::ChooseTile(QWidget *parent,Chess::PIECE piece,
                       Chess::COLOR color,const Chess::Pos& position, const Chess::Pos& attacked, Chess::ChessBoard* chessboard)
    :chessboard(chessboard),pieceType(piece),pieceColor(color),position(position),attacked(attacked),QLabel(parent)
{
    pieceWidget=nullptr;
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setStyleSheet("QLabel { background-color : yellow; }");
    QVBoxLayout* layout= new QVBoxLayout(this);
    pieceWidget = new PieceWidget(piece,color,this);
    layout->addWidget(pieceWidget);

    pieceWidget->show();

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

    if (position.x==7)
    {
        switch(pieceType)
        {
        case Chess::QUEEN: chessboard->move(attacked,position); break;
        case Chess::ROOK: chessboard->move(attacked,Chess::Pos(position.x+1,position.y)); break;
        case Chess::KNIGHT: chessboard->move(attacked,Chess::Pos(position.x+2,position.y)); break;
        case Chess::BISHOP: chessboard->move(attacked,Chess::Pos(position.x+3,position.y)); break;
        }
    }

    else if (position.x==0)
    {
        switch(pieceType)
        {
        case Chess::QUEEN: chessboard->move(attacked,position); break;
        case Chess::ROOK: chessboard->move(attacked,Chess::Pos(position.x-1,position.y)); break;
        case Chess::KNIGHT: chessboard->move(attacked,Chess::Pos(position.x-2,position.y)); break;
        case Chess::BISHOP: chessboard->move(attacked,Chess::Pos(position.x-3,position.y)); break;
        }
    }

    emit refresh(attacked);
    emit refresh(position);
    emit madeMove();
}
