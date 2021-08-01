/*
INTまたはPARファイルの名前を一括変更
*/
//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <algorithm> 
#include <stdexcept>
#include "tool.h"
#include "StradaINT.h"
#include "StradaPAR.h"

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
bool comment_line(const char* buff) {
	char* p = (char*)buff;
	while( isspace(*p) ) p++;
	if( *p == '#' || *p == '\0' ) return true;
	return false;
};

void usage() {
	printf("Rename node name in a link file or par file\n");
	printf("\nUSAGE: nodechg source_file param_file output_file\n");
	printf("source_file : link file (INT) or par file (PAR)\n");
	puts("File extension is mandatory.");
	printf("[param_file]: C style scanf format\n");
	printf("existing_name   new_name\n\n");
	exit(1);
}

int main(int argc, char* argv[])
{
    FILE* fp;
    StradaINT s_int;
	StradaPAR par;
	int readtype = 0;

	char buff[2048];
	char src[12];
	char dst[12];

    if( argc != 4 ) usage();
	std::string arg = argv[1];
	size_t pos = arg.find_last_of(".");
	std::string ext = arg.substr(pos+1);
	std::transform(ext.cbegin(),ext.cend(),ext.begin(), toupper);

	if( ext == "INT" ) readtype = 1;
	else if( ext == "PAR" ) readtype = 2;
	else usage();

    try {
		if ( readtype == 1 )
			s_int.Read(argv[1]);
		else if ( readtype == 2)
			par.Read(argv[1]);
    } catch (std::runtime_error& e) {
        printf("%s\n", e.what());
        exit(1);
    }

    if( (fp = fopen(argv[2], "rt")) != NULL )
    {
		try {
			while( fgets(buff, 2048, fp ) != NULL ) {
				if( comment_line(buff) ) continue;
				buff[strlen(buff)-1] = '\0';
				if ( sscanf(buff, "%s%s", src, dst) != 2 ) {
					fprintf(stderr, "File format error.\n");
					throw 1;
				}
				if( strlen(src) > 10 || strlen(dst) > 10 ) {
					fprintf(stderr, "Node name length > 10\n");
					throw 2;
				}
				if( readtype == 1 ) {
					for(auto& link: s_int.links) {
						if( strncmp(link->sNode, src, 10) == 0 ) strncpy(link->sNode, dst,10);
						else if( strncmp(link->eNode, src, 10) == 0 ) strncpy(link->eNode, dst, 10);
					}
				} else if (readtype == 2) {
					par.rename_nodes(src, dst);
				}
			}
		} catch( int e) {
			fclose(fp);
			exit(1);
		}
        fclose(fp);

        if( (fp = fopen(argv[3], "wt")) != NULL )
        {
			if( readtype == 1)
				s_int.Write(fp);
			else if ( readtype == 2)
				par.Write(fp);
            fclose(fp);
        } else {
            fprintf(stderr,"\nFile creat error - %s\n", argv[2]);
            exit(1);
        }

    } else {
        fprintf(stderr, "\nFile open error - %s\n", argv[2]);
        exit(1);
    }
    return 0;
}
//---------------------------------------------------------------------------
