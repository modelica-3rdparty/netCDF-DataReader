/*
 * Simple getopt variant, found as public domain
 * original written by Michael Tokarev
 */

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

extern int getopt(int argc, char *const *argv, const char *opts);
