//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <unordered_map>
//---------------------------------------------------------------------------
#include "StradaINT.h"
#include "StradaTNT.h"
//---------------------------------------------------------------------------
char* makeKey(char* key, const char* s1, const char* s2) {
    sprintf(key, "%s\t%s", s1, s2);
    return key;
}

void usage(const char* str) {
    char fname[_MAX_FNAME];
    _splitpath(str,NULL,NULL, fname, NULL);
    fprintf(stderr, "Usage: %s int tnt\n", fname);
}


int main(int argc, char* argv[])
{

	LinkPtr link;
	StradaINT s_int;
	StradaTNT tnt;
	char key[30];
	int nLine;

	double length;
    std::unordered_map<std::string, LinkPtr> linkhash;

	if( argc != 3 ) {
        usage(argv[0]);
		exit(1);
	}

	try {
		s_int.Read(argv[1]);
		printf("INT: %s\n", argv[1]);
		tnt.Read(argv[2]);
		nLine = tnt.nLine;
		printf("TNT: %s\n", argv[2]);

		for(const auto& link : s_int.links) {
			makeKey(key, link->sNode, link->eNode);
			if( linkhash.find(key) == linkhash.end() ) {
				printf("Error: The same node name pairs are found.\n");
				exit(1);
			}
            linkhash[key] = link;
		}

		for(int i=0; i < nLine; i++) {

			if( tnt.lines[i].nNode < 2 ) {
				printf("No. of nodes < 2 in %s.", tnt.lines[i].name);
				continue;
			}
			length = 0;
			std::list<TRNNode>::iterator snode = tnt.lines[i].nodes.begin();
			std::list<TRNNode>::iterator enode = std::next(snode);
			while (enode != tnt.lines[i].nodes.end() ) {
				makeKey(key, snode->name, enode->name);
				std::unordered_map<std::string, LinkPtr>::iterator it = linkhash.find(key);
				if( it == linkhash.end() ) {
					makeKey(key, enode->name, snode->name);
					it = linkhash.find(key);
					if( it != linkhash.end() ) {
						printf("No link between %s and %s of line %s.\n", snode->name, enode->name, tnt.lines[i].name);
					} else {
						length += link->length;
					}
				} else {
					length += link->length;
				}
				snode = enode;
				enode = std::next(snode);
			}
			printf("%5d     %-10s%15.2lf%5d\n", tnt.lines[i].mode, tnt.lines[i].name, length, tnt.lines[i].frequency);
		}

	}catch (const std::exception& e ) {
		printf("%s\n", e.what());
		return 1;
	}
    return 0;
}
//---------------------------------------------------------------------------
