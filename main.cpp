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
#include <unordered_map>
using namespace std;

//- Begin Point
class Garage;

class Point {
	private:
		friend Garage;
		int i, j; // 行、列
		static int w, h; // 宽高<100
	public:
		Point() = default;
		Point(int i, int j): i(i), j(j) {}
		explicit Point(int pos) {
			i = pos / w;
			j = pos % w;
		}
		explicit operator int() {
			return i * w + j;
		}

		bool operator==(const Point& b) const {
			return i == b.i && j == b.j;
		}
		void show() {
			printf("(%d, %d)\n", i, j);
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
		friend class Point;
	public:
		Point start, end;
		vector<Point> parks;
		void loadData();
		void showData();
};
int Garage::w = 0;
int Garage::h = 0;

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


	return 0;
}
