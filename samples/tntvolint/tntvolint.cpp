//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <memory>
#include <stdexcept>
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

class LinkVol {
public:
    char name[11];
    int vol1;
    int vol2;
};

int main(int argc, char* argv[])
{

	LinkPtr link;
	StradaINT s_int;
	StradaTNT tnt;
	char key[30];
	int nLine;
    std::vector<std::shared_ptr<LinkVol>> links;

    std::unordered_map<std::string, std::shared_ptr<LinkVol>> linkhash;

	if( argc != 3 ) {
        usage(argv[0]);
		exit(1);
	}

	try {
		s_int.Read(argv[1]);
		tnt.Read(argv[2]);
		nLine = tnt.nLine;

	}catch (const std::exception& e ) {
		printf("%s\n", e.what());
        return 1;
	}

        links.resize(s_int.nLink);

		for(auto& link: s_int.links) {
			makeKey(key, link->sNode, link->eNode);
			std::shared_ptr<LinkVol> lkvol = std::make_shared<LinkVol>();
            strncpy(lkvol->name, link->name, 10);
            lkvol->vol1 = 0;
            lkvol->vol2 = 0;
			linkhash[std::string(key)] = lkvol;
		}

		for(int i=0; i < nLine; i++) {
			TRNLine* line = tnt.getLine(i);
			if( tnt.lines[i].nNode < 2 ) {
				fprintf(stderr, "No. of nodes < 2 in %s.", tnt.lines[i].name);
				continue;
			}
			std::list<TRNNode>::iterator snode = line->nodes.begin();
			std::list<TRNNode>::iterator enode = std::next(snode);
			while( enode != line->nodes.end() ) {
				makeKey(key, snode->name, enode->name);
				if( linkhash.find(key) != linkhash.end() ) {
					std::shared_ptr<LinkVol> lkvol = linkhash[key];
					lkvol->vol1 += line->frequency;
				} else {
					makeKey(key, enode->name, snode->name);
					if( linkhash.find(key) != linkhash.end()  ) {
						std::shared_ptr<LinkVol> lkvol = linkhash[key];
                        lkvol->vol2 += tnt.lines[i].frequency;
					} else {
						printf("No link between %s and %s of line %s.\n",
										snode->name, enode->name, line->name);
					}
				}
				snode = enode;
			}
		}

    for(unsigned int counter =0; counter < links.size() ; counter++) {
        if (links[counter]->vol1 > 0 || links[counter]->vol2 > 0)
            printf("%-10s%10d%10d\n", links[counter]->name, links[counter]->vol1, links[counter]->vol2);
    }
    return 0;
}
//---------------------------------------------------------------------------
