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
//---------------------------------------------------------------------------
#include <string>
#include <set>
// #include <boost/algorithm/string/trim.hpp>
#include <boost/program_options.hpp> 

// 車種別に速度調整がされている場合がある。

int max_mode;
std::set<std::string> linklist;
double free_speed[10];

void usage() {
    puts("\nUsage: iresum ire_file [Options]");
	puts("\nOutput: UP  = Vehilce-km  Down= Passenger-hours");
	puts("\nire_file   File name of IRE file in JICA STRADA format");
	puts("");
	/*
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
	*/
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
	using namespace boost::program_options;
    if( argc < 2 ) {
		usage();
        exit(1);
    }
	for(int i=0; i < 10; i++ ) free_speed[i] = DBL_MAX;

    StradaIRE s_ire;
    StradaPAR s_par;
    IRELinkPtr link;

	double veh_hr[10];
	double pss_hr[10];
	double pcu_hr[10];
	double veh_km[10];
	double pss_km[10];
	double pcu_km[10];
	double PCU[10];
	double sp_adj[10];
	double pk, ph;

// その他
	bool use_linklist = false;
	bool use_eval = false;
	bool eval_flag;
	bool use_user1 = false;
	bool excl_user1 = false;
	std::string user;
	bool disp = false;
	bool sp_reset = false;	//QV式が9である場合、最終速度が0でもVmaxを使う。
	bool tab_separate = false;
	bool use_maxspeed = false;
	bool excl_linktype = false; //Exclude linktype=1 and 2 for free speed
	int in_out = 0;	//合計、内々、内外、外外
	int width = 8;
	bool check;

	max_mode = 10;
	for(int i=0; i < 10; i++) sp_adj[i] = 1.0;

	options_description description("Options");
	description.add_options()
		("PAR,p", value<std::string>(), "Parameter file for speed adjustment")
		("exclude,e", value<int>(), "Excludes when evaluation flag == arg (0 or 1)")
		("max_mode,M", value<int>(), "Specify max mode to be displayed to arg")
		("include-userflag,u", value<std::string>(), "User flag in arg is calculated")
		("exclude-userflag,U", value<std::string>(), "User flag in arg is NOT calculated")
		("in-out,i", value<std::string>(), "arg=0,1,2,3 means all, in-in, in-out, out-out")
		("linklist,f", value<std::string>(), "Use link list file for calculation")
		("tab,t", "Output separator is TAB")
		("use-qv9,9", "Last speed = Vmax for QV=9")
		("freespeed,s", "Use free speed")
		("link-ignored,l", "-s parameter is ignored when link type is not 0");
	variables_map vm;
	store(parse_command_line(argc, argv, description), vm);
	notify(vm);

	if (vm.count("PAR")) {
		std::string par_file = vm["PAR"].as<std::string>();
		try {
			s_par.Read(par_file.c_str());
			for (int i = 0; i < 10; i++) {
				if (s_par.sp_modify[i] == 0) sp_adj[i] = 1.0;
				else sp_adj[i] = s_par.sp_modify[i];
			}
		}
		catch (std::runtime_error& ) {
			fprintf(stderr, "Cannot read %s.\n", par_file.c_str());
		}
	}
	if (vm.count("exclude")) {
		int n = vm["exclude"].as<int>();
		use_eval = true;
		if (n == 0) eval_flag = false;
		else if (n == 1) eval_flag = true;
		else use_eval = false;
	}
	if (vm.count("max_mode")) {
		max_mode = vm["max_mode"].as<int>();
		if (max_mode > 10) max_mode = 10;
		if (max_mode <= 0) {
			fprintf(stderr, "Wrong argument for -M.\n");
			exit(1);
		}
	}
	if (vm.count("include-userflag")) {
		use_user1 = true;
		excl_user1 = false;
		user = vm["include-userflag"].as<std::string>();
	}
	if (vm.count("exclude-userflag")) {
		use_user1 = false;
		excl_user1 = true;
		user = vm["exclude-userflag"].as<std::string>();
	}
	if (vm.count("in-out")) {
		in_out = vm["in-out"].as<int>();
		if (in_out > 3 || in_out < 0) in_out = 0;
	}
	if (vm.count("linklist")) {
		std::string name = vm["linklist"].as<std::string>();
		if (read_linkfile(name.c_str())) use_linklist = true;
	}
	if (vm.count("tab")) {
		tab_separate = true;
	}
	if (vm.count("use-qv9")) {
		sp_reset = true;
	}
	if (vm.count("freespeed")) {
		use_maxspeed = true;
	}
	if (vm.count("link-ignored")) {
		excl_linktype = true;
	}
	if (vm.count("disp")) {
		disp = true;
	}
	if (vm.count("width")) {
		width = vm["width"].as<int>();
		if (width < 6) width = 8;
		if (width > 16) width = 16;
	}
//初期化
	if( use_maxspeed) read_free_speed();

	if( disp ) {
		if( use_eval)
			printf("Excludes links with evaluation flag %d\n", eval_flag);
		if( use_user1 )
			printf("Calculate for user flag 1 = %s\n", user.c_str());
	}
    try {
        s_ire.Read(argv[1]);
    } catch (std::runtime_error& e) {
		fprintf(stderr,"Cannot read file %s. \nException: %s ", argv[1], e.what() );
		fprintf(stderr,"Message: %s\n", s_ire.msg.c_str());
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
			for (size_t i = 0; i < user.length(); i++) {
				if (user[i] == link->aFlag1) {
					check = true;
					break;
				}
			}
		} else if( excl_user1 ) {
			check = true;
			for (size_t i = 0; i < user.length(); i++) {
				if (user[i] == link->aFlag1) {
					check = false;
					break;
				}
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
