#include "chesswidget.h"
#include <QBoxLayout>
#include "tile.h"
#include "QPushButton"
#include "QGridLayout"
#include "piecewidget.h"
#include "chessboard.h"
#include "choosetile.h"

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
            pos.x=7-i; pos.y=j;
            if (white) tile =new Tile(this,Chess::COLOR::WHITE,pos,chessboard);
            else        tile =new Tile(this,Chess::COLOR::BLACK,pos,chessboard);
            layout->addWidget(tile,i,j);
            tiles[Chess::Pos(7-i,j)]=tile;
            connect(tile,&Tile::clicked,this,&ChessWidget::hide_promotion);
            connect(tile,&Tile::attacked,this,&ChessWidget::under_attack);

            white=!white;
        }
        white=!white;
    }



    load_chessboard();
    repaint();

    for (int i=0;i<4;i++) promotionTile[i]=nullptr;
    connect(chessboard,&ChessBoard::promotion,this,&ChessWidget::choose_promotion);
    attacking_piece=Pos(-1,-1);
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
            if ( piece== Chess::BLANK) continue;
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

void ChessWidget::choose_promotion(const Pos& pos)
{
    if (pos.x!=0 && pos.x!=7) return;
    int x=pos.x,y =pos.y;
    if (x==0)
    {
        promotionTile[0]=new ChooseTile(tiles[Pos(x,y)],Chess::QUEEN,Chess::BLACK,chessboard);
        promotionTile[1]=new ChooseTile(tiles[Pos(x+1,y)],Chess::ROOK,Chess::BLACK,chessboard);
        promotionTile[2]=new ChooseTile(tiles[Pos(x+2,y)],Chess::KNIGHT,Chess::BLACK,chessboard);
        promotionTile[3]=new ChooseTile(tiles[Pos(x+3,y)],Chess::BISHOP,Chess::BLACK,chessboard);
        for (int i=0;i<4;i++)
            tiles[Pos(x+i,y)]->setChooseTile(promotionTile[i]);

    }
    else
    {
        promotionTile[0]=new ChooseTile(tiles[Pos(x,y)],Chess::QUEEN,Chess::BLACK,chessboard);
        promotionTile[1]=new ChooseTile(tiles[Pos(x-1,y)],Chess::ROOK,Chess::BLACK,chessboard);
        promotionTile[2]=new ChooseTile(tiles[Pos(x-2,y)],Chess::KNIGHT,Chess::BLACK,chessboard);
        promotionTile[3]=new ChooseTile(tiles[Pos(x-3,y)],Chess::BISHOP,Chess::BLACK,chessboard);
        for (int i=0;i<4;i++)
            tiles[Pos(x-i,y)]->setChooseTile(promotionTile[i]);
    }

    for (int i=0;i<4;i++)
    {
        connect(promotionTile[i],&ChooseTile::clicked,this,&ChessWidget::hide_promotion);
        promotionTile[i]->show();
    }
    promotion_position=pos;
    repaint();
}

void ChessWidget::hide_promotion()
{
    if (attacking_piece!=Pos(-1,-1))
        tiles[attacking_piece]->placePiece(chessboard->piece_at(attacking_piece),
                                       chessboard->color_at(attacking_piece));

    for (auto elem : tiles)
    {
        elem.second->restore_color();
    }
    if (promotionTile[0]==nullptr) return;
    for (int i=0;i<4;i++)
    {
        disconnect(promotionTile[i],&ChooseTile::clicked,this,&ChessWidget::hide_promotion);
        promotionTile[i]->hide();
        promotionTile[i]->deleteLater();
        promotionTile[i]=nullptr;
    }
    if (promotion_position.x==0)
        for (int i=0;i<4;i++) tiles[Pos(promotion_position.x+i,promotion_position.y)]->removeChooseTile();
    else
        for (int i=0;i<4;i++) tiles[Pos(promotion_position.x-i,promotion_position.y)]->removeChooseTile();


    repaint();
}

void ChessWidget::under_attack(const Chess::Pos& pos,std::list<Chess::Move> moves)
{
    attacking_piece=pos;
    for (const Chess::Move& move: moves)    tiles[move.to]->enhanceWith(pos);
}
