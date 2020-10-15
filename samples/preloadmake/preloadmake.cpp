/*
トランジト配分の結果(TRE)からプレロード用のファイルを作成
*/
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <list>
//---------------------------------------------------------------------------
#include "tool.h"
#include "StradaINT.h"
#include "StradaIRE.h"
//---------------------------------------------------------------------------
char* make_key(char* key, char* str1, char* str2)
{
    sprintf(key, "%-10s%-10s", str1, str2);
    return key;
}
//---------------------------------------------------------------------------
void usage(const char* str) {
    char fname[_MAX_FNAME];
    _splitpath(str,NULL,NULL, fname, NULL);
    printf("Usage: %s int tre txt output\n", fname);
    printf("txt: number VOR PCU mode\n");
}
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    StradaINT s_int;
    StradaIRE s_ire;
    IRELinkPtr link;
    char buff[1024];
    char key[21];
    char name[11];
    char node[11];
    char str1[11];
    char str2[11];
    int vol, max_vol;
    int n, m;
    double data1, data2;
    double VOR[20];
    double PCU[20];
//    double paskm[10];
//    double pcukm[10];
//    double vehkm[10];
    double pcu;
    int modes[20];

    FILE* fp;
    std::unordered_map<std::string, IRELinkPtr> hmap;
    std::list<IRELinkPtr> irelist;
    std::list<std::string> nodelist;

    if( argc != 5 ) {
        usage(argv[0]);
        exit(1);
    }
    key[20] = '\0';
    try {
        s_int.Read(argv[1]);
        s_ire.init(s_int);
        s_ire.nMode = 10;
        for(int i=0; i < 10; i++) {
            s_ire.PCU[i]=1;
            s_ire.APC[i]=1;
        }

    } catch (std::runtime_error& e) {
        fprintf(stderr, "File reading error - %s\n", e.what());
        exit(1);
    }
    /// VOR , PCU 変換用のファイルを先に読む
    if( (fp=fopen(argv[3],"rt"))==NULL) {
        fprintf(stderr, "Cannot open file - %s\n", argv[3]);
        exit(1);
    }
    for(int i=0; i < 20; i++) {
        VOR[i]=0;
        PCU[i]=0;
    }
    /*
    for(int i=0; i < 10; i++) {
        vehkm[i]=pcukm[i]=paskm[i]=0;
    }
    */
    while( fgets(buff, 1024, fp) != NULL ) {
        if ( sscanf(buff, "%d%lf%lf%d\n", &n, &data1, &data2, &m) != 4 ){
            printf("Invalid format %s.\n", argv[3]);
            fclose(fp);
            exit(1);
        }
        if( n <= 0 || n > 20 ) continue;
        VOR[n-1]=data1;
        PCU[n-1]=data2;
        if(m <= 0 || m > 10) {
            printf("Invalid Mode Number %d: 1 is used for %d\n", m, n);
        }
        modes[n-1]=m-1;
    }
    fclose(fp);
    for(int i=0; i < s_ire.nLink; i++) {
        link = s_ire.getLink(i);
        make_key(key, link->sNode, link->eNode);
        hmap[std::string(key)] = link;
    }

    if( (fp=fopen(argv[2],"rt"))==NULL) {
        fprintf(stderr, "Cannot open file - %s\n", argv[2]);
        exit(1);
    }
    int direction;
    while( fgets(buff, 1024, fp) != NULL ) {
        if( buff[0] == 'A') break;
    }
    str1[10]='\0';
	str2[10]='\0';
	name[10]='\0';
	do {
		if( buff[0] != 'A' ) break;
		m = getbufInt(buff,21,2);
		nodelist.clear();

		if( fgets(buff, 1024, fp) == NULL) break;
		max_vol = 0;
		do {
			strncpy(node, &buff[30], 10);
			nodelist.push_back(std::string(node));
			vol = getbufInt(buff, 70, 10);
			if( vol > max_vol ) max_vol = vol;

			if( fgets(buff, 1024, fp) == NULL) break;
			strncpy(name, &buff[6], 10);
			trim(name);
		} while( name[0] == '\0');

		pcu = max_vol/VOR[modes[m-1]] * PCU[modes[m-1]];
		printf("%s\t%lf\n", name, pcu);
		std::unordered_map<std::string, IRELinkPtr>::iterator ireit;

		std::list<std::string>::iterator it = nodelist.begin();
		strncpy(str1, (*it).c_str(), 10);
		++it;
		while ( it != nodelist.end() ) {
			strncpy(str2, (*it).c_str(), 10);
			make_key(key, str1, str2);
			direction = -1;
			ireit = hmap.find(std::string(key));
			if( ireit == hmap.end() ){
				make_key(key, str2, str1);
				ireit = hmap.find(std::string(key));
				if( ireit == hmap.end() ) {
					printf("%s-%s is not found.\n", str1, str2);
				} else direction = 1;
			} else direction = 0;

			if (direction>-1) {
				link = ireit->second;
				link->result[direction].inVol[modes[m-1]] += pcu;
			}
			strncpy(str1,str2,10);
			++it;
		}


	} while ( buff[0]=='A' );

	fclose(fp);

	for(int i=0; i < s_ire.nLink; i++) {
		link = s_ire.getLink(i);
		for(int j=0; j < 10; j++) {
			for(int k=0; k < 2; k++) {
				link->result[k].Vol += link->result[k].inVol[j];
			}
		}
	}
/*
	for(int i=0; i < 10; i++) {
		if( vehkm[i] > 0 ) {
			s_ire.APC[i] = paskm[i]/vehkm[i];
			s_ire.PCU[i] = pcukm[i]/vehkm[i];
		}
	}
*/
	strncpy(s_ire.comment, s_int.comment, 256);
	s_ire.Write(argv[4]);
	printf("Finish.\n");
	return 0;
}
//---------------------------------------------------------------------------
