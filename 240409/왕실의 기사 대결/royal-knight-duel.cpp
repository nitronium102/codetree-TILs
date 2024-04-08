#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;
typedef pair<int, int> ci;
typedef pair<int, pair<int, int>> pi;

const int MAX_L = 50;
const int MAX_N = 35;

int l, n, q;

int board[MAX_L][MAX_L];
int row[MAX_N], col[MAX_N], height[MAX_N], width[MAX_N], stamina[MAX_N];
int initial_stamina[MAX_N];

int nrow[MAX_N], ncol[MAX_N];
int damage[MAX_N];
bool is_moved[MAX_N];

// 위쪽 오른쪽 아래쪽 왼쪽
int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};

bool isRange(int x) {
    if (nrow[x] < 1 || ncol[x] < 1 || nrow[x] + height[x] - 1 > l || ncol[x] + width[x] - 1 > l){
        return false;
    }
    return true;
}

bool playChess(int idx, int dir) {
    // 초기화
    for (int i=1; i<=n; i++){
        damage[i] = 0;
        is_moved[i] = false;
        nrow[i] = row[i];
        ncol[i] = col[i];
    }

    queue<int> q;

    q.push(idx);
    is_moved[idx] = true;

    while(!q.empty()) {
        int x = q.front();
        q.pop();

        // 상하좌우 중 하나로 이동
        nrow[x] += dx[dir];
        ncol[x] += dy[dir];

        // 기사가 이동하려는 방향의 끝에 벽이 있다면 모든 기사는 이동할 수 없음
        if (!isRange(x)) return false;

        // 이동하려는 위치에 기사가 있으면 연쇄적으로 한 칸 밀림
        for (int i=nrow[x]; i<=nrow[x] + height[x] - 1; i++) {
            for (int j=ncol[x]; j<=ncol[x] + width[x] - 1; j++) {
                if (board[i][j] == 1) {
                    damage[x]++;
                }
                if (board[i][j] == 2){
                    return false;
                }
            }
        }
        
        // 다른 기사와 충돌한 경우 그 기사도 이동
        for (int i=1; i<=n; i++) {
            if (is_moved[i] || stamina[i] <= 0) continue;
            if (row[i] > nrow[x] + height[x] - 1 || nrow[x] > row[i] + height[i] - 1) continue;
            if (col[i] > ncol[x] + width[x] - 1 || ncol[x] > col[i] + width[i] - 1) continue;
            is_moved[i] = true;
            q.push(i);
        }
    }
    damage[idx] = 0;
    return true;
}

void moveKnight(int idx, int dir) {
    // 체스판에서 사라진 기사에게 명령을 내리면 아무런 반응 없음
    if (stamina[idx] <= 0) return;

    // 이동이 가능한 경우 실제 위치와 체력 업데이트
    if (playChess(idx, dir)) {
        for (int i=1; i<=n; i++) {
            row[i] = nrow[i];
            col[i] = ncol[i];
            stamina[i] -= damage[i];
        }
    }
}

int main() {
    // 입력
    cin >> l >> n >> q;

    for (int i=1; i<=l; i++) {
        for (int j=1; j<=l; j++){
            cin >> board[i][j];
        }
    }

    for (int i=1; i<=n; i++){
        cin >> row[i] >> col[i] >> height[i] >> width[i] >> stamina[i];
        initial_stamina[i] = stamina[i];
    }

    int idx, dir;
    for (int i=1; i<=q; i++){
        cin >> idx >> dir;
        moveKnight(idx, dir);
    }

    // 출력
    long long ans = 0;
    for (int i=1; i<=n; i++){
        if (stamina[i] > 0) {
            ans += initial_stamina[i] - stamina[i];
        }
    }

    cout << ans;
    return 0;
}