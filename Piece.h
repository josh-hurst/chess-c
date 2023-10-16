#ifndef PIECE_H
#define PIECE_H

extern const int PIECE_SIZE;

extern const int HAS_MOVED;

struct Piece {
    int sMove; // straight move
    int dMove; // diagonal move
    int pMove; // perpindicular move
    int fwdOnly;
};
struct Piece getPieceInfo(char pieceType);

#endif
