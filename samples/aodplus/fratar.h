//---------------------------------------------------------------------------
#ifndef fratarH
#define fratarH
//---------------------------------------------------------------------------
//! フレーター法による収束計算
class fratar {
    int nZone;
    double **data;
    double *Gp;  //現在
    double *Ap;
    double *Gf;  //将来
    double *Af;
    double *Fg;
    double *Fa;
    double *Lg;
	double *La;
	double Ft;
private:
	char err_msg[128];
public:
	float max_error;  //!< 収束しなかった場合の最大誤差
	int max_iter;	  //!< 収束回数の最大値
	float min_error;
	int chk_zone;
	double gen_f, gen_p; // エラーが生じた場合の発生量
	double att_f, att_p; // エラーが生じた場合の集中量
	int count;		//!< 収束までに計算した回数
public:
	fratar();
	~fratar();
	bool init(int nZone);
	int start();
	void update();
	void update_avg();	//平均成長率法
	void update_det();	//デトロイト法
	bool check();
	int calc();
	void clear();
	bool set_error(float e);
	void set_od(int i, int j, float od);
	void set_ga(int zone, float generation, float attraction);
	float get_od(int i, int j);
	float get_gen_f(int zone);
	float get_att_f(int zone);
	void print_ga();
	char* msg() {return err_msg;}
};

#endif
