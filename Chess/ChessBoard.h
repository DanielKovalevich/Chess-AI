#pragma once
#include <iostream>
#include <bitset>
#include <iomanip>
#include <string>

class ChessBoard {
public:
	// Use this to eliminate moves on the A and H rank, ie, +-1, +-7, +-9
	const std::bitset<64> notAFile = 0xfefefefefefefefe; // ~0x0101010101010101
	const std::bitset<64> notHFile = 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080

	enum pieces {
		white,
		black,
		king,
		queen,
		rook,
		bishop,
		knight,
		pawn
	};

	// initialize all the bitboards
	// get whether user chooses to be white or black
	ChessBoard() : didWin(false) {
		pieceBitBoard[white]  = 0x000000000000ffff;
		pieceBitBoard[black]  = 0xffff000000000000;
		pieceBitBoard[king]   = 0x1000000000000010;
		pieceBitBoard[queen]  = 0x0800000000000008;
		pieceBitBoard[rook]   = 0x8100000000000081;
		pieceBitBoard[bishop] = 0x2400000000000024;
		pieceBitBoard[knight] = 0x4200000000000042;
		pieceBitBoard[pawn]   = 0x00ff00000000ff00;
	}

	// outputs to console 
	void inline drawBoard();

	std::bitset<64> getAllPieces() const {
		return pieceBitBoard[white] | pieceBitBoard[black];
	}

	std::bitset<64> getPieces(pieces color, pieces piece) const {
		return pieceBitBoard[color] & pieceBitBoard[piece];
	}
	std::bitset<64> getPieces(pieces color) const {
		return pieceBitBoard[color];
	}

	bool won() const {
		return didWin;
	}

	// this will handle all of the moving of the pieces
	void inline move(short original, short destination);

private:
	// store all bitboards in an array - 2 for each colors and the other for pieces
	std::bitset<64> pieceBitBoard[8];
	//int whiteOrBlack; // keeps track of user choice for updating the board initially

	//-------------helper functions for board outlay---------------//
	void inline static createPreBoard();
	void inline static createPostBoard();

	bool didWin; // store win condition
};

// --------------------------------Board Creation---------------------------------- //

// this function prints out the ASCII chessboard to the console
// i want to see if I can do it all in the console before moving to 3D graphics
void ChessBoard::drawBoard() {
	std::bitset<64> occupied = getAllPieces();

	createPreBoard();

	int sideNumbers = 8;
	// change board orientation based on which piece to start off.
	// int alternateRow = ~whiteOrBlack;
	int alternateRow = white;
	for (int i = 56; i >= 0; i-=8) {
		std::cout << std::setw(2) << sideNumbers;
		std::cout << "  |" << std::setw(4);

		for (int j = i; j - i < 8; j++) {
			// place all the pieces according to the bitboards
			if (occupied[j] & pieceBitBoard[white][j]) {
				if (occupied[j] & pieceBitBoard[king][j])
					std::cout << "<K>";
				else if (occupied[j] & pieceBitBoard[queen][j])
					std::cout << "<Q>";
				else if (occupied[j] & pieceBitBoard[rook][j])
					std::cout << "<R>";
				else if (occupied[j] & pieceBitBoard[bishop][j])
					std::cout << "<B>";
				else if (occupied[j] & pieceBitBoard[knight][j])
					std::cout << "<N>";
				else if (occupied[j] & pieceBitBoard[pawn][j])
					std::cout << "<P>";
			}
			else if (occupied[j] & pieceBitBoard[black][j]) {
				if (occupied[j] & pieceBitBoard[king][j])
					std::cout << "[k]";
				else if (occupied[j] & pieceBitBoard[queen][j])
					std::cout << "[q]";
				else if (occupied[j] & pieceBitBoard[rook][j])
					std::cout << "[r]";
				else if (occupied[j] & pieceBitBoard[bishop][j])
					std::cout << "[b]";
				else if (occupied[j] & pieceBitBoard[knight][j])
					std::cout << "[n]";
				else if (occupied[j] & pieceBitBoard[pawn][j])
					std::cout << "[p]";
			}
			else {
				// i need to adjust the rows for the black and white pattern
				if ((j + alternateRow) % 2 == 0)
					std::cout << "   ";
				else
					std::cout << ":::";
			}
		}

		std::cout << std::setw(2);
		std::cout << "|" << std::setw(3) << sideNumbers << std::endl;
		sideNumbers--;
		alternateRow = ~alternateRow;
	}

	createPostBoard();
}

void ChessBoard::createPreBoard(){
	std::cout << std::setw(21) << "Chess\n" << std::endl;
	std::cout << std::setw(8) << 'A';
	for (int i = 1; i <= 7; i++) {
		std::cout << std::setw(3) << static_cast<char>(i + 'A');
	}
	std::cout << std::endl;
	std::cout << std::setw(33) << "+--------------------------+ " << std::endl;
	std::cout << std::setw(5) << "|" << std::setw(27) << "|" << std::endl;
}

void ChessBoard::createPostBoard(){
	std::cout << std::setw(5) << "|" << std::setw(27) << "|" << std::endl;
	std::cout << std::setw(33) << "+--------------------------+ " << std::endl;
	std::cout << std::setw(8) << 'A';
	for (int i = 1; i <= 7; i++) {
		std::cout << std::setw(3) << static_cast<char>(i + 'A');
	}
	std::cout << std::endl << std::endl;
	std::cout << std::setw(19) << "Key" << std::endl;
	std::cout << std::setw(25) << "<Piece> is white" << std::endl << std::setw(25) << "[piece] is black" << std::endl;
}

// --------------------------------Board Creation---------------------------------- //

void ChessBoard::move(short original, short destination) {
	std::bitset<64> occupied = getAllPieces();
	// first check if user even chose a piece
	if (!occupied[original]) {
		std::cout << "There isn't a piece there!" << std::endl;
		return;
	}

	// white is 0 black is 1
	short color = ~pieceBitBoard[white][original];
	short pieceType = -1;

	// figures out what piece the user is moving
	for (int i = 2; i < 8; i++)
		if (pieceBitBoard[i][original])
			pieceType = i;

	// TODO castling

	// Make sure user isn't trying to move on their own piece
	if (pieceBitBoard[color][destination]) {
		std::cout << "You can't move onto your own piece!" << std::endl;
		return;
	}

	// TODO check if move is valid with piece

	std::bitset<64> pieceMoved = 0;;
	pieceMoved[original] = true;
	pieceMoved[destination] = true;
	// verifies that user is not attacking
	if (!occupied[destination]) {
		pieceBitBoard[color] ^= pieceMoved;		// update color bitboard
		pieceBitBoard[pieceType] ^= pieceMoved; //update piece specific bitboard
	}
	

}