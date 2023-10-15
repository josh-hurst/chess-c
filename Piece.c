#include <math.h>
#include "Piece.h"

struct Pieces {
    struct Piece k;
    struct Piece q;
    struct Piece r;
    struct Piece b;
    struct Piece n;
    struct Piece p;
};

struct Piece getPieceInfo(char pieceType) {
    static struct Piece king = {1, 1, 0, 0};
    static struct Piece queen = {INFINITY, INFINITY, 0, 0};
    static struct Piece rook = {INFINITY, 0, 0, 0};
    static struct Piece bishop = {0, INFINITY, 0, 0};
    static struct Piece knight = {2, 0, 1, 0};
    static struct Piece pawn = {1, 0, 0, 1};

    struct Piece piece;
    switch (pieceType) {
        case 'k':
            piece = king;
            break;
        case 'q':
            piece = queen;
            break;
        case 'r':
            piece = rook;
            break;
        case 'b':
            piece = bishop;
            break;
        case 'n':
            piece = knight;
            break;
        case 'p':
            piece = pawn;
            break;
    };

    return piece;
}

