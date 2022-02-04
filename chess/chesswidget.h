#ifndef CHESSWIDGET_H
#define CHESSWIDGET_H

#include <QWidget>
#include <map>
#include "chessboard.h"
#include "tile.h"



class ChessWidget : public QWidget
{
    Q_OBJECT
public:
    ChessWidget(QWidget *parent = nullptr);
    ~ChessWidget(){}

    void addModel( Chess::ChessBoard*);
    bool is_legal(const Chess::Pos& from, const Chess::Pos& to) const
    {
        return chessboard->is_legal(from,to);
    }

private:
    Chess::ChessBoard *chessboard;

    std::unordered_map<Chess::Pos,Tile*> tiles;
    void load_chessboard();

public slots:
    void piece_added(const Chess::Pos&);
    void piece_removed(const Chess::Pos&);
};
#endif // CHESSWIDGET_H
