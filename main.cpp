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
struct Point;

//- Begin Car
class Car {
	private:
		int id; // ID
		int tIn, tOut; // 申请入库时间点，申请出库时间点
		int t, m; // 最大等待时间，质量
		Pos park = -1; // 停车点
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
	for (size_t i = 0; i < cars.size(); ++i) {
		puts("=====================");
		cars[i].showData();
	}
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
		int schedule(int botNum, pair<int, int> &ret); // 核心算法，调度，return Z
		vector<Point> parks; // 所有停车位
		void loadData();
		void showData();
		void run();
		void showSolution(int botNum, pair<int, int> &ans);
};
int Garage::w = 0;
int Garage::h = 0;

void Garage::showSolution(int botNum, pair<int, int> &ans) {
	printf("%d %d %d\n", botNum, ans.first, ans.second);
	vector<Car> carInfo = cars;
	sort(carInfo.begin(), carInfo.end(), [](const Car &a , const Car &b) {
			return a.id < b.id;
		});
	for(const auto &car: carInfo) {
		printf("%d %s", car.id, car.park == -1?"yes\n":"no ");
		if(car.park != -1) {
			// 入库
			printf("%d %d  ", car.inBot, car.realTIn);
			start.show();
			printf(" ");
			for(auto pos: startToPark[car.park]) {
				Point(pos).show();
				printf(" ");
			}
			Point(car.park).show();
			printf(" ");

			// 出库
			printf("%d %d ", car.outBot, car.realTOut);
			Point(car.park).show();
			printf(" ");
			for(auto pos: parkToEnd[car.park]) {
				Point(pos).show();
				printf(" ");
			}
			end.show();
			puts("");
		}
	}
}

void Garage::run() {
	if(!checkMap()) {
		puts("NO");
		return;
	} else puts("YES");

	pair<int, int> ans;
	int bestBotNum = 0, Z = numeric_limits<int>::max();

	int tZ = 0; // minZ不变次数
	int upperBotNum = min(parks.size(), cars.size());
	for(int botNum = 0; tZ <= (upperBotNum >> 1) && botNum * a < Z && botNum <= upperBotNum; ++botNum) {
		int curZ = schedule(botNum, ans);
		if(Z > curZ) {
			Z = curZ;
			tZ = 0; // 归零
			bestBotNum = botNum;
		}
		++tZ;
		// printf("tz: %d\n", tZ);
	}

	// printf("minZ = %d\n", Z);
	schedule(bestBotNum, ans);

#ifdef netcanMachine
	// 打印算法结果
	showData();
	int q = 0;
	for(const auto &car: cars) if(car.park == -1) ++q;
	fprintf(stderr, "minZ:%d, botNum:%d, T:%d(%d+%d), W:%d, acc:%ld/%ld\n", Z, bestBotNum, ans.first, ans.first - p * q, p * q, ans.second, cars.size() - q, cars.size());
	// 打印结束
#endif

	showSolution(bestBotNum, ans);
}

int Garage::schedule(int botNum, pair<int, int> &ret) { // 核心算法，调度，return Z
	// using bot = pair<int, int>; // time, id
	struct bot {
		int time, id;
		enum class stat {
			GETIN, // 最终到达起点
			TAKEOFF // 最终到达终点
		} s;
		bot(int time, int id, stat s = stat::GETIN): time(time), id(id), s(s) {}
		bool operator>(const bot &b) const {
			return time > b.time;
		}
	};

	using car = pair<int, int>; // 取车time, id

	int T = 0; // 时间
	queue<bot> idleBot; // 入口处空闲机器人队列
	auto cmp = [this](const Pos &a, const Pos &b)->bool {
		int distA = startToPark[Pos(a)].size() + parkToEnd[Pos(a)].size();
		int distB = startToPark[Pos(b)].size() + parkToEnd[Pos(b)].size();
		if(distA == distB) return parkToEnd[Pos(a)].size() > parkToEnd[Pos(b)].size(); // 停车场到终点的距离越小越好
		else return distA > distB;
		return true;
	};

	priority_queue<Pos, vector<Pos>, decltype(cmp)> idlePark(cmp); // 空闲停车场
	priority_queue<bot, vector<bot>, greater<bot>> busyBot;
	priority_queue<car, vector<car>, greater<car>> takeAway; // 取车队列
	ret.first = ret.second = 0; // T, W

	for (int i = 0; i < botNum; ++i) idleBot.push(bot(0, i)); // 入口处空闲机器人队列
	for (size_t i = 0; i < parks.size(); ++i) idlePark.push(Pos(parks[i]));

	size_t carIdx = 0; // 处理到的汽车
	// 第一阶段，接车/取车
	while(carIdx < cars.size()) {
		Car &curCar = cars[carIdx];

		// 接车，只有第一阶段才有
		if(T > curCar.tIn + curCar.t) { // 放弃接车
			curCar.park = -1;
			++carIdx; // 处理下一辆车
			ret.first += p; // 罚时，T2
			continue;
		} else if(T >= curCar.tIn) { // 当前车辆
			if(! idleBot.empty() && !idlePark.empty()) { // 入口处有空闲机器人
				int botId = idleBot.front().id;
				Pos park = idlePark.top();
				if((unsigned)p <=
						b * (T - curCar.tIn + parkToEnd[park].size()) +
						k * curCar.m * (startToPark[park].size() + parkToEnd[park].size() + 2)) { // 剪枝，拒载
					curCar.park = -1;
					++carIdx; // 处理下一辆车
					ret.first += p; // 罚时，T2
					continue;
				}
				idleBot.pop();
				idlePark.pop();


				// printf("park: ");
				// Point(park).show();
				// puts("");

				busyBot.push(bot(T + (startToPark[park].size() + 1)*2 , botId)); // 来回路程*2
				takeAway.push(make_pair(curCar.tOut - (max(parkToEnd[park].size(), startToPark[park].size()) + 1), carIdx)); // 取车队列，提前取车

				ret.second += k * curCar.m * (startToPark[park].size() + parkToEnd[park].size() + 2); // W
				curCar.park = park;
				curCar.realTIn = T;
				curCar.inBot = botId;
				++carIdx; // 处理下一辆车
				continue;
			}
		}
		// 检查繁忙机器人
		if(!busyBot.empty() && T >= busyBot.top().time) {
			int botId = busyBot.top().id;
			if(busyBot.top().s == bot::stat::TAKEOFF) {
				busyBot.pop();
				busyBot.push(bot(T + startToEnd.size() + 1, botId, bot::stat::GETIN)); // 回到起点
			} else {
				busyBot.pop();
				idleBot.push(bot(0, botId)); // 回到起点
			}

			continue;
		}

		// 取车
		if(! takeAway.empty() && T >= takeAway.top().first) {
			if(! idleBot.empty()) { // 入口处有空闲机器人
				int curId = takeAway.top().second; takeAway.pop();
				int botId = idleBot.front().id; idleBot.pop();
				Car & tcar = cars[curId];
				Pos park = tcar.park;
				idlePark.push(park);

				tcar.realTOut = max<int>(T + startToPark[park].size() + 1, tcar.tOut);
				busyBot.push(bot(tcar.realTOut + parkToEnd[park].size() + 1 , botId, bot::stat::TAKEOFF)); // 运到出口

				tcar.outBot = botId;
				ret.first += b * ((tcar.realTIn - tcar.tIn) + (tcar.realTOut + parkToEnd[park].size() + 1 - tcar.tOut)); // T1
				continue;
			}
		}

		++T;
	}

	// 第二阶段：取车
	while(! takeAway.empty()) { // 取走剩余车辆
		// 检查繁忙机器人
		if(!busyBot.empty() && T >= busyBot.top().time) {
			int botId = busyBot.top().id;
			if(busyBot.top().s == bot::stat::TAKEOFF) {
				idleBot.push(bot(0, botId, bot::stat::TAKEOFF)); // 待在终点
			} else {
				idleBot.push(bot(0, botId)); // 回到起点
			}
			busyBot.pop();

			continue;
		}

		// 取车
		if(T >= takeAway.top().first) {
			if(! idleBot.empty()) { // 有空闲机器人
				int curId = takeAway.top().second; takeAway.pop();
				bot curBot = idleBot.front(); idleBot.pop();

				Car & tcar = cars[curId];
				Pos park = tcar.park;
				idlePark.push(park);

				if(curBot.s == bot::stat::TAKEOFF) { // 机器人在出口
					tcar.realTOut = max<int>(T + parkToEnd[park].size() + 1, tcar.tOut);
					busyBot.push(bot(tcar.realTOut + parkToEnd[park].size() + 1, curBot.id, bot::stat::TAKEOFF)); // 运到出口，然后回来
				} else { // 机器人在入口
					tcar.realTOut = max<int>(T + startToPark[park].size() + 1, tcar.tOut);
					busyBot.push(bot(tcar.realTOut + parkToEnd[park].size() + 1, curBot.id, bot::stat::TAKEOFF));
				}

				tcar.outBot = curBot.id;
				ret.first += b * ((tcar.realTIn - tcar.tIn) + (tcar.realTOut + parkToEnd[park].size() + 1 - tcar.tOut)); // T1
				continue;
			}
		}
		++T;
	}

	return a * botNum + ret.first + ret.second;
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
		if(! path.count(Pos(parks[i]))) return false;
		Pos t = path[Pos(parks[i])];
		do {
			if(in == start) {
				startToPark[Pos(parks[i])].push_back(t);
			}
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
	// 出口和入口各只有一个，且不重合，分布在地图边缘，泊车机器人从入口和出口都能到达每个车位。
	if( (start.i != 0 && start.i != h - 1 && start.j != 0 && start.j != w - 1) ||
			(end.i != 0 && end.i != h - 1 && end.j != 0 && end.j != w - 1)
	  ) return false;

	int ic = 0, ec = 0;
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			if(Map[i][j] == mapType::I) ++ic;
			else if(Map[i][j] == mapType::E) ++ec;
			if(ic > 1 || ec > 1) return false;
		}
	}

	// 每个车位有且只有一个入口, 即每个红色区域旁边有且只有一个白色区域；
	int di[] = {0, -1, 0, 1};
	int dj[] = {1, 0, -1, 0};
	for(const Point &p: parks) {
		int xc = 0;
		for(size_t k = 0; k < sizeof(di) / sizeof(int); ++k) {
			int ni = p.i + di[k], nj = p.j + dj[k];
			if(ni >= 0 && ni < h && nj >= 0 && nj < w && Map[ni][nj] == mapType::X)
				++xc;
			if(xc > 1) return false;
		}
	}

	// 泊车机器人必须能到达每个车位
	if(getPath(start) && getPath(end)) {
		startToEnd = getPath(start, end);
		// printf("sesize: %ld\n", startToEnd.size());
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
	fprintf(stderr, "能耗系数k:%d, ", k);
	fprintf(stderr, "罚时系数p:%d, ", p);
	fprintf(stderr, "机器人系数a:%d, ", a);
	fprintf(stderr, "等待系数b:%d\n", b);
	fprintf(stderr, "w:%d, h:%d\n", w, h);
	/*
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j)
			printf("%c ", static_cast<char>(Map[i][j]));
		puts("");
	}
	*/
}

Garage garage; // 只有一个车库
//- End Garage



int main(int argc, char **argv) {
#ifdef netcanMachine
	if(argc > 1) freopen(argv[1], "r", stdin);
	else freopen("cases/1.txt", "r", stdin);
#endif
	garage.loadData();
	carLoadData();
	garage.run();
	// garage.showData();
	// carShowData();


	return 0;
}
