#ifndef StradaGADH
#define StradaGADH

#include <stdio.h>
#include <vector>
#include <string>
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
class GAData {
public:
    std::string name;
    std::vector<double> data;
    GAData();
    ~GAData();
};

class StradaGAD {
	std::string err_msg;
public:
	std::string title;	//タイトル
	bool csv ;	//csv形式か否か
	int nZone;
    int nData;
	std::vector<GAData> GA;
public:
	StradaGAD();
    ~StradaGAD();
    void init(int nZone, int nData);
    void clear();
    int Read(FILE* fp);
    void Read(char* fname);
    void Write(FILE* fp);
	void set_title(int n, const char* str);
	int get_data(int field, int zone);
	void set_data(int nd, int nz, double n);
	void calc_total();
	int field_no(const char* str);
	const char* msg() { return err_msg.c_str(); }
};

#endif
