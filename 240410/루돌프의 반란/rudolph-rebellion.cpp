#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

typedef pair<int, int> ci;
typedef pair<pair<int, int>, pair<int, int>> pi;

const int MAX_N = 55;
const int MAX_P = 35;

int n, m, p, c, d;
struct Santa {
	int idx;
	int x, y;
	int turn = 0;
	bool is_out = false;
	int score = 0;
};

int board[MAX_N][MAX_N];
vector<Santa> santas;
ci roudolf;

int dx[8] = {-1, 0, 1, 0, -1, -1, 1, 1};
int dy[8] = {0, 1, 0, -1, 1, -1, 1, -1};

bool isRange(ci coord) {
	int x = coord.first;
	int y = coord.second;
	return x >= 1 && y >= 1 && x <= n && y <= n;
}

int calcDist(ci coord1, ci coord2) {
	int x1 = coord1.first;
	int y1 = coord1.second;
	int x2 = coord2.first;
	int y2 = coord2.second;

	return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

void debugPrintBoard() {
	for (int i=1; i<=n; i++){
		for (int j=1; j<=n; j++){
			cout << board[i][j] << ' ';
		}
		cout << "\n";
	}
	cout << "\n\n";
}

void debugPrintScore() {
	cout << "현재 산타 점수 현황\n";
	for (int i=1; i<santas.size(); i++) {
		cout << santas[i].idx << " : " << santas[i].score << '\n';
	}
}

bool santaCmp(Santa s1, Santa s2){
	return s1.idx < s2.idx;
}

bool cmp(pi s1, pi s2) {
	// 1) 거리가 적은 순
	if (s1.first.first != s2.first.first){
		return s1.first.first < s2.first.first;
	}
	// 2) x좌표가 큰 순
	if (s1.second.first != s2.second.first){
		return s1.second.first > s2.second.first;
	}
	// 3) y좌표가 큰 순
	return s1.second.second > s2.second.second;
}

int getRDir(ci roudolf, int x, int y) {
	int dir = -1;

	int dist = calcDist(roudolf, {x, y});
	for (int i=0; i<8; i++){
		int nx = roudolf.first + dx[i];
		int ny = roudolf.second + dy[i];

		if (!isRange({nx, ny})) continue;
		int curDist = calcDist({nx, ny}, {x, y});
		if (dist > curDist) {
			dir = i;
			dist = curDist;
		}
	}
	return dir;
}

void pushSanta(int tidx, int x, int y, int dir) {
	// 해당 칸에 산타 없는 경우(더 이상의 상호작용 발생 x)
	if (board[x][y] == 0) {
		santas[tidx].x = x;
		santas[tidx].y = y;
		board[x][y] = tidx;
		return;
	}
	int nextIdx = board[x][y];
	int nx = x + dx[dir];
	int ny = y + dy[dir];

	santas[tidx].x = x;
	santas[tidx].y = y;
	board[x][y] = tidx;

	if (!isRange({nx, ny})) {
		santas[nextIdx].is_out = true;
		return;
	}
	pushSanta(nextIdx, nx, ny, dir);
}

void moveRoudolf(int cur_turn) {
	vector<pi> santaDists; // dist, santa idx, santa x, santa y

	// 1) 가장 가까운 산타 찾기
	for (Santa santa : santas) {
		// 산타가 탈락한 경우 패스
		if (santa.is_out) continue;
		int curDist = calcDist(roudolf, {santa.x, santa.y});

		santaDists.push_back({{curDist, santa.idx}, {santa.x, santa.y}});
	}
	sort(santaDists.begin(), santaDists.end(), cmp);

	// 2) 해당 산타에게 돌진할 방향 정하기
	pi target = santaDists[0];
	int rdir = getRDir(roudolf, target.second.first, target.second.second);
//	cout << "루돌프 타겟 산타 idx: " << target.first.second << " 좌표: " << target.second.first << ' ' << target.second.second <<"\n";

	board[roudolf.first][roudolf.second] = 0;
	roudolf.first += dx[rdir];
	roudolf.second += dy[rdir];
	board[roudolf.first][roudolf.second] = -1;
//	cout << "돌진할 좌표 : " << roudolf.first << ", " << roudolf.second << "\n";

	// 3) 돌진 후 충돌 확인
	if (roudolf.first == target.second.first && roudolf.second == target.second.second) {
		// 해당 산타 점수 +c
		int idx = target.first.second;
		santas[idx].score += c;
		// 해당 산타 기절
		santas[idx].turn = cur_turn + 1;

		// 해당 산타 루돌프 방향으로 +c칸
		int nx = roudolf.first + dx[rdir]*c;
		int ny = roudolf.second + dy[rdir]*c;

		// -- 만약 게임판 밖이면 탈락
		if (!isRange({nx, ny})){
			santas[idx].is_out = true;
		}
		// -- 다른 산타 있으면 상호작용
		else {
			pushSanta(idx, nx, ny, rdir);
		}
	}
}

int findSDir(Santa santa) {
	int dir = -1;
	int dist = calcDist(roudolf, {santa.x, santa.y});

	for (int i=0; i<4; i++) {
		int nx = santa.x + dx[i];
		int ny = santa.y + dy[i];

		if (!isRange({nx, ny}) || board[nx][ny] > 0) continue;
		int curDist = calcDist(roudolf, {nx, ny});

		if (curDist < dist){
			dist = curDist;
			dir = i;
		}
	}
	return dir;
}

int reverseDir(int dir) {
	return (dir + 2) % 4;
}

void moveSanta(int cur_turn) {
	for (int i=1; i<santas.size(); i++) {
		Santa santa = santas[i];
		if (santa.is_out || santa.turn >= cur_turn) continue;

		// 루돌프에게 가까워지는 방향 찾기
		int sdir = findSDir(santa);
		if (sdir == -1) continue;

		// 산타 이동
		santa.x += dx[sdir];
		santa.y += dy[sdir];

//		cout << santa.idx << "번 산타 : (" << santa.x << ", " << santa.y << ")로 이동\n";

		// 루돌프한테 충돌하는지 확인
		if (santa.x == roudolf.first && santa.y == roudolf.second) {
			// 기절
			santas[i].turn = cur_turn + 1;
			// 산타 점수 처리
			santas[i].score += d;
			board[santas[i].x][santas[i].y] = 0;

			// 반대 방향 d칸 이동
			int ndir = reverseDir(sdir);
			int nx = santa.x + dx[ndir] * d;
			int ny = santa.y + dy[ndir] * d;
			// --게임판 밖이면 탈락
			if (!isRange({nx, ny})) {
				santas[i].is_out = true;
			}
			// --산타 있으면 상호작용
			else {
				pushSanta(santa.idx, nx, ny, ndir);
			}
		} else {
			board[santas[i].x][santas[i].y] = 0;
			board[santa.x][santa.y] = i;
			santas[i].x = santa.x;
			santas[i].y = santa.y;
		}
	}
}

int main() {
	// 초기화
	Santa isanta;
	isanta.idx = -1;
	isanta.x = -1; isanta.y = -1;
	santas.push_back(isanta);

	// 입력
	cin >> n >> m >> p >> c >> d;
	cin >> roudolf.first >> roudolf.second;
	board[roudolf.first][roudolf.second] = -1;

	int idx, sr, sc;
	for (int i=1; i<=p; i++) {
		cin >> idx >> sr >> sc;
		Santa santa;
		santa.idx = idx;
		santa.x = sr;
		santa.y = sc;
		santas.push_back(santa);

		board[santa.x][santa.y] = idx;
	}
	sort(santas.begin(), santas.end(), santaCmp);

//	cout << "산타 좌표 디버깅\n";
//	for (Santa santa : santas) {
//		cout << santa.idx << " " << santa.x << " " << santa.y << "\n";
//	}
	// 연산
	for(int turn = 1; turn <=m; turn++){
//		cout << "$$$=================== 현재 턴 : " << turn << "\n";

//		cout << "루돌프 이동 함수 시작\n";
		moveRoudolf(turn);
//		cout << "루돌프 이동 끝\n";
//		debugPrintBoard();

//		cout << "산타 이동 시작\n";
		moveSanta(turn);
//		cout << "산타 이동 끝\n";
//		debugPrintBoard();

		// 턴 종료 후 살아남은 산타들 점수 증가
		for (int i=1; i<santas.size(); i++){
			if (santas[i].is_out) continue;
			santas[i].score++;
		}
//		debugPrintScore();
	}

	// 출력
	for (int i=1; i<santas.size(); i++) {
		cout << santas[i].score << ' ';
	}
}