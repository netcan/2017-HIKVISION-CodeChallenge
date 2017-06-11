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
#include <algorithm>
#include <queue>
#include <unordered_map>
using namespace std;

//- Begin Point
class Garage;
using Pos = int;

struct Point {
		friend class Garage;
		int i, j; // 行、列
		static int w, h; // 宽高<100
		bool connectStart = false, connectEnd = false; // 是否和起点/终点联通

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
	public:
		Point start, end;
		unordered_map<Pos, vector<Pos>> startToPark; // 存放从起点到所有停车位的路径
		unordered_map<Pos, vector<Pos>> parkToEnd; // 存放所有停车位到终点的路径

		bool checkMap();
		vector<Point> parks; // 所有停车位
		void loadData();
		void showData();
};
int Garage::w = 0;
int Garage::h = 0;

bool Garage::getPath(const Point &in) {
	int di[] = {0, -1, 0, 1};
	int dj[] = {1, 0, -1, 0};
	unordered_map<Pos, bool> visited; // 记录到达的点
	unordered_map<Pos, Pos> path; // 记录所有路径
	queue<Point> que;

	que.push(in); // 起点
	path[Pos(in)] = -1;
	visited[Pos(in)] = true;

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
		/*
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

//- Begin Car
class Car {
	private:
		int id; // ID
		int tIn, tOut; // 申请进入时间点，申请离开时间点
		int t, m; // 最大等待时间，质量
	public:
		Car(int id, int tIn, int tOut, int t, int m):
			id(id), tIn(tIn), tOut(tOut), t(t), m(m) {}
		void showData() {
			printf("%d %d %d %d %d\n", id, tIn, tOut, t, m);
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


int main(void) {
#ifdef netcanMachine
	freopen("cases/1.txt", "r", stdin);
#endif
	garage.loadData();
	carLoadData();

	garage.showData();
	garage.start.show();
	garage.end.show();
	carShowData();
	if(!garage.checkMap()) {
		puts("NO"); return 0;
	} else puts("YES");

	return 0;
}
