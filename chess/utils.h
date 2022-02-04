#ifndef UTIL_H
#define UTIL_H

#include <unordered_map>

#include "qbytearray.h"
#include "Qt"
#include "qmetatype.h"
#include <iostream>


namespace Chess
{
    enum COLOR { WHITE=0, BLACK =1};
    enum PIECE { KING=0, QUEEN=1, BISHOP=2, KNIGHT=3, ROOK=4, PON=5, BLANK=6 };

    struct Pos
    {
        Pos(int x, int y) : x(x),y(y){}
        //Pos(const Pos &other) :x(other.x),y(other.y){}
        //Pos(const Pos && other ) :x(other.x),y(other.y) {}
        Pos(){}
        //Pos operator=(Pos other){ x= other.x; y=other.y; return other;}
        int x;
        int y;
        bool operator==(const Pos &other) const
        {
            return x==other.x && y==other.y;
        }
        bool operator<(const Pos& pos) const
        {
            return x*8+y < pos.x*8+y;
        }
        operator int()
        {
            return 8*x+y;
        }
    };



    bool is_in_board(const Pos& pos);
    void refresh_chessboard(bool tab[8][8]);
    void display_table(bool tab[8][8]);
    bool is_penetrable(const PIECE&);
    bool is_nonpenetrable(const PIECE&);



};

template<>
struct std::hash<Chess::Pos>
{
    size_t operator() (Chess::Pos pos)const
    {
        return 8*pos.x+pos.y;
    }
};



#endif // UTIL_H
