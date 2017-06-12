/*************************************************************************
	> File Name: main.cpp
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2017-05-26 Fri 10:07:25 CST
 ************************************************************************/

#include <cstdio>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <queue>
#include <unordered_map>
using namespace std;

using Pos = int;
class Point;

//- Begin Car
class Car {
	private:
		int id; // ID
		int tIn, tOut; // 申请进入时间点，申请离开时间点
		int t, m; // 最大等待时间，质量
		Pos park; // 停车点
		int realTIn = -1, realTOut = -1, inBot = -1, outBot = -1;
	public:
		friend class Garage;
		Car(int id, int tIn, int tOut, int t, int m):
			id(id), tIn(tIn), tOut(tOut), t(t), m(m) {}
		void showData() {
			printf("id = %d tIn = %d/%d tOut = %d/%d t = %d m = %d\n", id, tIn, realTIn, tOut, realTOut, t, m);
			printf("park = %d inBot = %d outBot = %d\n", realTIn, realTOut, park);
		}
		bool operator<(const Car& b) const {
			if(tIn == b.tIn) return tOut < b.tOut;
			return tIn < b.tIn;
		}
};

vector<Car> cars; // 汽车

void carLoadData() {
	int N; // 车辆数目N
	scanf("%d", &N);
	int id, tIn, tOut, t, m;
	for (int i = 0; i < N; ++i) {
		scanf("%d%d%d%d%d", &id, &tIn, &tOut, &t, &m);
		cars.push_back(Car(id, tIn, tOut, t, m));
	}

	sort(cars.begin(), cars.end());
}

void carShowData() {
	printf("车辆数目N = %ld\n", cars.size());
	for (size_t i = 0; i < cars.size(); ++i)
		cars[i].showData();
}
//- EndCar

//- Begin Point

struct Point {
		friend class Garage;
		int i, j; // 行、列
		static int w, h; // 宽高<100

		Point() = default;
		Point(int i, int j): i(i), j(j) {}
		explicit Point(Pos pos) { // 数字转坐标
			i = pos / w;
			j = pos % w;
		}
		explicit operator Pos() const { // 坐标转数字
			return i * w + j;
		}

		bool operator==(const Point& b) const {
			return i == b.i && j == b.j;
		}
		void show() const {
			printf("(%d,%d)", i, j);
		}
};
int Point::w = 0;
int Point::h = 0;

//- End Point

//- Begin Garage
class Garage {
	private:
		int k, p, a, b; // 能耗系数k 罚时系数p 泊车机器人系数a 客户停车等待系数b
		static int w, h; // 宽高<100
		enum class mapType: char { // P表示车位，B表示障碍物，I表示入口，E表示出口，X表示过道
			P = 'P', B = 'B', I = 'I',
			E = 'E', X = 'X'
		};
		mapType Map[105][105];
		friend struct Point;
		bool getPath(const Point & in);
		vector<Pos> getPath(const Point &s, const Point &e); // 获得起点到终点的最短路
	public:
		Point start, end;
		unordered_map<Pos, vector<Pos>> startToPark; // 存放从起点到所有停车位的路径
		unordered_map<Pos, vector<Pos>> parkToEnd; // 存放所有停车位到终点的路径
		vector<Pos> startToEnd;

		bool checkMap(); // 判断地图是否合法
		void schedule(int botNum); // 调度算法(机器人数量)， Z=a*n+T+W, min Z
		vector<Point> parks; // 所有停车位
		void loadData();
		void showData();
};
int Garage::w = 0;
int Garage::h = 0;

void Garage::schedule(int botNum) { // 核心算法，调度
	using bot = pair<int, int>; // time, id
	using car = pair<int, int>; // 取车time, id

	int T = 0; // 时间
	queue<int> idleBot; // 入口处空闲机器人队列
	queue<Pos> idlePark; // 空闲停车场
	priority_queue<bot, vector<bot>, greater<bot>> busyBot;
	priority_queue<car, vector<bot>, greater<car>> takeAway; // 取车队列

	for (int i = 0; i < botNum; ++i) idleBot.push(i); // 入口处空闲机器人队列
	for (size_t i = 0; i < parks.size(); ++i) idlePark.push(Pos(parks[i]));

	size_t carIdx = 0; // 处理到的汽车
	while(carIdx < cars.size()) {
		Car &curCar = cars[carIdx];

		// 处理停车
		if(T >= curCar.tIn && T <= curCar.tIn + curCar.t) { // 当前车辆
			if(! idleBot.empty()) { // 入口处有空闲机器人
				int botId = idleBot.front(); idleBot.pop();
				Pos park = idlePark.front(); idlePark.pop();
				busyBot.push(make_pair(T + (startToPark[park].size() + 1)*2 , botId)); // 来回路程*2
				takeAway.push(make_pair(curCar.tOut, carIdx)); // 取车队列

				curCar.park = park;
				curCar.realTIn = T;
				curCar.inBot = botId;
				++carIdx; // 处理下一辆车
			}
		} else if(T > curCar.tIn + curCar.t) {
			++carIdx; // 处理下一辆车
		}

		// 检查繁忙机器人
		if(!busyBot.empty() && T >= busyBot.top().first) {
			int botId = busyBot.top().second; busyBot.pop();
			idleBot.push(botId); // 回到起点
		}

		// 取车
		if(! takeAway.empty() && T >= takeAway.top().first) {
			if(! idleBot.empty()) { // 入口处有空闲机器人
				int curId = takeAway.top().second; takeAway.pop();
				int botId = idleBot.front(); idleBot.pop();
				Car & tcar = cars[curId];
				Pos park = tcar.park;
				busyBot.push(make_pair(startToPark[park].size() + parkToEnd[park].size() + startToEnd.size() + 3 , botId)); // 运到出口，然后回来

				tcar.realTOut = T + startToPark[park].size() + parkToEnd[park].size() + 2;
				tcar.outBot = botId;
			}
		}

		++T;
	}

	while(! takeAway.empty()) { // 取走剩余车辆
		// 检查繁忙机器人
		if(!busyBot.empty() && T >= busyBot.top().first) {
			int botId = busyBot.top().second; busyBot.pop();
			idleBot.push(botId); // 回到起点
		}

		// 取车
		if(T >= takeAway.top().first) {
			if(! idleBot.empty()) { // 入口处有空闲机器人
				int curId = takeAway.top().second; takeAway.pop();
				int botId = idleBot.front(); idleBot.pop();
				Car & tcar = cars[curId];
				Pos park = tcar.park;
				busyBot.push(make_pair(startToPark[park].size() + parkToEnd[park].size() + startToEnd.size() + 3 , botId)); // 运到出口，然后回来

				tcar.realTOut = T + startToPark[park].size() + parkToEnd[park].size() + 2;
				tcar.outBot = botId;
			}
		}
		++T;
	}
}

vector<Pos> Garage::getPath(const Point &s, const Point &e) {
	int di[] = {0, -1, 0, 1};
	int dj[] = {1, 0, -1, 0};
	unordered_map<Pos, bool> visited; // 记录到达的点
	unordered_map<Pos, Pos> path; // 记录所有路径
	queue<Point> que;

	que.push(s); // 起点
	path[Pos(s)] = -1;
	visited[Pos(s)] = true;

	// BFS
	while(! que.empty()) {
		Point cur = que.front(); que.pop();
		if(cur == e) break; // 到达终点

		for(size_t k = 0; k < sizeof(di) / sizeof(int); ++k) {
			int ni = cur.i + di[k], nj = cur.j + dj[k];
			if(ni >= 0 && ni < h && nj >= 0 && nj < w &&
					Map[ni][nj] != mapType::B && Map[ni][nj] != mapType::P) { // 不越界 && 不是障碍物
				Point next(ni, nj);
				if(! visited[Pos(next)]) { // 状态转移
					visited[Pos(next)] = true;
					path[Pos(next)] = Pos(cur); // 保存路径
					que.push(next);
				}
			}
		}
	}

	vector<Pos> ret;
	// 获取路径
	Pos t = path[Pos(e)];
	do {
		ret.push_back(t);
		if(! path.count(t)) return {}; // 未找到路径
	} while( (t = path[t]) != -1 && path[t] != -1);
	reverse(ret.begin(), ret.end()); // 反转
	return ret;
}

bool Garage::getPath(const Point &in) {
	int di[] = {0, -1, 0, 1};
	int dj[] = {1, 0, -1, 0};
	unordered_map<Pos, bool> visited; // 记录到达的点
	unordered_map<Pos, Pos> path; // 记录所有路径
	queue<Point> que;

	que.push(in); // 起点
	path[Pos(in)] = -1;
	visited[Pos(in)] = true;

	// BFS
	while(! que.empty()) {
		Point cur = que.front(); que.pop();
		if(Map[cur.i][cur.j] == mapType::P) continue; // 到达停车位

		for(size_t k = 0; k < sizeof(di) / sizeof(int); ++k) {
			int ni = cur.i + di[k], nj = cur.j + dj[k];
			if(ni >= 0 && ni < h && nj >= 0 && nj < w &&
					Map[ni][nj] != mapType::B) { // 不越界 && 不是障碍物
				Point next(ni, nj);
				if(! visited[Pos(next)]) { // 状态转移
					visited[Pos(next)] = true;
					path[Pos(next)] = Pos(cur); // 保存路径
					que.push(next);
				}
			}
		}
	}

	// 获取路径
	for(size_t i = 0; i < parks.size(); ++i) {
		Pos t = path[Pos(parks[i])];
		do {
			if(in == start) startToPark[Pos(parks[i])].push_back(t);
			else parkToEnd[Pos(parks[i])].push_back(t);

			if(! path.count(t)) return false; // 未找到路径
		} while( (t = path[t]) != -1 && path[t] != -1);
		if(in == start)
			reverse(startToPark[Pos(parks[i])].begin(),
					startToPark[Pos(parks[i])].end());
	}

	return true;
}

bool Garage::checkMap() {
	if(getPath(start) && getPath(end)) {
		startToEnd = getPath(start, end);
		/*
		printf("======path========\n");
		start.show();
		printf("->");
		for(auto pos: startToEnd) {
			Point(pos).show();
			printf("->");
		}
		end.show();
		puts("");
		puts("start to parks");
		for(size_t i = 0; i < parks.size(); ++i) {
			start.show();
			for(const auto &t: startToPark[Pos(parks[i])]) {
				printf("->");
				Point(t).show();
			}
			printf("->");
			parks[i].show();
			puts("");
		}

		puts("parks to end");
		for(size_t i = 0; i < parks.size(); ++i) {
			parks[i].show();
			for(const auto &t: parkToEnd[Pos(parks[i])]) {
				printf("->");
				Point(t).show();
			}
			printf("->");
			end.show();
			puts("");
		}
		*/
		return true;
	}


	return false;
}

void Garage::loadData() {
	scanf("%d%d%d%d", &k, &p, &a, &b);
	scanf("%d%d", &h, &w); // 官方把h, w搞反了= =
	Point::h = h;
	Point::w = w;

	char c[2];
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			scanf("%s", c);
			Map[i][j] = static_cast<mapType>(c[0]);
			switch(Map[i][j]) {
				case mapType::I:
					start = Point(i, j);
					break;
				case mapType::E:
					end = Point(i, j);
					break;
				case mapType::P:
					parks.push_back(Point(i, j));
					break;
				default:
					break;
			}
		}
	}
}
void Garage::showData() {
	printf("能耗系数k = %d\n", k);
	printf("罚时系数p = %d\n", p);
	printf("机器人系数a = %d\n", a);
	printf("等待系数k = %d\n", b);
	printf("w = %d h = %d\n", w, h);
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j)
			printf("%c ", static_cast<char>(Map[i][j]));
		puts("");
	}
}

Garage garage; // 只有一个车库
//- End Garage



int main(void) {
#ifdef netcanMachine
	freopen("cases/1.txt", "r", stdin);
#endif
	garage.loadData();
	carLoadData();

	garage.showData();
	garage.start.show();
	garage.end.show();
	if(!garage.checkMap()) {
		puts("NO"); return 0;
	} else puts("YES");
	garage.schedule(1);

	carShowData();
	return 0;
}
