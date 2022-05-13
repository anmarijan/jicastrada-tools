//---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdexcept>
#include <string>
//#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include <list>
#include <unordered_map>
#include "StradaINT.h"
#include "tool.h"
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
bool comment_line(const char* buff) {
	char* p = (char*)buff;
	while( isspace(*p) ) p++;
	if( *p == '#' || *p == '\0' ) return true;
	return false;
};


void change_link(LinkPtr link, char* command, char* value) {
    float fx;
    int ix;
    char ch;
	int len;

	if( strcmp(command, "vmax") == 0 ) {
		fx = (float)atof(value);
		if( fx >= 0 ) link->Vmax = fx;
	} else if( strcmp(command, "length") == 0 ) {
		fx = (float)atof(value);
		if( fx >= 0 ) link->length = fx;
	} else if( strcmp(command, "capacity") == 0 ) {
		fx = (float)atof(value);
		if( fx >= 0 ) link->Capa = fx;
	} else if( strcmp(command, "QV") == 0 ) {
		ix = atoi(value);
		if( ix >= 0 ) link->QV = ix;
	} else if( strcmp(command, "fare0") == 0 ) {
		fx = (float)atof(value);
		link->fare[0] = fx;
	} else if(strcmp(command, "fare1") == 0 ) {
		fx = (float)atof(value);
		link->fare[1] = fx;
	} else if(strcmp(command, "fare2") == 0 ) {
		fx = (float)atof(value);
		link->fare[2] = fx;
	} else if(strcmp(command, "fare3") == 0 ) {
		fx = (float)atof(value);
		link->fare[3] = fx;
	} else if(strcmp(command, "fare4") == 0 ) {
		fx = (float)atof(value);
		link->fare[4] = fx;
	} else if( strcmp(command, "fare5") == 0 ) {
		fx = (float)atof(value);
		link->fare[5] = fx;
	} else if(strcmp(command, "fare6") == 0 ) {
		fx = (float)atof(value);
		link->fare[6] = fx;
	} else if(strcmp(command, "fare7") == 0 ) {
		fx = (float)atof(value);
		link->fare[7] = fx;
	} else if(strcmp(command, "fare8") == 0 ) {
		fx = (float)atof(value);
		link->fare[8] = fx;
	} else if(strcmp(command, "fare9") == 0 ) {
		fx = (float)atof(value);
		link->fare[9] = fx;
	} else if(strcmp(command, "farea") == 0 ) {
		fx = (float)atof(value);
		for(int i=0; i < 10; i++) link->fare[i] = fx;
	} else if(strcmp(command, "way0") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			link->ways[0] = ch;
	} else if(strcmp(command, "way1") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			link->ways[1] = ch;
	} else if(strcmp(command, "way2") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			link->ways[2] = ch;
	} else if(strcmp(command, "way3") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			link->ways[3] = ch;
	} else if(strcmp(command, "way4") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			link->ways[4] = ch;
	} else if(strcmp(command, "way5") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			link->ways[5] = ch;
	} else if(strcmp(command, "way6") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			link->ways[6] = ch;
	} else if(strcmp(command, "way7") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			link->ways[7] = ch;
	} else if(strcmp(command, "way8") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			link->ways[8] = ch;
	} else if(strcmp(command, "way9") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			link->ways[9] = ch;
	} else if(strcmp(command, "waya") == 0 ) {
		ch = value[0];
		if( ch =='0' || ch == '1' || ch == '2' || ch == '3' )
			for( int i=0; i < 10; i++) link->ways[i] = ch;
	} else if(strcmp(command, "usr1") == 0 ) {
		ch = value[0];
		link->aFlag1 = ch;
	} else if(strcmp(command, "usr2") == 0 ) {
		ix = atoi(value);
		link->nFlag2 = ix;
	} else if(strcmp(command, "usr3") == 0 ) {
		ix = atoi(value);
		link->nFlag3 = ix;
	} else if(strcmp(command, "usr4") == 0 ) {
		len = strlen(value);
		if( len == 0 ) {
			value[0]=value[1]=' ';
		} else if (len == 1) {
			value[1] = ' ';
		}
		link->aFlag4[0] = value[0];
		link->aFlag4[1] = value[1];
	} else if(strcmp(command, "usr5") == 0 ) {
		len = strlen(value);
		if( len == 0 ) {
			value[0]=value[1]=value[2]=' ';
		} else if (len == 1) {
			value[1]=value[2]=' ';
		} else if (len==2) {
			value[2]=' ';
		}
		link->aFlag5[0] = value[0];
		link->aFlag5[1] = value[1];
		link->aFlag5[2] = value[2];
	} else if(strcmp(command, "disp") == 0 ) {
		ch = value[0];
		link->display = ch;
	} else if(strcmp(command,"ltype") == 0 ) {
		ch = value[0];
		link->linktype = ch;
	} else if(strcmp(command,"color") == 0 ) {
		ch = value[0];
		link->color = ch;
	}

}

const char* getdbstr(const char* source, char* dst, int maxlen) {

    const char* p = source;
    char* q = dst;
    int c;

    while ( *p == ' ' || *p == '\t' ) p++;
    if ( *p == 0 ) {
        dst[0] = 0;
    } else if( *p == '\"' ) {
        bool meta = false;
        p++;
        c =0;
        while( c < maxlen-1) {
            if (*p == 0 ) {
                dst[0] = 0;
                break;
            }
            if (*p == '\"' && meta==false) {
                p++;
                *q = 0;
                break;
            }
            meta =  (*p == '\\' ) ? true : false;
            if( !meta) {
                *q = *p;
                q++;
            }
            p++;
            c++;
        }
        if(c == maxlen-1) dst[0]=0;
        else *q = 0;

    } else {
        c = 0;
        while( *p != ' ' && *p != '\t' && *p != 0 && c < maxlen-1) {
            *q = *p;
            p++;
            q++;
            c++;
        }
        if( c == maxlen-1 ) {
            dst[0]=0;
        } else {
            *q = 0;
        }
    }
    return p;
}

int main(int argc, char* argv[])
{
    if( argc < 4 ) {
        printf("\nUSAGE: intchg int_file param_file output_file [p]\n");
		printf("[param_file]: C style scanf format\n");
		printf("link_name   field_name    value\n\n");
		printf("[field_name]\n");
        printf("        vmax: Max speed\n");
		  puts("      length: link length in km");
        printf("    capacity: Capacity\n");
        printf("          QV: QV code\n");
        printf("fare0 -fare9: fare\n");
        printf(" way0 - way9: direction (0-3)\n");
        printf("usr1 - usr5 : user flag\n");
		printf("       ltype: link type (0-2)\n");
		printf("       color: link color(0-8)\n");
        printf("        disp: display flag (0-9)\n");
		printf("[Option]\n");
		printf("p           : Use regular expression\n");
        return 1;
    }
	StradaINT s_int;
	std::unordered_map<std::string, LinkPtr> linkhash;
	bool use_prefix = false;

	char name[100];
    char command[100];
    char value[100];
	int len;
	int sn;
	bool data_valid;

    try {
        s_int.Read(argv[1]);
        for(const auto& link : s_int.links ) {
			auto it = linkhash.find(link->name);
			if( it != linkhash.end() ) {
				printf("Link name %s is duplicated.\n", link->name);
				printf("The previous link will be ignored.\n");
			}
            linkhash[link->name] = link;
        }
    } catch (std::runtime_error& e) {
        printf("%s\n", e.what());
        exit(1);
    }

	if( argv[4] && argv[4][0] == 'p' ) {
		use_prefix = true;
	}
	std::ifstream ifs(argv[2]);
	if (!ifs) {
		printf("Cannot open %s.\n", argv[2]);
		return 1;
	}
	std::string buff;
    while( std::getline(ifs, buff) ) {
		if (comment_line(buff.c_str())) continue;
		data_valid = true;
        const char* str = buff.c_str();
        //sn = sscanf(buf,"%s%s%s", name, command, value );
        sn = 3;
        str = getdbstr(str, name, 100);
        if(name[0]==0 || str == 0 ) {
//                printf("[%s][%s]", name, str);
//                fprintf(stderr, "name error\n");
            continue;
        }
//            printf("name=%s\n", name);
        str = getdbstr(str, command,100);
        if( command[0] ==0 ) {
//                fprintf(stderr, "command error\n");
            continue;
        }
//            printf("command=%s\n", command);

        if( str == 0 ) {
            sn = 2;
        } else {
            str = getdbstr(str, value,100);
            if( value[0] == 0 ) sn = 2;
        }
//            printf("value=%s\n", value);

		if( use_prefix==false && strlen(name) > 10 ) {
			fprintf(stderr, "Name length exceeds 10.\n");
			continue;
		}
		if( sn == 2 ) {
			if( strcmp(command, "drop")==0 ) {
				std::list<LinkPtr>::iterator it = s_int.links.begin();
				while(it != s_int.links.end()) {
					bool change_link = false;
					LinkPtr link = (*it);
					if (use_prefix) {
						boost::regex reg(name);
						std::string str = link->name;
						if (boost::regex_match(str, reg)) {
							change_link = true;
						}
					}
					else if (strcmp(link->name, name) == 0) {
						change_link = true;
					}
					if( change_link ) {
						linkhash.erase(link->name);
						it = s_int.links.erase(it);
					} else ++it;
				}
				continue;
			} else {
				data_valid = false;
			}
		} else if (sn != 3) data_valid = false;
		if( data_valid == false ) continue;

		if( use_prefix ) {
			for(auto& link : s_int.links) {
                boost::regex reg(name);
                std::string str = link->name;
                if( boost::regex_match(str, reg)) {
                        change_link(link, command, value);
                }
/*
				len = strlen(name);

				if( strncmp(link->name, name, len) == 0 ) {
					change_link(link, command, value);
                }
*/
			}
		} else {
			if( name[strlen(name)-1] == '*' ) {  //prefix
				len = strlen(name) -1 ;
				for(auto& link : s_int.links ) {
					if (len == 0 ){
						change_link(link, command, value);
					} else if( strncmp(link->name, name, len) == 0 ) {
						change_link(link, command, value);
					}
				}
			} else {
				auto it = linkhash.find(name);
				if( it != linkhash.end() ) {
					change_link(it->second, command, value);
				}
			}
		}
    }
    ifs.close();
	try {
		s_int.Write(argv[3]);
	}
	catch (const std::runtime_error& e) {
		printf("Error: %s\n", e.what());
		return 1;
	}
    return 0;
}
//---------------------------------------------------------------------------
