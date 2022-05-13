//---------------------------------------------------------------------------
#ifndef StradaPARH
#define StradaPARH
//---------------------------------------------------------------------------
#include <stdio.h>
#include <memory>
#include <list>
#include <vector>
#include <string>
//---------------------------------------------------------------------------
extern char strada_error[256];
//---------------------------------------------------------------------------
struct str10{
	char name[11];
	str10();
	str10(const str10&);
	str10& operator=(const str10&);
};
struct CentInfo {
	char name[11];
	bool flag;
	CentInfo();
	CentInfo(const CentInfo&);
	CentInfo& operator=(const CentInfo&);
};
struct TurnControl {
	char FromNode[11];
	char ToNode[11];
	char TurnNode[11];
	float penalty;
	TurnControl();
	TurnControl(const char* s, const char* e, const char* n, float f);
	TurnControl(const TurnControl&);
	TurnControl& operator=(const TurnControl&);
};
struct LinkNode {
	char sNode[11];
	char eNode[11];
	char name[11];
	LinkNode();
	LinkNode(const char* s, const char* e, const char* n);
	LinkNode(const LinkNode&);
	LinkNode& operator=(const LinkNode&);
};
struct JHPC_Param {
	int mode;
	double K;
	double S;
	double a;
	double b;
	double c;
	JHPC_Param() : mode(0), K(1),S(1.471),a(0.94),b(0.94),c(0.86){};
};

struct PUB_Param {
	bool mode;
	double a;
	double b;
	double c;
	PUB_Param(): mode(false), a(1),b(-1),c(-1){};
};
struct AssRate {
	bool flag;
	short rate[10];
	AssRate();
};

struct QV_PARAM {
	int code;
	float v1, q1;
	float v2, q2;
	float v3, q3;
	float v4, q4;
	float delay;
	QV_PARAM();
};

class StradaPAR {
public:
	std::string name;
	int nZone;
	int nLink;
	int nNode;
	int nMode;

	char version;
	bool bSearchByMode;
	bool bCountByMode;
	bool bCalcOdDetail;
	bool bAnalyzeTurn;
	char ZoneImpedance;
	char ConvertType;
	char LinkCostType;
	bool bParam;      
	bool bTurnControl;
	bool bRouteInformation;
	bool bTripRank;
	bool bDivideCapacity;
	bool bPreLoad;		
	bool bMinRoute;
	char EquibriumType;	
	int MaxIteration;
	float Error;
	float Damp;
	int unit_hours;		
	int time_units;		
	short assign_rate[10];
	bool rate_by_mode[10];
	short arate_mode[10][10];
//////////////////////////////
//  Centroid
	std::vector<CentInfo> centroids;
//////////////////////////////
//  Time value
	int base_mode;
	float time_value[10];	// min/cost
	float sp_modify[10];
	float APC[10];	//
	float PCU[10];	//
//////////////////////////////
//  Volume-speed      (A)
	int nParam;
//	float Davf;		// for Davidson
//	float K_x;		// for BPR
//	float alpha;	// for BPR
	std::unique_ptr<QV_PARAM>	qvdata[99];
/////////////////////////////
//  Direction       (B)
	int nTurn;
	std::list<TurnControl>	Turns;
/////////////////////////////
//                  (C)
	int nDirection;
	std::list<str10> d_nodes;
////////////////////////////////////////////////
//                  (D)
	int nOdDetail;
	std::list<LinkNode> od_links;
////////////////////////////////////////////////
//                  (E)
	int nRouteInf;
	std::list<LinkNode> ri_links;
////////////////////////////////
//                  (F)
	int pub_mode ;
	JHPC_Param	jhpc[10];
	PUB_Param	pubp[9];
//////////////////////////////////////////////////
//                   (G)
	float trip_range[6];

//////////////////////////////////////////////////
public:
	StradaPAR();
	int Read(FILE* fp);
	void Read(const char* file_name);
	bool ReadV4(const char* fname, int& line_number);

	void Write(FILE* fp);
	void Write(const char* file_name);

	void init();

	char* msg() { return strada_error; };
	void zone_init(int n);
	void direction_init(int n);
	void turn_init(int n);
	void oddetail_init(int n);
	void routeinf_init(int n);
	int rename_nodes(const char* curname, const char* newname);
	void set_qv(int n, const QV_PARAM& qv);
	void remove_qv(int n);
	void print_data();

private:

	int read_od(FILE* fp, int &line_number);
	bool ReadV4(FILE* fp, int &line_number);
	bool read_odv4(FILE* fp, int& line_number);

	bool read_param(FILE* fp,  int c,int &line_number);     //A
	bool read_turn(FILE* fp,  int c,int &line_number);      //B
	bool read_direction(FILE* fp, int c, int &line_number) ; //C
	bool read_detail(FILE* fp, int c, int &line_number) ;   //D
	bool read_route(FILE* fp, int c, int &line_number);    //E
	bool read_divparam(FILE* fp, int c, int &line_number);  //F
	bool read_range(FILE* fp, int c, int &line_number);  //G

	void write_od(FILE* fp);
	void write_bpr(FILE* fp);
	void write_turn(FILE* fp);
	void write_detail(FILE* fp);
	void write_direc(FILE* fp);
	void write_route(FILE* fp);
	void write_divparam(FILE* fp);
	void write_range(FILE* fp);

};

#endif
