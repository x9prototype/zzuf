/*
 *  zzuf - general purpose fuzzer
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 *  main.c: main program
 */

#include "config.h"

#if defined HAVE_STDINT_H
#   include <stdint.h>
#elif defined HAVE_INTTYPES_H
#   include <inttypes.h>
#endif
#if defined(HAVE_GETOPT_H)
#   include <getopt.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "random.h"

static void set_ld_preload(char const *);
static void version(void);
#if defined(HAVE_GETOPT_H)
static void usage(void);
#endif

int main(int argc, char *argv[])
{
    char **newargv;

#if defined(HAVE_GETOPT_H)
    for(;;)
    {
#   ifdef HAVE_GETOPT_LONG
#       define MOREINFO "Try `%s --help' for more information.\n"
        int option_index = 0;
        static struct option long_options[] =
            {
                /* Long option, needs arg, flag, short option */
                { "include", 1, NULL, 'i' },
                { "exclude", 1, NULL, 'e' },
                { "seed",    1, NULL, 's' },
                { "ratio",   1, NULL, 'r' },
                { "debug",   1, NULL, 'd' },
                { "help",    0, NULL, 'h' },
                { "version", 0, NULL, 'v' },
            };

        int c = getopt_long(argc, argv, "i:e:s:r:dhv",
                            long_options, &option_index);
#   else
#       define MOREINFO "Try `%s -h' for more information.\n"
        int c = getopt(argc, argv, "i:e:s:r:dhv");
#   endif
        if(c == -1)
            break;

        switch(c)
        {
        case 'i': /* --include */
            setenv("ZZUF_INCLUDE", optarg, 1);
            break;
        case 'e': /* --exclude */
            setenv("ZZUF_EXCLUDE", optarg, 1);
            break;
        case 's': /* --seed */
            setenv("ZZUF_SEED", optarg, 1);
            break;
        case 'r': /* --ratio */
            setenv("ZZUF_RATIO", optarg, 1);
            break;
        case 'd': /* --debug */
            setenv("ZZUF_DEBUG", "1", 1);
            break;
        case 'h': /* --help */
            usage();
            return 0;
        case 'v': /* --version */
            version();
            return 0;
        default:
            printf("%s: invalid option -- %c\n", argv[0], c);
            printf(MOREINFO, argv[0]);
            return 1;
        }
    }
#else
#   define MOREINFO "Usage: %s message...\n"
    int optind = 1;
#endif

    if(optind >= argc)
    {
        printf("%s: missing argument\n", argv[0]);
        printf(MOREINFO, argv[0]);
        return -1;
    }

    /* Create new argv */
    newargv = malloc((argc - optind + 1) * sizeof(char *));
    memcpy(newargv, argv + optind, (argc - optind) * sizeof(char *));
    newargv[argc - optind] = (char *)NULL;

    /* Preload libzzuf.so */
    set_ld_preload(argv[0]);

    /* Call our process */
    if(execvp(newargv[0], newargv))
    {
        perror(newargv[0]);
        return -1;
    }

    return 0;    
}

static void set_ld_preload(char const *progpath)
{
    char *libpath, *tmp;
    int len = strlen(progpath);

    libpath = malloc(len + strlen("/.libs/libzzuf.so") + 1);
    strcpy(libpath, progpath);
    tmp = strrchr(libpath, '/');
    strcpy(tmp ? tmp + 1 : libpath, ".libs/libzzuf.so");
    if(access(libpath, R_OK) == 0)
    {
        setenv("LD_PRELOAD", libpath, 1);
        return;
    }
    free(libpath);

    setenv("LD_PRELOAD", LIBDIR "/libzzuf.so", 1);
}

static void version(void)
{
    printf("zzuf %s\n", VERSION);
    printf("Copyright (C) 2006 Sam Hocevar <sam@zoy.org>\n");
    printf("This is free software.  You may redistribute copies of it under the\n");
    printf("terms of the Do What The Fuck You Want To Public License, Version 2\n");
    printf("<http://sam.zoy.org/wtfpl/>.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
    printf("\n");
    printf("Written by Sam Hocevar. Report bugs to <sam@zoy.org>.\n");
}

#if defined(HAVE_GETOPT_H)
static void usage(void)
{
    printf("Usage: zzuf [ -vdh ] [ -i include ] [ -e exclude ]\n");
    printf("                     [ -r ratio ] [ -s seed ] COMMAND [ARGS]...\n");
    printf("Run COMMAND and randomly fuzz its input files.\n");
    printf("\n");
    printf("Mandatory arguments to long options are mandatory for short options too.\n");
#   ifdef HAVE_GETOPT_LONG
    printf("  -i, --include <regex>  only fuzz files matching <regex>\n");
    printf("  -e, --exclude <regex>  do not fuzz files matching <regex>\n");
    printf("  -r, --ratio <ratio>    bit fuzzing ratio (default 0.004)\n");
    printf("  -s, --seed <seed>      random seed (default 0)\n");
    printf("  -d, --debug            print debug messages\n");
    printf("  -h, --help             display this help and exit\n");
    printf("  -v, --version          output version information and exit\n");
#   else
    printf("  -i <regex>  only fuzz files matching <regex>\n");
    printf("  -e <regex>  do not fuzz files matching <regex>\n");
    printf("  -r <ratio>  bit fuzzing ratio (default 0.004)\n");
    printf("  -s <seed>   random seed (default 0)\n");
    printf("  -d          print debug messages\n");
    printf("  -h          display this help and exit\n");
    printf("  -v          output version information and exit\n");
#   endif
    printf("\n");
    printf("Written by Sam Hocevar. Report bugs to <sam@zoy.org>.\n");
}
#endif

