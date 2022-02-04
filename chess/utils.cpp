#include "utils.h"
#include "QDebug"


using namespace Chess;


void Chess::refresh_chessboard(bool tab[8][8])
{
    for (int i=0;i<8;i++)
        for (int j=0;j<8;j++)
            tab[i][j]=false;
}
bool Chess::is_in_board(const Pos& pos)
{
    return !((pos.x<0 || pos.x >7) && (pos.y<0 || pos.y >7));
}

void Chess::display_table(bool tab[8][8])
{
    QDebug debug(QtDebugMsg);
    debug.nospace();
    for (int i=0;i<8;i++) debug<<i<<"  ";
    debug<<'\n';
    for (int i=0;i<8;i++)
    {
        for (int j=0;j<8;j++)
        {
            if (tab[i][j]) debug<<"X  ";
            else debug<<"_  ";
        }
        debug<<'\n';
    }
}

bool Chess::is_penetrable(const PIECE& piece)
{
    return piece==QUEEN || piece == ROOK || piece == BISHOP;
}
bool Chess::is_nonpenetrable(const PIECE& piece)
{
    return piece==KNIGHT || piece == PON;
}
