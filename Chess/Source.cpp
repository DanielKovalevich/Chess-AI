// This is currently here to test parts of my chessboard

#include <iostream>
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

		if (CB.inCheck) {
			CB.isCheckMate();
		}

		// don't want input on a checkmate
		if (!CB.won()) {
			std::cout << "Piece to move and destination: ";
			std::cin >> origFile >> origRank >> destFile >> destRank;
			origFile = toupper(origFile);
			destFile = toupper(destFile);
			if (CB.inCheck && verifyPositionInput(origFile, origRank) && verifyPositionInput(destFile, destRank)) {
				// used to restore the threat piece position
				CB.generateAttacks();
				auto move = convertPositionToNum(destFile, destRank);

				if (CB.possibleMovements[move] || CB.threatPiece[move]) {
					if (CB.move(convertPositionToNum(origFile, origRank), convertPositionToNum(destFile, destRank))) {
						CB.inCheck = false;
						CB.possibleMovements = 0x0;
						CB.possibleMovements = 0x0;
					}
				}
				else {
					std::cout << "You must get the king out of danger first!" << std::endl;
					system("pause");
				}

			}
			else if (verifyPositionInput(origFile, origRank) && verifyPositionInput(destFile, destRank)) {
				CB.move(convertPositionToNum(origFile, origRank), convertPositionToNum(destFile, destRank));
			}
			else {
				std::cout << "Improper input!" << std::endl;
				system("pause");
			}
			system("cls");
		}
	}

	std::cout << "Thanks for playing!" << std::endl;
	return 0;
}

// this converts the 'A1' style input to a number
short convertPositionToNum(char file, short rank) {
	return file - 64 + (rank - 1) * 8 - 1;
}

bool verifyPositionInput(char file, short rank) {
	return file >= 'A' && file <= 'H' && rank >= 1 && rank <= 8;
}