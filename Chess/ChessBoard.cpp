// Written by Daniel Kovalevich
// Separation of implementation and interface

#include "ChessBoard.h"
#include <iomanip>
#include <string>
#include <iostream>

// --------------------------------Board Creation---------------------------------- //

// this function prints out the ASCII chessboard to the console
// i want to see if I can do it all in the console before moving to a GUI
void ChessBoard::drawBoard() {
	auto occupied = getAllPieces();

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

	// for testing purposes
	// TODO delete when done
	for (int i = 56; i >= 0; i -= 8) {
		for (int j = i; j - i < 8; j++) {
			std::cout << attackBB[white][j];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
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

// ------------------------------------End of Board Creation---------------------------------- //

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

		std::cout << "The queen can't move that way!" << std::endl;
		return false;
	}

	if (piece == rook) {
		if (distance % north == 0)
			return positive ? noBlockingPieces(north) : noBlockingPieces(south);
		if (to - from + from >= leftMaxRange && to - from + from <= rightMaxRange)
			return positive ? noBlockingPieces(east) : noBlockingPieces(west);
		
		std::cout << "The rook can't move that way!" << std::endl;
		return false;
	}

	if (piece == bishop) {
		if (distance % northWest == 0)
			return positive ? noBlockingPieces(northWest) : noBlockingPieces(southEast);
		if (distance % northEast == 0)
			return positive ? noBlockingPieces(northEast) : noBlockingPieces(southWest);
		
		std::cout << "The bishop can't move that way!" << std::endl;
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

//----------------------------------------check validation------------------------------------------------------//
// this generates all possible attacks. This helps with doing the check validation
void ChessBoard::generateAttacks() {
	clearAttackBB();
	auto occupied = getAllPieces();
	for (int i = 0; i < 64; i++) {
		if (occupied[i] == 1) {
			short pieceType = getPieceType(i);
			bool pieceColor = occupied[i] & pieceBitBoard[0][i] ? white : black;

			if (pieceType == pawn) {
				if (validatePawnMovementForAttacks(i, i + northWest, pieceColor) && !pieceBitBoard[pieceColor][i + northWest]) {
					attackBB[pawn][i + northWest] = true;
					attackBB[pieceColor][i + northWest] = true;
					if (pieceBitBoard[king][i + northWest] & pieceBitBoard[!pieceColor][i + northWest]) {
						threatPiece[i] = true;
						threatPiece[i] = true;
					}
				}
				if (validatePawnMovementForAttacks(i, i + northEast, pieceColor) && !pieceBitBoard[pieceColor][i + northEast]) {
					attackBB[pawn][i + northEast] = true;
					attackBB[pieceColor][i + northEast] = true;
					if (pieceBitBoard[king][i + northEast] & pieceBitBoard[!pieceColor][i + northEast]) {
						threatPiece[i] = true;
					}
				}
				if (validatePawnMovementForAttacks(i, i + southEast, pieceColor) && !pieceBitBoard[pieceColor][i + southEast]) {
					attackBB[pawn][i + southEast] = true;
					attackBB[pieceColor][i + southEast] = true;
					if (pieceBitBoard[king][i + southEast] & pieceBitBoard[!pieceColor][i + southEast]) {
						threatPiece[i] = true;
					}
				}
				if (validatePawnMovementForAttacks(i, i + southWest, pieceColor) && !pieceBitBoard[pieceColor][i + southWest]) {
					attackBB[pawn][i + southWest] = true;
					attackBB[pieceColor][i + southWest] = true;
					if (pieceBitBoard[king][i + southWest] & pieceBitBoard[!pieceColor][i + southWest]) {
						threatPiece[i] = true;
					}
				}

				// TODO en passant
			}
			
			if (pieceType == queen) {
				short possibleMoves[8] = { north, northWest, northEast, east, southEast, south, southWest, west };
				for (short move : possibleMoves) {
					short possibleMove = i + move;
					// if the queen is on the edge, restrict some of the moves
					// this must be done or else the moves wrap around the board
					bool edge = AFile[i] && (move == northWest || move == southWest || move == west) || 
								HFile[i] && (move == northEast || move == southEast || move == east);

					// steps through each space in every direction and stops when it hits a same color piece
					// it will also stop if it hits the edge of the board or gets out of range
					while (possibleMove >= 0 && possibleMove <= 63 && !edge) {
						// don't want to restrict it from moving along the edge
						if ((HFile[possibleMove] || AFile[possibleMove]) && move != north && move != south) {
							attackBB[queen][possibleMove] = true;
							attackBB[pieceColor][possibleMove] = true;
							break;
						}

						// if it's an enemy piece mark space as attack and break out
						if (pieceBitBoard[!pieceColor][possibleMove]) {
							attackBB[queen][possibleMove] = true;
							attackBB[pieceColor][possibleMove] = true;
							if (pieceBitBoard[king][possibleMove] & pieceBitBoard[!pieceColor][possibleMove]) {
								threatPiece[i] = true;
							}
							break;
						}
						
						if (pieceBitBoard[pieceColor][possibleMove]) {
							break;
						}

						attackBB[queen][possibleMove] = true;
						attackBB[pieceColor][possibleMove] = true;
						possibleMove += move;
					}
				}
			}

			if (pieceType == rook) {
				short possibleMoves[4] = { north, east, south, west };
				for (short move : possibleMoves) {
					short possibleMove = i + move;

					bool edge = AFile[i] && move == west || HFile[i] && move == east;
					while (possibleMove >= 0 && possibleMove <= 63 && !edge) {
						// don't want to restrict it from moving along the edge
						if ((HFile[possibleMove] || AFile[possibleMove]) && move != north && move != south) {
							attackBB[rook][possibleMove] = true;
							attackBB[pieceColor][possibleMove] = true;
							break;
						}

						if (pieceBitBoard[!pieceColor][possibleMove]) {
							attackBB[rook][possibleMove] = true;
							attackBB[pieceColor][possibleMove] = true;
							if (pieceBitBoard[king][possibleMove] & pieceBitBoard[!pieceColor][possibleMove]) {
								threatPiece[i] = true;
							}
							break;
						}

						if (pieceBitBoard[pieceColor][possibleMove]) {
							break;
						}

						attackBB[rook][possibleMove] = true;
						attackBB[pieceColor][possibleMove] = true;
						possibleMove += move;
					}
				}
			}

			if (pieceType == bishop) {
				short possibleMoves[4] = { northEast, southEast, southWest, northWest };
				for (short move : possibleMoves) {
					short possibleMove = i + move;

					bool edge = AFile[i] && (move == northWest || move == southWest) || 
								HFile[i] && (move == northEast || move == southEast);
					while (possibleMove >= 0 && possibleMove <= 63 && !edge) {
						if (HFile[possibleMove] || AFile[possibleMove]) {
							attackBB[bishop][possibleMove] = true;
							attackBB[pieceColor][possibleMove] = true;
							break;
						}

						if (pieceBitBoard[!pieceColor][possibleMove]) {
							attackBB[bishop][possibleMove] = true;
							attackBB[pieceColor][possibleMove] = true;
							if (pieceBitBoard[king][possibleMove] & pieceBitBoard[!pieceColor][possibleMove]) {
								threatPiece[i] = true;
							}
							break;
						}

						if (pieceBitBoard[pieceColor][possibleMove]) {
							break;
						}

						attackBB[bishop][possibleMove] = true;
						attackBB[pieceColor][possibleMove] = true;
						possibleMove += move;
					}
				}
			}

			if (pieceType == knight) {
				short possibleMoves[8] = { -6, 6, -10, 10, -15, 15, -17, 17 };

				for (short move : possibleMoves) {
					bool edge = false;

					if (i + move >= 0 && i + move <= 63) {
						// separated all the edge case conditions so that it is easier to read
						if (AFile[i] && (move == 15 || move == -10 || move == 6 || move == -17))
							edge = true;
						else if (AFile[i] && (move == -10 || move == 6))
							edge = true;
						else if (HFile[i] && (move == 17 || move == 10 || move == -6 || move == -15))
							edge = true;
						else if (HFile[i] && (move == 10 || move == -6))
							edge = true;

						if (!edge && !pieceBitBoard[pieceColor][i + move]) {
							attackBB[knight][i + move] = true;
							attackBB[pieceColor][i + move] = true;
							if (pieceBitBoard[king][i + move] & pieceBitBoard[!pieceColor][i + move]) {
								threatPiece[i] = true;
							}
						}
					}
					
				}

			}

			if (pieceType == king) {
				short possibleMoves[8] = { north, northWest, northEast, east, southEast, south, southWest, west };
				for (short move : possibleMoves) {
					// avoid wrapping around the board
					bool edge = AFile[i] && (move == northWest || move == southWest || move == west) ||
						HFile[i] && (move == northEast || move == southEast || move == east);

					if (i + move >= 0 && i + move <= 63 && !edge) {
						// makes sure king won't try to attack own piece or space that will get attacked
						if (!pieceBitBoard[pieceColor][i + move] && !attackBB[!pieceColor][i + move]) {
							attackBB[king][i + move] = true;
							attackBB[pieceColor][i + move] = true;
						}
					}
				}
			}
		}
	}
}

// clears the bitboards each time so previous attacks don't stay
void ChessBoard::clearAttackBB() {
	for (int i = 0; i < 8; i++) {
		attackBB[i] = 0x0;
	}
}

// checks if the king is sitting on one of the possible spaces of attack
void ChessBoard::isInCheck() {
	auto enemyKing = getPieces(!color, king);
	
	if ((enemyKing & attackBB[color]) != 0x0) {
		inCheck = true;
		std::cout << "Check!" << std::endl;
	}
}

// checks for all possibilities of a checkmate
void ChessBoard::isCheckMate() {
	auto kingPiece = getPieces(color, king);
	short possibleMoves[8] = { north, northWest, northEast, east, southEast, south, southWest, west };
	bool attackMove = false;
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

	// if all escapes fail, the game ends
	if (possibleMovements.any() && !attackMove && threatPiece.any()) {
		std::cout << "Checkmate!" << std::endl;
		std::cout << (color ? "White" : "Black") << " is the winner" << std::endl;
		didWin = true;
		system("pause");
	}
}


//----------------------------------------end of check validation------------------------------------------------------//