#ifndef MTGCOMPAT_H
#define MTGCOMPAT_H
#ifdef __cplusplus
extern "C" {
#endif
extern char *optarg;
extern int opterr, optind, optopt;

int getopt(int nargc, char * const nargv[], const char *ostr);
#ifdef __cplusplus
}
#endif
#endif