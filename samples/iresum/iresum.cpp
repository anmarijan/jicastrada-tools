/*
結果ファイル(IRE)のリンク情報を集計
*/
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <new>
#include <float.h>
//---------------------------------------------------------------------------
#include "StradaIRE.h"
#include "StradaPAR.h"
#include "tool.h"
#include "stgetopt.h"
//---------------------------------------------------------------------------
#include <string>
#include <set>

// 車種別に速度調整がされている場合がある。

int max_mode;
std::set<std::string> linklist;
double free_speed[10];

void usage() {
    puts("\nUsage: iresum ire_file [Options]");
	puts("\nOutput: UP  = Vehilce-km  Down= Passenger-hours");
	puts("\nire_file   File name of IRE file in JICA STRADA format");
	puts("");
	puts("Options");
	puts("fname.par  Parameter file for speed adjustment");
	puts("-e n       Excludes when evaluation flag == n");
	puts("-M n       Specify max mode to be displayed to n");
	puts("-u c...    User flag in c... is calculated");
	puts("-U c...    User flag in c... is NOT calculated");
	puts("-i n       n=0,1,2,3 means all, in-in, in-out, out-out");
	puts("-f fname   Use link list file for calculation");
	puts("-t         Output separator is TAB");
	puts("-9         Last speed = Vmax for QV=9");
	puts("-s         Use free speed");
	puts("-l         -s ignored when link type is not 0");
	puts("-v         Display the accepted options");
}

void read_free_speed() {

	FILE* fp;
	int num;
	double data;
	char buff[50];

	if ( (fp = fopen("fspeed.dat", "rt")) != 0) {

		int ret ;
		while( fgets(buff, 50, fp) ) {
			ret = sscanf(buff,"%d%lf\n", &num, &data);
			if( ret != 2 ) break;
			if( num > 0 && num <= 10 && data > 0 && data < 151 ) {
				free_speed[num-1] = data;
			}
			//fprintf(stderr, "%d %lf\n", num, data);
		}
		fclose(fp);
	} else {
		fprintf(stderr, "Cannnot find free speed data.\n");
	}
}

bool read_linkfile(const char* fname) {
	FILE* fp;
	char buff[256];
	if( (fp = fopen(fname, "rt")) == NULL ) {
		fprintf(stderr,"Cannot find file %s\n", fname);
		return false;
	}
	while (fgets(buff, 256, fp) != NULL){
		buff[strlen(buff)-1] = '\0';
		linklist.insert(buff);
	}
	fclose(fp);
	return true;
}

int main(int argc, char* argv[])
{
    if( argc < 2 ) {
		usage();
        exit(1);
    }
	for(int i=0; i < 10; i++ ) free_speed[i] = DBL_MAX;

    StradaIRE s_ire;
    StradaPAR s_par;
    char fname[260];
    IRELinkPtr link;

	double veh_hr[10];
	double pss_hr[10];
	double pcu_hr[10];

	double veh_km[10];
	double pss_km[10];
	double pcu_km[10];
	double PCU[10];
	double sp_adj[10];

//	double revenue[10];

	double pk, ph;

// オプション読み込み
	optarg = NULL;
	optind = 2 ;
	char ch;
// その他
	bool use_linklist = false;
	bool use_eval = false;
	bool eval_flag;
	bool use_user1 = false;
	bool excl_user1 = false;
	char user[260];
	bool disp = false;
	bool sp_reset = false;	//QV式が9である場合、最終速度が0でもVmaxを使う。
	bool tab_separate = false;
	bool use_maxspeed = false;
	bool excl_linktype = false; //Exclude linktype=1 and 2 for free speed
	int in_out = 0;	//合計、内々、内外、外外
	int num;
	int width = 8;
	char* p;
	bool check;

	max_mode = 10;
	for(int i=0; i < 10; i++) sp_adj[i] = 1.0;
	while( optind < argc ) {
		ch = get_opt(argc, argv, "e:u:U:M:i:f:w:vt9sl");
		if( ch == EOF ) {
			get_ext(optarg, fname);
			conv_upper(fname);
			if( strcmp(fname, "PAR") == 0 ) {
				try {
					s_par.Read(optarg);
					for(int i=0; i < 10; i++) {
						if( s_par.sp_modify[i] == 0 ) sp_adj[i] = 1.0;
						else sp_adj[i] = s_par.sp_modify[i];
					}
				} catch ( std::runtime_error& e ) {
					fprintf(stderr,"Cannot read %s.\n", optarg);
				}
			}
			continue;
		} else {
			switch (ch) {
			case 'M':
				max_mode = atoi(optarg);
				if (max_mode > 10 ) max_mode = 10;
				if (max_mode <= 0 ) {
					fprintf(stderr,"Wrong argument for -M.\n");
					exit(1);
				}
				break;
			case 'e':
				use_eval = true;
				if( optarg[0] == '0' && optarg[1] == '\0' ) eval_flag = false;
				else if( optarg[0] == '1' && optarg[1] == '\0' ) eval_flag = true;
				else use_eval = false;
				break;
			case 'f':
				if (read_linkfile(optarg) ) use_linklist = true;
				break;
			case 'u':
				use_user1 = true;
				excl_user1 = false;
				num = 0;
				while( optarg[num] != '\0' ) {
					user[num] = optarg[num];
					num++;
				}
				user[num] = '\0';
				break;
			case 'U':
				use_user1 = false;
				excl_user1 = true;
				num = 0;
				while( optarg[num] != '\0' ) {
					user[num] = optarg[num];
					num++;
				}
				user[num] = '\0';
				break;
			case 'i':
				in_out = atoi(optarg);
				if (in_out > 3 || in_out < 0 ) in_out = 0;
				break;
			case '9':
				sp_reset = true;
				break;
			case 't':
				tab_separate = true;
				break;
			case 'w':
				width = atoi(optarg);
				if (width < 6 ) width = 8;
				if (width > 16) width = 16;
				break;
			case 's':
				use_maxspeed = true;
				break;
			case 'l':
				excl_linktype = true;
				break;
			case 'v':
				disp = true;
				break;
			default:
			;
			}
		}
	}

//初期化
	if( use_maxspeed) read_free_speed();

	if( disp ) {
		if( use_eval)
			printf("Excludes links with evaluation flag %d\n", eval_flag);
		if( use_user1 )
			printf("Calculate for user flag 1 = %s\n", user);
	}
    try {
        s_ire.Read(argv[1]);
    } catch (std::runtime_error& e) {
		fprintf(stderr,"Cannot read file %s. \nException: %s ", argv[1], e.what() );
		fprintf(stderr,"Message: %s\n", s_ire.msg);
		exit(1);
	}

	for(int i=0; i < 10; i++ ) {
		veh_hr[i] = pss_hr[i] = pcu_hr[i] = 0;
		veh_km[i] = pss_km[i] = pcu_km[i] = 0;
		// revenue[i] = 0;
		if( s_ire.PCU[i] > 0 ) PCU[i] = 1.0/s_ire.PCU[i];
		else PCU[i] = 1.0;
	}

	for(int i=0; i < s_ire.nLink; i++) {
		link = s_ire.links[i];
		if( use_linklist) {
			if( linklist.find(link->name) == linklist.end() ) continue;
		}
		if( use_eval && link->evaluation != eval_flag ) continue;	//計算しない。
		check = true;
		if( use_user1 ) {
			check = false;
			p = user;
			while( *p ) {
				if( *p == link->aFlag1 ) {
					check = true;
					break;
				}
				p++;
			}
		} else if( excl_user1 ) {
			check = true;
			p = user;
			while( *p ) {
				if( *p == link->aFlag1 ) {
					check = false;
					break;
				}
				p++;
			}
		}
		if( !check ) continue;
		//evaluationの定義が逆
		for(int m =0; m < 10; m++) {

			if( sp_reset && link->QV == 9 ) {
				link->result[0].ltSp = link->result[1].ltSp = link->Vmax;
			}
			pk = link->pcu_km(m, in_out);
			pcu_km[m] += pk;
			veh_km[m] += PCU[m] * pk;
			pss_km[m] += PCU[m] * pk * s_ire.APC[m];
			if( use_maxspeed && free_speed[m] < DBL_MAX ) {
				if( excl_linktype && link->linktype > 0 ) ph = link->pcu_hr(m, in_out);
				else ph = link->pcu_hr_sp(m, free_speed[m], in_out);
			} else {
				ph = link->pcu_hr(m, in_out);
			}
			pcu_hr[m] += ph / sp_adj[m];
			veh_hr[m] += PCU[m] * ph / sp_adj[m];
			pss_hr[m] += PCU[m] * ph * s_ire.APC[m] / sp_adj[m];

		}
	}
	if( tab_separate) {
		for(int m=0; m < max_mode; m++) printf("%.0f\t", veh_km[m]);
		for(int m=0; m < max_mode-1; m++) printf("%.0f\t", pss_hr[m]);
		if( max_mode > 1 ) printf("%.0lf\n", pss_hr[max_mode-1]);
	} else {
		for(int m=0; m < max_mode; m++) printf("%*.0f", width, veh_km[m]);
		for(int m=0; m < max_mode; m++) printf("%*.0f", width, pss_hr[m]);
		puts("");
	}

    return 0;
}
//---------------------------------------------------------------------------
