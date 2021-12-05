//---------------------------------------------------------------------------
#ifndef fratarH
#define fratarH
//---------------------------------------------------------------------------
#include <memory>
//---------------------------------------------------------------------------
class fratar {
    int nZone;
    std::unique_ptr<double[]> data;
	std::unique_ptr<double[]> Gp;  //現在
    std::unique_ptr<double[]> Ap;
    std::unique_ptr<double[]> Gf;  //将来
    std::unique_ptr<double[]> Af;
    std::unique_ptr<double[]> Fg;
    std::unique_ptr<double[]> Fa;
    std::unique_ptr<double[]> Lg;
	std::unique_ptr<double[]> La;
	double Ft;
private:
	char err_msg[128];
public:
	double max_error;  //!< 収束しなかった場合の最大誤差
	int max_iter;	  //!< 収束回数の最大値
	double min_error;
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
	bool set_error(double e);
	void set_od(int i, int j, double od);
	void set_ga(int zone, double generation, double attraction);
	void ga_adjust();
	double get_od(int i, int j);
	double get_gen_f(int zone);
	double get_att_f(int zone);
	void print_ga();
	char* msg() {return err_msg;}
};

#endif
