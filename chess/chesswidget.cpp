#include "chesswidget.h"
#include <QBoxLayout>
#include "tile.h"
#include "QPushButton"
#include "QGridLayout"
#include "piecewidget.h"
#include "chessboard.h"

using namespace Chess;

ChessWidget::ChessWidget(QWidget *parent)
    :  QWidget(parent)
{
    QSizePolicy p= this->sizePolicy();
    p.setHeightForWidth(true);
    setSizePolicy(p);
    resize(720,720);


    chessboard=new  Chess::ChessBoard(this);
    connect(chessboard,&ChessBoard::s_add_piece,this,&ChessWidget::piece_added);
    connect(chessboard,&ChessBoard::s_remove_piece,this,&ChessWidget::piece_removed);
   // connect(chessboard,&ChessBoard::s_add_piece,this,&ChessWidget::piece_added);
   // connect(chessboard,&ChessBoard::s_add_piece,this,&ChessWidget::piece_added);

    QGridLayout * layout = new QGridLayout();
    setLayout(layout);
    layout->setSpacing(0);
    bool white = true;
    Tile* tile;
    Chess::Pos pos;
    for (int i=0;i<8;i++)
    {
        for (int j=0;j<8;j++)
        {
            pos.x=7-i; pos.y=7-j;
            if (white) tile =new Tile(this,Chess::COLOR::WHITE,pos,chessboard);
            else        tile =new Tile(this,Chess::COLOR::BLACK,pos,chessboard);
            layout->addWidget(tile,i,j);
            tiles[Chess::Pos(7-i,7-j)]=tile;

            white=!white;
        }
        white=!white;
    }


    load_chessboard();

}

void ChessWidget::load_chessboard()
{
    PIECE piece;
    Pos pos;
    COLOR color;
    for (int i=0;i<8;i++)
    {
        for (int j=0;j<8;j++)
        {
            pos.x =i; pos.y=j;
            piece = chessboard->piece_at(pos);
            if ( piece== Chess::PIECE::BLANK) continue;
            color = chessboard->color_at(pos);
            tiles[pos]->placePiece(piece,color);
        }
    }
}

void ChessWidget::piece_added(const Chess::Pos& pos)
{
   tiles[pos]->placePiece(chessboard->piece_at(pos),chessboard->color_at(pos));
}

void ChessWidget::piece_removed(const Chess::Pos& pos)
{
    tiles[pos]->placePiece(Chess::BLANK,Chess::WHITE);
}


