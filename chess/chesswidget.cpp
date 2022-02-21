#include "chesswidget.h"
#include <QBoxLayout>
#include "tile.h"
#include "QPushButton"
#include "QGridLayout"
#include "piecewidget.h"
#include "chessboard.h"
#include "choosetile.h"
#include "QPushButton"
#include "QMessageBox"
#include "QCoreApplication"


using namespace Chess;

ChessWidget::ChessWidget( COLOR side, bool singlePlayer,QWidget *parent)
    :  side(side),singlePlayer(singlePlayer),QWidget(parent)
{
    QSizePolicy p= this->sizePolicy();
    p.setHeightForWidth(true);
    setSizePolicy(p);
    resize(720,720);


    chessboard=new  Chess::ChessBoard(this);
    connect(chessboard,&ChessBoard::stalemate,this,&ChessWidget::stalemate);
    connect(chessboard,&ChessBoard::checkmate,this,&ChessWidget::checkmate);

    if (singlePlayer)
    {
        COLOR botSide= side==WHITE? BLACK:WHITE;
        bot =  new StockOkon(botSide,chessboard,this);

        bot->makeMove();

        connect(bot,&StockOkon::refresh,this,&ChessWidget::refresh);
    }


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
            if (side==WHITE)
            {
                pos.x=7-i; pos.y=j;
            }
            else
            {
                pos.x=i;
                pos.y=7-j;
            }
            if (white) tile =new Tile(this,Chess::COLOR::WHITE,pos,chessboard);
            else        tile =new Tile(this,Chess::COLOR::BLACK,pos,chessboard);
            layout->addWidget(tile,i,j);
            tiles[pos]=tile;
            connect(tile,&Tile::clicked,this,&ChessWidget::hide_promotion);
            connect(tile,&Tile::attacked,this,&ChessWidget::under_attack);
            connect(tile,&Tile::refresh,this,&ChessWidget::refresh);
            connect(tile,&Tile::promotion,this,&ChessWidget::choose_promotion);

            if (singlePlayer) connect(tile,&Tile::madeMove,bot,&StockOkon::makeMove);

            white=!white;
        }
        white=!white;
    }





    load_chessboard();

    for (int i=0;i<4;i++) promotionTile[i]=nullptr;
    attacking_piece=Pos(-1,-1);

    /* debugging
    QPushButton *button = new QPushButton();
    button ->show();
    connect(button,&QPushButton::clicked,chessboard,&ChessBoard::undoMove);
    connect(button,&QPushButton::clicked,this,&ChessWidget::load_chessboard);*/
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
            color = chessboard->color_at(pos);
            tiles[pos]->placePiece(piece,color);
        }
    }
    repaint();
}

void ChessWidget::refresh(const Chess::Pos& pos)
{
   tiles[pos]->placePiece(chessboard->piece_at(pos),chessboard->color_at(pos));
}

void ChessWidget::choose_promotion(const Pos& pos, const Pos& attacked)
{
    if (pos.x!=0 && pos.x!=7) return;
    int x=pos.x,y =pos.y;
    if (x==0)
    {
        promotionTile[0]=new ChooseTile(tiles[Pos(x,y)],Chess::QUEEN,Chess::BLACK,pos,attacked,chessboard);
        promotionTile[1]=new ChooseTile(tiles[Pos(x+1,y)],Chess::ROOK,Chess::BLACK,pos,attacked,chessboard);
        promotionTile[2]=new ChooseTile(tiles[Pos(x+2,y)],Chess::KNIGHT,Chess::BLACK,pos,attacked,chessboard);
        promotionTile[3]=new ChooseTile(tiles[Pos(x+3,y)],Chess::BISHOP,Chess::BLACK,pos,attacked,chessboard);
        for (int i=0;i<4;i++)
        {
            tiles[Pos(x+i,y)]->setChooseTile(promotionTile[i]);
            connect(promotionTile[i],&ChooseTile::refresh,this,&ChessWidget::refresh);
        }

    }
    else
    {
        promotionTile[0]=new ChooseTile(tiles[Pos(x,y)],Chess::QUEEN,Chess::BLACK,pos,attacked,chessboard);
        promotionTile[1]=new ChooseTile(tiles[Pos(x-1,y)],Chess::ROOK,Chess::BLACK,pos,attacked,chessboard);
        promotionTile[2]=new ChooseTile(tiles[Pos(x-2,y)],Chess::KNIGHT,Chess::BLACK,pos,attacked,chessboard);
        promotionTile[3]=new ChooseTile(tiles[Pos(x-3,y)],Chess::BISHOP,Chess::BLACK,pos,attacked,chessboard);
        for (int i=0;i<4;i++)
        {
            tiles[Pos(x-i,y)]->setChooseTile(promotionTile[i]);
            connect(promotionTile[i],&ChooseTile::refresh,this,&ChessWidget::refresh);
        }
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
        connect(promotionTile[i],&ChooseTile::clicked,this,&ChessWidget::hide_promotion);
        if (singlePlayer)  connect(promotionTile[i],&ChooseTile::madeMove,bot,&StockOkon::makeMove);
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
    for (const Chess::Move& move: moves)
        if (move.to.x >=0 && move.to.x<8)
            tiles[move.to]->enhanceWith(pos);
}

void ChessWidget::stalemate()
{
    QMessageBox *message =new QMessageBox(this);
    message->setText("Stalemate!");
    message->show();

    connect(message,&QMessageBox::rejected,QCoreApplication::instance(),&QCoreApplication::quit);
    connect(message,&QMessageBox::accepted,QCoreApplication::instance(),&QCoreApplication::quit);
}
void ChessWidget::checkmate(COLOR color)
{
    QMessageBox *message =new QMessageBox(this);
    if (color == BLACK) message->setText("Black Won !");
    else                message->setText("White Won!");
    message->show();

    connect(message,&QMessageBox::rejected,QCoreApplication::instance(),&QCoreApplication::quit);
    connect(message,&QMessageBox::accepted,QCoreApplication::instance(),&QCoreApplication::quit);
}
