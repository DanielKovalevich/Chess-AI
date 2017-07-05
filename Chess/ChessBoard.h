#pragma once
#include <bitset>

class ChessBoard {
public:
	// Use this to eliminate moves on the A and H rank, ie, +-1, +-7, +-9
	const std::bitset<64> 
		notAFile   = 0xfefefefefefefefe, // ~0x010101010101010
		notHFile   = 0x7f7f7f7f7f7f7f7f, // ~0x8080808080808080
		origWhite  = 0x000000000000ffff,
		origBlack  = 0xffff000000000000,
		origKing   = 0x1000000000000010,
		origQueen  = 0x0800000000000008,
		origRook   = 0x8100000000000081,
		origBishop = 0x2400000000000024,
		origKnight = 0x4200000000000042,
		origPawn   = 0x00ff00000000ff00;

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
	ChessBoard() {
		pieceBitBoard[white]  = origWhite;
		pieceBitBoard[black]  = origBlack;
		pieceBitBoard[king]   = origKing;
		pieceBitBoard[queen]  = origQueen;
		pieceBitBoard[rook]   = origRook;
		pieceBitBoard[bishop] = origBishop;
		pieceBitBoard[knight] = origKnight;
		pieceBitBoard[pawn]   = origPawn;

		// white always starts first
		color = 0;
		to = 0;
		from = 0;
		fiftyMoves = 0;
		inCheck = false;
		didWin = false;
		pawnMovement = false;
		capture = false;
	}

	// outputs to console 
	void drawBoard();

	std::bitset<64> getAllPieces() const {
		return pieceBitBoard[white] | pieceBitBoard[black];
	}

	std::bitset<64> getPieces(bool color, pieces piece) const {
		return pieceBitBoard[color] & pieceBitBoard[piece];
	}
	std::bitset<64> getPieces(bool color) const {
		return pieceBitBoard[color];
	}

	void updateMove(short from, short to) {
		this->from = from;
		this->to = to;
	}

	// at the end of each valid move, turn changes
	void changeTurn() {
		this->color = !color;
	}

	// TODO actually finish the win condition
	bool won() const {
		return didWin;
	}

	// this will handle all of the moving of the pieces
	void move(short original, short destination);

	void generateAttacks();

private:
	// store all bitboards in an array - 2 for each colors and the other for pieces
	std::bitset<64> pieceBitBoard[8];
	std::bitset<64> attackBB[8];

	//int whiteOrBlack; // keeps track of user choice for updating the board initially

	short from, to; // orign and destination
	bool color; // keeps track of whose turn it is
	bool inCheck;
	bool didWin; // store win condition

	void pawnPromotion(short pieceType, std::bitset<64> toBitBoard);
	short getPieceType();
	short getPieceType(short from);

	//-------------helper functions for board outlay---------------//
	void static createPreBoard();
	void static createPostBoard();
	//---------end of helper functions for board outlay------------//

	//--------------move validation---------------------//
	bool validateMove(short piece);
	bool validatePawnMovementForAttacks(short from, short to, bool color);
	bool noBlockingPieces(short modifier) const;
	//------------end of move validation----------------//

	//--------------------fifty move rule--------------------------//
	// keep track of information for the fifty move rule
	bool pawnMovement;
	bool capture;
	short fiftyMoves;

	// fifty move stalemate applies if there is no capture
	// or pawn movement over fifty moves
	void fiftyMoveRule();
	//---------------end of fifty move rule------------------------//
};