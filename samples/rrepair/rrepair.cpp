#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include "tool.h"
#include "StradaINT.h"
#include "Cube.hpp"

void usage(const char* str) {
    char fname[_MAX_FNAME];
    _splitpath(str,NULL,NULL, fname, NULL);
    fprintf(stderr, "Usage: %s int rre link1 link2 aod\n", fname);
    fprintf(stderr, "Make OD passing link1 and line2\n");
}

int main(int argc, char* argv[]) {
    StradaINT sint;
    LinkPtr link;
    LinkPtr slink;
    LinkPtr elink;

    FILE* fp;
    char buff[256];
    // int nLink;
    int nZone;
    int line_no ;
    int org, dst;
    //int cnt;
    int vol[10];
    int nNode;
    Cube<int> mat;

    if( argc != 6 ) {
        usage(argv[0]);
        exit(1);
    }

    try {
        sint.Read(argv[1]);

	}catch (std::exception& e ) {
		fprintf(stderr, "%s\n", e.what());
		fprintf(stderr, "%s\n", sint.msg());
        return (1);
	}

    slink = elink = NULL;
    for(int i=0; i < sint.nLink; i++) {
        link = sint.getLink(i);
        if( strcmp(link->name, argv[3]) == 0 ) slink = link;
        if( strcmp(link->name, argv[4]) == 0 ) elink = link;

    }
    if( slink == NULL || elink == NULL) {
        fprintf(stderr, "Cannot find link.");
        exit(1);
    }
    printf("%10s%10s\n", slink->name, elink->name);
    if((fp = fopen(argv[2],"rt"))==NULL) {
        fprintf(stderr,"Cannnot open file %s", argv[2]);
        exit(1);
    }
    if( fgets(buff, 256, fp) == NULL || strlen(buff) < 5 || strncmp(buff,"RRE2",4) != 0) {
        fprintf(stderr, "File format error\n");
        fclose(fp);
        exit(1);
    }
    fgets(buff, 6, fp);
    // nLink = atoi(buff);
    fgets(buff, 6, fp);
    nZone = atoi(buff);
//    printf("%5d%5d\n", nLink, nZone);
    if( mat.init(10,nZone,nZone,0) == false ) {
        fprintf( stderr, "Cannot allocate memory\n");
        fclose(fp);
        exit(1);
    }
    try {
        if( fgets(buff, sizeof(buff), fp) == NULL ) throw 2;
        while (strlen(buff)==(sizeof(buff)-1)) {
            if( fgets(buff, sizeof(buff), fp) == NULL ) throw 2;
        }
        line_no = 3;
        while(true) {
            if( fgets(buff, 91, fp) == NULL ) break;
//            printf("%s*",buff);
            if( strlen(buff) != 90) throw line_no;
            org = getbufInt(buff, 0, 5);
            dst = getbufInt(buff, 5, 5);
            // cnt = getbufInt(buff,10, 5);
            for(int i=0; i < 10; i++) vol[i] = getbufInt(buff,15+7*i,7);
            nNode = getbufInt(buff, 85, 5);
            bool s_found = false;
            bool e_found = false;
            int s_stat = 0;
//            printf("%5d%5d%5d%5d\n", org, dst, cnt, nNode);
            for(int i=0; i < nNode; i++) {
                if( fgets(buff, 11, fp) == NULL || strlen(buff) != 10 ) throw line_no;
                if( e_found == true) continue;
                trim(buff);
                if( s_found == false ) {
                    if( s_stat == 0 ) {
                        if( strcmp(buff, slink->sNode) == 0 ) s_stat = 1;
                        else if( strcmp(buff, slink->eNode) == 0 ) s_stat = 2;
                    } else if( s_stat == 1) {
                        if( strcmp(buff, slink->eNode) == 0 ) {
                            s_found = true;
                            s_stat = 0;
                        } else if( strcmp(buff, slink->sNode) != 0 ) {
                            s_stat = 0;
                        }
                    } else if( s_stat == 2) {
                        if( strcmp(buff, slink->sNode) == 0 ) {
                            s_found = true;
                            s_stat = 0;
                        } else if( strcmp(buff, slink->eNode) != 0 ) {
                            s_stat = 0;
                        }
                    }
                } else {
                    if( s_stat == 0 ) {
//                        if( line_no ==613) printf("%s:%s-%s\n", buff, elink->sNode, elink->eNode);
                        if( strcmp(buff, elink->sNode) == 0 ) s_stat = 1;
                        else if( strcmp(buff, elink->eNode) == 0 ) s_stat = 2;
                    } else if( s_stat == 1) {
                        if( strcmp(buff, elink->eNode) == 0 ) {
                            e_found = true;
                            s_stat = 0;
                        } else if( strcmp(buff, elink->sNode) != 0 ) {
                            s_stat = 0;
                        }
                    } else if( s_stat == 2) {
                        if( strcmp(buff, elink->sNode) == 0 ) {
                            e_found = true;
                            s_stat = 0;
                        } else if( strcmp(buff, elink->eNode) != 0 ) {
                            s_stat = 0;
                        }
                    }
                }
                //     printf("%2d", s_stat);
            }
            fgets(buff, 256, fp); // return mark
            line_no++;
            //if( line_no > 22) break;
            //if( s_found == true)    printf("S:%5d\n", line_no);

            if( e_found == true) {
//                printf("%5d%10d\n", line_no, vol[0]);
                for(int i=0; i < 10; i++) mat.add(i, org-1, dst-1, vol[i]);
            }
        }
    } catch (int e) {
        fprintf(stderr, "RRE format error in %d\n", line_no);
    }

    fclose(fp);
    fp = fopen(argv[5],"wt");
    for(int t=0; t< 10; t++) {
        for(int i=0; i < nZone; i++) {
            for(int j=0; j < nZone; j++) {
                if (mat.get(t,i,j) > 0) {
                    fprintf(fp, "%d,%d,%d,%d\n", t+1,i+1,j+1, mat.get(t,i,j));
                }
            }
        }
    }
    fclose(fp);
    return 0;
}
