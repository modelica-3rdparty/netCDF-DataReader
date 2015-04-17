/*
 * Simple getopt variant, found as public domain
 * original written by Michael Tokarev
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
