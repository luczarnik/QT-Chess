#ifndef CHESSWIDGET_H
#define CHESSWIDGET_H

#include <QWidget>
#include <map>
#include "chessboard.h"
#include "tile.h"
#include "utils.h"
#include "stockokon.h"



class ChessWidget : public QWidget
{
    Q_OBJECT
public:
    ChessWidget(COLOR side, bool singlePlayer,QWidget *parent = nullptr);
    ~ChessWidget(){}

    void addModel( Chess::ChessBoard*);
    bool is_legal(const Chess::Pos& from, const Chess::Pos& to) const
    {
        return chessboard->is_legal(from,to);
    }

private:
    Chess::ChessBoard *chessboard;
    StockOkon* bot;
    bool singlePlayer;
    COLOR side;


    std::unordered_map<Chess::Pos,Tile*> tiles;


    bool promotion_menu_active;
    Chess::Pos promotion_position;
    Chess::Pos attacking_piece;
    ChooseTile* promotionTile[4];

public slots:
    void refresh(const Chess::Pos&);
    void choose_promotion(const Chess::Pos&,const Pos& attacked);
    void hide_promotion();
    void under_attack(const  Chess::Pos&,std::list<Chess::Move>);
    void load_chessboard();
    void stalemate();
    void checkmate(COLOR);
};
#endif // CHESSWIDGET_H
