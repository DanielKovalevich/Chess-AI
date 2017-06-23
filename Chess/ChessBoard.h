#pragma once
#include <iostream>
#include <bitset>
#include <iomanip>

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
	ChessBoard() {
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
	void inline drawBoard() const;

	std::bitset<64> getAllPieces() const {
		return pieceBitBoard[white] | pieceBitBoard[black];
	}

	std::bitset<64> getPieces(pieces color, pieces piece) const {
		return pieceBitBoard[color] & pieceBitBoard[piece];
	}
	std::bitset<64> getPieces(pieces color) const {
		return pieceBitBoard[color];
	}

private:
	// store all bitboards in an array - 2 for each colors and the other for pieces
	std::bitset<64> pieceBitBoard[8];
};

void ChessBoard::drawBoard() const {
	std::bitset<64> occupied = getAllPieces();
	char bullet = static_cast<char>(250);
	std::cout << "  +--------------------------+ " << std::endl;
	std::cout << "  |" << std::setw(27) << "|" << std::endl;
	for (int i = 0; i < 64; i++) {
		if (i % 8 == 0) {
			std::cout << std::setw(3) << "|";
		}

		if (occupied[i] & pieceBitBoard[king][i])
			std::cout << std::setw(3) <<  "K";
		else if (occupied[i] & pieceBitBoard[queen][i])
			std::cout << std::setw(3) << "Q";
		else if (occupied[i] & pieceBitBoard[rook][i])
			std::cout << std::setw(3) << "R";
		else if (occupied[i] & pieceBitBoard[bishop][i])
			std::cout << std::setw(3) << "B";
		else if (occupied[i] & pieceBitBoard[knight][i])
			std::cout << std::setw(3) << "N";
		else if (occupied[i] & pieceBitBoard[pawn][i])
			std::cout << std::setw(3) << "P";
		else
			std::cout << std::setw(3) << bullet;

		if (i != 0 && (i + 1) % 8 == 0)
			std::cout << std::setw(3) << "|" << std::endl;
	}

	std::cout << "  |" << std::setw(27) << "|" << std::endl;
	std::cout << "  +--------------------------+ " << std::endl;
}