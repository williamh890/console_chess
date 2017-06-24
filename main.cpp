// chess.cpp
// William Horn
// 6.18.17
// Console Chess

#include <iostream>
using std::cout;
using std::cin;
using std::endl;
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <sstream>
using std::istringstream;
using std::ostringstream;
#include <cmath>
using std::sqrt;
#include <exception>
using std::runtime_error;
#include <stdlib.h>

const int BOARD_SIZE = 8;
const int NUM_PAWNS = BOARD_SIZE;
const int WHITE_PAWN_STARTING_ROW = 1;
const int WHITE_PIECE_STARTING_ROW = 0;

const int BLACK_PAWN_STARTING_ROW = 6;
const int BLACK_PIECE_STARTING_ROW = 7;

const int UPPER_TO_DIGIT_DEL = 17;
const int LOWER_TO_DIGIT_DEL = 49;

enum Color {BLACK, WHITE};

struct Vec2 {
    int x;
    int y;
    Vec2() {}
    Vec2(int _x, int _y) : x(_x), y(_y) {}

    string to_string() {
        ostringstream buffer;
        buffer << "(" << x << ", " << y << ")";

        return buffer.str();
    }

};

float mag(Vec2 vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

struct Piece {

    Piece(){};
    Piece(int x, int y, Color color_) : pos(Vec2(x, y)), color(color_) {}

    bool move(int x, int y) {
        return this->move(Vec2(x, y));
    }
    virtual bool move(Vec2 delta) = 0;

    virtual string get_type() = 0;

    void setValues(int x, int y, Color color_) {
        pos.x = x;
        pos.y = y;
        color = color_;
    }

    Vec2 pos;
    Color color;
    string model;
};

struct Pawn : public Piece {
    Pawn(){
        model = 'P';
    }
    Pawn(int x, int y, Color c) : Piece(x, y, c) {
        model = 'P';
    }

    virtual bool move(Vec2 delta) {
        if(mag(delta) <= 1) {
            pos.x += delta.x;
            pos.y += delta.y;
            return true;
        }
        // Invalid Move
        return false;
    }

    virtual string get_type() {
        return "pawn";
    }
};

struct Castle : public Piece {
    Castle() {
        model = 'C';
    }
    Castle(int x, int y, Color c) : Piece(x, y, c) {
        model = 'C';
    }

    virtual bool move(Vec2 delta) {
        //Either the x has a value or they y has a value but not both
        if((delta.x != 0 && delta.y == 0) ||
          (delta.y != 0 && delta.x == 0)) {
            pos.x += delta.x;
            pos.y += delta.y;

            return true;
        }
        // Invalid Move
        return false;
    }

    virtual string get_type() {
        return "castle";
    }
};

struct Knight : public Piece {
    Knight() {
        model = 'k';
    }
    Knight(int x, int y, Color c) : Piece(x, y, c) {
        model = 'k';
    }

    virtual bool move(Vec2 delta) {
        auto deltaMag = mag(delta);

        if((deltaMag <= knightMoveVec + 0.01) ||
          (deltaMag >= knightMoveVec - 0.01)) {
            pos.x += delta.x;
            pos.y += delta.y;

            return true;
          }
        return false;
    }

    virtual string get_type() {
        return "knight";
    }

    const float knightMoveVec = mag(Vec2(2, 3));
};

struct Bishop : public Piece {
    Bishop(){
        model = 'B';
    }
    Bishop(int x, int y, Color c) : Piece(x, y, c) {
        model = 'B';
    }

    virtual bool move(Vec2 delta) {
        if(delta.x == delta.y) {
            pos.x += delta.x;
            pos.y += delta.y;

            return true;
        }
        return false;
    }

    virtual string get_type() {
        return "bishop";
    }
};

struct Queen : public Piece {
    Queen() {
        model = 'Q';
    }
    Queen(int x, int y, Color c) : Piece(x, y, c) {
        model = 'Q';
    }

    virtual bool move(Vec2 delta) {
        if(delta.x == delta.y ||
          (delta.x != 0 && delta.y == 0) ||
          (delta.y != 0 && delta.x == 0)) {
            pos.x += delta.x;
            pos.y += delta.y;

            return true;
        }

        return false;
    }

    virtual string get_type() {
        return "queen";
    }
};

struct King : public Piece {
    King() {
        model = 'K';
    }
    King(int x, int y, Color c) : Piece(x, y, c) {
        model = 'K';
    }

    virtual bool move(Vec2 delta) {
        if(delta.x == delta.y ||
          (delta.x != 0 && delta.y == 0) ||
          (delta.y != 0 && delta.x == 0)) {
            pos.x += delta.x;
            pos.y += delta.y;

            return true;
        }
        return false;
    }

    virtual string get_type() {
        return "king";
    }
};

Piece * pieceFactory(string pieceType) {
    Piece * newPiece;

    if (pieceType == "pawn")
        newPiece = new Pawn;
    else if (pieceType == "castle")
        newPiece = new Castle;
    else if (pieceType == "knight")
        newPiece = new Knight;
    else if (pieceType == "bishop")
        newPiece = new Bishop;
    else if (pieceType == "queen")
        newPiece = new Queen;
    else if (pieceType == "king")
        newPiece = new King;
    else
        throw runtime_error("Invalid piece given: " + pieceType);

    return newPiece;
}

struct Board {
    static vector<string> whitePieceOrder;
    static vector<string> blackPieceOrder;

    const int size;

    Board() : size(BOARD_SIZE) {
        // Add pawns
        for(int c = 0; c < 2; c++) {
            Color pieceColor = (c < 1) ? WHITE : BLACK;

            int pawnRow, pieceRow;

            if (pieceColor == WHITE) {
                pawnRow = WHITE_PAWN_STARTING_ROW;
                pieceRow = WHITE_PIECE_STARTING_ROW;
            }
            else {
                pawnRow = BLACK_PAWN_STARTING_ROW;
                pieceRow = BLACK_PIECE_STARTING_ROW;
            }

            auto pieceOrder = (pieceColor == WHITE) ? whitePieceOrder : blackPieceOrder;

            // Add all the pawns in there starting positions
            for(int col = 0; col < NUM_PAWNS; ++col) {
                // White is the bottom players color

                // Add a pawn
                auto newPawn = new Pawn(pawnRow, col, pieceColor);
                pieces.push_back(newPawn);

                // Add a piece
                auto newPiece = pieceFactory(pieceOrder[col]);
                newPiece->setValues(pieceRow, col, pieceColor);
                pieces.push_back(newPiece);

            }
        }
    }

    void show() {
        string frame = "";

        for(int x = 0; x < size; ++x) {
            if (x == 0) frame += "    -----------------------------\n";

            frame += (x +  49);
            frame += " ";

            frame += "| ";
            for(int y = 0; y < size; ++y) {
                bool hasPiece = false;
                // Look through the pieces
                for(int p = 0; p < pieces.size(); ++p) {
                    // Check that the pieces
                    if(pieces[p]->pos.x == x && pieces[p]->pos.y == y) {
                        frame += pieces[p]->model;
                        hasPiece = true;
                        break;
                    }
                }
                // Print empty space if no piece
                if(!hasPiece) frame += " ";

                frame += " | ";
            }
            frame += (x != size - 1) ? "\n  | -   -   -   -   -   -   -   - |" : "\n    -----------------------------";
            frame += "\n";
        }
        frame += "    a   b   c   d   e   f   g   h\n";
        cout << frame << endl;
    }

    vector<Piece *> pieces;
};

vector<string> Board::whitePieceOrder{"castle", "knight", "bishop", "queen", "king", "bishop", "knight", "castle"};
vector<string> Board::blackPieceOrder{"castle", "knight", "bishop", "king", "queen", "bishop", "knight", "castle"};

bool validLowerPosChar(char posChar) {
    return (posChar >= 97 && posChar <= 104);
}
bool validUpperPosChar(char posChar) {
    return (posChar >=  65 && posChar <= 90);
}
bool validDigit(char posChar) {
    return (posChar >= 48 && posChar <= 55);
}

void convetChessQuards(string & piece, string & proposed_move) {
    vector<char> positions{piece[0], piece[1], proposed_move[0], proposed_move[1]};

    for(auto & p : positions) {
        //upper case between a-h
        if(validUpperPosChar(p)) {
            // Turn it into a digit
            p = p - UPPER_TO_DIGIT_DEL;
        }
        else if(validLowerPosChar(p)) {
            p = p - LOWER_TO_DIGIT_DEL;
        }
        else if(validDigit(p)) {
            p = p;
        }
        else{
            throw runtime_error("invalid move arg");
        }
    }
}

/*
    7
    6
    5
    4
    3
    2
    1
    0
      a b c d e f g h

*/

int main()
{
    Board board;

    board.show();
    while(true) {
        string piece, proposed_move;
        cout << "(piece move): ";
        cin >> piece >> proposed_move;
        try {
            convetChessQuards(piece, proposed_move);
        }
        catch(const runtime_error & e) {
            cout << "invalid move, try again." << endl;
            continue;
        }

        board.show();

        system("cls");
    }

    return 0;
}
