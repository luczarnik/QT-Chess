
#include "piece.h"
#include <cmath>
#include "QtDebug"

using namespace Chess;

bool Pon::is_legal(const Pos &pos, bool occupied[8][8]) const
{
    if (pos.y>7||pos.y<0) return false;
    if (color == WHITE)
    {
        if (pos.y==position.y)
        {
            if (occupied[pos.x][pos.y]) return false;
            if (pos.x-position.x == 1 ) return true;
            else if (pos.x-position.x==2)
            {
                if (position.x !=1) return false;
                if (occupied[position.x+1][position.y]) return false;
                return true;
            }
        }
        if (abs(pos.y-position.y)==1&&pos.x== position.x+1) return true;

        return false  ;
    }
    else
    {
        if (pos.y==position.y)
        {
            if (occupied[pos.x][pos.y]) return false;
            if (position.x-pos.x == 1 ) return true;
            else if (position.x-pos.x==2 )
            {
                if (position.x !=6) return false;
                if (occupied[position.x-1][position.y]) return false;
                return true;
            }
        }
        else if (abs(pos.y-position.y)==1 && pos.x== position.x-1) return true;

        return false  ;
    }
}

bool Knight::is_legal(const Pos& to, bool occupied[8][8]) const
{
    if (!is_in_board(to)) return false;

    return ( (abs(position.x-to.x)==2 && abs(position.y-to.y)==1) ||
            (abs(position.x-to.x)==1 && abs(position.y-to.y)==2) );
}

bool Rook::is_legal(const Pos &to, bool occupied[8][8]) const
{
    if (!is_in_board(to)) return false;

    if (to.x==position.x && to.y == position.y) return false;

    if (!( to.x==position.x || to.y == position.y)) return false;

    int x_direction, y_direction;
    if (to.x>position.x) x_direction = 1;
    else if ( to.x <position.x) x_direction=-1;
    else x_direction=0;

    if (to.y>position.y) y_direction = 1;
    else if ( to.y <position.y) y_direction=-1;
    else y_direction=0;

    for (int i=position.x+x_direction, j=position.y+y_direction;
         i!=to.x||j!=to.y;i+=x_direction, j+=y_direction)
        if (occupied[i][j] && ! (i== to.x && j==to.y)) return false;
    return true;
}

bool Bishop::is_legal(const Pos &to, bool occupied[8][8]) const
{
    if (!is_in_board(to)) return false;

    if (to.x==position.x && to.y == position.y) return false;


    if ( !(abs(position.x-to.x) == abs(position.y - to.y)) ) return false;

    int x_direction, y_direction;
    if (to.x>position.x) x_direction = 1;
    else if ( to.x <position.x) x_direction=-1;

    if (to.y>position.y) y_direction = 1;
    else if ( to.y <position.y) y_direction=-1;

    for (int i=position.x+x_direction, j=position.y+y_direction;
         i!=to.x||j!=to.y;i+=x_direction, j+=y_direction)
        if (occupied[i][j] && ! (i== to.x && j==to.y)) return false;

    return true;
}



bool King::is_legal(const Pos &to, bool occupied[8][8]) const
{
    if (!is_in_board(to)) return false;

    if (to.x==position.x && to.y == position.y) return false;

    if (abs(position.x-to.x) <= 1 && abs(position.y-to.y) <= 1) return true;

    if (castle)
    {
        if ( color==WHITE && to.x==0 )
        {
             if (to.y == 2 && ! occupied[0][2] && !occupied[0][3] && !occupied[0][1]) return true;
             if (to.y == 6 && ! occupied[0][5] && !occupied[0][6]) return true;
        }

        if ( color==BLACK && to.x==7 )
        {
             if (to.y == 2 && ! occupied[7][2] && !occupied[7][3] && !occupied[7][1]) return true;
             if (to.y == 6 && ! occupied[7][5] && !occupied[7][6]) return true;
        }
    }
    return false;
}




bool Queen::is_legal(const Pos &to, bool occupied[8][8]) const
{
    if (!is_in_board(to)) return false;

    if (to.x==position.x && to.y == position.y) return false;

    if (!( abs(position.x-to.x) == abs (position.y - to.y)
            || to.x==position.x || to.y == position.y)) return false;

    int x_direction, y_direction;
    if (to.x>position.x) x_direction = 1;
    else if ( to.x <position.x) x_direction=-1;
    else x_direction=0;

    if (to.y>position.y) y_direction = 1;
    else if ( to.y <position.y) y_direction=-1;
    else y_direction=0;

    for (int i=position.x+x_direction, j=position.y+y_direction;
         i!=to.x||j!=to.y;i+=x_direction, j+=y_direction)
        if (occupied[i][j] && ! (i== to.x && j==to.y)) return false;
    return true;
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
    int add;
    if (color == WHITE)
    {
        add =1;
        if (position.x == 6)
        {
            for ( int i= 2;i<=4;i++)
            {
                ls.push_back(Move(position,Pos(position.x+i,position.y+1)));
                ls.push_back(Move(position,Pos(position.x+i,position.y-1)));
            }
            for ( int i= 3;i<=4;i++)
                ls.push_back(Move(position,Pos(position.x+i,position.y)));
        }
        else if (position.x==1)
            ls.push_back(Move(position,Pos(position.x+2*add,position.y)));
    }
    else
    {
        add =-1;
        if (position.x == 0)
        {
            for ( int i= -2;i>=-4;i--)
            {
                ls.push_back(Move(position,Pos(position.x+i*add,position.y+1)));
                ls.push_back(Move(position,Pos(position.x+i*add,position.y-1)));
            }
            for ( int i= -3;i>=-4;i--)
                ls.push_back(Move(position,Pos(position.x+i*add,position.y)));

        }
        else if (position.x==6)
            ls.push_back(Move(position,Pos(position.x+2*add,position.y)));
    }

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
    if (position.y==4)
    {
        if (position.x==0)
        {
            ls.push_back(Move(position,Pos(0,2)));
            ls.push_back(Move(position,Pos(0,6)));
        }
        else if (position.x==7)
        {
            ls.push_back(Move(position,Pos(7,2)));
            ls.push_back(Move(position,Pos(7,6)));
        }
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
         i>=0&&j>=0&&i<8&&j<8;i+=direction_x[k],j+=direction_y[k])
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
         i>=0&&j>=0&&i<8&&j<8;i+=direction_x[k],j+=direction_y[k])
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
         i>=0&&j>=0&&i<8&&j<8;i+=direction_x[k],j+=direction_y[k])
    {
        ls.push_back(Move(position,Pos(i,j)));
        if (occupied[i][j]==true) break;
    }
    return ls;
}








