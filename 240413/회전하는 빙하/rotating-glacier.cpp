//
// Created by KangMinji on 2024-04-10.
//

#include <iostream>
#include <queue>
using namespace std;
typedef pair<int, int> ci;

const int MAX_N = 64;

int n, q;
int board[MAX_N][MAX_N];
int tmp_board[MAX_N][MAX_N];
int bsize;

int dx[4] = {0, 1, -1, 0};
int dy[4] = {1, 0, 0, -1};

// bfs용
bool visited[MAX_N][MAX_N];

void initTmpBoard() {
	for (int i=0; i<bsize; i++){
		for (int j=0; j<bsize; j++){
			tmp_board[i][j] = 0;
		}
	}
}

bool inRange(int x, int y) {
	return x >= 0 && y >= 0 && x < bsize && y < bsize;
}

int countCloseIce(int x, int y) {
	int cnt = 0;
	for (int i=0; i<4; i++){
		int nx = x + dx[i];
		int ny = y + dy[i];

		if (inRange(nx, ny) && board[nx][ny]) {
			cnt++;
		}
	}
	// cout << "(" << x << ", " << y << ") : " << board[x][y] << ' ' << cnt << "\n";
	return cnt;
}

void melt() {
	// 1) 초기화
	initTmpBoard();

	// 2) 인접한 곳의 얼음 찾아서 녹이기
	for (int i=0; i<bsize; i++){
		for (int j=0; j<bsize; j++){
			if (board[i][j] && countCloseIce(i, j) < 3) {
				tmp_board[i][j] = board[i][j] - 1;;
			} else {
				tmp_board[i][j] = board[i][j];
			}
		}
	}

	// 3) 녹은 이후의 결과 찾기
	for (int i=0; i<bsize; i++){
		for (int j=0; j<bsize; j++){
			board[i][j] = tmp_board[i][j];
		}
	}
}

void move(int row, int col, int size, int dir) {
	//cout << row << " " << col << ' ' << size << ' ' << dir << '\n';
	for (int x=row; x<row+size; x++){
		for (int y=col; y<col+size; y++){
			int nx = x + dx[dir] * size;
			int ny = y + dy[dir] * size;

			tmp_board[nx][ny] = board[x][y];
		}
	}
	// cout << " \n [move 후 tmp_board] : " << dir << "\n";
	// for (int i=0;i <bsize; i++){
	// 	for (int j=0; j<bsize; j++){
	// 		cout << tmp_board[i][j] << ' ';
	// 	}
	// 	cout << "\n";
	// }
}

void rotate(int level) {
	// 1) 초기화
	initTmpBoard();

	int gsize = (1<<level);
	int half = gsize/2;
	// cout << "[level] : " << level << "  gsize: " << gsize << " half: " << half << "\n";

	// 2) 2**l x 2**l 왼쪽 위 모서리 잡기
	for (int i=0; i<bsize; i+=gsize){
		for (int j=0; j<bsize; j+=gsize) {
			// 3) 2**(l-1) x 2**(l-1) 왼쪽 위 모서리 잡고 이동 시키기
			move(i, j, half, 0);
			move(i, j+half, half, 1);
			move(i+half, j, half, 2);
			move(i+half, j+half, half, 3);
		}
	}

	// 4) 원래 배열에 덮어씌우기
	for (int i=0; i<bsize; i++){
		for (int j=0; j<bsize; j++){
			board[i][j] = tmp_board[i][j];
		}
	}
}

int calcIceNum() {
	int cnt = 0;
	for (int i=0; i<bsize; i++){
		for (int j=0; j<bsize; j++){
			cnt += board[i][j];
		}
	}
	return cnt;
}

int bfs(int sx, int sy) {
	queue<ci> q;

	q.push({sx, sy});
	visited[sx][sy] = true;
	int area = 0;

	while(!q.empty()) {
		int x = q.front().first;
		int y = q.front().second;
		q.pop();

		area++;

		for (int i=0; i<4; i++){
			int nx = x + dx[i];
			int ny = y + dy[i];

			if (!inRange(nx, ny) || board[nx][ny] == 0 || visited[nx][ny]) continue;
			q.push({nx, ny});
			visited[nx][ny] = true;
		}
	}
	return area;
}

int calcBiggestSize() {
	int max_area = 0;

	for (int i=0; i<bsize; i++){
		for (int j=0; j<bsize; j++){
			if (board[i][j] && !visited[i][j]) {
				int area = bfs(i, j);
				max_area = max(max_area, area);
			}
		}
	}
	return max_area;
}

void debugPrintBoard() {
	cout << "[bsize] : " << bsize << "\n";
	for (int i=0; i<bsize; i++){
		for (int j=0; j<bsize; j++){
			cout << board[i][j] << ' ';
		}
		cout << "\n";
	}
	cout << "\n";
}

int main() {
	cin >> n >> q;
	bsize = (1 << n);

	for (int i=0; i<bsize; i++){
		for (int j=0; j<bsize; j++){
			cin >> board[i][j];
		}
	}

	int level;
	// cout << "원래 board\n";
	// debugPrintBoard();

	while(q--) {
		cin >> level;

		if (level){
			rotate(level);
		}
//		 cout << "회전 후 board\n";
//		 debugPrintBoard();
		melt();
//		 cout << "녹은 후 board\n";
//		 debugPrintBoard();
	}
//	cout << "최종\n";
//	debugPrintBoard();

	// 출력
	cout << calcIceNum() << "\n";
	cout << calcBiggestSize();
}