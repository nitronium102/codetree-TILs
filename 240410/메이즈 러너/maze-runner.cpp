#include <iostream>
#include <queue>

using namespace std;
typedef pair<int, int> ci;

const int MAX_N = 15;
const int MAX_M = 15;

int n, m, k;
int board[MAX_N][MAX_N]; // 내구도 정보
ci people[MAX_M]; // 사람들 좌표
bool is_out[MAX_M]; // 탈출 여부

ci exit_coord; // 출구 좌표
int answer; // 모든 참가자들의 거리 합

int next_board[MAX_N][MAX_N];
int sr, sc, ssize; // 회전해야 하는 최소 정사각형

// 상하좌우
int dx[4] = {-1, 1, 0, 0};
int dy[4] = {0, 0, -1, 1};

void debugPrintBoard() {
	cout << "보드 회전 after\n";
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= n; j++) {
			cout << board[i][j] << ' ';
		}
		cout << "\n";
	}
	cout << "\n";
}

void debugPrintPeople() {
	cout << "사람 위치\n";
	for (int i = 1; i <= m; i++) {
		cout << is_out[i] << " | " << people[i].first << ' ' << people[i].second << "\n";
	}
}


bool checkAllOut() {
	for (int i = 1; i <= m; i++) {
		if (!is_out[i]) return false;
	}
	return true;
}

int moveParticipants() {
	int sum = 0;
	for (int i = 1; i <= m; i++) {
		// 이미 탈출한 경우 패스
		if (is_out[i]) continue;

		int original_dist = abs(people[i].first - exit_coord.first) + abs(people[i].second - exit_coord.second);
		for (int dir = 0; dir < 4; dir++) {
			int nx = people[i].first + dx[dir];
			int ny = people[i].second + dy[dir];

			// 범위 바깥이거나 벽인 경우
			if (nx < 1 || ny < 1 || nx > n || ny > n) continue;
			if (board[nx][ny]) continue;

			// 출구인 경우
			if (nx == exit_coord.first && ny == exit_coord.second) {
				is_out[i] = true;
				sum++;
				break;
			}

			// 거리가 1 차이나는 경우
			int dist = abs(nx - exit_coord.first) + abs(ny - exit_coord.second);
			if (original_dist - dist == 1) {
				people[i] = {nx, ny};
				sum++;
				break;
			}
		}
	}
//	debugPrintPeople();
	return sum;
}

void initTmpBoard() {
	// 변수 초기화
	for (int i = 0; i < MAX_N; i++) {
		for (int j = 0; j < MAX_N; j++) {
			next_board[i][j] = 0;
		}
	}
}

void findMinimumSquare() {
	initTmpBoard();

	ssize = 1;
	for (int i = 1; i <= m; i++) {
		if (is_out[i]) continue;
		next_board[people[i].first][people[i].second] = 1;
	}

//	cout << "next_board 출력\n";
//	for (int i = 1; i <= n; i++) {
//		for (int j = 1; j <= n; j++) {
//			cout << next_board[i][j] << ' ';
//		}
//		cout << "\n\n";
//	}

	// 사이즈별로 만들기
	while ((++ssize) < n) {
		// r, c 시작좌표
		for (int r = 1; r <= n - ssize; r++) {
			for (int c = 1; c <= n - ssize; c++) {
				bool find_people = false, find_exit = false;
				// r, c를 시작으로 하고 size가 ssize인 정사각형 안에서
				// 사람과 출구가 있는지 확인
				for (int x = r; x < r + ssize; x++) {
					for (int y = c; y < c + ssize; y++) {
						if (next_board[x][y]) find_people = true;
						if (x == exit_coord.first && y == exit_coord.second) find_exit = true;
					}
				}
				if (find_people && find_exit) {
					sr = r;
					sc = c;
					return;
				}
//				cout << "size : " << ssize << "(" << r << ' ' << c << ")에서 못 찾음\n";
			}
		}
	}
	sr = 0;
	sc = 0;
	ssize = 0;
	return;
}

void rotateSquare() {
	initTmpBoard();

//	cout << "최소 정사각형 " << sr << " " << sc << " | 사이즈 : " << ssize << "\n";
//	cout << "보드 회전 before\n";
//	debugPrintBoard();

	// 내구도 옮기기
	for (int i = 0; i < ssize; i++) {
		for (int j = 0; j <= ssize; j++) {
			next_board[i][j] = board[sr + ssize - j - 1][sc + i];
		}
	}

//	cout << "내구도 옮긴 next_board\n";
//	for (int i=1; i<=n; i++){
//		for (int j=1; j<=n; j++){
//			cout << next_board[i][j] << ' ';
//		}
//		cout << "\n";
//	}

	// 내구도 감소
	for (int i = 0; i < ssize; i++) {
		for (int j = 0; j < ssize; j++) {
			int nx = sr + i;
			int ny = sc + j;
			board[nx][ny] = next_board[i][j] > 0 ? next_board[i][j] - 1 : 0;
		}
	}

	// 사람들 위치와 exit 위치 변경
	for (int i = 1; i <= m; i++) {
		if (is_out[i]) continue;
		int x = people[i].first;
		int y = people[i].second;
		if (sr <= x && x < sr + ssize && sc <= y && y < sc + ssize) {
//			int nx = sc + (ssize - (y - sc) - 1);
//			int ny = sr + (x - sr);
//			people[i] = {nx, ny};
			// Step 1. (sx, sy)를 (0, 0)으로 옮겨주는 변환을 진행합니다.
			int ox = x - sr, oy = y - sc;
			// Step 2. 변환된 상태에서는 회전 이후의 좌표가 (x, y) -> (y, square_n - x - 1)가 됩니다.
			int rx = oy, ry = ssize - ox - 1;
			// Step 3. 다시 (sx, sy)를 더해줍니다.
			people[i] = make_pair(rx + sr, ry + sc);
		}
	}

	if (sr <= exit_coord.first && exit_coord.first < sr + ssize && sc <= exit_coord.second &&
		exit_coord.second < sc + ssize) {
		int x = exit_coord.first;
		int y = exit_coord.second;
		int ox = x - sr, oy = y - sc;
		// Step 2. 변환된 상태에서는 회전 이후의 좌표가 (x, y) -> (y, square_n - x - 1)가 됩니다.
		int rx = oy, ry = ssize - ox - 1;
		// Step 3. 다시 (sx, sy)를 더해줍니다.
		exit_coord = make_pair(rx + sr, ry + sc);
//		exit_coord = {sc + exit_coord.second, sr + ssize - exit_coord.first - 1};
	}

//	debugPrintBoard();

//	debugPrintPeople();
//
//	cout << "exit 위치\n";
//	cout << exit_coord.first << " " << exit_coord.second << "\n";
}

void rotateBoard() {
	findMinimumSquare();
	rotateSquare();
}

int main() {
	// 입력
	cin >> n >> m >> k;

	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= n; j++) {
			cin >> board[i][j];
		}
	}

	for (int i = 1; i <= m; i++) {
		cin >> people[i].first >> people[i].second;
	}

	cin >> exit_coord.first >> exit_coord.second;

	// 연산
	for (int i = 1; i <= k; i++) {
//		cout << "============" << i << "초 이동 시작\n";
		int num = moveParticipants();
		answer += num;
		// 모든 참가자가 탈출했는지 확인
		if (checkAllOut()) {
			break;
		}
//		cout << num << "만큼 이동거리 추가\n";
		rotateBoard();
	}

	// 출력
	cout << answer << "\n";
	cout << exit_coord.first << ' ' << exit_coord.second << "\n";
}