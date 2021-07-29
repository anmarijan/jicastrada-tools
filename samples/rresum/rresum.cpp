//---------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
//---------------------------------------------------------------------------
#include "tool.h"
#include "StradaINT.h"

#include "Matrix.hpp"
//---------------------------------------------------------------------------
int error_exit(const char* str) {
    printf("%s\n", str);
    exit(1);
}
//---------------------------------------------------------------------------
char* make_key(char* key, const char* str1, const char* str2) {
    sprintf(key, "%-10s%-10s", str1, str2);
    return key;
}
//---------------------------------------------------------------------------
void usage(const char* str) {
    char fname[_MAX_FNAME];
    _splitpath(str,NULL,NULL, fname, NULL);
    fprintf(stderr, "Usage: %s int rre output\n", fname);
}
//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    StradaINT s_int;
    LinkPtr link;
    char buff[1024];
    char str1[11];
    char str2[11];
    char key[21];
    int nLink;
    int nZone;
    int n;
    int vols[10];

    FILE* fp;
    Matrix<int> mat;
    std::vector<int> vec;
    std::vector<LinkPtr> array;
    std::unordered_map<std::string, LinkPtr> hmap;
    std::unordered_map<std::string, int> zoneid;


    if( argc != 4 ) {
        usage(argv[0]);
        exit(1);
    }

    try {
        s_int.Read(argv[1]);

    } catch (std::runtime_error& e) {
        fprintf(stderr, "File reading error - %s\n", e.what());
        exit(1);
    }
    for(int i=0; i < s_int.nLink; i++ ) {
        link = s_int.getLink(i);
        hmap[std::string(link->name)] = link;
    }

    if( (fp=fopen(argv[2],"rt"))==NULL) {
        fprintf(stderr, "Cannot open file - %s\n", argv[2]);
        exit(1);
    }
    if ( fgets(buff, 1024, fp) == NULL ) error_exit("Header Error");
    if ( strncmp(buff,"RRE2",4) != 0 ) error_exit("Header Error");
//    if ( fgets(buff, 10, fp) == NULL ) error_exit("Error");
    if ( fread(buff, sizeof(char), 10, fp) < 10 ) error_exit("Error");
    buff[10]='\0';
    nLink = getbufInt(buff, 0, 5);
    nZone = getbufInt(buff, 5, 5);
//    printf("%5d%5d\n", nLink, nZone);
    if( nLink <= 0 || nZone <= 0 ) error_exit("Error in the header");

    mat.init(nLink, nLink, 0);
    vec.resize(nLink);
    array.resize(nLink);
    key[20]='\0';


    std::unordered_map<std::string, LinkPtr>::iterator it_link;
    for(int i=0; i < nLink; i++ ) {
//        if ( fgets(buff, 10, fp) == NULL ) error_exit("Error");
        if ( fread(buff, sizeof(char), 10, fp) < 10 ) error_exit("Error");
        buff[10]='\0';
        trim(buff);
        it_link = hmap.find(std::string(buff));
        if (it_link != hmap.end()) {
            link = it_link->second;
            make_key(key, link->sNode, link->eNode);
            zoneid[std::string(key)]=i;
            make_key(key, link->eNode, link->sNode);
            zoneid[std::string(key)]=i;
            array[i] = link;
        } else {
            fprintf(stderr, "Cannot find link [%s].\n", buff);
        }
    }
    if( fgets(buff, 10, fp) == NULL) error_exit("Error in the end of Line 1");  // teturn mark
/*
    for(int i=0; i < nLink; i++) {
        link = array[i];
        printf("%s:\n", link->name);
    }
*/
    str1[10] = '\0';
    str2[10] = '\0';
//    int count = 0;
    while( fread(buff, sizeof(char), 90, fp) == 90 ) {
        buff[90]='\0';
//        if( fgets(buff, 90, fp) == NULL) break;
        //sZone = getbufInt(buff, 0, 5);
        //eZone = getbufInt(buff, 5, 5);
        //m = getbufInt(buff, 10, 5);
        for(int i=0; i < 10; i++) vols[i] = getbufInt(buff, 15+i*7, 7);
        n = getbufInt(buff, 85, 5);
//        if( fgets(buff, 10, fp) == NULL) error_exit("Data Error");
        if( fread(buff, sizeof(char), 10, fp) < 10 ) error_exit("Data_Error");
        buff[10]='\0';
        strncpy(str1, buff, 10);

        std::unordered_map<std::string, int>::iterator it;
        int vn = 0;
        for(int i=0; i < n-1; i++) {
//            if( fgets(buff, 10, fp) == NULL) error_exit("Data Error");
            if( fread(buff, sizeof(char), 10, fp) < 10) error_exit("Data Error");
            buff[10]='\0';
            trim(buff);
            strncpy(str2, buff, 10);
            make_key(key, str1, str2);
//            if( count==0) printf("[%s]\n", key);
            it = zoneid.find(std::string(key));
            if( it != zoneid.end()) {
                vec[vn] = it->second;
                vn++;
//                puts("+");
            }
            strncpy(str1, str2, 10);
        }
        fgets(buff, 10, fp);  // return
        for(int i=0; i < vn-1; i++) {
            int origin = vec[i];
            for(int j=1; j < vn; j++) {
                int dest = vec[j];
                mat.add(origin, dest, vols[0]);
            }
        }
//        count++;
    }
    fclose(fp);


    if( (fp=fopen(argv[3],"wt"))==NULL) {
        fprintf(stderr, "Cannot open file - %s\n", argv[2]);
        exit(1);
    }
    fprintf(fp,"Mode=1");
    for(int i=0; i < nLink; i++) {
        link = array[i];
        fprintf(fp, ",%s", link->name);
    }
    fprintf(fp,"\n");
    for(int i=0; i < nLink; i++) {
        link = array[i];
        fprintf(fp,"%s", link->name);
        for(int j=0; j< nLink; j++) {
            fprintf(fp,",%d",mat.get(i,j));
        }
        fprintf(fp,"\n");
    }
    fclose(fp);

//    printf("Finish.\n");
    return 0;
}
//---------------------------------------------------------------------------
