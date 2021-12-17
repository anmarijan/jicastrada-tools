//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <new>
//---------------------------------------------------------------------------
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <unordered_map>
#include <boost/program_options.hpp>
//---------------------------------------------------------------------------
#include "tool.h"
#include "StradaIRE.h"
//---------------------------------------------------------------------------
int mode;
int max_mode;
int way;         //0:all, 1:both, 2:peak, 3:1, 4:2, 5: total
double conv[10];
bool kms;

double max_vol1[10];
double max_vol2[10];
double max_total[10];

struct RoadList {
    std::string name;
    std::string remark;
};

std::vector<RoadList> link_list;

void read_linkfile(const char* fname, int &count) {
    int total = 0;
    int pos;
	std::ifstream ifs(fname, std::ios_base::in);
	if (!ifs) {
		fprintf(stderr, "Cannot open file %s.\n", fname);
		exit(1);
	}
	std::string buff;
	while (std::getline(ifs, buff)) {
		pos = 0;
		while (isspace(buff[pos])) pos++;
		if (buff[pos] == '#' || buff[pos] == '\0') continue;
		total++;
	}
	ifs.clear();
	ifs.seekg(0);
	try {
		link_list.resize(total);
	}
	catch (std::bad_alloc& ) {
		printf("Cannot allocate memory for %s.\n", fname);
		printf("The total number of lines is %d.\n", total);
		exit(1);
	}
	int i = 0;
	while (std::getline(ifs, buff)) {
		pos = 0;
		while (isspace(buff[pos])) pos++;
		if (buff[pos] == '#' || buff[pos] == '\0') continue;
		// trim from StradaCmn.h
		//std::string str = trim(buff);
		std::string x;
		std::string y;
		std::stringstream line(buff);
		line >> x >> y;
		link_list[i].name = x;
		//str = buff.substr(10, 20);
		link_list[i].remark = y;
		i++;
	}
	count = total;
	ifs.close();
}
// sw = way
void print_value(int sw, double vol1, double vol2) {
	double vol;
	switch(sw) {
		case 0:
			vol = vol1 + vol2;
			printf("%8.0f%8.0f%8.0f", vol, vol1, vol2);
			break;
		case 1:
			printf("%8.0f%8.0f", vol1, vol2);
			break;
		case 2:
			vol = (vol1 > vol2 ) ? vol1 : vol2;
			printf("%8.0f", vol);
			break;
		case 3:
			printf("%8.0f", vol1);
			break;
		case 4:
			printf("%8.0f", vol2);
			break;
		case 5:
			vol = vol1 + vol2;
			printf("%8.0f", vol);
			break;
	}
}

void print_header(int sw) {

	switch(sw) {
		case 0:
			printf("     Both    A->B    B->A");
			break;
		case 1:
			printf("     A->B    B->A");
			break;
		default:
			break;
	}
}

void display_link(IRELinkPtr link ) {

	double vol1 = 0;
	double vol2 = 0;

	if( max_mode > 0 ) {
		for(int i=0; i < max_mode; i++) {
			vol1 = link->result[0].mode_vol(i) * conv[i];
			vol2 = link->result[1].mode_vol(i) * conv[i];
			if( kms ) {
				vol1 *= link->length;
				vol2 *= link->length;
			}
			print_value(way, vol1, vol2);
			if( max_vol1[i] < vol1) max_vol1[i] = vol1;
			if( max_vol2[i] < vol2) max_vol2[i] = vol2;
			if( max_total[i] < vol1+vol2) max_total[i] = vol1 + vol2;
		}
	} else {
		if (mode > 0 ) {
			vol1 = link->result[0].mode_vol(mode-1) * conv[mode-1];
			vol2 = link->result[1].mode_vol(mode-1) * conv[mode-1];
			if( kms ) {
				vol1 *= link->length;
				vol2 *= link->length;
			}
			print_value(way, vol1, vol2);
		} else if (mode == 0 ) {
			vol1 = vol2 = 0;
			for(int i=0; i < 10; i++) {
				vol1 += link->result[0].mode_vol(i) * conv[i];
				vol2 += link->result[1].mode_vol(i) * conv[i];
			}
			if( kms ) {
				vol1 *= link->length;
				vol2 *= link->length;
			}
			print_value(way, vol1, vol2);
		}
		if( max_vol1[0] < vol1) max_vol1[0] = vol1;
		if( max_vol2[0] < vol2) max_vol2[0] = vol2;
		if( max_total[0] < vol1+vol2) max_total[0] = vol1 + vol2;
	}
}

void usage() {
    puts("\nUsage: IRECutter ire_file {-f list_file | -l link_name} [Options]");
	puts("");
	puts("ire_file   File name of IRE file in JICA STRADA format");
	puts("list_file  File name of a text file of the list of link names");
	puts("            0- 9: Name of the link to be displayed");
	puts("           10-30: Comment");
	puts("link_name  Name of a link to be displayed");
	puts("");
	puts("Options");
	puts("-u xxx   Specity the unit, xxx = { pcu | veh | pax }");
	puts("-k       Multiplied by link length");
	puts("-m #     Specify the mode number (# = a number from 1 to 10)");
	puts("         If # is 0, the total of all modes is displayed (default)");
	puts("-M #     Specify the number of modes to be displayed form mode 1");
	puts("         (# = a number from 1 to 10)");
	puts("         Option -m is ignored if -M is specified.");
	puts("-d xxx   Specify the direciton, xxx = {all | both | peak | 1 | 2 | total}");
	puts("\t\tall  : total, start->end, end->start");
	puts("\t\tboth : start->end, end->start");
	puts("\t\tpeak : peak direction only");
	puts("\t\t1    : start->end only");
	puts("\t\t2    : end->start only");
	puts("\t\ttotal: total");
	puts("-L       Display length");
	puts("-s       Display speed");
	puts("-H       Display header");
	puts("-p       Use prefix");
	puts("-v       Display the accepted options");
}

int main(int argc, char* argv[])
{
	namespace po = boost::program_options;
    if( argc < 4 || argv[2][0] != '-' || (argv[2][1] != 'f' && argv[2][1] != 'l') ) {
		usage();
        exit(1);
    }
	std::string ire_name;
    StradaIRE s_ire;
    IRELinkPtr link;
    std::unordered_map<std::string, IRELinkPtr> linkhash;
    int nRoad;
    int type ;	//0:PCU, 1:Vehicles, 2:Pax
    const char* type_str[] = {"pcu", "veh","pax"};
    const char* way_str[] = {"All", "Both", "Peak", "ANode->BNode", "BNode->ANode", "Total"};
    bool header = false;
    bool disp   = false;

	po::options_description description("Options");
	description.add_options()
		("ire-file",po::value<std::string>(),"IRE file")
		("file,f",po::value<std::string>(), "Name of the link list file")
		("link,l",po::value<std::string>(), "Link name")
		("unit,u", po::value<std::string>(), "Specity the unit { pcu | veh | pax }")
		("kms,k", "Multiplied by link length")
		("mode,m", po::value<int>()->default_value(0), "Specify the mode number")
		("max_mode,M", po::value<int>(), "Specify the number of modes to be displayed form mode 1")
		("direction,d", po::value<std::string>(), "Specify the direciton")
		("length,L", "Display length")
		("speed,s", "Display speed")
		("header,H", "Display header")
		("prefix,p", "Use prefix")
		("regex,r", "Use regular expression (ignore -p option)")
		("options,o", "Display options");
	po::variables_map vm;
	po::positional_options_description pod;
	pod.add("ire-file",1);
	try {
		po::store(po::command_line_parser{argc, argv}.options(description).positional(pod).run(), vm);
		po::notify(vm);
	} catch (const std::exception& e) {
		std::cout << e.what() << "\n";
		exit(1);
	}
	std::string type_arg = "none";
	std::string way_arg = "none";
	char read_mode;
	bool match;
	bool use_prefix = false;
	bool disp_speed = false;
	bool disp_length = false;
	bool use_regex = false;
	read_mode = argv[2][1];

//初期化
    mode = 0;
    max_mode = 0;
	kms = false;
	for(int i=0; i < 10; i++) {
		max_vol1[i]=0;
		max_vol2[i]=0;
		max_total[i]=0;
	}
	if (vm.count("ire-file")) {
		ire_name = vm["ire-file"].as<std::string>();
	}
	if (vm.count("unit")) {
		type_arg = vm["unit"].as<std::string>();
	}
	if (vm.count("direction")) {
		way_arg = vm["direction"].as<std::string>();
	}
	if (vm.count("mode")) {
		mode = vm["mode"].as<int>();
		if (mode <= 0 || mode > 10) {
			fprintf(stderr, "Wrong argument for -m.\n");
			exit(1);
		}
	}
	if (vm.count("max_mode")) {
		max_mode = vm["max_mode"].as<int>();
		if (max_mode > 10) max_mode = 10;
		if (max_mode <= 0) {
			fprintf(stderr, "Wrong argument for -M.\n");
			exit(1);
		}
	}
	if (vm.count("header")) {
		header = true;
	}
	if (vm.count("options")) {
		disp = true;
	}
	if (vm.count("prefix")) {
		use_prefix = true;
	}
	if( vm.count("regex")) {
		use_regex = true;
	}
	if (vm.count("kms")) {
		kms = true;
	}
	if (vm.count("length")) {
		disp_length = true;
	}
	if (vm.count("speed")) {
		disp_speed = true;
	}
	if( type_arg == "pcu" || type_arg == "none" ) type=0;
	else if( type_arg == "veh" ) type = 1;
	else if( type_arg == "pax" ) type = 2;
	else {
		fprintf(stderr,"Wrong argument for -u.\n");
		exit(1);
	}
	if( way_arg == "all" || way_arg == "none" ) way=0;
	else if(way_arg == "both") way = 1;
	else if(way_arg == "peak") way = 2;
	else if(way_arg == "1") way = 3;
	else if(way_arg == "2") way = 4;
	else if(way_arg == "total") way = 5;
	else {
		fprintf(stderr,"Wrong argument for -d.\n");
		exit(1);
	}
    try {
        s_ire.Read(argv[1]);
    } catch (std::runtime_error& ) {
		fprintf(stderr,"Cannot read file %s.\n", argv[1]);
		exit(1);
	}

	if( disp) {
		printf("PCU       = ");
		for(int i=0; i < 10; i++) printf("%5.1f", s_ire.PCU[i]);
		puts("");
		printf("APC       = ");
		for(int i=0; i < 10; i++) printf("%5.1f", s_ire.APC[i]);
		puts("");
		printf("Unit      = %s\n", type_str[type]);
		printf("Direction = %s\n", way_str[way]);
		if( max_mode > 0 ) printf("Mode      = 1 - %d\n", max_mode);
		else if(mode > 0 ) printf("Mode      = %d\n", mode);
		else if(mode == 0) printf("Mode      = The total of all modes\n");
	}

	switch(type) {
		case 0:
			for(int i=0; i<10; i++) conv[i] = 1.0;
			break;
		case 1:
			for(int i=0; i < 10; i++) {
				if( s_ire.PCU[i] <= 0 ) conv[i] = 1.0;
				else conv[i] = 1.0/s_ire.PCU[i];
			}
			break;
		case 2:
			for(int i=0; i < 10; i++) {
				if( s_ire.PCU[i] <= 0 ) conv[i] = s_ire.APC[i];
				else conv[i] = s_ire.APC[i]/s_ire.PCU[i];
			}
			break;
	}

	if( header ) {
		printf("Link      ");
		if (disp_length) printf("  length");
		if( max_mode > 0 ) {
			for(int i=0; i < max_mode; i++) {
				printf("  Mode%02d", i+1);
				if(way==0) printf("-       -       ");
				else if(way==1) printf("-       ");
			}
			// second row
			if(way < 2) {
				puts("");
				printf("-         ");
				if(disp_length) printf("-       ");
				for(int i=0; i < max_mode; i++) print_header(way);
			}
			puts("");
		} else if( mode > 0 ) {
			print_header(way);
			puts("");
		}
		else if(mode == 0 ) printf("Total   A->B    B->A    \n");
	}
	if( read_mode == 'f' ) {
		//ファイルからリストを読み込む場合
		read_linkfile(argv[3], nRoad );
	    for(int i=0; i < s_ire.nLink; i++) {
	        link = s_ire.getLink(i);
	        linkhash[link->name] = link;
	    }
	    for(int i=0; i < nRoad; i++) {
			if (use_prefix ) {
				for(int j=0; j < s_ire.nLink; j++) {
					link = s_ire.getLink(j);
					if( strncmp( link->name, link_list[i].name.c_str(), strlen(link_list[i].name.c_str())) == 0 ) {
						printf("%-10s", link->name);
						if( disp_length) printf("%8.2f", link->length);
						if( disp_speed ) printf("%8.2f%8.2f", link->result[0].ltSp, link->result[1].ltSp);
						display_link(link);
						puts("");
					}
				}
			} else {
				if( linkhash.find(link_list[i].name) != linkhash.end()) {
					link = linkhash[link_list[i].name];
					printf("%-10s", link_list[i].name.c_str());
					if( disp_length) printf("%8.2f", link->length);
					if( disp_speed ) printf("%8.2f%8.2f", link->result[0].ltSp, link->result[1].ltSp);
					display_link(link);
					puts("");
				} else {
					fprintf(stderr, "Cannot find %s in %s.\n", link_list[i].name.c_str(), argv[1]);
				}
			}
        }
	} else {
		//リンクを一つだけ指定する場合
		if ( use_regex) {
			std::regex reg(argv[3]);
			for(int i=0; i < s_ire.nLink; i++) {
				link = s_ire.getLink(i);
				std::string link_name(link->name);
				if( std::regex_match(link_name, reg) ) {
					printf("%-10s", link->name);
					if( disp_length) printf("%8.2f", link->length);
					if( disp_speed ) printf("%8.2f%8.2f", link->result[0].ltSp, link->result[1].ltSp);
					display_link(link);
					puts("");
				}
			}
		} else if( use_prefix) {
			size_t pre_len = strlen(argv[3]);
			for(int i=0; i < s_ire.nLink; i++) {
				link = s_ire.getLink(i);
				if( strncmp(argv[3], link->name, pre_len ) == 0 ) {
					printf("%-10s", link->name);
					if( disp_length) printf("%8.2f", link->length);
					if( disp_speed ) printf("%8.2f%8.2f", link->result[0].ltSp, link->result[1].ltSp);
					display_link(link);
					puts("");
				}
			}
		} else {
			match = false;
			for(int i=0; i < s_ire.nLink; i++) {
				link = s_ire.getLink(i);
				if( strcmp(argv[3], link->name) == 0 ) {
					match = true;
					break;
				}
			}
			if( match ) {
				printf("%-10s", argv[3]);
				display_link(link);

			} else {
				fprintf(stderr,"Link name %s was not found in %s.\n", argv[3], argv[1]);
			}
		}
	}
    return 0;
}
//---------------------------------------------------------------------------
