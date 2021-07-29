//---------------------------------------------------------------------------
#ifndef StradaTPAH
#define StradaTPAH
//---------------------------------------------------------------------------
#include <stdio.h>
//---------------------------------------------------------------------------
#include <vector>
#include <string>
#include "StradaCmn.h"
//---------------------------------------------------------------------------
class TermPenalty {
public:
	char node[11];
	char fline[11];
	char tline[11];
	int fpenalty;
	int tpenalty;

	TermPenalty();
	int Read(const char* str);
};

class StradaMode {
public:
	int 	id;
	bool	allow_transfer;
	float	base_fare;
	float	base_dist;
	float	excess;
	int	capacity;
	int	min_frequency;
	int	max_frequency;
	float	PCU;
	float	cWalkTime;
	float	cWaitTime;
	float	cLoadTime;
	float	cFareTime;
	float	cTravelTime;
	float	cTransTime;
	float	cCongTime;
	float	min_speed;
	float	max_speed;
public:
	StradaMode();
	int Read(FILE* fp);
	int Read( char* buff);
	void Write(FILE* fp);
	void setMode(StradaMode* sm);
	double board_fare(bool txf);
	double fare_cost(double sumdst);
};
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
class StradaTPA {
public:
	std::string	header;
	int 	nZone;
	int 	nMode;
	int 	nLine;
	int 	nPenalty;
	float	vf;	//The transit speed adjustment factor( = 1)
	float	wf; //The walk time adjustment factor( = 1)
	float	timevalue;	//(minutes/currency unit)
	char	type;	//IRE T(average) or F(peak)
	int		max_trans;
	int		max_path;
	int		path_limit;
	int 	mshar_type;	// 1 or 2
	int		max_headway_adj;
	int		min_load_limit;	//Minimum of OD
	bool	b_report[10];	//F:no, T:yes
	int		rate[10];
	int	  nRep1;
    std::vector<std::string> LineToLine;
	int	  nRep2;
    std::vector<std::string> Interline;
	int	  nRep3;
    std::vector<std::string> NodeToNode;
    std::vector<StradaMode> modes;
    std::vector<std::string> centroids;
    std::vector<TermPenalty> penalties;

public:
    StradaTPA();
	void clear();
public:
	const char* getCentroid(int i);
	int Read(FILE* fp);
    void Read(const char* fname);
private:
	int pack_data(char* buf, std::vector<std::string> &target);
};

#endif
