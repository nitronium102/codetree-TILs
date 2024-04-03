#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
typedef pair<int, int> ci;
typedef pair<int, pair<int, int>> pi;

const int MAX_N = 55;
const int MAX_P = 35;
const int ROUDOLF = -1;

int n, m, p, c, d;

ci santas[MAX_P];
ci roudolf;
int board[55][55];

/* 산타 상태 */
bool is_fail[MAX_P]; // 탈락 여부
int turns[MAX_P]; // 턴 수
int scores[MAX_P]; // 점수

int dx[8] = {1, 1, 1, 0, 0, -1, -1, -1};
int dy[8] = {1, 0, -1, 1, -1, 1, 0, -1};
int sdx[4] = {-1, 0, 1, 0};
int sdy[4] = {0, 1, 0, -1};

int calcDist(ci coord1, ci coord2){
    int r1 = coord1.first;
    int c1 = coord1.second;
    int r2 = coord2.first;
    int c2 = coord2.second;
    return (r1 - r2) * (r1 - r2) + (c1 - c2) * (c1 - c2);
}

bool isRange(ci coord) {
    int x = coord.first;
    int y = coord.second;
    return 1 <= x && x <= n && 1 <= y && y <= n;
}

void moveRoudolf(int turn) {
    // 1) 가장 가까운 산타 찾기
    ci target = {1e9, 1e9};
    int tidx = 0; // target 산타의 x, y, idx

    // 거리 적은 순, x 좌표 작은 순, y 좌표 작은 순
    pi minDist = {calcDist(roudolf, target), {-target.first, -target.second}};
    for (int i=1; i<=p; i++){
        if (is_fail[i]) continue;
        pi curDist = {calcDist(roudolf, santas[i]), {-santas[i].first, -santas[i].second}};

        if (curDist < minDist) {
            target = santas[i];
            minDist = curDist;
            tidx = i;
        }
    }

//    cout << "가장 가까운 산타 idx: " << tidx << " 좌표: " << target.first << " " << target.second << " | 거리 : " << mindist << "\n";
    
    // 2) 해당 산타에게로 한 칸 돌진
    if (tidx) {
        ci prevRodolf = roudolf;
        ci dir = {0,0};

        // 이동 방향 설정
        if (target.first < roudolf.first) dir.first--;
        else if (target.first > roudolf.first) dir.first++;
        if (target.second < roudolf.second) dir.second--;
        else if (target.second > target.first) dir.second++;

        // 루돌프 이동
        roudolf = dir;
        board[prevRodolf.first][prevRodolf.second] = 0;

        // 산타와 충동한 경우
        if (roudolf.first == target.first && roudolf.second == target.second) {
            ci startCoord = {santas[tidx].first + dir.first * c, santas[tidx].second + dir.second * c};
            ci lastCoord = startCoord;

            // 이동한 위치에 산타가 있을 경우 연쇄충돌 발생
            while(isRange(lastCoord) && board[lastCoord.first][lastCoord.second] > 0) {
                lastCoord.first += dir.first;
                lastCoord.second += dir.second;
            }

            // 마지막 위치부터 되짚어가며 산타 한 칸씩 이동
            while(!(startCoord.first == lastCoord.first && startCoord.second == lastCoord.second)) {
                ci before = {lastCoord.first - dir.first , lastCoord.second - dir.second};

                if (!isRange(before)) break;
                
                int idx = board[before.first][before.second];
                
                if (!isRange(lastCoord)) {
                    is_fail[idx] = true;
                } else {
                    board[lastCoord.first][lastCoord.second] = board[before.first][before.second];
                    santas[idx] = lastCoord;
                }
                lastCoord = before;
            }

            // 루돌프한테 부딪힌 산타 점수 반영
            turns[tidx] = turn + 1;
            scores[tidx] += c;
            santas[tidx] = startCoord;
            if (isRange(startCoord)) {
                board[startCoord.first][startCoord.second] = tidx;
            } else {
                is_fail[tidx] = true;
            }
        }

        board[roudolf.first][roudolf.second] = ROUDOLF;
    }
}

void moveSanta(int turn) {
    // 루돌프와 가장 가까운 방향으로 한 칸 이동
    for (int i=1; i<=p; i++) {
        if (is_fail[i] || turns[i] >= turn) continue;

        int minDist = calcDist(santas[i], roudolf);
        int moveDir = -1;

        for (int dir = 0; dir < 4; dir++) {
            ci next = {santas[i].first + dx[dir], santas[i].second + dy[dir]};

            if (!isRange(next) || board[next.first][next.second] > 0) continue;

            int dist = calcDist(next, roudolf);
            if (minDist > dist) {
                minDist = dist;
                moveDir = dir;
            }
        }

        // 루돌프와 가까워지게 이동할 수 있는 경우
        if (moveDir == -1) continue;

        ci next = {santas[i].first + dx[moveDir], santas[i].second + dy[moveDir]};

        // 산타가 루돌프한테 갖다 박은 경우
        if (next.first == roudolf.first && next.second == roudolf.second) {
            // 현재 산타 스턴
            turns[i] = turn + 1;

            ci roudolf_dir = {-dx[moveDir], -dy[moveDir]};
            ci startCoord = {next.first + roudolf_dir.first * d, 
                            next.second + roudolf_dir.second * d};
            ci lastCoord = startCoord;

            if (d == 1) {
                scores[i] += d;
            } else {
                // 산타 연쇄 이동
                    // 이동한 위치에 산타가 있을 경우 연쇄충돌 발생
                while(isRange(lastCoord) && board[lastCoord.first][lastCoord.second] > 0) {
                    lastCoord.first += roudolf_dir.first;
                    lastCoord.second += roudolf_dir.second;
                }

                // 마지막 위치부터 되짚어가며 산타 한 칸씩 이동
                while(!(startCoord.first == lastCoord.first && startCoord.second == lastCoord.second)) {
                    ci before = {lastCoord.first - roudolf_dir.first , lastCoord.second - roudolf_dir.second};

                    if (!isRange(before)) break;
                    
                    int idx = board[before.first][before.second];
                    
                    if (!isRange(lastCoord)) {
                        is_fail[idx] = true;
                    } else {
                        board[lastCoord.first][lastCoord.second] = board[before.first][before.second];
                        santas[idx] = lastCoord;
                    }
                    lastCoord = before;
                }
            }
            scores[i] += d;
            board[santas[i].first][santas[i].second] = 0;
            santas[i] = startCoord;
            if (isRange(startCoord)) {
                board[startCoord.first][startCoord.second] = i;
            } else {
                is_fail[i] = true;
            }
        } else {
            board[santas[i].first][santas[i].second] = 0;
            santas[i] = next;
            board[next.first][next.second] = i;
        }

    }
}

bool checkSantaAllFail() {
    for (int i=1; i<=p; i++){
        if (!is_fail[i]) return false;
    }
    return true;
}

int main() {

    // 입력
    cin >> n >> m >> p >> c >> d;
    cin >> roudolf.first >> roudolf.second;
    board[roudolf.first][roudolf.second] = ROUDOLF;

    for (int i=1; i<=p; i++) {
        int idx;
        cin >> idx;
        cin >> santas[idx].first >> santas[idx].second;
        board[santas[idx].first][santas[idx].second] = idx; // 각 산타의 위치 표시
    }

    // 연산
    for (int t=1; t<=m; t++) {

        // 1) 루돌프 이동
        moveRoudolf(t);

        // 2) 산타 이동
        moveSanta(t);

        // 3) 산타가 모두 게임에서 탈락했는지 확인
        // if (checkSantaAllFail()){
        //     break;
        // }

        // 4) 산타 점수 부여
        for (int i=1; i<=p; i++){
            if (!is_fail[i]){
                scores[i]++;
            }
        }
    }

    // 출력
    for (int i=1; i<=p; i++){
        cout << scores[i] << ' ';
    }
    return 0;
}