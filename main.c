#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <regex.h>
#include "Board.h"
#include "Check.h"

struct MoveInstructions {
    int success;
    CellCoordinate from;
    CellCoordinate to;
};

struct InputResponse {
    struct MoveInstructions moveInstructions;
};

struct MoveInstructions resolveMoveInstructions(char* input) {
    struct MoveInstructions moveInstructions = {0};
    regmatch_t pmatch[5];
    regex_t regex;
    int compiled = regcomp(&regex, "([0-9]+),([0-9]+)->([0-9]+),([0-9]+)", REG_EXTENDED);
    if (compiled) {
        return moveInstructions;
    }

    int regRes = regexec(&regex, input, (size_t) 5, pmatch, 0);
    regfree(&regex);
    if (regRes) {
        return moveInstructions;
    }

    CellCoordinate fromCell = {
        .row = atoi(&input[pmatch[2].rm_so]) - 1,
        .column = atoi(&input[pmatch[1].rm_so]) - 1
    };
    CellCoordinate toCell = {
        .row = atoi(&input[pmatch[4].rm_so]) - 1,
        .column = atoi(&input[pmatch[3].rm_so]) - 1
    };

    moveInstructions.from = fromCell;
    moveInstructions.to = toCell;
    moveInstructions.success = 1;

    return moveInstructions;
}

struct InputResponse resolveUserInput(int *coordinates) {
    struct InputResponse inputResponse = {};
    char input[9]; // max expected input length is 9
    scanf("%s", input);

    if (strcmp(input, "q") == 0) {
        return inputResponse;
    } else {
        struct MoveInstructions moveInstructions = resolveMoveInstructions(input);
        if (moveInstructions.success) {
            inputResponse.moveInstructions = moveInstructions;
        }
        return inputResponse;
    }
}

void switchTurn(char *turn) {
    *turn = *turn == 'a' ? 'b' : 'a';
}

void deleteLine() {
    printf("\33[2K\r");
    printf("\033[A");
}

void clearTerminal(int *printedLines) {
    for (int i = 0; i < 10 + *printedLines; i++)
        deleteLine();
    *printedLines = 0;
}

int main() {
    char turn = 'a'; // which team is going
    struct Board board = createBoard(turn);
    char **layout = board.layout;
    
    int printedLines = 0;
    int moveSuccess = 1;
    while (1) {
        if (moveSuccess) {
            int inCheck = isInCheck(layout, turn);
            int inCheckMate = inCheck ? isInCheckMate(layout, turn) : 0;
            printBoard(layout, turn, inCheck, inCheckMate);
            printedLines += inCheck;

            if (inCheckMate) {
                break;
            }
        }

        int coordinates[4];
        struct InputResponse inputResponse = resolveUserInput(coordinates);
        struct MoveInstructions moveInstructions = inputResponse.moveInstructions;
        printedLines++;
        if (!moveInstructions.success) {
            printf("Failed to read input.\n");
            printedLines++;
            moveSuccess = 0;
            continue;
        }

        char *pieceTaken;
        char **fromCell = getCell(layout, moveInstructions.from.row, moveInstructions.from.column);
        char **toCell = getCell(layout, moveInstructions.to.row, moveInstructions.to.column);
        if (!moveTo(layout, fromCell, toCell, turn, &pieceTaken, 0)) {
            clearTerminal(&printedLines);
            switchTurn(&turn);
            moveSuccess = 1;
        } else {
            printedLines ++;
            moveSuccess = 0;
        }
    }
    free(board.layout);
    free(board.teamA);
    free(board.teamB);
    return 0;
}
