//
// Created by KangMinji on 2024-04-13.
//

#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <vector>
using namespace std;

const int MAX_N = 50005; // 채점기 수
const int MAX_D = 305;	// 도메인 수
const int INF = 1987654321;

int n;

// 채점기
priority_queue<int, vector<int>, greater<>> waiting_grader;
int grading_domain[MAX_N];

// url
set<int> inreadyq[MAX_D];
int waiting_cnt = 0;
int start_time[MAX_D];
int gap[MAX_D];
int end_time[MAX_D];

map<string, int> domainToIdx;
int domain_cnt = 0; // domain_to_idx 변환 도와줌

struct Task {
	int priority;
	string domain;
	int id;
	int time;

	bool operator < (const Task &b) const {
		if (priority != b.priority) {
			return priority > b.priority;
		}
		return time > b.time;
	}
};
priority_queue<Task> waiting_pq[MAX_D];

// N개의 채점기가 있으며 초기 문제 url이 u0임을 뜻합니다.
void setupGrader() {
	string url;
	cin >> n >> url;

	// 채점기 세팅
	for (int i=1; i<=n; i++){
		waiting_grader.push(i);
	}

	// 도메인 분리
	int split_idx = 0;
	for (int i=0; i<url.length(); i++){
		if (url[i] == '/') {
			split_idx = i;
			break;
		}
	}
	string domain = url.substr(0, split_idx);
	int id = stoi(url.substr(split_idx + 1));

	// 처음 들어오는 도메인인 경우
	if (!domainToIdx[domain]) {
		domainToIdx[domain] = ++domain_cnt;
	}
	int domain_idx = domainToIdx[domain];

	// readyq에 number 넣어주기
	inreadyq[domain_idx].insert(id);

	Task task;
	task.priority = 1;
	task.domain = domain;
	task.id = id;
	task.time = 0;
	waiting_pq[domain_idx].push(task);

	waiting_cnt++;
}

// t초에 채점 우선순위가 p이며 url이 u인 문제에 대한 채점 요청이 들어왔음을 의미합니다.
void requestGrade() {
	int time, priority;
	string url;
	cin >> time >> priority >> url;

	// 도메인 분리
	int split_idx = 0;
	for (int i=0; i<url.length(); i++){
		if (url[i] == '/') {
			split_idx = i;
			break;
		}
	}
	string domain = url.substr(0, split_idx);
	int id = stoi(url.substr(split_idx + 1));

	// 처음 들어오는 도메인인 경우
	if (!domainToIdx[domain]) {
		domainToIdx[domain] = ++domain_cnt;
	}
	int domain_idx = domainToIdx[domain];

	// 채점 대기 큐에 있는 task 중 정확히 u와 일치하는 url이 존재할 경우
	if (inreadyq[domain_idx].find(id) != inreadyq[domain_idx].end()){
		return;
	}
	// 채점 대기 큐에 insert
	inreadyq[domain_idx].insert(id);

	Task task;
	task.priority = priority;
	task.domain = domain;
	task.id = id;
	task.time = time;
	waiting_pq[domain_idx].push(task);

	waiting_cnt++;
}

// t초에 채점 대기 큐에서 즉시 채점이 가능한 경우 중
// 우선순위가 가장 높은 채점 task를 골라 채점을 진행하게 됨을 뜻합니다.
void doGrade() {
	int time;
	cin >> time;

	// 쉬고 있는 채점기가 없다면 요청 무시
	if (waiting_grader.empty()) {
		return;
	}

	// 우선순위가 가장 높은 채점 task 찾기
	Task target;
	target.priority = INF;
	int target_domain_idx = 0;

	for (int i=1; i<=domain_cnt; i++){ // 도메인 개수만큼 돌기
		if (end_time[i] > time) continue;

		if (!waiting_pq[i].empty()) {
			Task curTask = waiting_pq[i].top();

			if (target < curTask) {
				target = curTask;
				target_domain_idx = i;
			}
		}
	}

	// 만약 우선순위에 해당하는 도메인이 있다면
	if (target_domain_idx) {
		int grader_idx = waiting_grader.top();
		waiting_grader.pop();
		grading_domain[grader_idx] = target_domain_idx;

		waiting_pq[target_domain_idx].pop();

		start_time[target_domain_idx] = time;
		end_time[target_domain_idx] = INF;

		inreadyq[target_domain_idx].erase(inreadyq[target_domain_idx].find(target.id));

		waiting_cnt--;
	}
}

//  t초에 id번 채점기가 진행하던 채점이 종료됨을 뜻합니다.
void endGrade() {
	int time, id;
	cin >> time >> id;

	// id번 채점기가 진행하던 채점이 없었다면 무시
	if (grading_domain[id] == 0){
		return;
	}
	// 다시 쉬게 해줌
	waiting_grader.push(id);

	int domain_idx = grading_domain[id];
	grading_domain[id] = 0;

	gap[domain_idx] = time - start_time[domain_idx];
	end_time[domain_idx] = start_time[domain_idx] + 3 * gap[domain_idx];
}

//시간 t에 채점 대기 큐에 있는 채점 task의 수를 출력해야합니다.
void printReadyTaskCnt() {
	int time;
	cin >> time;
	cout << waiting_cnt << "\n";
}

int main() {
	int q;
	cin >> q;

	int input;
	while(q--) {
		cin >> input;
		if (input == 100) {
			setupGrader();
		} else if (input == 200) {
			requestGrade();
		} else if (input == 300) {
			doGrade();
		} else if (input == 400) {
			endGrade();
		} else if (input == 500) {
			printReadyTaskCnt();
		}
	}
}