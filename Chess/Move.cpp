// Written by Daniel Kovalevich
// Separation of implementation and interface

#include "ChessBoard.h"
#include <string>

bool ChessBoard::move(short original, short destination) {
	auto occupied = getAllPieces();

	// update class data members
	updateMove(original, destination);

	// first check if user even chose a piece
	if (!occupied[original]) {
		std::cout << "There isn't a piece there!" << std::endl;
		system("pause");
		return false;
	}

	// white is 0 black is 1
	bool color = this->color;
	short pieceType = getPieceType();

	if (pieceBitBoard[!this->color][this->from]) {
		std::cout << "It's not your turn!" << std::endl;
		system("pause");
		return false;
	}

	if (!validateMove(pieceType) || to < 0 || to > 63) {
		std::cout << "That is not a valid move!" << std::endl;
		system("pause");
		return false;
	}

	// TODO castling

	// Make sure user isn't trying to move on their own piece
	if (pieceBitBoard[color][this->to]) {
		std::cout << "You can't move onto your own piece!" << std::endl;
		system("pause");
		return false;
	}

	// TODO en passant

	std::bitset<64> fromBitBoard = 0x0, toBitBoard = 0x0;
	fromBitBoard[this->from] = true;
	toBitBoard[this->to] = true;
	fromBitBoard ^= toBitBoard;

	// verifies if user is attacking
	if (occupied[this->to]) {
		short capturedPiece = -1;
		for (int i = 2; i < 8; i++) {
			if (pieceBitBoard[i][destination])
				capturedPiece = i;
		}

		pieceBitBoard[color] ^= fromBitBoard;		// update color bitboard
		pieceBitBoard[pieceType] ^= fromBitBoard;	//update piece specific bitboard
		pieceBitBoard[!color] ^= toBitBoard;		// update opponent bitboard
		pieceBitBoard[capturedPiece] ^= toBitBoard; // gets rid of opponent piece

		this->capture = true;
	}
	else {
		pieceBitBoard[color] ^= fromBitBoard;		// update color bitboard
		pieceBitBoard[pieceType] ^= fromBitBoard;	//update piece specific bitboard
	}

	pawnPromotion(pieceType, toBitBoard);

	fiftyMoveRule();
	generateAttacks();
	isInCheck();
	changeTurn();
	system("pause");

	return true;
}

// figures out what type of piece the user is trying to move
short ChessBoard::getPieceType() {
	short pieceType = -1;

	// figures out what piece the user is moving
	for (short i = 2; i < 8; i++) {
		if (pieceBitBoard[i][this->from])
			pieceType = i;
	}

	return pieceType;
}

short ChessBoard::getPieceType(short from) {
	short pieceType = -1;

	// figures out what piece the user is moving
	for (short i = 2; i < 8; i++) {
		if (pieceBitBoard[i][from])
			pieceType = i;
	}

	return pieceType;
}


// special rule in chess
void ChessBoard::pawnPromotion(short pieceType, std::bitset<64> toBitBoard) {
	std::bitset<64> oppositeRow = !this->color ? 0xff00000000000000 : 0x00000000000000ff;
	if (pieceType == pawn && oppositeRow[this->to]) {
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

// TODO validate edge cases
// verifies if move corresponds with piece
// also makes sure there aren't any pieces in the way
bool ChessBoard::validateMove(short piece) {
	short distance = abs(to - from);
	// going up or down the board
	bool positive = to - from > 0;
	// makes sure the pieces don't wrap around the board
	short leftMaxRange = from - from % 8;
	short rightMaxRange = 7 + leftMaxRange;

	if (piece == king) {
		// since the king moves in every direction, i only included the absolutes
		short possibleMoves[8] = { north, northWest, northEast, east};

		if (pieceBitBoard[!color][to]) {
			// this will allow me to temporarily modify the piecebitboards
			// to check if the king can actually attack the piece without going into check
			std::bitset<64> temp[8];
			for (auto i = 0; i < 8; i++) {
				temp[i] = pieceBitBoard[i];
			}

			auto piecePos = getPieceType(to);
			std::bitset<64> fromBitBoard = 0x0, toBitBoard = 0x0;
			fromBitBoard[this->from] = true;
			toBitBoard[this->to] = true;
			fromBitBoard ^= toBitBoard;

			pieceBitBoard[color] ^= fromBitBoard;
			pieceBitBoard[king] ^= fromBitBoard;
			pieceBitBoard[!color] ^= toBitBoard;
			pieceBitBoard[piecePos] ^= toBitBoard;

			generateAttacks();

			auto kingBB = getPieces(color, king);

			if ((kingBB & attackBB[!color]) == 0x0) {
				
				return true;
			}

			// revert the boards to normal if the above condition fails
			for (auto i = 0; i < 8; i++) {
				pieceBitBoard[i] = temp[i];
			}

			std::cout << "That would put your king in check!" << std::endl;
			return false;
		}

		if (!attackBB[!color][to]) {
			// verifies the move is possible
			for (auto move : possibleMoves) {
				if (move == distance)
					return true;
			}
		}
		
		return false;

	}

	if (piece == queen) {
		// there cannot be pieces in the path of the queen
		if (distance % north == 0) {
			return positive ? noBlockingPieces(north) : noBlockingPieces(south);
		}
		if (distance % northWest == 0) {
			return positive ? noBlockingPieces(northWest) : noBlockingPieces(southEast);
		}
		if (distance % northEast == 0) {
			return positive ? noBlockingPieces(northEast) : noBlockingPieces(southWest);
		}
		if (to - from + from >= leftMaxRange && to - from + from <= rightMaxRange) {
			return positive ? noBlockingPieces(east) : noBlockingPieces(west);
		}

		return false;
	}

	if (piece == rook) {
		if (distance % north == 0)
			return positive ? noBlockingPieces(north) : noBlockingPieces(south);
		if (to - from + from >= leftMaxRange && to - from + from <= rightMaxRange)
			return positive ? noBlockingPieces(east) : noBlockingPieces(west);
		
		return false;
	}

	if (piece == bishop) {
		if (distance % northWest == 0)
			return positive ? noBlockingPieces(northWest) : noBlockingPieces(southEast);
		if (distance % northEast == 0)
			return positive ? noBlockingPieces(northEast) : noBlockingPieces(southWest);
		
		return false;
	}

	// knight moves in an L-shape so I just used the numbers
	if (piece == knight) {
		short realDistance = to - from;

		//--------------------------------------literal edge cases------------------------------------------------------------//
		if (AFile[from] && (realDistance == 15 || realDistance == -10 || realDistance == 6 || realDistance == -17)) {
			std::cout << "You cannot move beyond the board!" << std::endl;
			return false;
		}

		if (AFile[from - 1] && (realDistance == -10 || realDistance == 6)) {
			std::cout << "You cannot move beyond the board!" << std::endl;
			return false;
		}

		if (HFile[from] && (realDistance == 17 || realDistance == 10 || realDistance == -6 || realDistance == -15)) {
			std::cout << "You cannot move beyond the board!" << std::endl;
			return false;
		}

		if (HFile[from + 1] && (realDistance == 10 || realDistance == -6)) {
			std::cout << "You cannot move beyond the board!" << std::endl;
			return false;
		}
		//--------------------------------------literal edge cases------------------------------------------------------------//

		return distance == 6 || distance == 10 || distance == 15 || distance == 17;
	}

	if (piece == pawn) {
		// pawns direction is dependent on color and starting position
		if ((pieceBitBoard[white][from] && to - from < 0) || (pieceBitBoard[black][from] && to - from > 0)) {
			return false;
		}
		auto colorPawns = getPieces(this->color, pawn);
		auto WrapAFile = colorPawns & ~AFile;
		auto WrapHFile = colorPawns & ~HFile;

		// makes sure user can't just try to wrap around the board
		if (pieceBitBoard[white][from] && distance == 7 && !WrapAFile[from] || pieceBitBoard[black][from] && distance == 9 && !WrapAFile[from])
			return false;
		if (pieceBitBoard[white][from] && distance == 9 && !WrapHFile[from] || pieceBitBoard[black][from] && distance == 7 && !WrapHFile[from])
			return false;

		this->pawnMovement = true;

		// verify there is an enemy piece on diagonal square
		if (distance == northEast || distance == northWest) {
			return pieceBitBoard[!this->color][to];
		}

		if (distance == 16) {
			return origPawn[from];
		}

		return distance == 8;
	}

	return true;
}

// validates pawn attacks only since their attacks are different from their movements
bool ChessBoard::validatePawnMovementForAttacks(short from, short to, bool color) {
	short distance = abs(to - from);

	// pawns direction is dependent on color and starting position
	if ((pieceBitBoard[white][from] && to - from < 0) || (pieceBitBoard[black][from] && to - from > 0)) {
		return false;
	}
	std::bitset<64> colorPawns = getPieces(color, pawn);
	std::bitset<64> WrapAFile = colorPawns & ~AFile;
	std::bitset<64> WrapHFile = colorPawns & ~HFile;

	// makes sure user can't just try to wrap around the board
	if (pieceBitBoard[white][from] && distance == 7 && !WrapAFile[from] || pieceBitBoard[black][from] && distance == 9 && !WrapAFile[from])
		return false;
	if (pieceBitBoard[white][from] && distance == 9 && !WrapHFile[from] || pieceBitBoard[black][from] && distance == 7 && !WrapHFile[from])
		return false;

	return distance == northEast || distance == northWest;
}


// checks for pieces blocking the way. Useful for rook, queen, and bishop
bool ChessBoard::noBlockingPieces(short modifier) const {
	std::bitset<64> occupied = getAllPieces();

	for (int i = from + modifier; i != to; i += modifier) {
		if (occupied[i]) {
			return false;
		}
	}

	return true;
}

void ChessBoard::fiftyMoveRule() {
	if (this->fiftyMoves == 50) {
		this->didWin = true;
		std::cout << "Stalemate! Fifty Move Rule!" << std::endl;
	}
	if (!pawnMovement && !capture) {
		fiftyMoves++;
	}
	else {
		fiftyMoves = 0;
		capture = pawnMovement = false;
	}
}