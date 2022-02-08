#include "chessboard.h"
#include <stdexcept>
#include "piece.h"
#include "utils.h"
#include "QtDebug"
#include "QDebug"
#include <algorithm>



using namespace Chess;

ChessBoard::ChessBoard(QObject* parent)
    : MOVE_TURN(WHITE), QObject(parent)
{
    MOVE_TURN=WHITE;

    initialize_pieces();
    about_to_castle=false;
    about_to_en_passant=false;
    b_en_passant=false;

}


void ChessBoard::initialize_pieces()
{
    for (int i=0;i<8;i++)
    {
        pieces[Pos(1,i)]= new Pon(WHITE,Pos(1,i));

        pieces[Pos(6,i)]= new Pon(BLACK,Pos(6,i));
    }
    pieces[Pos(0,0)]=new Rook    (WHITE,Pos(0,0));
    pieces[Pos(0,1)]=new Knight  (WHITE,Pos(0,1));
    pieces[Pos(0,2)]=new Bishop  (WHITE,Pos(0,2));
    pieces[Pos(0,3)]=new Queen   (WHITE,Pos(0,3));
    pieces[Pos(0,4)]=new King    (WHITE,Pos(0,4));
    pieces[Pos(0,5)]=new Bishop  (WHITE,Pos(0,5));
    pieces[Pos(0,6)]=new Knight  (WHITE,Pos(0,6));
    pieces[Pos(0,7)]=new Rook    (WHITE,Pos(0,7));

    pieces[Pos(7,0)]=new Rook    (BLACK,Pos(7,0));
    pieces[Pos(7,1)]=new Knight  (BLACK,Pos(7,1));
    pieces[Pos(7,2)]=new Bishop  (BLACK,Pos(7,2));
    pieces[Pos(7,3)]=new Queen   (BLACK,Pos(7,3));
    pieces[Pos(7,4)]=new King    (BLACK,Pos(7,4));
    pieces[Pos(7,5)]=new Bishop  (BLACK,Pos(7,5));
    pieces[Pos(7,6)]=new Knight  (BLACK,Pos(7,6));
    pieces[Pos(7,7)]=new Rook    (BLACK,Pos(7,7));

    w_king=pieces[Pos(0,4)]; b_king=pieces[Pos(7,4)];
    w_penetrable.insert(Pos(0,2));
    w_penetrable.insert(Pos(0,3));
    w_penetrable.insert(Pos(0,5));
    w_penetrable.insert(Pos(0,0));
    w_penetrable.insert(Pos(0,7));

    b_penetrable.insert(Pos(7,2));
    b_penetrable.insert(Pos(7,3));
    b_penetrable.insert(Pos(7,5));
    b_penetrable.insert(Pos(7,0));
    b_penetrable.insert(Pos(7,7));

    refresh_chessboard(occupied);
    refresh_chessboard(attacked_by_black);
    refresh_chessboard(attacked_by_white);

    for (int i=0;i<8;i++)
    {
        occupied[0][i] =true; occupied[1][i]= true;
        occupied[6][i] =true; occupied[7][i] = true;

        w_pieces.insert(Pos(0,i));
        w_pieces.insert(Pos(1,i));

        b_pieces.insert(Pos(7,i));
        b_pieces.insert(Pos(6,i));
    }

    b_king_attacked_by_non_penetrable=nullptr;
    w_king_attacked_by_non_penetrable=nullptr;
}

bool ChessBoard::move(const Pos& from, const Pos& to )
{
    if (!is_legal(from,to)) return false;

    Piece* piece=pieces[from];

    b_king_attacked_by_non_penetrable=nullptr;
    w_king_attacked_by_non_penetrable=nullptr;
    b_en_passant=false;

    if (about_to_castle)
    {
        perform_castle(to);
        switch_turn();
        about_to_castle=false;
        return true;
    }

    if (about_to_en_passant)
    {
        perform_en_passant(from);
        about_to_en_passant=false;
    }

    if ((piece->get_type()==PON && MOVE_TURN == WHITE && to.x==7)
        || (piece->get_type()==PON && MOVE_TURN==BLACK && to.x==0))
    {
        promotion_from=from;
        promotion_to = to;
        emit promotion(to);
        switch_turn();
        return false;
    }

    if (pieces.find(to)!=pieces.end())
        remove_piece(to);

    remove_piece(from);
    insert_piece(piece,to);

    if (piece->get_type()== ROOK) dynamic_cast<Rook*>(piece)->forbidd_castle();
    else if (piece->get_type()==KING) dynamic_cast<King*>(piece)->forbidd_castle();


    // check whther king is on attacked square, if so turn the check state on
    else if (piece->get_type() == PON || piece->get_type()==KNIGHT)
    {
        Pos pos;
        if (MOVE_TURN==WHITE)
        {
            piece->mark_attacked(attacked_by_white,occupied);
            pos=b_king->get_pos();
            if (attacked_by_white[pos.x][pos.y]) b_king_attacked_by_non_penetrable=piece;
        }
        else
        {
            piece->mark_attacked(attacked_by_black,occupied);
            pos=w_king->get_pos();
            if (attacked_by_black[pos.x][pos.y]) w_king_attacked_by_non_penetrable=piece;
        }

        if (piece->get_type()==PON && abs(from.x-to.x)==2)
        {
            b_en_passant=true;
            if (is_in_board(Pos(to.x,to.y-1)))  en_passant[0]=Pos(to.x,to.y-1);
            if (is_in_board(Pos(to.x,to.y+1)))  en_passant[1]=Pos(to.x,to.y+1);

        }

    }

    switch_turn();

    // checks if there are legal moves if there are not it's stalemate if king wasn't checkd or checkmate if he was

    return true;
}

bool ChessBoard::is_legal(const Pos& from, const Pos& to)
{  
    //if (pieces.find(from) == pieces.end()) return false;
    if (MOVE_TURN==WHITE)   refresh_chessboard(attacked_by_black);
    else                    refresh_chessboard(attacked_by_white);

    Piece* piece = pieces[from];
    COLOR color = piece->get_color();

    if (!piece->is_legal(to)) return false;

    if (color!=MOVE_TURN ||
            (pieces.find(to)!=pieces.end()&&pieces[to]->get_color()==MOVE_TURN)  ) return false;


    bool kingMove=false;
    if (piece->get_type()==KING)
    {
        if (dynamic_cast<King*>(piece)->can_castle())
        {
            if (castle(to)) { about_to_castle=true; return true;}
            if (abs(from.y-to.y) >1 ) return false;
        }
        kingMove =true;
    }

    Piece* enemy = nullptr;
    if (pieces.find(to)!=pieces.end())
    {
        enemy=pieces[to];
        //if in previous move king was put in check then if piece who was checking was non-penetrable type
        //it must be taken down, or king must be moved
        if (!kingMove&&
                (w_king_attacked_by_non_penetrable!=nullptr||
                b_king_attacked_by_non_penetrable!=nullptr) &&
                !(w_king_attacked_by_non_penetrable==enemy || b_king_attacked_by_non_penetrable==enemy))
            return false;

        remove_piece(to);
    }
    else if (!kingMove&&
             (w_king_attacked_by_non_penetrable!=nullptr||
             b_king_attacked_by_non_penetrable!=nullptr)) return false;



    if (piece->get_type()==PON )
    {

        if (abs(from.y-to.y)==1)
        {
            if (enemy==nullptr )
            {

                if (can_en_passant(from)) // if can enpassant then on pos "to" there certainly is no enemy,
                    //so we can return withowt carring about potentialy deleted enemy pon like in other cases
                {
                    about_to_en_passant=true;
                    if (from==en_passant[0])
                    {
                        enemy=pieces[Pos(from.x,from.y+1)];
                        remove_piece(Pos(from.x,from.y+1));
                    }
                    else
                    {
                        enemy=pieces[Pos(from.x,from.y-1)];
                        remove_piece(Pos(from.x,from.y-1));
                    }
                }
                else return false;
            }
        }
        else
        {
            if (enemy!=nullptr) return false;
            if (abs(from.x-to.x)==2)
            {
                if (MOVE_TURN==WHITE && occupied[from.x+1][from.y]) return false;
                if (MOVE_TURN==BLACK && occupied[from.x-1][from.y]) return false;
            }
        }

    }

    remove_piece(from);
    insert_piece(piece,to);

    bool answer;
    Pos pos;
    if (color==WHITE)
    {
        //if king moved we have to check every opponent piece if is attacking king
        if (kingMove)   for (const Pos &pos :b_pieces)
            pieces[pos]->mark_attacked(attacked_by_black,occupied);

        //otherwise king is eather already attacked by penetrable, or we made path for penetrable.
        // we rejected attacking by non-penetrable in earlier stage
        else   for (const Pos &pos: b_penetrable)
            pieces[pos]->mark_attacked(attacked_by_black,occupied);

        pos = w_king->get_pos();
        //display_table(attacked_by_black);

        if (attacked_by_black[pos.x][pos.y]) answer = false;//is king checked?
        else answer = true;
    }
    else//mirror to white
    {
        if (kingMove)   for (const Pos& pos: w_pieces)
            pieces[pos]->mark_attacked(attacked_by_white,occupied);

        else            for (const Pos& pos: w_penetrable)
            pieces[pos]->mark_attacked(attacked_by_white,occupied);

        pos = b_king->get_pos();
        display_table(attacked_by_white);

        if (attacked_by_white[pos.x][pos.y]) answer = false;
        else answer = true;
    }


    remove_piece(to);
    if (enemy!= nullptr)    insert_piece(enemy,enemy->get_pos());
    insert_piece(piece,from);

    if (about_to_en_passant && !answer) about_to_en_passant=false;


    return answer;
}

const PIECE ChessBoard::piece_at(const Pos& pos)
{
    if (pieces.find(pos) == pieces.end()) return BLANK;

    return pieces[pos]->get_type();
}
const COLOR ChessBoard::color_at(const Pos& pos)
{
    if (pieces.find(pos) == pieces.end()) exit(0);

    return pieces[pos]->get_color();
}


void ChessBoard::remove_piece(const Pos& pos)
{

    occupied[pos.x][pos.y]=false;
    Piece *piece = pieces[pos];
    pieces.erase(pos);

    if (piece->get_color()==WHITE) w_pieces.erase(pos);
    else b_pieces.erase(pos);

    if (!is_penetrable(piece->get_type())) return;

    if (piece->get_color() == WHITE) w_penetrable.erase(pos);
    else                             b_penetrable.erase(pos);
    //delete piece;
}
void ChessBoard::insert_piece(Piece* piece, const Pos& pos)
{
    pieces[pos]=piece;
    occupied[pos.x][pos.y]=true;
    piece->move(pos);

    if (piece->get_color()==WHITE) w_pieces.insert(pos);
    else b_pieces.insert(pos);

    if (!is_penetrable(piece->get_type())) return;

    if (piece->get_color()==WHITE) w_penetrable.insert(pos);
    else b_penetrable.insert(pos);
}

bool ChessBoard::castle(const Pos& pos)
{
    if (!(pos == Pos(0,2) || pos==Pos(0,6) || pos == Pos(7,2) || pos ==Pos(7,6))) return false;



    if (pos.x ==0) for (const Pos &pos :b_pieces)
        pieces[pos]->mark_attacked(attacked_by_black,occupied);
    else for (const Pos &pos :w_pieces)
        pieces[pos]->mark_attacked(attacked_by_white,occupied);


    if (pos == Pos(0,2) && !attacked_by_black[0][2] && !attacked_by_black[0][3] && !attacked_by_black[0][4]) return castle_rook(Pos(0,0));
    if (pos == Pos(0,6) && !attacked_by_black[0][4] && !attacked_by_black[0][5] && !attacked_by_black[0][6]) return castle_rook(Pos(0,7));
    if (pos == Pos(7,2) && !attacked_by_white[7][2] && !attacked_by_white[7][3] && !attacked_by_white[7][4]) return castle_rook(Pos(7,0));
    if (pos == Pos(7,6) && !attacked_by_white[7][4] && !attacked_by_white[7][5] && !attacked_by_white[7][6]) return castle_rook(Pos(7,7));

    return false;
}

bool ChessBoard::castle_rook(const Pos& pos)
{
    if (pieces.find(pos)==pieces.end()) return false;
    return dynamic_cast<Rook*>(pieces[pos])->can_castle();
}

void ChessBoard::switch_turn()
{
    if (MOVE_TURN == WHITE ) MOVE_TURN=BLACK;
    else MOVE_TURN = WHITE;
}

void ChessBoard::perform_castle(const Pos & pos)
{
    qInfo()<<"Cos";
    Piece* rook , *king;
    if (pos==Pos(0,2))
    {
        rook=pieces[Pos(0,0)];
        king=pieces[Pos(0,4)];
        dynamic_cast<Rook*>(rook)->forbidd_castle();
        dynamic_cast<King*>(king)->forbidd_castle();
        remove_piece(Pos(0,0));
        remove_piece(Pos(0,4));
        insert_piece(king,Pos(0,2));
        insert_piece(rook,Pos(0,3));

        emit s_remove_piece(Pos(0,4));
        emit s_remove_piece(Pos(0,0));
        emit s_add_piece(Pos(0,2));
        emit s_add_piece(Pos(0,3));
    }
    else if (pos==Pos(0,6))
    {
        rook=pieces[Pos(0,7)];
        king=pieces[Pos(0,4)];
        dynamic_cast<Rook*>(rook)->forbidd_castle();
        dynamic_cast<King*>(king)->forbidd_castle();
        remove_piece(Pos(0,7));
        remove_piece(Pos(0,4));
        insert_piece(king,Pos(0,6));
        insert_piece(rook,Pos(0,5));

        emit s_remove_piece(Pos(0,4));
        emit s_remove_piece(Pos(0,7));
        emit s_add_piece(Pos(0,5));
        emit s_add_piece(Pos(0,6));
    }
    else if (pos==Pos(7,2))
    {
        rook=pieces[Pos(7,0)];
        king=pieces[Pos(7,4)];
        dynamic_cast<Rook*>(rook)->forbidd_castle();
        dynamic_cast<King*>(king)->forbidd_castle();
        remove_piece(Pos(7,0));
        remove_piece(Pos(7,4));
        insert_piece(king,Pos(7,2));
        insert_piece(rook,Pos(7,3));

        emit s_remove_piece(Pos(7,4));
        emit s_remove_piece(Pos(7,0));
        emit s_add_piece(Pos(7,2));
        emit s_add_piece(Pos(7,3));
    }
    else if (pos==Pos(7,6))
    {
        rook=pieces[Pos(7,7)];
        king=pieces[Pos(7,4)];
        dynamic_cast<Rook*>(rook)->forbidd_castle();
        dynamic_cast<King*>(king)->forbidd_castle();
        remove_piece(Pos(7,7));
        remove_piece(Pos(7,4));
        insert_piece(king,Pos(7,6));
        insert_piece(rook,Pos(7,5));

        emit s_remove_piece(Pos(7,4));
        emit s_remove_piece(Pos(7,7));
        emit s_add_piece(Pos(7,5));
        emit s_add_piece(Pos(7,6));
    }
}

bool ChessBoard::can_en_passant(const Pos &pos)
{
    if (b_en_passant== false) return false;
    if (pos==en_passant[0] || pos == en_passant[1]) return true;
    return false;
}

void ChessBoard::perform_en_passant(const Pos &pos)
{
    if (pos == en_passant[0])
    {
        remove_piece(Pos(pos.x,pos.y+1));
        emit s_remove_piece(Pos(pos.x,pos.y+1));
    }
    else
    {
        remove_piece(Pos(pos.x,pos.y-1));
        emit s_remove_piece(Pos(pos.x,pos.y-1));
    }

}

void ChessBoard::promote(PIECE piece)
{

   if (pieces.find(promotion_to)!= pieces.end()) remove_piece(promotion_to);
   remove_piece(promotion_from);
   Piece *nowy;
   COLOR color;
   color = promotion_to.x==0 ? BLACK:WHITE;

   switch (piece)
   {
   case QUEEN: nowy= new Queen(color,promotion_to); break;
   case ROOK:  nowy= new Rook(color,promotion_to); break;
   case KNIGHT: nowy = new Knight(color,promotion_to); break;
   case BISHOP: nowy = new Bishop(color,promotion_to); break;
   }


   insert_piece(nowy,promotion_to);//
   emit s_remove_piece(promotion_from);
   emit s_add_piece(promotion_to);
}
