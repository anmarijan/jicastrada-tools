#ifndef stgetoptH
#define stgetoptH

extern char* optarg;
extern int optind;

int get_opt(int argc, char** argv, const char* opt_list);

#endif
