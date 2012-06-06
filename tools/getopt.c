/* $Id: getopt.c,v 1.2 2007/01/07 23:19:19 mjt Exp $
 * Simple getopt() implementation.
 *
 * Standard interface:
 *  extern int getopt(int argc, char *const *argv, const char *opts);
 *  extern int optind;    current index in argv[]
 *  extern char *optarg;  argument for the current option
 *  extern int optopt;    the current option
 *  extern int opterr;    to control error printing
 *
 * Some minor extensions:
 *  ignores leading `+' sign in opts[] (unemplemented GNU extension)
 *  handles optional arguments, in form "x::" in opts[]
 *  if opts[] starts with `:', will return `:' in case of missing required
 *    argument, instead of '?'.
 *
 * Compile with -DGETOPT_NO_OPTERR to never print errors internally.
 * Compile with -DGETOPT_NO_STDIO to use write() calls instead of fprintf() for
 *  error reporting (ignored with -DGETOPT_NO_OPTERR).
 * Compile with -DGETOPT_CLASS=static to get static linkage.
 * Compile with -DGETOPT_MY to redefine all visible symbols to be prefixed
 *  with "my_", like my_getopt instead of getopt.
 * Compile with -DTEST to get a test executable.
 *
 * Written by Michael Tokarev.  Public domain.
 */

#include <string.h>
#include <stdio.h>

char *optarg /* = NULL */;
int optind = 1;
int opterr = 1;
int optopt;

static char *nextc /* = NULL */;


static void printerr(char *const *argv, const char *msg) {
  if (opterr)
     fprintf(stderr, "%s: %s -- %c\n", argv[0], msg, optopt);
}


int getopt(int argc, char *const *argv, const char *opts) {
  char *p;

  optarg = 0;
  if (*opts == '+') /* GNU extension (permutation) - isn't supported */
    ++opts;

  if (!optind) {  /* a way to reset things */
    nextc = 0;
    optind = 1;
  }

  if (!nextc || !*nextc) {   /* advance to the next argv element */
    /* done scanning? */
    if (optind >= argc)
      return -1;
    /* not an optional argument */
    if (argv[optind][0] != '-')
      return -1;
    /* bare `-' */
    if (argv[optind][1] == '\0')
      return -1;
    /* special case `--' argument */
    if (argv[optind][1] == '-' && argv[optind][2] == '\0') {
      ++optind;
      return -1;
    }
    nextc = argv[optind] + 1;
  }

  optopt = *nextc++;
  if (!*nextc)
    ++optind;
  p = strchr(opts, optopt);
  if (!p || optopt == ':') {
    printerr(argv, "illegal option");
    return '?';
  }
  if (p[1] == ':') {
    if (*nextc) {
      optarg = nextc;
      nextc = NULL;
      ++optind;
    }
    else if (p[2] != ':') {	/* required argument */
      if (optind >= argc) {
        printerr(argv, "option requires an argument");
        return *opts == ':' ? ':' : '?';
      }
      else
        optarg = argv[optind++];
    }
  }
  return optopt;
}

#ifdef TEST

int main(int argc, char **argv) {
  int c;
  while((c = getopt(argc, argv, "ab:c::")) != -1) switch(c) {
  case 'a':
  case 'b':
  case 'c':
    printf("option %c %s\n", c, optarg ? optarg : "(none)");
    break;
  default:
    return -1;
  }
  for(c = optind; c < argc; ++c)
    printf("non-opt: %s\n", argv[c]);
  return 0;
}

#endif
