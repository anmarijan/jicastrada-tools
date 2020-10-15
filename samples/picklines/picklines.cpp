#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <list>
#include <unordered_map>
#include "StradaTNT.h"
#include "StradaINT.h"

/////////////////////////////////////////////////////////////////////////////////////
// Global
/////////////////////////////////////////////////////////////////////////////////////
void errmsg() {
	printf("Line Select for Transit Line of JICA STRADA (%s)\n", __DATE__);
	puts("");
	puts("Usage  : picklines network.int transit.tnt link1 link2 ..");
	puts("network.int     JICA STRADA link file");
	puts("transit.tnt     JICA STRADA transit line file");
	puts("link1 link2 ..  link name");
	puts("");
	puts("This program shows transit lines that go through link1 link2 ..");
}
/////////////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {

	StradaTNT stnt;
	StradaINT sint;

	LinkPtr link;
	TRNLine* line;

	std::unordered_map<std::string, LinkPtr> linkhash;
	std::list<LinkPtr> linklist;

	if( argc < 4) {
		errmsg();
		exit(1);
	}

	try {
		sint.Read(argv[1]);
		stnt.Read(argv[2]);
	} catch(std::runtime_error& e) {
		fprintf(stderr, "%s\n", e.what());
		exit(2);
	}
	// linkhash
	for(const auto& link: sint.links) {
		linkhash[link->name] = link;
	}

	// set linklist from argment
	for(int i = 3; i < argc; i++ ) {
		std::unordered_map<std::string, LinkPtr>::iterator it = linkhash.find(argv[i]);
		if( it == linkhash.end() ) {
			fprintf(stderr, "Error: Cannot find %s in %s\n", argv[i], argv[1]);
			exit(3);
		}
		linklist.push_back(it->second);
	}

	// Pickup lines
	for(int i=0; i < stnt.nLine; i++) {
		line = stnt.getLine(i);
		bool check = false;
		for(const auto& link : linklist ) {
			bool inside_check = false;
			std::list<TRNNode>::iterator snode = line->nodes.begin();
			std::list<TRNNode>::iterator enode = std::next(snode);
			while (enode != line->nodes.end() ) {
				if( strncmp(snode->name, link->sNode,10)==0 && strncmp(enode->name, link->eNode,10) == 0) {
						inside_check = true;
						break;
				} else if (strncmp(enode->name, link->sNode,10)==0 && strncmp(snode->name, link->eNode,10) == 0) {
						inside_check = true;
						break;
				}
				snode = enode;
				enode = std::next(snode);
			}
			if( inside_check == true) {
				check = true;
				break;
			}
		}
		if (check == true) {
			fprintf(stdout, "%-10s%10d\n", line->name, line->frequency);
		}
	}
	return 0;
}
