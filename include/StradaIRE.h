//---------------------------------------------------------------------------
#ifndef StradaIREH
#define StradaIREH
//---------------------------------------------------------------------------
#include <stdio.h>
#include "StradaCmn.h"
#include "StradaINT.h"
#include <vector>
#include <boost/intrusive_ptr.hpp>
//---------------------------------------------------------------------------
//! IREレコードの要素（１方向の結果）
//---------------------------------------------------------------------------
class OnewayResult {
public:
	float avSp;	//!<平均速度
	float ltSp;	//!<最終速度
	float VCR;	//!<混雑度
	float Vol;	//!<total
	float inVol[10];	//!<内内
	float thVol[10];	//!<内外
	float btVol[10];	//!<外外
	float ATL;	//!<avarage trip length
	float trVol[6];	//!<トリップ長分布別交通量

	OnewayResult();

	float mode_vol(int m) { return inVol[m] + thVol[m] + btVol[m]; }
	float mode_vol(int m, int t);	//t 内々=1、内外=2、外外=3
	float total(int t);
	float pass_vol(float pcu[], float apc[], int t = 0);
	float veh_vol(float pcu[], int = 0);

	int Read(char* str);
	int ReadCSV();
	void Write(FILE* fp);
	void WriteCSV(FILE* fp);
};
//------------------------------------------------------------------------------
//! IRE(結果ファイル)のレコード
//------------------------------------------------------------------------------
class IRELinkV2 : public SLinkV2 {
	int ref_counter;
public:
	OnewayResult result[2];

	IRELinkV2();
	IRELinkV2(SLinkV2& s);

	int Vol() { return (int)(result[0].Vol + result[1].Vol); }
	float pass_vol(float pcu[], float apc[], int t = 0);
	float veh_vol(float pcu[], int = 0);

	float pcu_km(int m, int t = 0);
	float pcu_hr(int m, int t = 0);
	double pcu_hr_sp(int m, double sp, int t=0);

    int Vol(int m) { return (int)(result[0].mode_vol(m)+result[1].mode_vol(m)); }
	float inVol(int m) { return result[0].inVol[m]+result[1].inVol[m]; }
	float thVol(int m) { return result[0].thVol[m]+result[1].thVol[m]; }
	float btVol(int m) { return result[0].btVol[m]+result[1].btVol[m]; }
	double VCR();
	double VCR(int d);

	int Read(char* str);
	void ReadCSV(char* str);
	void Write(FILE* fp);
	void WriteCSV(FILE* fp);

	friend void intrusive_ptr_add_ref(IRELinkV2* p);
	friend void intrusive_ptr_release(IRELinkV2* p);
};
using IRELinkPtr = boost::intrusive_ptr<IRELinkV2>;
//------------------------------------------------------------------------------
//! IRE(結果ファイル)を扱うクラス
//------------------------------------------------------------------------------
class StradaIRE {
public:
	bool csv;	//CSV形式か否か
	int nLink;			//!<リンク数
	int nNode;			//!<ノード数
	int nMode;			//!<モード数
	float Ranks[5];
	int coordinate;	//!<0:screen, 1:mathematical, 2:latitude,longitude
	float APC[10];		//!<モード別平均乗車人員数
	float PCU[10];		//!< PCU

    char comment[256];
    char msg[64];

	std::vector<IRELinkPtr> links;

	StradaIRE();
	~StradaIRE();

	IRELinkPtr getLink(int i){return links[i];}
	void clear();

	int Read(FILE* fp);
	void Read(const char* fname);
	void init(StradaINT& s_int);
	void resize(int nlink);
	void Write(FILE* fp);
    void Write(char* fname);
	void WriteAsINTV2(FILE* fp);
    void WriteAsINTV2(char* fname);
    void WriteMInfo(const char* fname, double dai, int vtype);
protected:
    double mix1, mix2, mix3, mix4, miy1, miy2, miy3, miy4 ;
    void calc_mixy(double xi, double yi, double xj, double yj, double width);
	void conv(int cd, int mergin);
};

#endif
