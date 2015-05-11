#include "fontmaker.h"

static Fm_Opts _opts;

static struct option _options[] =
{
     {"font-file",     required_argument, 0, 'f'},
     {"gc",            required_argument, 0, 'C'},
     {"gh",            required_argument, 0, 'H'},
     {"ghpp",          required_argument, 0, 'x'},
     {"map-file",      required_argument, 0, 'm'},
     {"prefix",        required_argument, 0, 'p'},
     {"attribute",     required_argument, 0, 'a'},
     {"font-size",     required_argument, 0, 's'},
     {"namespace",     required_argument, 0, 'n'},
     {"progmem",       no_argument,       0, 'P'},
     {"gpgm",          no_argument,       0, 'G'},
     {"verbose",       no_argument,       0, 'v'},
     {"help",          no_argument,       0, 'h'},
     {0, 0, 0, 0}
};



static void
_help(FILE *stream)
{
   fprintf(stream,
           "fontmaker 0.1:\n"
           "\n"
           "    --help, -h        Display this message\n"
           "    --verbose, -v     Increases verbosity\n"
           "    --progmem, -P     Places the generated bitmap in ROM instead of RAM when possible (AVR)\n"
           "\n"
           "    --font-file, -f   TTF input file path [REQUIRED]\n"
           "    --map-file, -m    File containing the glyphs to create [REQUIRED]\n"
           "    --font-size, -s   Font size (in points) to generate [REQUIRED]\n"
           "    --gc, -C          Output C code file path [DEFAULT: NONE]\n"
           "    --gh, -H          Output C header file path [DEFAULT: NONE]\n"
           "    --ghpp, -x        Output C++ header file path [DEFAULT: NONE]\n"
           "    --gpgm, -G        Output PGM images per glyph [DEFAULT: NO]\n"
           "    --namespace, -n   Provide a namespace used when C++ is generated [DEFAULT: NONE]\n"
           "    --prefix, -p      In C: prefix to prepend to the generated functions. [DEFAULT: NONE]\n"
           "                      In C++: name of the class. [DEFAULT: Font]\n"
           "    --attribute, -a   Add an attribute to the function [DEFAULT: NONE]\n"
           "\n");
}


int
fm_opts_init(int    argc,
             char **argv)
{
   int c, opt_idx = 0;

   if (argc == 1)
     {
        fprintf(stderr, "*** fontmaker requires arguments!\n");
        _help(stderr);
        return 1;
     }
   memset(&_opts, 0, sizeof(Fm_Opts));

   while (1)
     {
        c = getopt_long(argc, argv, "Gf:C:x:H:m:a:p:n:s:vhr", _options, &opt_idx);
        if (c == -1) break;

        switch (c)
          {
           case 'v':
              ++_opts.verbosity;
              break;

           case 'P':
              _opts.progmem = 1;
              break;

           case 'G':
              _opts.pgm = 1;
              break;

           case 'n':
              _opts.namespace_name = optarg;
              break;

           case 'a':
              _opts.attribute = optarg;
              break;

           case 'f':
              _opts.font_file = optarg;
              break;

           case 'm':
              _opts.map_file = optarg;
              break;

           case 'C':
              _opts.c_file = optarg;
              break;

           case 'H':
              _opts.h_file = optarg;
              break;

           case 'p':
              _opts.prefix = optarg;
              break;

           case 's':
              _opts.font_size = atoi(optarg);
              break;

           case 'x':
              _opts.hpp_file = optarg;
              break;

           case 'h':
              _help(stdout);
              exit(EXIT_SUCCESS);

           case '?':
              exit(errno);

           default:
              c = errno;
              fprintf(stderr, "*** Getopt failed with error [%s]\n",
                      strerror(c));
              return c;
          }
     }

   /* Unprocessed arguments */
   if (optind < argc)
     {
        fprintf(stderr, "*** Unprocessed options!\n");
        _help(stderr);
        return 1;
     }

   /* Check for required arguments */
   if (_opts.font_file == NULL)
     {
        fprintf(stderr, "*** --font-file option is required!\n");
        _help(stderr);
        return 1;
     }
   if (_opts.map_file == NULL)
     {
        fprintf(stderr, "*** --map-file option is required!\n");
        _help(stderr);
        return 1;
     }
   if (_opts.font_size <= 0)
     {
        fprintf(stderr, "*** --font-size option is required!\n");
        _help(stderr);
        return 1;
     }

   return 0;
}

void
fm_opts_shutdown(void)
{
   memset(&_opts, 0, sizeof(Fm_Opts));
}

Fm_Opts *
fm_opts_get(void)
{
   return &_opts;
}

