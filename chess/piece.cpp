
#include "piece.h"
#include <cmath>
#include "QtDebug"

using namespace Chess;

bool Pon::is_legal(const Pos &pos) const
{
    if (!is_in_board(pos)) return false;
    if (pos.y-position.y==0 &&
            ((color == WHITE && pos.x==3 && position.x==1) || (color==BLACK && pos.x==4 && position.x==6)))
        return true;
    if (abs(pos.y-position.y)>1) return false;
    return (color==WHITE && pos.x == position.x+1 ) || (color==BLACK && pos.x == position.x-1  );
}

bool Knight::is_legal(const Pos& to) const
{
    if (!is_in_board(to)) return false;

    return ( (abs(position.x-to.x)==2 && abs(position.y-to.y)==1) ||
            (abs(position.x-to.x)==1 && abs(position.y-to.y)==2) );
}

bool Rook::is_legal(const Pos &to) const
{
    if (!is_in_board(to)) return false;

    if (to.x==position.x && to.y == position.y) return false;

    return to.x==position.x || to.y == position.y;
}

bool Bishop::is_legal(const Pos &to) const
{
    if (!is_in_board(to)) return false;

    if (to.x==position.x && to.y == position.y) return false;

    return abs(position.x-to.x) == abs (position.y - to.y);
}



bool King::is_legal(const Pos &to) const
{
    if (!is_in_board(to)) return false;

    if (to.x==position.x && to.y == position.y) return false;

    if (castle)
    {
        if ( color==WHITE &&
             ((position.x==0&& position.y==4)&& (to.x==0 && (to.y==2 || to.y==6)))) return true;

        if (color == BLACK &&
                ((position.x==7&& position.y==4)&& (to.x==7 && (to.y==2 || to.y==6)))) return true;
    }


    return abs(position.x-to.x) <= 1 && abs(position.y-to.y) <= 1;
}



bool Queen::is_legal(const Pos &to) const
{
    if (!is_in_board(to)) return false;

    if (to.x==position.x && to.y == position.y) return false;

    return abs(position.x-to.x) == abs (position.y - to.y)
            || to.x==position.x || to.y == position.y;
}


void Pon::mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const
{
    int add=color==WHITE?1:-1;

    if (position.y>0) chessboard[position.x+add][position.y-1]=true;
    if (position.y<7) chessboard[position.x+add][position.y+1]=true;
}

void Knight::mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const
{
    short xdist[] ={1,2,2,1,-1,-2,-2,-1};
    short ydist[] ={2,1,-1,-2,-2,-1,1,2};
    Pos pos;

    for (int i=0;i<8;i++)
    {
        pos = {xdist[i]+position.x,ydist[i]+position.y};
        if (is_in_board(pos)) chessboard[pos.x][pos.y] = true;
    }
}

void Bishop::mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const
{
    for (int i=position.x+1,j=position.y+1;
         i<8&&j<8;i++,j++)
    {
        chessboard[i][j]=true;
        if (occupied[i][j]==true) break;
    }

    for (int i=position.x+1,j=position.y-1;
         i<8&&j>=0;i++,j--)
    {
        chessboard[i][j]=true;
        if (occupied[i][j]==true) break;
    }

    for (int i=position.x-1,j=position.y+1;
         i>=0&&j<8;i--,j++)
    {
        chessboard[i][j]=true;
        if (occupied[i][j]==true) break;
    }

    for (int i=position.x-1,j=position.y-1;
         i>=0&&j>=0;i--,j--)
    {
        chessboard[i][j]=true;
        if (occupied[i][j]==true) break;
    }
}

void Rook::mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const
{
    for (int i=position.x+1;i<8;i++)    {chessboard[i][position.y]=true;    if (occupied[i][position.y]==true) break;}
    for (int i=position.x-1;i>=0;i--)   {chessboard[i][position.y]=true;    if (occupied[i][position.y]==true) break;}
    for (int i=position.y+1;i<8;i++)    {chessboard[position.x][i]=true;    if (occupied[position.x][i]==true) break;}
    for (int i=position.y-1;i>=0;i--)   {chessboard[position.x][i]=true;    if (occupied[position.x][i]==true) break;}
   // qInfo()<<"Rook";
  //  display_table(chessboard);
}

void Queen::mark_attacked(bool chessboard[8][8],  bool occupied[8][8]) const
{
    for (int i=position.x+1,j=position.y+1;
         i<8&&j<8;i++,j++)
    {
        chessboard[i][j]=true;
        if (occupied[i][j]==true) break;
    }

    for (int i=position.x+1,j=position.y-1;
         i<8&&j>=0;i++,j--)
    {
        chessboard[i][j]=true;
        if (occupied[i][j]==true) break;
    }

    for (int i=position.x-1,j=position.y+1;
         i>=0&&j<8;i--,j++)
    {
        chessboard[i][j]=true;
        if (occupied[i][j]==true) break;
    }

    for (int i=position.x-1,j=position.y-1;
         i>=0&&j>=0;i--,j--)
    {
        chessboard[i][j]=true;
        if (occupied[i][j]==true) break;
    }

    for (int i=position.x+1;i<8;i++)    {chessboard[i][position.y]=true;    if (occupied[i][position.y]==true) break;}
    for (int i=position.x-1;i>=0;i--)   {chessboard[i][position.y]=true;    if (occupied[i][position.y]==true) break;}
    for (int i=position.y+1;i<8;i++)    {chessboard[position.x][i]=true;    if (occupied[position.x][i]==true) break;}
    for (int i=position.y-1;i>=0;i--)   {chessboard[position.x][i]=true;    if (occupied[position.x][i]==true) break;}
   // qInfo()<<"Queen";
  //  display_table(chessboard);
}

void King::mark_attacked(bool chessboard[8][8], bool occupied[8][8]) const
{
    int vert[]{1,1,1,0,0,-1,-1,-1};
    int hor[]{-1,0,1,-1,1,-1,0,1};
    for (int i=0;i<8;i++)
    {
        if (!is_in_board( Pos(position.x+vert[i],position.y+hor[i]))) continue;
        chessboard[position.x+vert[i]][position.y+hor[i]]=true;
    }
}

std::list<Move> Pon::moveList(bool occupied[8][8]) const
{
    std::list <Move> ls;
    int add= color==WHITE? 1:-1;
    ls.push_back(Move(position,Pos(position.x+add,position.y)));
    ls.push_back(Move(position,Pos(position.x+add,position.y+1)));
    ls.push_back(Move(position,Pos(position.x+add,position.y-1)));
    return ls;
}

std::list<Move> Knight::moveList(bool occupied[8][8]) const
{
    std::list <Move> ls;
    short xdist[] ={1,2,2,1,-1,-2,-2,-1};
    short ydist[] ={2,1,-1,-2,-2,-1,1,2};
    Pos pos;

    for (int i=0;i<8;i++)
    {
        pos = {xdist[i]+position.x,ydist[i]+position.y};
        ls.push_back(Move(position,pos));
    }
    return ls;
}

std::list<Move> King::moveList(bool occupied[8][8]) const
{
    std::list <Move> ls;
    short xdist[]{1,1,1,0,0,-1,-1,-1};
    short ydist[]{-1,0,1,-1,1,-1,0,1};
    Pos pos;

    for (int i=0;i<8;i++)
    {
        pos = {xdist[i]+position.x,ydist[i]+position.y};
        ls.push_back(Move(position,pos));
    }
    return ls;
}

std::list<Move> Bishop::moveList(bool occupied[8][8]) const
{
    std::list <Move> ls;

    int direction_x[]{1,1,-1,-1};
    int direction_y[]{-1,1,1,-1};

    for (int k=0;k<4;k++)
    for (int i=position.x+direction_x[k],j=position.y+direction_y[k];
         i<8&&j<8;i+=direction_x[k],j+=direction_y[k])
    {
        ls.push_back(Move(position,Pos(i,j)));
        if (occupied[i][j]==true) break;
    }
    return ls;
}

std::list<Move> Rook::moveList(bool occupied[8][8]) const
{
    std::list <Move> ls;

    int direction_x[]{1,-1,0,0};
    int direction_y[]{0,0,1,-1};

    for (int k=0;k<4;k++)
    for (int i=position.x+direction_x[k],j=position.y+direction_y[k];
         i<8&&j<8;i+=direction_x[k],j+=direction_y[k])
    {
        ls.push_back(Move(position,Pos(i,j)));
        if (occupied[i][j]==true) break;
    }
    return ls;
}

std::list<Move> Queen::moveList(bool occupied[8][8]) const
{
    std::list <Move> ls;

    int direction_x[]{1,-1,0,0,1,1,-1,-1};
    int direction_y[]{0,0,1,-1,-1,1,1,-1};

    for (int k=0;k<8;k++)
    for (int i=position.x+direction_x[k],j=position.y+direction_y[k];
         i<8&&j<8;i+=direction_x[k],j+=direction_y[k])
    {
        ls.push_back(Move(position,Pos(i,j)));
        if (occupied[i][j]==true) break;
    }
    return ls;
}








