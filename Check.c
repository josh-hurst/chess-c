#include "Board.h"

int isInCheck(char *board, char team) {
    char attackingTeam = team == 'b' ? 'a' : 'b';
    char *king;
    char *teamPieces[16];
    int numPieces;
    getTeamPieces(board, attackingTeam, teamPieces, &numPieces);
    getTeamKing(board, team, &king);
    for (int i = 0; i < numPieces; i++) {
        char* piece = teamPieces[i];
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (isValidMove(board, piece, king, 1, 1)) {
                    // char pieceCoordinate[2];
                    // char kingCoordinate[2];
                    // getCoordinate(board, king, kingCoordinate);
                    // getCoordinate(board, piece, pieceCoordinate);
                    // printf("Piece %c (%c: %d, %d) can attack king at (%d, %d)", *piece, piece[1], pieceCoordinate[0], pieceCoordinate[1], kingCoordinate[0], kingCoordinate[1]);
                    return 1;
                }
            }
        }
    }
    return 0;
}

