#include "stockokon.h"
#include <list>
#include "QDebug"
using namespace Chess;

StockOkon::StockOkon(COLOR color, ChessBoard *chessboard,QObject* parent)
                     :side(color),chessboard(chessboard),QObject(parent)
{
}

int i=0,j=0,k=0;

int StockOkon::evaluateBest(int depth, COLOR color)
{

    if (depth ==0 ) return chessboard->evaluate();
    std::list<Move> moves = chessboard->movesList();

    if (color==WHITE)
    {
        int max=-1000;
        int temp;
        for (Move move : moves)
        {
            chessboard->move(move.from,move.to);
            temp = evaluateBest(depth-1,BLACK);
            if (temp>max)   max=temp;
            chessboard->undoMove();
        }
        return max;
    }

    else
    {
        int min=1000;
        int temp;
        for (Move move : moves)
        {
            chessboard->move(move.from,move.to);
            temp = evaluateBest(depth-1,WHITE);
            if (temp<min)   min=temp;
            chessboard->undoMove();
        }
        return min;
    }
}

void StockOkon::makeMove()
{
    if (chessboard->side() != side) return;
    i=j=k=0;
    std::list<Move> moves = chessboard->movesList();
    if (moves.size()==0) return;
    Move bestMove;
    bestMove=moves.front();
    if (side == WHITE)
    {
        int max=-1000;
        int temp;

        for (Move move: moves)
        {
            chessboard->move(move.from,move.to);
            temp = evaluateBest(3,BLACK);
            if (temp>max)
            {
                max=temp;
                bestMove=move;
            }
            chessboard->undoMove();
        }
        chessboard->move(bestMove.from,bestMove.to);
        chessboard->check_status();
    }
    else
    {
        int min=1000;
        int temp;

        for (Move move : moves)
        {
            chessboard->move(move.from,move.to);
            temp = evaluateBest(3,WHITE);
            if (temp<min)
            {
                min=temp;
                bestMove=move;
            }
            chessboard->undoMove();
        }
        chessboard->move(bestMove.from,bestMove.to);
        chessboard->check_status();
    }
    emit refresh(bestMove.from);
    emit refresh(bestMove.to);
    if (chessboard->piece_at(bestMove.to)==Chess::PON)
        refresh(Pos(bestMove.to.x,bestMove.from.y));
    else if (chessboard->piece_at(bestMove.to) == Chess:: KING && abs(bestMove.to.y-bestMove.from.y)==2)
    {
        if (bestMove.to.y==2)
        {
            emit refresh(Chess::Pos(bestMove.to.x,0));
            emit refresh(Chess::Pos(bestMove.to.x,3));
        }
        else
        {
            emit refresh(Chess::Pos(bestMove.to.x,5));
            emit refresh(Chess::Pos(bestMove.to.x,7));
        }
    }
}
