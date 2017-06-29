// Written by Daniel Kovalevich
// Separation of implementation and interface

#include "ChessBoard.h"

// --------------------------------Board Creation---------------------------------- //

// this function prints out the ASCII chessboard to the console
// i want to see if I can do it all in the console before moving to a GUI
void ChessBoard::drawBoard() {
	std::bitset<64> occupied = getAllPieces();

	createPreBoard();

	int sideNumbers = 8;
	// change board orientation based on which piece to start off.
	// int alternateRow = ~whiteOrBlack;
	bool alternateRow = white;
	for (int i = 56; i >= 0; i -= 8) {
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
		alternateRow = !alternateRow;
	}

	createPostBoard();
}

void ChessBoard::createPreBoard() {
	std::cout << std::setw(21) << "Chess\n" << std::endl;
	std::cout << std::setw(8) << 'A';
	for (int i = 1; i <= 7; i++) {
		std::cout << std::setw(3) << static_cast<char>(i + 'A');
	}
	std::cout << std::endl;
	std::cout << std::setw(33) << "+--------------------------+ " << std::endl;
	std::cout << std::setw(5) << "|" << std::setw(27) << "|" << std::endl;
}

void ChessBoard::createPostBoard() {
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
		system("pause");
		return;
	}

	// white is 0 black is 1
	bool color = this->color;
	short pieceType = -1;

	// figures out what piece the user is moving
	for (int i = 2; i < 8; i++) {
		if (pieceBitBoard[i][original])
			pieceType = i;
	}

	if (pieceBitBoard[!this->color][original]) {
		std::cout << "It's not your turn!" << std::endl;
		system("pause");
		return;
	}

	if (!validateMove(original, destination, pieceType)) {
		std::cout << "That is not a valid move!" << std::endl;
		system("pause");
		return;
	}

	// TODO castling

	// Make sure user isn't trying to move on their own piece
	if (pieceBitBoard[color][destination]) {
		std::cout << "You can't move onto your own piece!" << std::endl;
		system("pause");
		return;
	}

	// TODO en passant

	std::bitset<64> fromBitBoard = 0, toBitBoard = 0;;
	fromBitBoard[original] = 1;
	toBitBoard[destination] = 1;
	fromBitBoard ^= toBitBoard;

	// verifies if user is attacking
	if (occupied[destination]) {
		short capturedPiece = -1;
		for (int i = 2; i < 8; i++) {
			if (pieceBitBoard[i][destination])
				capturedPiece = i;
		}

		pieceBitBoard[color] ^= fromBitBoard; // update color bitboard
		pieceBitBoard[pieceType] ^= fromBitBoard; //update piece specific bitboard
		pieceBitBoard[!color] ^= toBitBoard; // update opponent bitboard
		pieceBitBoard[capturedPiece] ^= toBitBoard; // gets rid of opponent piece
	}
	else {
		pieceBitBoard[color] ^= fromBitBoard; // update color bitboard
		pieceBitBoard[pieceType] ^= fromBitBoard; //update piece specific bitboard
	}

	pawnPromotion(pieceType, destination, toBitBoard);

	changeTurn();
	system("pause");
}

// special rule in chess
// TODO REWRITE FOR COMPUTER LATER
void ChessBoard::pawnPromotion(short pieceType, short destination, std::bitset<64> toBitBoard) {
	std::bitset<64> oppositeRow = !this->color ? 0xff00000000000000 : 0x00000000000000ff;
	if (pieceType == pawn && oppositeRow[destination]) {
		std::cout << std::setw(23) << "Promotion!" << std::endl;
		std::cout << "(q)ueen, (r)ook, (b)ishop, or (k)night: ";
		bool correctUserInput = false;
		char promotion;

		// get rid of the pawn in the spot
		pieceBitBoard[pawn] ^= toBitBoard;

		while (!correctUserInput) {
			std::cin >> promotion;
			correctUserInput = true;
			switch (toupper(promotion)) {
			case 'Q':
				pieceBitBoard[queen] ^= toBitBoard;
				break;
			case 'R':
				pieceBitBoard[rook] ^= toBitBoard;
				break;
			case 'B':
				pieceBitBoard[bishop] ^= toBitBoard;
				break;
			case 'K':
				pieceBitBoard[knight] ^= toBitBoard;
				break;
			default:
				std::cout << "Improper input! Retry: ";
				correctUserInput = false;
				break;
			}
		}
	}
}

// TODO check if move is valid with piece
// verifies if move corresponds with piece
// also makes sure there aren't any pieces in the way
bool ChessBoard::validateMove(short from, short to, short piece) {
	if (piece == pawn) {
		// pawns direction is dependent on color and starting position
		if ((pieceBitBoard[white][from] && to - from < 0) || (pieceBitBoard[black][from] && to - from > 0)) {
			return false;
		}

		short distance = abs(to - from);
		std::bitset<64> colorPawns = getPieces(this->color, pawn);
		std::bitset<64> WrapAFile = colorPawns & notAFile;
		std::bitset<64> WrapHFile = colorPawns & notHFile;

		// makes sure user can't just try to wrap around the board
		if (pieceBitBoard[white][from] && distance == 7 && !WrapAFile[from] || pieceBitBoard[black][from] && distance == 9 && !WrapAFile[from])
			return false;
		if (pieceBitBoard[white][from] && distance == 9 && !WrapHFile[from] || pieceBitBoard[black][from] && distance == 7 && !WrapHFile[from])
			return false;

		// verify there is an enemy piece on diagonal square
		if (distance == 7 || distance == 9) {
			return pieceBitBoard[!this->color][to];
		}
		
		if (distance == 16) {
			return origPawn[from];
		}

		return distance == 8;
	}
	else if (piece == queen) {
		short leftMaxRange = from - from % 8;
		short rightMaxRange = 7 + leftMaxRange;
		
		std::cout << leftMaxRange << ' ' << rightMaxRange << std::endl;
	}

	return true;
}