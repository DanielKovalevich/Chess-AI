#include "ChessBoard.h"

//----------------------------------------check validation------------------------------------------------------//
// this generates all possible attacks. This helps with doing the check validation
void ChessBoard::generateAttacks() {
	clearAttackBB();
	auto occupied = getAllPieces();
	for (int i = 0; i < 64; i++) {
		if (occupied[i] == 1) {
			short pieceType = getPieceType(i);
			bool pieceColor = occupied[i] & pieceBitBoard[0][i] ? white : black;

			if (pieceType == pawn)
				generatePawnAttacks(i, pieceColor);
			else if (pieceType == queen)
				generateQueenAttacks(i, pieceColor);
			else if (pieceType == rook)
				generateRookAttacks(i, pieceColor);
			else if (pieceType == bishop)
				generateBishopAttacks(i, pieceColor);
			else if (pieceType == knight)
				generateKnightAttacks(i, pieceColor);
			else 
				generateKingAttacks(i, pieceColor);
		}
	}
}

void ChessBoard::generatePawnAttacks(short pos, bool color) {
	if (validatePawnMovementForAttacks(pos, pos + northWest, color) && !pieceBitBoard[color][pos + northWest]) {
		attackBB[pawn][pos + northWest] = true;
		attackBB[color][pos + northWest] = true;
		if (pieceBitBoard[king][pos + northWest] & pieceBitBoard[!color][pos + northWest]) {
			threatPiece[pos] = true;
			threatPiece[pos] = true;
		}
	}
	if (validatePawnMovementForAttacks(pos, pos + northEast, color) && !pieceBitBoard[color][pos + northEast]) {
		attackBB[pawn][pos + northEast] = true;
		attackBB[color][pos + northEast] = true;
		if (pieceBitBoard[king][pos + northEast] & pieceBitBoard[!color][pos + northEast]) {
			threatPiece[pos] = true;
		}
	}
	if (validatePawnMovementForAttacks(pos, pos + southEast, color) && !pieceBitBoard[color][pos + southEast]) {
		attackBB[pawn][pos + southEast] = true;
		attackBB[color][pos + southEast] = true;
		if (pieceBitBoard[king][pos + southEast] & pieceBitBoard[!color][pos + southEast]) {
			threatPiece[pos] = true;
		}
	}
	if (validatePawnMovementForAttacks(pos, pos + southWest, color) && !pieceBitBoard[color][pos + southWest]) {
		attackBB[pawn][pos + southWest] = true;
		attackBB[color][pos + southWest] = true;
		if (pieceBitBoard[king][pos + southWest] & pieceBitBoard[!color][pos + southWest]) {
			threatPiece[pos] = true;
		}
	}

	// TODO en passant
}

void ChessBoard::generateKnightAttacks(short pos, bool color) {
	short possibleMoves[8] = { -6, 6, -10, 10, -15, 15, -17, 17 };

	for (short move : possibleMoves) {
		bool edge = false;

		if (pos + move >= 0 && pos + move <= 63) {
			// separated all the edge case conditions so that it is easier to read
			if (AFile[pos] && (move == 15 || move == -10 || move == 6 || move == -17))
				edge = true;
			else if (AFile[pos] && (move == -10 || move == 6))
				edge = true;
			else if (HFile[pos] && (move == 17 || move == 10 || move == -6 || move == -15))
				edge = true;
			else if (HFile[pos] && (move == 10 || move == -6))
				edge = true;

			if (!edge && !pieceBitBoard[color][pos + move]) {
				attackBB[knight][pos + move] = true;
				attackBB[color][pos + move] = true;
				if (pieceBitBoard[king][pos + move] & pieceBitBoard[!color][pos + move]) {
					threatPiece[pos] = true;
				}
			}
		}
	}
}

void ChessBoard::generateBishopAttacks(short pos, bool color) {
	short possibleMoves[4] = { northEast, southEast, southWest, northWest };
	for (short move : possibleMoves) {
		short possibleMove = pos + move;

		bool edge = AFile[pos] && (move == northWest || move == southWest) ||
			HFile[pos] && (move == northEast || move == southEast);
		while (possibleMove >= 0 && possibleMove <= 63 && !edge) {
			if (HFile[possibleMove] || AFile[possibleMove]) {
				attackBB[bishop][possibleMove] = true;
				attackBB[color][possibleMove] = true;
				break;
			}

			if (pieceBitBoard[!color][possibleMove]) {
				attackBB[bishop][possibleMove] = true;
				attackBB[color][possibleMove] = true;
				if (pieceBitBoard[king][possibleMove] & pieceBitBoard[!color][possibleMove]) {
					threatPiece[pos] = true;
				}
				break;
			}

			if (pieceBitBoard[color][possibleMove]) {
				break;
			}

			attackBB[bishop][possibleMove] = true;
			attackBB[color][possibleMove] = true;
			possibleMove += move;
		}
	}
}

void ChessBoard::generateRookAttacks(short pos, bool color) {
	short possibleMoves[4] = { north, east, south, west };
	for (short move : possibleMoves) {
		short possibleMove = pos + move;

		bool edge = AFile[pos] && move == west || HFile[pos] && move == east;
		while (possibleMove >= 0 && possibleMove <= 63 && !edge) {
			// don't want to restrict it from moving along the edge
			if ((HFile[possibleMove] || AFile[possibleMove]) && move != north && move != south) {
				attackBB[rook][possibleMove] = true;
				attackBB[color][possibleMove] = true;
				break;
			}

			if (pieceBitBoard[!color][possibleMove]) {
				attackBB[rook][possibleMove] = true;
				attackBB[color][possibleMove] = true;
				if (pieceBitBoard[king][possibleMove] & pieceBitBoard[!color][possibleMove]) {
					threatPiece[pos] = true;
				}
				break;
			}

			if (pieceBitBoard[color][possibleMove]) {
				break;
			}

			attackBB[rook][possibleMove] = true;
			attackBB[color][possibleMove] = true;
			possibleMove += move;
		}
	}
}

void ChessBoard::generateQueenAttacks(short pos, bool color) {
	short possibleMoves[8] = { north, northWest, northEast, east, southEast, south, southWest, west };
	for (short move : possibleMoves) {
		short possibleMove = pos + move;
		// if the queen is on the edge, restrict some of the moves
		// this must be done or else the moves wrap around the board
		bool edge = AFile[pos] && (move == northWest || move == southWest || move == west) ||
			HFile[pos] && (move == northEast || move == southEast || move == east);

		// steps through each space in every direction and stops when it hits a same color piece
		// it will also stop if it hits the edge of the board or gets out of range
		while (possibleMove >= 0 && possibleMove <= 63 && !edge) {
			// don't want to restrict it from moving along the edge
			if ((HFile[possibleMove] || AFile[possibleMove]) && move != north && move != south) {
				attackBB[queen][possibleMove] = true;
				attackBB[color][possibleMove] = true;
				break;
			}

			// if it's an enemy piece mark space as attack and break out
			if (pieceBitBoard[!color][possibleMove]) {
				attackBB[queen][possibleMove] = true;
				attackBB[color][possibleMove] = true;
				if (pieceBitBoard[king][possibleMove] & pieceBitBoard[!color][possibleMove]) {
					threatPiece[pos] = true;
				}
				break;
			}

			if (pieceBitBoard[color][possibleMove]) {
				break;
			}

			attackBB[queen][possibleMove] = true;
			attackBB[color][possibleMove] = true;
			possibleMove += move;
		}
	}
}

void ChessBoard::generateKingAttacks(short pos, bool color) {
	short possibleMoves[8] = { north, northWest, northEast, east, southEast, south, southWest, west };
	for (short move : possibleMoves) {
		// avoid wrapping around the board
		bool edge = AFile[pos] && (move == northWest || move == southWest || move == west) ||
			HFile[pos] && (move == northEast || move == southEast || move == east);

		if (pos + move >= 0 && pos + move <= 63 && !edge) {
			// makes sure king won't try to attack own piece or space that will get attacked
			if (!pieceBitBoard[color][pos + move] && !attackBB[!color][pos + move]) {
				attackBB[king][pos + move] = true;
				attackBB[color][pos + move] = true;
			}
		}
	}
}

// clears the bitboards each time so previous attacks don't stay
void ChessBoard::clearAttackBB() {
	for (int i = 0; i < 8; i++) {
		attackBB[i].reset();
	}
}

// checks if the king is sitting on one of the possible spaces of attack
void ChessBoard::isInCheck() {
	auto enemyKing = getPieces(!color, king);

	if ((enemyKing & attackBB[color]) != 0x0) {
		inCheck = true;
		std::cout << std::setw(21) << "Check!" << std::endl;
	}
}

// checks for all possibilities of a checkmate
void ChessBoard::isCheckMate() {
	auto kingPiece = getPieces(color, king);
	short possibleMoves[8] = { north, northWest, northEast, east, southEast, south, southWest, west };
	bool attackMove = false;
	// pawn and knight can't be blocked so if they can't be taken out, it is checkmate
	bool blocked = false;
	short position = 0;

	// first i need to find the position of the king
	bool found = false;
	while (!found) {
		kingPiece[position] ? found = true : position++;
	}

	// check if the king can escape the check by moving
	for (auto move : possibleMoves) {
		if (move + position >= 0 && move + position <= 63) {
			if (!attackBB[!color][move + position] && !pieceBitBoard[color][move + position]) {
				possibleMovements[move + position] = true;
			}
			// finds if king can attack enemy piece - temp attacks and then checks if it is valid
			if (pieceBitBoard[!color][move + position]) {
				auto piece = getPieceType(move + position);
				// i need one to store just the initial pos while other holds the destination too
				std::bitset<64> tempPieceBB = 0x0, tempInitialBB = 0x0;
				tempInitialBB[move + position] = true;
				tempPieceBB[move + position] = true;
				tempPieceBB[position] = true;

				// this will allow me to temporarily modify the piecebitboards to generate possible attacks
				std::bitset<64> temp[8];
				for (auto i = 0; i < 8; i++) {
					temp[i] = pieceBitBoard[i];
				}

				pieceBitBoard[color] ^= tempPieceBB;
				pieceBitBoard[king] ^= tempPieceBB;
				pieceBitBoard[!color] ^= tempInitialBB;
				pieceBitBoard[piece] ^= tempInitialBB;

				generateAttacks();

				auto kingBB = getPieces(color, king);

				if ((kingBB & attackBB[!color]) == 0x0) {
					possibleMovements[move + position] = true;
				}

				// revert the bitboards back to normal
				for (auto i = 0; i < 8; i++) {
					pieceBitBoard[i] = temp[i];
				}
			}
		}
	}

	// checks if it is possible for the attacking pieces to be taken out
	if (threatPiece.any()) {
		for (int i = 0; i < 64; i++) {
			if (threatPiece[i] & attackBB[color][i]) {
				threatPiece[i] = false;
			}
		}
	}

	

	// if there are any threats left, check to see if can block attack with own piece
	// pawn or knight can't be blocked
	if (threatPiece.any()) {
		for (int i = 0; i < 64; i++) {
			if (threatPiece[i]) {

				short leftMaxRange = i - i % 8;
				short rightMaxRange = 7 + leftMaxRange;

				auto pieceType = getPieceType(i);
				if (pieceType == knight || pieceType == pawn)
					blocked = true;

				short distance = position - i;

				// this will be vertical attacks
				if (distance % 8 == 0) {
					possibleBlock(8, distance, blocked, i);
				}
				// this will be the diagonal attacks
				else if (distance % 7 == 0) {
					possibleBlock(7, distance, blocked, i);
				}
				else if (distance % 9 == 0) {
					possibleBlock(9, distance, blocked, i);
				}
				// this will be horizontal attacks
				else if (position - i + i >= leftMaxRange && position - i + i <= rightMaxRange) {
					possibleBlock(1, distance, blocked, i);
				}
				
			}
		}
	}

	// if all escapes fail, the game ends
	if (possibleMovements.none() && !attackMove && threatPiece.any() && !blocked) {
		std::cout << std::setw(22) << "Checkmate!" << std::endl;
		std::cout << std::setw(13) << (color ? "White" : "Black") << " is the winner" << std::endl;
		didWin = true;
		system("pause");
	}
}

// checks if it is possible to block an attack with another piece
void ChessBoard::possibleBlock(short modifier, short distance, bool & blocked, short threatPos) {
	bool positive = distance > 0;
	// i need to make sure I don't include the two pieces
	distance += positive ? -1 * modifier : modifier;

	auto allColorPieces = getPieces(color);

	// this means the piece is next to the king and is protected
	if (distance == 0)
		blocked = true;
	else {
		for (int i = 0; i < 64; i++) {
			int temp = distance;
			if (allColorPieces[i]) {
				auto piece = getPieceType(i);
				while (distance != 0) {
					// i'm doing this so i don't need to overload my validateMove function
					updateMove(i, threatPos + distance);

					if (validateMove(piece)) {
						possibleMovements[threatPos + distance] = true;
					}
					distance += positive ? -1 * modifier : modifier;
				}
			}
			distance = temp;
		}
	}
}
//----------------------------------------end of check validation------------------------------------------------------//