#ifndef OPTION_H
#define OPTION_H

//#include "qany.h"
#include <qlist.h>
#include <qstring.h>
#include <string>
#include <getopt.h>

struct opts_s;
typedef struct opts_s *opts_t;

const struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"size",0,0,'s'},
		{"number",0,0,'n'},
		{"auto-close",0,0,'c'},
		{"cmd",0,0,'C'},
		{"multi-thread",0,0,'m'},
		{"extract",1,0,'x'},
		{"steps",1,0,'T'},
		{"parser",2,0,'t'},
		{"diy",2,0,'d'},
		{"stdin",0,0,'I'},
		{"stdout",0,0,'O'},
		{"outdir",0,0,'o'},
		{"hide",0,0,'H'},
		{0, 0, 0, 0}
	};

struct opts_s {
	int argc;                      /* number of non-option arguments */
	char **argv;
	char* program_name;

	char* cmd;

	const char* parser_type;
	int unit;
	int steps;
	int multi_thread;
	int auto_close;
	int hide;
	const char* x_file;
	const char* out_dir; //default is ./
	int help;
	int optind;

	int diy;
	int Stdin;
	int Stdout;

};

//const char* opts_s::parser_type="tar";

extern opts_t opts_parse(int, char **);
extern void opts_free(opts_t);


#endif // OPTION_H
