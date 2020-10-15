//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdexcept>
#include <new>
//---------------------------------------------------------------------------
#include "StradaIRE.h"
#include "tool.h"
#include "stgetopt.h"
#include <unordered_map>
#include <vector>
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
    char name[11];
    char remark[21];
};

std::vector<RoadList> link_list;

void read_linkfile(const char* fname, int &count) {
    FILE* fp;
    char buf[256];
    int total = 0;
    int pos;
    if( (fp = fopen(fname, "rt")) != NULL )
    {
        while( fgets(buf, 256, fp) != NULL ){
			pos = 0;
			while ( isspace(buf[pos])) pos++;
			if( buf[pos] == '#' || buf[pos] == '\0' ) continue;
            total++;
        }
        fseek(fp, 0, SEEK_SET );    //ファイルの先頭に戻る
        try {
        	link_list.resize(total);
        } catch( std::bad_alloc& e) {
			printf("Cannot allocate memory for %s.\n", fname);
			printf("The total number of lines is %d.\n", total);
			exit(1);
		}
		int i = 0;
        while( fgets(buf, 256, fp) != NULL ){
			pos = 0;
			while ( isspace(buf[pos])) pos++;
			if( buf[pos] == '#' || buf[pos] == '\0' ) continue;

            buf[strlen(buf)-1] = '\0';
            strncpy(link_list[i].name, buf, 10 );
            link_list[i].name[10] = '\0';
            strncpy(link_list[i].remark, buf+10, 20 );
            link_list[i].remark[20] = '\0';

            trim(link_list[i].name, 10);
            trim(link_list[i].remark, 20);
			i++;
        }
        count = total;
        fclose(fp);
    } else {
		fprintf(stderr,"Cannot open file %s.\n", fname);
        exit(1);
    }
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
			printf("Both    A->B    B->A    ");
			break;
		case 1:
			printf("A->B    B->A    ");
			break;
		default:
			break;
	}
}

void display_link(IRELinkPtr link ) {

	double vol1, vol2;

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
	puts("-l       Display length");
	puts("-s       Display speed");
	puts("-H       Display header");
	puts("-p       Use prefix");
	puts("-v       Display the accepted options");
}

int main(int argc, char* argv[])
{
    if( argc < 4 || argv[2][0] != '-' || (argv[2][1] != 'f' && argv[2][1] != 'l') ) {
		usage();
        exit(1);
    }

    StradaIRE s_ire;
    IRELinkPtr link;
    std::unordered_map<std::string, IRELinkPtr> linkhash;
    int nRoad;
    int type ;	//0:PCU, 1:Vehicles, 2:Pax
    const char* type_str[] = {"pcu", "veh","pax"};
    const char* way_str[] = {"All", "Both", "Peak", "ANode->BNode", "BNode->ANode", "Total"};
    bool header = false;
    bool disp   = false;


// オプション読み込み
	optarg = NULL;
	optind = 4 ;
	char ch;
	char* type_arg = NULL;
	char* way_arg = NULL;
	char read_mode;
// その他
	bool match;
	bool use_prefix = false;
	bool disp_speed = false;
	bool disp_length = false;
	int pre_len;

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

	while( optind < argc ) {
		ch = get_opt(argc, argv, "u:d:m:M:Hvpksl");
		if( ch == EOF ) {
			continue;
		} else {
			switch (ch) {
			case 'u':
				type_arg = optarg;
				break;
			case 'd':
				way_arg = optarg;
				break;
			case 'm':
				if( *optarg == '0' ) mode = 0;
				else {
					mode = atoi(optarg);
					if( mode <= 0 || mode > 10) {
						fprintf(stderr,"Wrong argument for -m.\n");
						exit(1);
					}
				}
				break;
			case 'M':
				max_mode = atoi(optarg);
				if (max_mode > 10 ) max_mode = 10;
				if (max_mode <= 0 ) {
					fprintf(stderr,"Wrong argument for -M.\n");
					exit(1);
				}
				break;
			case 'H':
				header = true;
				break;
			case 'v':
				disp = true;
				break;
			case 'p':
				use_prefix = true;
				break;
			case 'k':
				kms = true;
				break;
			case 'l':
				disp_length = true;
				break;
			case 's':
				disp_speed = true;
				break;
			default:
			;
			}
		}
	}
	if( type_arg ) {
		if( strcmp(type_arg, "pcu")==0 ) type=0;
		else if(strcmp(type_arg,"veh")==0) type = 1;
		else if(strcmp(type_arg,"pax")==0) type = 2;
		else {
			fprintf(stderr,"Wrong argument for -u.\n");
			exit(1);
		}
	} else type = 0;
	if( way_arg ) {
		if( strcmp(way_arg, "all")==0 ) way=0;
		else if(strcmp(way_arg,"both")==0) way = 1;
		else if(strcmp(way_arg,"peak")==0) way = 2;
		else if(strcmp(way_arg,"1")==0) way = 3;
		else if(strcmp(way_arg,"2")==0) way = 4;
		else if(strcmp(way_arg,"total")==0) way = 5;
		else {
			fprintf(stderr,"Wrong argument for -d.\n");
			exit(1);
		}
	} else way = 0;

    try {
        s_ire.Read(argv[1]);
    } catch (std::runtime_error& e) {
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
					if( strncmp( link->name, link_list[i].name, strlen(link_list[i].name)) == 0 ) {
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
					printf("%-10s", link_list[i].name);
					if( disp_length) printf("%8.2f", link->length);
					if( disp_speed ) printf("%8.2f%8.2f", link->result[0].ltSp, link->result[1].ltSp);
					display_link(link);
					puts("");
				} else {
					fprintf(stderr, "Cannot find %s in %s.\n", link_list[i].name, argv[1]);
				}
			}
        }
	} else {
		//リンクを一つだけ指定する場合
		if( use_prefix) {
			pre_len = strlen(argv[3]);
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
