#include <list>
#include <iostream>
#include <string>

using namespace std;
// breath first
//class node { // subtracts depth as it goes deeper. eliminates bad ones?
//	private:
//		int depth;
//		char board[8][8];
//		list<node> nodes;
//		chessboard* board;
//	public:
//		node(chessboard* ptr) { &board = &ptr; }
class chessboard { 
	protected:
		char board[8][8];
	int fromX, fromY;
	struct move {
		int x, y, fromY, fromX;
		char piece;
		int ev;
	} loc;
	list<move> possible;
public:
	chessboard(char b[][8]);
	void printboard();
	void addloc(int x, int y, int fromX, int fromY);
	void repeat(int piecesquares[][2], int size, int x, int y, char side);
	void possibleMoves(char side);
	void move(char piece, int x, int y);
	void move(char piece, int x, int y, int fromX, int fromY);
	void printpossible();
	int eval(char side);
	void callPossible(int depth);
};
chessboard::chessboard(char b[][8]){
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = b[y][x];
		}
	}
}
void chessboard::printboard() {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			cout << board[y][x];
		}
		cout << endl;
	}
	cout << endl;
}
void chessboard::addloc(int x, int y, int fromX, int fromY) {
	loc.x = x; loc.y = y;
	if (board[fromY][fromX] == 'p' && fromY == 1 && y == 0 && board[y][x] == '0') {
		loc.piece = 'q';
	}
	else if (board[fromY][fromX] == 'P' && fromY == 6 && y == 7 && board[y][x] == '0'){
		loc.piece = 'Q';
	}
	else {
		loc.piece = board[fromY][fromX];
	}
	loc.fromX = fromX;
	loc.fromY = fromY;
	possible.push_back(loc);
}
bool isvalid(int x, int y) {
	return (y >= 0 && y < 8 && x >= 0 && x < 8);
}
void chessboard::repeat(int piecesquares[][2] ,int size,  int x, int y, char side) {
	int tempY, tempX;
	bool valid;
	char square = '0';
	for (int i = 0; i < size; i++) {
		tempY = y + piecesquares[i][0]; tempX = x + piecesquares[i][1];
		valid = isvalid(tempX, tempY);
		if (valid) {
			square = board[tempY][tempX];
			while (square == '0' && valid) {
				addloc(tempX, tempY, x, y);
				tempY += piecesquares[i][0]; tempX += piecesquares[i][1];
				valid = isvalid(tempX, tempY);
				if (valid) { square = board[tempY][tempX]; } // må teste om tempy og tempx er utenfor
			}
			if (islower(square) && side == 'W' && isupper(board[y][x])) { // error free code since 1981
				addloc(tempX, tempY, x, y);
			}
			if (isupper(square) && side == 'B' && islower(board[y][x])) {
				addloc(tempX, tempY, x, y);
			}
		}
	}
}
void chessboard::possibleMoves(char side) {
	int knightsquares[8][2] = { {2,-1},{2,1},{1,2},{-1,2},{-2,1},{-2,-1},{-1,-2}, {1,-2} };
	int kingsquares[8][2] = { {1,-1} ,{1,0} ,{1,1} ,{0,1} ,{-1,-1} ,{-1,0} ,{-1,-1} ,{0,-1} };
	int bishopsquares[4][2] = { {1,-1}, {1, 1}, {-1,1}, {-1, -1} };
	int rooksquares[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1}};
	int pawnsquares[2][2] = { {1,-1}, {1,1} }; // POTENTIAL REFACTORING
	char square = '0';
	int tempX, tempY;
	bool valid = false;

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (isupper(board[y][x]) && side == 'W') { // hvit
				if (board[y][x] == 'P') { // hvit bonde
					if (board[y + 1][x] == '0') { // bonde 1 fram
						addloc(x, y + 1, x, y);
						if (y == 1 && board[y + 2][x] == '0') { // bonde 2 fram
							addloc(x, y + 2, x, y);
						}
					}
					if (islower(board[y + 1][x + 1]) && board[y + 1][x + 1] != 'k' && isvalid(x+1, y+1)) {
						addloc(x + 1, y + 1, x, y);
					}
					if (islower(board[y + 1][x - 1]) && board[y + 1][x + 1] != 'k' && isvalid(x-1, y+1)) {
						addloc(x - 1, y + 1, x, y);
					}
				}
			}
			if (board[y][x] == 'N' || board[y][x] == 'n') { // knight moves the same way independent of side
				for (int i = 0; i < 8; i++) {
					tempY = y + knightsquares[i][0]; tempX = x + knightsquares[i][1];
					if (tempY >= 0 && tempY < 8 && tempX >= 0 && tempX < 8) {
						square = board[tempY][tempX];
						if ((square == '0' || islower(square)) && side == 'W' && isupper(board[y][x]) && board[tempY][tempX] != 'k') {
							addloc(tempX, tempY, x, y);
						}
						if ((square == '0' || isupper(square)) && side == 'B' && islower(board[y][x]) && board[tempY][tempX] != 'K') {
							addloc(tempX, tempY, x, y);
						}
					}
				}
			}
			if (board[y][x] == 'B' || board[y][x] == 'b') {
				repeat(bishopsquares, 4, x, y, side);
			}
			if (board[y][x] == 'R' || board[y][x] == 'r') {
				repeat(rooksquares, 4, x, y, side);
			}
			if (board[y][x] == 'Q' || board[y][x] == 'q') {
				repeat(rooksquares, 4, x, y, side);
				repeat(bishopsquares, 4, x, y, side);
			}
			if (board[y][x] == 'k' || board[y][x] == 'K') { // knight moves the same way independent of side
				for (int i = 0; i < 8; i++) {
					tempY = y + kingsquares[i][0]; tempX = x + kingsquares[i][1];
					if (tempY >= 0 && tempY < 8 && tempX >= 0 && tempX < 8) {
						square = board[tempY][tempX];
						if ((square == '0' || islower(square)) && side == 'W' && isupper(board[y][x]) && board[tempY][tempX] != 'k') {
							addloc(tempX, tempY, x, y);
						}
						if ((square == '0' || isupper(square)) && side == 'B' && islower(board[y][x]) && board[tempY][tempX] != 'K') {
							addloc(tempX, tempY, x, y);
						}
					}
				}
			}
			
			if (islower(board[y][x]) && side == 'B') { // svart
				if (board[y][x] == 'p') {
					if (board[y - 1][x] == '0') { // bonde 1 fram
						addloc(x, y-1, x, y);
						if (y == 6 && board[y - 2][x] == '0') { // bonde 2 fram
							addloc(x, y-2, x, y);
						}
					}
					if (isupper(board[y - 1][x + 1]) && board[y - 1][x + 1] != 'k'  && isvalid(x + 1, y - 1)) {
						addloc(x + 1, y - 1, x, y);
					}
					if (isupper(board[y - 1][x - 1]) && board[y - 1][x - 1] != 'k'  && isvalid(x - 1, y - 1)) {
						addloc(x - 1, y - 1, x, y);
					}
				}
			}
		}
	}
}

void chessboard::move(char piece, int x, int y){
	board[fromY][fromX] = '0';
	board[y][x] = piece;
}
void chessboard::move(char piece, int x, int y, int fromX, int fromY) {
	board[fromY][fromX] = '0';
	board[y][x] = piece;
}

void chessboard::printpossible(){
	int count = 0;
	for (auto i = possible.begin(); i != possible.end(); i++) {
		count += 1;
		cout << "(x, y): " << (*i).x << ", " << (*i).y;
		cout << " piece: " << (*i).piece << " to square:  " << board[(*i).y][(*i).x];
		cout << " from (x, y): " << (*i).fromX << ", " << (*i).fromY;
		cout << endl;
	}
	cout << "possible moves: " << count << endl;
}

int chessboard::eval(char side){ // not working
	char pieces[6] = { 'k','q','b','n','r', 'p' };
	int value[6] = { 200, 10, 3, 3, 5, 1};
	int blackev = 0;
	int whiteev = 0;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			for (int i = 0; i < 6; i++) {
				if (tolower(board[y][x]) == pieces[i]) {
					(isupper(board[y][x])) ? (whiteev += value[i]) : (blackev += value[i]);
					break;
				}
			}
		}
	}

	return (whiteev-blackev);
}
void chessboard::callPossible(int depth){
	list<chessboard*> boards;
	//list<int> ev;
	//int evaliation;
	chessboard* bPtr;
	int d = depth - 1;
	if (d != 0) {
		for (auto i = possible.begin(); i != possible.end(); i++) {
			bPtr = new chessboard(board);
			bPtr->move((*i).piece, (*i).x, (*i).y, (*i).fromX, (*i).fromY);
			bPtr->possibleMoves(((islower((*i).piece)) ? 'W' : 'B'));
			boards.push_back(bPtr);

			(*i).ev = bPtr->eval(((isupper((*i).piece)) ? 'W' : 'B'));
			//ev.push_back(evaliation);
			cout << (*i).ev << " " << (*i).x << " " << (*i).y << endl;

			bPtr->callPossible(d);
		}
	}
	
}
//class player : public chessboard {
//private:
//	int depth;
//	list<node> nodes;
//public:
//};
char startboard[8][8] = { {'R','N','B','K','Q','B','N','R'},
						  {'P','P','P','P','P','P','P','P'},
					      {'0','0','0','0','0','0','0','0'},
					      {'0','0','0','0','0','0','0','0'},
					      {'0','0','0','0','0','0','0','0'},
					      {'0','0','0','0','0','0','0','0'},
					      {'p','p','p','p','p','p','p','p'},
					      {'r','n','b','q','k','b','n','r'} };
int main() {
	chessboard board(startboard);
	board.printboard();
	board.possibleMoves('W');
	board.printpossible();
	cout << board.eval('W') << endl;
	board.callPossible(1);
	cout << "finished" << endl;

}