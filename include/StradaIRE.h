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
// A part of a IRE record (oneway)
//---------------------------------------------------------------------------
class OnewayResult {
public:
	float avSp;	// average speed
	float ltSp;	// last speed
	float VCR;	// congestion rate
	float Vol;	// total
	float inVol[10];	// inside traffic
	float thVol[10];	// inside - outside
	float btVol[10];	// outside - outside
	float ATL;	        // avarage trip length
	float trVol[6];	    // distribution by trip length

	OnewayResult();

	float mode_vol(int m) { return inVol[m] + thVol[m] + btVol[m]; }
	float mode_vol(int m, int t);	//t =1(inside), 2(in-out), 3(out-out)
	float total(int t);
	float pass_vol(float pcu[], float apc[], int t = 0);
	float veh_vol(float pcu[], int = 0);

	int Read(const char* str);
	int ReadCSV(char* next_token);
	void Write(FILE* fp);
	void WriteCSV(FILE* fp);
};
//------------------------------------------------------------------------------
// A Record of IRE file
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

	int Read(const char* str);
	void ReadCSV(const char* str);
	void Write(FILE* fp);
	void WriteCSV(FILE* fp);

	friend void intrusive_ptr_add_ref(IRELinkV2* p);
	friend void intrusive_ptr_release(IRELinkV2* p);
};
using IRELinkPtr = boost::intrusive_ptr<IRELinkV2>;
//------------------------------------------------------------------------------
// IRE file class
//------------------------------------------------------------------------------
class StradaIRE {
public:
	bool csv;
	int nLink;
	int nNode;
	int nMode;
	float Ranks[5];
	int coordinate;	// 0:screen, 1:mathematical, 2:latitude,longitude
	float APC[10];	// average number of passengers by mode
	float PCU[10];  // PCU by mode

    std::string comment;
    std::string msg;

	std::vector<IRELinkPtr> links;

	StradaIRE();
	~StradaIRE();

	IRELinkPtr getLink(int i){return links[i];}
	void clear();

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
