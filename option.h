#ifndef OPTION_H
#define OPTION_H

#include <getopt.h>
#include "qany.h"
#include <qlist.h>

struct opts_s;
typedef struct opts_s *opts_t;

const struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"size",0,0,'s'},
		{"number",0,0,'n'},
		{"auto-close",0,0,'c'},
		{"multi-thread",0,0,'m'},
		{"extract",1,0,'x'},
		{"steps",1,0,'T'},
		{"parser",2,0,'t'},
		{"diy",2,0,'d'},
		{"stdin",0,0,'I'},
		{"stdout",0,0,'O'},
		{"hide",0,0,'H'},
		{0, 0, 0, 0}
	};

struct opts_s {
	int argc;                      /* number of non-option arguments */
	char **argv;
	char* program_name;

	const char* parser_type;
	int unit;
	int steps;
	int multi_thread;
	int auto_close;
	int hide;
	const char* x_file;
	int help;
	int optind;

	int diy;
	int Stdin;
	int Stdout;

};

//const char* opts_s::parser_type="tar";

extern opts_t opts_parse(int, char **);
extern void opts_free(opts_t);

//#define TEST_MYOPT 0
#if TEST_MYOPT
typedef void* (*do_func)();
enum ArgumentFlag {
	NoArgument=0,RequiredArgument,OptinalArgument
};

struct QOptionsPrivate
{
	const char* long_opt;
	char opt;
	QAny value;
	ArgumentFlag arg_flag;
	const char* description;
	do_func func_ptr;

};

class QOptions
{
public:
	QOptions(int argc,char** argv);
	~QOptions();

	QOptions& operator()(const char* name,const QAny& value,const char* description=0);
	QOptions& operator()(const char* name,const char* description=0);
	QAny& operator[](const char* name);

	void readCommand(int argc,char** argv);
	void doOptions();

private:
	QList<QOptionsPrivate*> opts, opts_in;

};
#endif //TEST_MYOPT

#endif // OPTION_H
