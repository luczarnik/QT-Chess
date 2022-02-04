#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <unordered_map>
#include <unordered_set>
#include "utils.h"
#include "piece.h"
#include <set>
#include "QObject"

namespace Chess
{

class ChessBoard : public QObject
{   
    Q_OBJECT
    static const size_t SIZE = 8;//size of the board;
public:
    explicit ChessBoard(QObject*);

    bool is_legal(const Chess::Pos& from,const Chess::Pos& to) ; // does piece move outside board or ally location or discovers check for own king ?
    bool is_mate() const;
    bool move(const Chess::Pos& from,const  Chess::Pos&to);
    const PIECE piece_at(const Chess::Pos&) ;// wymysl cos lepszego potem
    const COLOR color_at(const Chess::Pos&) ;
    void remove_piece(const Chess::Pos&);
    void insert_piece(Piece*,const Chess::Pos&);


private:
    void initialize_pieces();
    bool castle(const Pos& pos);
    bool castle_rook(const Pos&);
    void perform_castle(const Pos&);
    bool about_to_castle;
    Chess::COLOR MOVE_TURN;
    void switch_turn();

    Pos en_passant[2];
    bool b_en_passant;
    bool about_to_en_passant;
    void perform_en_passant(const Pos& Pos);
    bool can_en_passant(const Pos& Pos);

    bool w_checked_by_penetrable;
    bool w_checked_by_normal;
    bool b_ckecked_by_penetrable;
    bool b_checked_by_normal;
    std::unordered_map<Chess::Pos,Chess::Piece*> pieces;

    bool attacked_by_black[SIZE][SIZE];
    bool attacked_by_white[SIZE][SIZE];
    bool occupied[SIZE][SIZE];

    std::unordered_set<Pos> b_penetrable; //pieces which can see through entire raw or diagonal
    std::unordered_set<Pos> w_penetrable;

    std::unordered_set<Pos> b_pieces;
    std::unordered_set<Pos> w_pieces;
    Piece* w_king;
    Piece* b_king;

    //if king is attacked by non penetrable stroes pointer to attacking piece
    Piece* b_king_attacked_by_non_penetrable;
    Piece* w_king_attacked_by_non_penetrable;


    friend class Knight;
    friend class Pon;
    friend class Rook;
    friend class King;
    friend class Bishop;
    friend class Queen;

public slots:

public :
signals:
    void s_remove_piece(const Pos& pos);// in most cases view can by itself easly display and hide pieces
    void s_add_piece(const Pos& pos); // due to drag and drop implementation, but special moves like castle
    // requires addditional steps which are taken in chess engine so there is no need to implement them once again
    // in view

   // void s_checkmate();
    //void s_stalemate();

};

}


#endif // CHESSBOARD_H
