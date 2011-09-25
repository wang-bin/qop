#include "option.h"
#include "qtcompat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <qstring.h>

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
	const char *short_options = "ai:t:mnshHT:x:o:cd::C:";
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
	opts->out_dir="./";
	opts->auto_close=0;
	opts->help=0;
	opts->multi_thread=0;
	//opts->program_name="qop";
	opts->interval = 300;
	opts->steps=-1;
	opts->unit=2;
	opts->all_msg = false;

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
			case 'a': opts->all_msg = true; break;
			case 'i': {
				//QRegExp exp("(\\d+)(s|sec|secs|seconds)"); //qt2 is much different
				QString t(optarg);
				bool ok = false;
				if (t.endsWith("msec") || t.endsWith("msecs")) {
					t.remove(t.indexOf("msec"), 5);
					int msec = t.toInt(&ok);
					if (ok)
						opts->interval = msec;
				} else if (t.endsWith("s") || t.endsWith("sec") || t.endsWith("secs") || t.endsWith("seconds")) {
					t.remove(t.indexOf("s"), 7);
					float s = t.toFloat(&ok);
					if (ok)
						opts->interval = s*1000;
				} else {
					int msec = t.toInt(&ok);
					if (ok)
						opts->interval = msec;
				}
				qDebug("interval=%s", qPrintable(t));
				if (!ok)
					fprintf(stderr, "Wrong time format!\nUse Ns, Nsec, Nsecs, Nseconds, Nmsec, Nmsecs, N");
				break;
			}
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
				return opts;
			}
			case 'x': opts->x_file=optarg; break;
			case 'd': opts->diy=1; if(optarg!=NULL) opts->x_file=optarg; break;
			case 'I': opts->Stdin=1; break;
			case 'O': opts->Stdout=1; break;
			case 'o': opts->out_dir=optarg; break;
			case '?': printHelp(); exit(0);//opts->help=1;break;//
			default:
				break;
			}
		} while (c != -1);

	opts->optind=optind;

	if(opts->x_file!=NULL && strcmp("tar",opts->parser_type)==0) opts->parser_type="untar";

	return opts;
}
