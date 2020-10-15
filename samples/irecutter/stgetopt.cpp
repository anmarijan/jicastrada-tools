#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* optarg;
int optind;

int get_opt(int argc, char** argv, const char* opt_list) {
	char c;
	char* cp;
	static int others = 0;

	if(others) {
		c = argv[optind][others];
		if (argv[optind][others+1] != '\0' ) others++;
		else {
			others = 0;
			optind++;
		}
		optarg = NULL;
		return c;
	}

	if( optind >= argc ) return EOF;
	if( argv[optind][0] == '-' && argv[optind][1] != '\0') {
		c = argv[optind][1];
		cp = strchr((char*)opt_list, c);
		if( cp == NULL || *cp == ':' ) {
			optarg = argv[optind];
			optind++;
			return EOF;
		} else {
			if(cp[1] == ':') {
				if( argv[optind][2] != '\0' ) {
					optarg = argv[optind] + 2;
					optind++;
					return c;
				} else if (optind < argc - 1 ){
					optarg = argv[optind+1];
					optind += 2;
					return c;
				}
			} else {
				if( argv[optind][2] != '\0' ) {
					optarg = NULL;
					others = 2;
					return c;
				} else {
					optarg = NULL;
					optind++;
					return c;
				}
			}
		}
	}
	optarg = argv[optind];
	optind++;
	return EOF;
}
