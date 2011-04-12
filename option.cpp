#include "option.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void printHelp();

void opts_free(opts_t opts)
{
	if (!opts)
		return;
	if (opts->argv)
		free(opts->argv);
	free(opts);
}

opts_t opts_parse(int argc, char **argv)
{
	int option_index = 0;
	const char *short_options = "t:mnshHT:x:cd::C:";
	int c;
	opts_t opts;

	opts = (opts_t)calloc(1, sizeof(*opts));
	if (!opts) {
		fprintf(stderr,"%s: option structure allocation failed (%s)",argv[0], strerror(errno));
		fprintf(stderr, "\n");
		return 0;
	}

	opts->program_name = argv[0];
	opts->cmd=0;
	opts->parser_type="tar";
	opts->x_file=NULL;
	opts->auto_close=0;
	opts->help=0;
	opts->multi_thread=0;
	//opts->program_name="qop";
	opts->steps=-1;
	opts->unit=0;

	opts->argc = 0;
	opts->argv = (char**)calloc(argc + 1, sizeof(char *));
	if (!opts->argv) {
		fprintf(stderr,"%s: option structure argv allocation failed (%s)",argv[0], strerror(errno));
		fprintf(stderr, "\n");
		opts_free(opts);
		return 0;
	}

	do {
		c = getopt_long(argc, argv, short_options, long_options, &option_index);
		if (c < 0) continue;
			switch (c) {
			case 't': opts->parser_type=optarg; break;
			case 'm': opts->multi_thread=1; break;
			case 'n': opts->unit=1; break;
			case 's': opts->unit=0; break;
			case 'T': opts->steps=atoi(optarg); break;
			case 'h': printHelp(); exit(0);//opts->help=1;break;//
			case 'H': opts->hide=1; break;
			case 'c': opts->auto_close=1; break;
			case 'C': {
				opts->x_file=0;
				opts->optind=optind;
				//std::string s;
				opts->cmd=new char[256];
				opts->cmd[0]=0;
				for(int i=optind-1;i<argc;++i) {
					strcat(strcat(opts->cmd,argv[i])," ");
					//s.append(argv[i]).append(" ");
				}
				//opts->cmd=const_cast<char*>(s.c_str());
				ZDEBUG("cmd: %s",opts->cmd);
				//opts->steps=CommandParser(opts->cmd)::size()
				return opts;
			}
			case 'x': opts->x_file=optarg; break;
			case 'd': opts->diy=1; if(optarg!=NULL) opts->x_file=optarg; break;
			case 'I': opts->Stdin=1; break;
			case 'O': opts->Stdout=1; break;
			case '?': printHelp(); exit(0);//opts->help=1;break;//
			default:
				break;
			}
		} while (c != -1);

	opts->optind=optind;

	if(opts->x_file!=NULL && strcmp("tar",opts->parser_type)==0) opts->parser_type="untar";

	return opts;
}


/***************************************************************************************/
#if TEST_MYOPT

QOptions::QOptions(int argc, char **argv)
{
	readCommand(argc,argv);
}

QOptions::~QOptions()
{
	qDeleteAll(opts);
	opts.clear();
}

void QOptions::readCommand(int argc, char **argv)
{
	//opts.resize(0);

}

void QOptions::doOptions()
{
	QList<QOptionsPrivate*>::Iterator it;
	for(it=opts_in.begin();it!=opts_in.end();++it) {
		(*it)->func_ptr();
	}
}

QOptions& QOptions::operator ()(const char* name,const QAny& value,const char* description)
{
	//opts.insert();
	return *this;
}

QOptions& QOptions::operator ()(const char* name,const char* description)
{
	return *this;

}

QAny& QOptions::operator [](const char* name)
{
}


#endif //TEST_MYOPT
