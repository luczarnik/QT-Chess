#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <unordered_map>
#include <unordered_set>
#include "utils.h"
#include "piece.h"
#include <set>
#include "QObject"
#include <list>
#include "fstream"
#include <stack>
#include "metamove.h"


namespace Chess
{

class ChessBoard : public QObject
{   
    Q_OBJECT
    static const size_t SIZE = 8;//size of the board;
public:
    explicit ChessBoard(QObject*);

    bool is_legal(const Chess::Pos& from, Chess::Pos to);
    bool is_mate() const;
    void move(const Chess::Pos& from, Chess::Pos to);//should be provided only with legal moves - otherwise undefined behavior
    const PIECE piece_at(const Chess::Pos&) ;// wymysl cos lepszego potem
    const COLOR color_at(const Chess::Pos&) ;
    std::list<Move> movesList();
    std::list<Move> moveList(const Chess::Pos&);
    int evaluate();
    COLOR side() {return MOVE_TURN;}
    void check_status();



private:
    void initialize_pieces();


    void remove_piece(const Chess::Pos&);
    void hard_remove_piece(const Chess::Pos&);
    void insert_piece(Piece*,const Chess::Pos&);
    void hard_insert_piece(Piece*,const Chess::Pos&);

    bool castle(const Pos& pos);
    bool castle_rook(const Pos&);
    void perform_castle(const Pos&);
    Chess::COLOR MOVE_TURN;
    void switch_turn();

    Pos en_passant_source;
    bool b_en_passant;


    bool w_checked_by_penetrable;
    bool w_checked_by_normal;
    bool b_checked_by_penetrable;
    bool b_checked_by_normal;
    std::unordered_map<Chess::Pos,Chess::Piece*> pieces;

    bool attacked_by_black[SIZE][SIZE];
    bool attacked_by_white[SIZE][SIZE];
    bool occupied[SIZE][SIZE];

    std::unordered_set<Pos> b_penetrable; //pieces which can see through entire raw or diagonal
    std::unordered_set<Pos> w_penetrable;
    bool haveMoves();

    std::unordered_set<Pos> b_pieces;//changes when we check if move is legal
    std::unordered_set<Pos> b_pieces_non_volatile;//changes only if we decided to load move into the engine
    std::unordered_set<Pos> w_pieces;
    std::unordered_set<Pos> w_pieces_non_volatile;

    Piece* w_king;
    Piece* b_king;

    //if king is attacked by non penetrable stroes pointer to attacking piece
    Piece* b_king_attacked_by_non_penetrable;
    Piece* w_king_attacked_by_non_penetrable;

    std::fstream debug_output;

    std::stack<MetaMove> moves;
    void registerMove(const Pos& from, const Pos& to);


    friend class Knight;
    friend class Pon;
    friend class Rook;
    friend class King;
    friend class Bishop;
    friend class Queen;

public slots:
    void undoMove();

public:
signals:

    void stalemate();
    void checkmate(COLOR);
};

}


#endif // CHESSBOARD_H
