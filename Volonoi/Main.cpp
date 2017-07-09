#include <deque>
#include <vector>
#include <algorithm>
#include <Siv3D.hpp>

const Array<std::pair<int,int>,4> d{
	{-1,0},
	{0,-1},
	{1,0},
	{0,1}
};

struct P{
	Point v;
	Color c;
};

constexpr int window_x=640,window_y=480;

double calcDistance(double y1,double x1,double y2,double x2) {
	return Math::Sqrt(Math::Square(y1-y2)+Math::Square(x1-x2));
}
inline double calcDistance(const Point& v,double y,double x) {
	return calcDistance(v.y,v.x,y,x);
}

Array<Array<double,window_y>,window_x> distance(window_y);
inline void initDistance() {
	std::for_each(distance.begin(),distance.end(),[](auto& o){
		o=Array<double,window_x>(window_x);
		std::fill(o.begin(),o.end(),INFINITY);
	});
}
constexpr bool inside(int y, int x) {
	return 0<=y && y<window_y && 0<=x && x<window_x;
}

Image display(window_x,window_y,Palette::White);

void updateDistance(const P& p) {
	std::deque<Point> stack;
	stack.emplace_back(p.v);
	distance[p.v.y][p.v.x]=0;

	while (stack.size()) {
		const Point& pop=stack.front();
		stack.pop_front();

		display[pop.y][pop.x]=p.c;

		for (auto d_:d) {
			const int ny = pop.y+d_.first,nx=pop.x+d_.second;
			const double dist = calcDistance(p.v,ny,nx);

			if (inside(ny, nx)&&(distance[ny][nx]>dist)) {
				distance[ny][nx]=dist;
				stack.emplace_back(nx,ny);
			}
		}
	}
}

inline Color generateRandomColor(int r=0,int g=0,int b=0) {
	return Color(r+Random(255-r),g+Random(255-g),b+Random(255-b));
}
inline Color generateRandomColorBlue() {
	return generateRandomColor(56,56,200);
}

std::vector<P> Points;
inline void addPoint(const P& p) {
	Points.emplace_back(p);
	updateDistance(p);
}

void Main() {
	Window::Resize(window_x,window_y);
	initDistance();

	int c=0;

	DynamicTexture texture=DynamicTexture(display);

	bool updated=false;

	while (System::Update()){
		if(Input::KeyEnter.clicked){
			c+=10;
		}else if(Input::AnyKeyClicked()){
			c++;
		}

		if(c){
			c--;
			addPoint({
				{Random(window_x-1),Random(window_y-1)},
				generateRandomColorBlue()
			});
			updated=true;
		}

		if(Input::MouseL.clicked){
			addPoint({
				Mouse::Pos(),
				generateRandomColorBlue()
			});
			updated=true;
		}
		if(updated){
			updated=false;
			texture.fill(display);
		}
		texture.draw();
	}
}