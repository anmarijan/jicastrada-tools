//---------------------------------------------------------------------------
#include <new>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <limits>
#include <cassert>
//---------------------------------------------------------------------------
#include "fratar.h"
//---------------------------------------------------------------------------
//! コンストラクタ
fratar::fratar(){
	nZone = 0;
	max_iter = 1000;
	min_error = 0.01;
	memset(err_msg,'\0',128);
}
//---------------------------------------------------------------------------
//! 初期化
bool fratar::init(int nZone) {
	this->nZone = nZone;
	try {
		data = std::make_unique<double[]>(nZone*nZone);
		Gp = std::make_unique<double[]>(nZone);
		Ap = std::make_unique<double[]>(nZone);
		Gf = std::make_unique<double[]>(nZone);
		Af = std::make_unique<double[]>(nZone);
		Fg = std::make_unique<double[]>(nZone);
		Fa = std::make_unique<double[]>(nZone);
		Lg = std::make_unique<double[]>(nZone);
		La = std::make_unique<double[]>(nZone);
		for(int i=0; i < nZone; i++) {
			Fg[i] = Fa[i] = Lg[i] = La[i] = 0;
			Gp[i] = Ap[i] = Gf[i] = Af[i] = 0;
			for (int j = 0; j < nZone; j++) {
				data[i * nZone + j] = 0;
			}
		}
	} catch (std::bad_alloc) {
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
//! データをゼロにする。
void fratar::clear(){
	for(int i=0; i < nZone; i++) {
		Fg[i] = Fa[i] = Lg[i] = La[i] = 0;
		Gp[i] = Ap[i] = Gf[i] = Af[i] = 0;
    	for(int j=0; j < nZone; j++) {
			data[nZone*i+j] = 0;
        }
	}
}
//---------------------------------------------------------------------------
//! デストラクタ
fratar::~fratar(){
}
int fratar::start() {
	double dg = 0;
	double da = 0;
	double pg, pa;
	for(int i=0; i < nZone; i++ ) {
		Gp[i] = Ap[i] = 0;
		dg += Gf[i];
		da += Af[i];
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			Gp[i] += data[i*nZone+j];
			Ap[j] += data[i*nZone+j];
		}
	}
	pg  = 0;
	pa = 0;
	for(int i=0; i < nZone; i++ ) {
		if( (Gf[i] != 0 && Gp[i] == 0) || (Af[i] != 0 && Ap[i] == 0) ) {
			 sprintf_s(err_msg,128,"%10.0lf%10.0lf%10.0lf%10.0lf\n",
									Gf[i],Gp[i],Af[i],Ap[i]);
			return (i+1);
		}
		if( Gp[i] > 0 ) Fg[i] = Gf[i] / Gp[i]; else Fg[i] = 0;
		if( Ap[i] > 0 ) Fa[i] = Af[i] / Ap[i]; else Fa[i] = 0;
		pg += Gp[i];
		pa += Ap[i];
	}
//	Ft = (dg+da)/(pg+pa);
	Ft = dg+da;
	return 0;
}
//---------------------------------------------------------------------------
//! 計算本体
/*! @return 収束したらtrue、失敗したらマイナス値 */
//---------------------------------------------------------------------------
void fratar::update() {

	for(int i=0; i < nZone; i++){
		if( Gp[i] > 0 ) Fg[i] = Gf[i] / Gp[i]; else Fg[i] = 0;
		if( Ap[i] > 0 ) Fa[i] = Af[i] / Ap[i]; else Fa[i] = 0;
	}

	for(int i=0; i < nZone; i++){
		double sum_g = 0;
		double sum_a = 0;
		for(int j=0; j < nZone; j++) {
			sum_g += data[i*nZone+j] * Fa[j];
			sum_a += data[j*nZone+i] * Fg[j];
		}
		if( sum_g == 0 ) Lg[i] = 0;
		else Lg[i] = Gp[i]/sum_g;
		if( sum_a == 0 ) La[i] = 0;
		else La[i] = Ap[i]/sum_a;
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			data[i*nZone+j] =
				data[i*nZone+j]*Fg[i]*Fa[j]*(Lg[i]+La[j])/2;
		}
	}

	for(int i=0; i < nZone; i++ ) {
		Gp[i] = Ap[i] = 0;
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			Gp[i] += data[i*nZone+j];
			Ap[j] += data[i*nZone+j];
		}
	}
}
// 平均成長率法
void fratar::update_avg() {

	for(int i=0; i < nZone; i++){
		if( Gp[i] > 0 ) Fg[i] = Gf[i] / Gp[i]; else Fg[i] = 0;
		if( Ap[i] > 0 ) Fa[i] = Af[i] / Ap[i]; else Fa[i] = 0;
	}

	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			data[i*nZone+j] =
				data[i*nZone+j]*(Fg[i]+Fa[j])/2;
		}
	}

	for(int i=0; i < nZone; i++ ) {
		Gp[i] = Ap[i] = 0;
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			Gp[i] += data[i*nZone+j];
			Ap[j] += data[i*nZone+j];
		}
	}
}
// デトロイト法
void fratar::update_det() {
	
	double pg, pa;
	pg = pa = 0;
	for(int i=0; i < nZone; i++){
		if( Gp[i] > 0 ) Fg[i] = Gf[i] / Gp[i]; else Fg[i] = 0;
		if( Ap[i] > 0 ) Fa[i] = Af[i] / Ap[i]; else Fa[i] = 0;
		pg += Gp[i];
		pa += Ap[i];
	}

	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			data[i*nZone+j] = data[i*nZone+j]*Fg[i]*Fa[j]/(Ft/(pg+pa));
		}
	}

	for(int i=0; i < nZone; i++ ) {
		Gp[i] = Ap[i] = 0;
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			Gp[i] += data[i*nZone+j];
			Ap[j] += data[i*nZone+j];
		}
	}
}

bool fratar::check() {
	bool flag = true;
	max_error = 0;
	for(int i=0; i< nZone; i++) {
		if( Fg[i] > 0 ) {
			if( fabs(Fg[i]-1) >= min_error ) flag = false;
			if( fabs(Fg[i]-1) > max_error ) {
				max_error = fabs(Fg[i]-1);
				gen_f = Gf[i]; gen_p = Gp[i];
				att_f = Af[i]; att_p = Ap[i];
				chk_zone=i+1;
			}
		}
		if( Fa[i] > 0 ) {
			if( fabs(Fa[i]-1) >= min_error ) flag = false;
			if( fabs(Fa[i]-1) > max_error ) {
				max_error = fabs(Fa[i]-1);
				gen_f = Gf[i]; gen_p = Gp[i];
				att_f = Af[i]; att_p = Ap[i];
				chk_zone=i+1;
			}
		}
	}
	return flag;
}

int fratar::calc(){
	for (int i = 0; i < nZone; i++) {
		Gp[i] = Ap[i] = 0;
	}
	for (int i = 0; i < nZone; i++) {
		for (int j = 0; j < nZone; j++) {
			Gp[i] += data[i * nZone + j];
			Ap[j] += data[i * nZone + j];
		}
	}
	for (int i = 0; i < nZone; i++) {
		if ((Gf[i] != 0 && Gp[i] == 0) || (Af[i] != 0 && Ap[i] == 0)) {
			sprintf_s(err_msg, 128, "%10.0lf%10.0lf%10.0lf%10.0lf\n",
				Gf[i], Gp[i], Af[i], Ap[i]);
			return (i + 1);
		}
	}
	for(count=0; count < max_iter; count++) {

		bool check = true;
		max_error = 0;
		for(int i=0; i< nZone; i++) {
			if( Gp[i] == 0 ) Fg[i] = 1;
			else {
				if (Gf[i] < 0 || Gp[i] < 0) {
					printf("M: %d %f  %f\n", i+1, Gf[i], Gp[i]);
				}
				Fg[i] = Gf[i] / Gp[i];
				assert(Fg[i] >= 0);
				if( fabs(Fg[i]-1) >= min_error ) check = false;
				if( fabs(Fg[i]-1) > max_error ) {
					max_error = fabs(Fg[i]-1);
					gen_f = Gf[i]; gen_p = Gp[i];
					att_f = Af[i]; att_p = Ap[i];
					chk_zone=i+1;
				}
			}
			if( Ap[i] == 0 ) Fa[i] = 1;
			else {
				Fa[i] = Af[i] / Ap[i];
				assert(Fa[i] >= 0);
				if( fabs(Fa[i]-1) >= min_error ) check = false;
				if( fabs(Fa[i]-1) > max_error ) {
					max_error = fabs(Fa[i]-1);
					gen_f = Gf[i]; gen_p = Gp[i];
					att_f = Af[i]; att_p = Ap[i];
					chk_zone=i+1;
				}
			}
		}
//        printf("max: %f\n", max_error);
		if( check == true ) return 0;  //収束する。ここで外に出る。

		for(int i=0; i < nZone; i++){
			double sum_g = 0;
			double sum_a = 0;
			for(int j=0; j < nZone; j++) {
				sum_g += data[i*nZone+j] * Fa[j];
				sum_a += data[j*nZone+i] * Fg[j];
			}
			if( sum_g == 0 ) Lg[i] = 0;
			else Lg[i] = Gp[i]/sum_g;
			if( sum_a == 0 ) La[i] = 0;
			else La[i] = Ap[i]/sum_a;
		}
		for(int i=0; i < nZone; i++) {
			for(int j=0; j < nZone; j++) {
				data[i*nZone+j] = data[i*nZone+j]*Fg[i]*Fa[j]*(Lg[i]+La[j])/2;
				// if( count == max_iter-1) printf("%f\n", Fg[i] * Fa[j] * (Lg[i] + La[j]) / 2);
				// if (count == max_iter - 1)printf("%d\t%d\t%f\t%f\t%f\t%f\n", i + 1, j + 1, Fg[i], Fa[j], Lg[i], La[j]);
				// assert( data[i * nZone + j] >= 0 );
				if (data[i * nZone + j] < 0) {
					printf("%d\t%d\t%f\t%f\t%f\t%f\n", i+1, j+1, Fg[i], Fa[j], Lg[i], La[j]);
					exit(1);
				}
			}
		}
		for (int i = 0; i < nZone; i++) {
			Gp[i] = Ap[i] = 0;
		}
		for (int i = 0; i < nZone; i++) {
			for (int j = 0; j < nZone; j++) {
				Gp[i] += data[i * nZone + j];
				Ap[j] += data[i * nZone + j];
			}
		}
	}
	double check_error = 0;
	int check_index = 0;
	double fc = 1;
	double pc = 1;
	for (int i = 0; i < nZone; i++) {
		if (fabs(Af[i] / Ap[i] - 1) > check_error) {
			check_error = fabs(Af[i] / Ap[i] - 1);
			fc = Af[i]; pc = Ap[i];
			check_index = i;
		}
		if (fabs(Gf[i] / Gp[i] - 1) > check_error) {
			check_error = fabs(Gf[i] / Gp[i] - 1);
			fc = Gf[i]; pc = Gp[i];
			check_index = i;
		}
	}
	sprintf_s(err_msg, 128, "Max error = %f (%f/%f) at Zone %d\n", check_error, fc, pc, check_index + 1);
#ifdef _DEBUG
	FILE* fp;
	errno_t err = fopen_s(&fp, "FratarError.txt", "wt");
	if (err == 0) {
		fprintf_s(fp, "ZONE\tGP\tGF\tAP\tAF\n");
		for (int i = 0; i < nZone; i++) {
			//double gerr = 0;
			//double aerr = 0;
			//if (Ap[i] > 0) aerr = Af[i] / Ap[i] - 1;
			//if (Gp[i] > 0) gerr = Gf[i] / Gp[i] - 1;
			fprintf_s(fp, "%d\t%.0f\t%.0f\t%.0f\t%.0f\n", i + 1, Gp[i], Gf[i], Ap[i], Af[i]);
		}
	}
#endif
	return (-2);
}
//---------------------------------------------------------------------------
//! ODデータを設定する
void fratar::set_od(int i, int j, double od){
	data[i*nZone+j] = od;
}
//---------------------------------------------------------------------------
//! ODデータを取得(計算後に取得する事で答えを得る)
double fratar::get_od(int i, int j ){
	return data[i*nZone+j];
}
//---------------------------------------------------------------------------
//! 現在発生量
double fratar::get_gen_f(int zone) {
	return Gf[zone];
}
//---------------------------------------------------------------------------
//! 現在集中量
double fratar::get_att_f(int zone) {
	return Af[zone];
}
//---------------------------------------------------------------------------
//! 将来の発生集中総量を各ゾーンに設定する。
void fratar::set_ga(int zone, double generation, double attraction)
{
    Gf[zone] = generation;
    Af[zone] = attraction;
}
// 発生量の合計=集中量の合計となるよう、平均をとる
void fratar::ga_adjust() {
	double g_sum = 0;
	double a_sum = 0;
	for (int i = 0; i < nZone; i++) {
		g_sum += Gf[i];
		a_sum += Af[i];
	}
	double g_factor = 0.5 * (g_sum + a_sum) / g_sum;
	double a_factor = 0.5 * (g_sum + a_sum) / a_sum;
	for (int i = 0; i < nZone; i++) {
		Gf[i] = Gf[i] * g_factor;
		Af[i] = Af[i] * a_factor;
	}
}
////////////////////////////////////////////////////////////////////////////////
//  エラー値(%ではない)： > 0
////////////////////////////////////////////////////////////////////////////////
bool fratar::set_error(double e){
    if( e <= 0 ) return false;
    min_error = e;
    return true;
}
////////////////////////////////////////////////////////////////////////////////
//  もとの発生集中量と、現在の量を比較して表示
////////////////////////////////////////////////////////////////////////////////
void fratar::print_ga(){
	double a, b;
	for(int i=0; i < nZone; i++ ) {
		Gp[i] = Ap[i] = 0;
	}
	for(int i=0; i < nZone; i++) {
		for(int j=0; j < nZone; j++) {
			Gp[i] += data[i*nZone+j];
			Ap[j] += data[i*nZone+j];
		}
	}
    for(int i=0; i < nZone; i++) {
		if( Gp[i] > 0 ) a = Gf[i] / Gp[i]; else a = 0;
		if( Ap[i] > 0 ) b = Af[i] / Ap[i]; else b = 0;
        printf("%10.0lf%10.0lf%10.5lf%10.0lf%10.0lf%10.5lf\n",
            Gf[i], Gp[i], a, Af[i], Ap[i], b);
    }
}
