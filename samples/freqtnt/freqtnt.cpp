/*
ラインデータ(TNT)の頻度を合計し、通過する各リンクのvolに設定する。
*/
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdexcept>
#include <memory>
#include <list>
#include <unordered_map>
#include <vector>
#include <boost/algorithm/string.hpp>  //trim
//---------------------------------------------------------------------------
#include "tool.h"
#include "StradaINT.h"
#include "StradaTNT.h"
#include "Hashtable.h"
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class LinkInfo {
public:
	char name[21];
	int vol;
    bool flag; // check for double counting due to 1-direction transit net
	LinkInfo(){memset(name, 0 , 21); vol = 0; flag=false; }
};

int main(int argc, char* argv[])
{
    FILE* fp;
    StradaINT s_int;
	StradaTNT s_tnt;
    std::unordered_map<std::string, std::shared_ptr<LinkInfo>> linkhash;
	std::unordered_map<std::string, int> listhash;

    if( argc < 3 ) {
        printf("\nFrequency setting: a tool for JICA-STRADA\n");
        printf("\nUSAGE: freqtnt int_file tnt_file [frequency_file]\n");
		puts("frequecy_file (TAB separated): RouteName Frequency ");
        exit(1);
    }
    char buf[2048];
    char key[2048];
    LinkPtr s_link;
//	TRNLine* line;
//	TRNNode*  node;
	std::list<std::shared_ptr<LinkInfo>> links;
//    int nLink;
	std::vector<int> vols; //volume from frequency_file
	int nList; //No. of data in frequency_file
	int freq ;

    // Read INT and TNT file
	try {
        s_int.Read(argv[1]);
		// nLink = s_int.nLink;
		s_tnt.Read(argv[2]);
        for(const auto& s_link : s_int.links ) {
			std::shared_ptr<LinkInfo> info_ptr = std::make_shared<LinkInfo>();
			strncpy(info_ptr->name, s_link->name, 10);
			sprintf(key, "%10s%10s", s_link->sNode, s_link->eNode);
            linkhash[key] = info_ptr;
			links.push_back(info_ptr);
        }
    } catch (std::runtime_error& e) {
        fprintf(stderr, "%s\n", e.what());
        exit(1);
    }
	// Read frequency data from a text file
	nList = 0;
	char* p;
	if( argc > 3 ) {
		try {
			if( (fp = fopen(argv[3], "rt")) == NULL ) throw 1;
			while( fgets(buf, 2048, fp) != NULL) {
				buf[strlen(buf)-1] = '\0';
				p = strtok(buf,"\t"); if ( p == NULL ) throw 2;
				p = strtok(NULL, "\t"); if ( p == NULL ) throw 2;
				nList++;
			}
			fseek(fp, 0L, SEEK_SET);
			if (nList == 0) throw 3;
			vols.resize(nList);
			for(int i=0; i < nList; i++) vols[i] = 0;
			nList = 0;
			while( fgets(buf, 2048, fp) != NULL) {
				buf[strlen(buf)-1] = '\0';
				p = strtok(buf,"\t");
				strcpy(key, p);
				p = strtok(NULL, "\t");
				freq = atoi(p);
				vols[nList] = freq;
				listhash[key] = vols[nList];
				nList++;
				//printf("%30s%10d\n", key, freq);
			}
			fclose(fp);
		} catch(int e) {
			fprintf(stderr, "%d File read error %s\n", e, argv[3]);
			exit(1);
	    }
	}

	// Set frequency data to link
	for(const auto& line : s_tnt.lines ) {
		std::list<TRNNode>::const_iterator snode = line.nodes.begin();
		std::list<TRNNode>::const_iterator enode = std::next(snode);
		while( enode != line.nodes.end()) {
			std::shared_ptr<LinkInfo> info = nullptr;
		    sprintf(key, "%10s%10s",snode->name, enode->name);
			if (linkhash.find(key) == linkhash.end() ) {
				sprintf(key, "%10s%10s", enode->name, snode->name);
				if (linkhash.find(key) == linkhash.end() ) {
					info = nullptr;
				} else info = linkhash[key];
			} else info = linkhash[key];
			if (info != nullptr ) {
				if( nList > 0 ) {
					std::string str = line.name;
					boost::trim(str);
					if( listhash.find(str) != listhash.end()) {
						if( info->flag == false ) {
							info->vol += listhash[str];
							info->flag = true;
						}
					}
				} else { //Use frequency data if frequency data is not specified
					info->vol += line.frequency;
			    }
			}
			snode = enode;
			enode = std::next(snode);
		}
		for (auto& link: links) {
			link->flag = false;
		}
	}

	for(const auto& link : links) {
		printf("%s\t%d\n", link->name, link->vol);
	}
    return 0;
}
