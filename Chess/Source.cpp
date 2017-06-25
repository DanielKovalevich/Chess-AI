// This is currently here to test parts of my chessboard

#include "ChessBoard.h"

short convertPositionToNum(char file, short rank);
bool verifyPositionInput(char file, short rank);

int main() {
	//char whiteOrBlack = 'W'; // i might use this later 
	char origFile, destFile;
	short origRank, destRank;

	ChessBoard CB;

	while (!CB.won()) {
		CB.drawBoard();
		std::cout << "Piece to move and destination: ";
		std::cin >> origFile >> origRank >> destFile >> destRank;
		origFile = toupper(origFile);
		destFile = toupper(destFile);
		if (verifyPositionInput(origFile, origRank) && verifyPositionInput(destFile, destRank)) {
			CB.move(convertPositionToNum(origFile, origRank), convertPositionToNum(destFile, destRank));
		}
		else {
			std::cout << "Improper input!" << std::endl;
			system("pause");
		}

		system("cls");
	}
}

// this converts the 'A1' style input to a number
short convertPositionToNum(char file, short rank) {
	return file - 64 + (rank - 1) * 8 - 1;
}

bool verifyPositionInput(char file, short rank) {
	return file >= 'A' && file <= 'H' && rank >= 1 && rank <= 8;
}