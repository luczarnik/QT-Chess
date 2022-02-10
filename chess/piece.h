#ifndef PIECE_H
#define PIECE_H

#include "utils.h"
#include <list>
namespace Chess{

class Piece
{
protected:
    Chess::Pos position;
    Chess::PIECE type;
    Chess::COLOR color;

public:
    Chess::Pos get_pos() const { return position;}
    explicit Piece(Chess::PIECE piece, Chess::COLOR color, Chess::Pos pos)
        : color(color), type(piece), position(pos){};

    virtual bool is_legal(const Chess::Pos& pos) const  = 0; // does this piece can move like this ?
    virtual void mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const = 0 ;
    PIECE get_type() const {return type;}
    COLOR get_color() const {return color;}
    void move(const
              Chess::Pos& pos)
    {
        position.x=pos.x;
        position.y=pos.y;
    }

    virtual std::list<Move> moveList(bool occupied[8][8]) const = 0;
};

class Pon : public Piece
{
public:
    explicit Pon(Chess::COLOR COLOR,Chess::Pos pos)
        : Piece(Chess::PIECE::PON,COLOR, pos){}

    virtual bool is_legal(const Chess::Pos& pos)  const ;
    virtual void mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const ;
    virtual std::list<Move> moveList(bool occupied[8][8]) const;
};

class Knight : public Piece
{
public:
    explicit Knight( Chess::COLOR COLOR, Chess::Pos pos)
        : Piece(Chess::PIECE::KNIGHT,COLOR,pos){}

    virtual bool is_legal(const Chess::Pos& to) const;
    virtual void mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const;
    virtual std::list<Move> moveList(bool occupied[8][8]) const;
};
class Rook : public Piece
{
    bool castle;
public:
    explicit Rook( Chess::COLOR COLOR, Chess::Pos pos)
        : castle(true),Piece(Chess::PIECE::ROOK,COLOR,pos){}

    virtual bool is_legal(const Chess::Pos& to) const;
    virtual void mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const;
    virtual std::list<Move> moveList(bool occupied[8][8]) const;

    bool can_castle() {return castle;}
    void forbidd_castle() {castle=false;}
};
class Queen  : public Piece
{
public:
    explicit Queen(Chess::COLOR COLOR, Chess::Pos pos)
        : Piece(Chess::PIECE::QUEEN,COLOR,pos){}

    virtual bool is_legal(const Chess::Pos& to) const;
    virtual void mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const;
    virtual std::list<Move> moveList(bool occupied[8][8]) const;
};
class Bishop  : public Piece
{
public:
    explicit Bishop(Chess::COLOR COLOR, Chess::Pos pos)
        : Piece(Chess::PIECE::BISHOP,COLOR,pos){}

    virtual bool is_legal(const Chess::Pos& to) const;
    virtual void mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const;
    virtual std::list<Move> moveList(bool occupied[8][8]) const;
};
class King  : public Piece
{
    bool castle;
public:
    explicit King(Chess::COLOR COLOR, const Chess::Pos pos)
        :castle(true),Piece(Chess::PIECE::KING,COLOR,pos){}

    virtual bool is_legal(const Chess::Pos& to) const;
    virtual void mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const;
    virtual std::list<Move> moveList(bool occupied[8][8]) const;


    bool can_castle() {return castle;}
    void forbidd_castle() {castle=false;}
};

}
#endif // PIECE_H

