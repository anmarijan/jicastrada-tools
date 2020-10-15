//---------------------------------------------------------------------------
#ifndef StradaZXYH
#define StradaZXYH
//---------------------------------------------------------------------------
#include "StradaGAD.h"
//---------------------------------------------------------------------------
struct XYPoint {
	double x;
	double y;
};

class Border {
public:
	double x, y ;
	int nPoint;
	XYPoint* pt;
	Border() : nPoint(0), pt(NULL) {} ;
	~Border();
	void init(int n);
	void set_center();
	bool inside(double cx, double cy);
};
class StradaZXY {
public:
	int version;
	int nZone;
	int nBorder;	//境界線データの数がゾーン数に等しいとは限らない
	int scale;
	int coordinate;
	bool csv;
	char filename[21];
	double min_x ;
	double min_y ;
	double max_x ;
	double max_y ;
	double width ;
	double height;
public:
	XYPoint* zones;
	Border* borders;
	//	ZoneXY* getZone(int i);
	StradaZXY();
	~StradaZXY();
	void init(int nZone, int nBorder);
	void Write(const char* fname);
	int Read(FILE* fp);
	void Read(char* fname);
	void conv(int cd, int mergin);	//数学座標<-->スクリーン座標
	void circulate();//数学座標の時、閉曲線を全て時計回りにする
	void clear();
	void calc_boundary();
	void WriteMInfo(char* fname);
    void WriteMInfo(char* fname, StradaGAD& gad);
};

#endif
