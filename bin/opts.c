#include "fontmaker.h"

static Fm_Opts _opts;

#define OPT_BITS_DEFAULT 8

static struct option _options[] =
{
     {"font-file",  required_argument, 0, 'f'},
     {"gc",         required_argument, 0, 'C'},
     {"gh",         required_argument, 0, 'H'},
     {"map-file",   required_argument, 0, 'm'},
     {"bits",       required_argument, 0, 'b'},
     {"prefix",     required_argument, 0, 'p'},
     {"attribute",  required_argument, 0, 'a'},
     {"font-size",  required_argument, 0, 's'},
     {"progmem",    no_argument,       0, 'P'},
     {"verbose",    no_argument,       0, 'v'},
     {"help",       no_argument,       0, 'h'},
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
           "    --gh, -H          Output C code file path [DEFAULT: NONE]\n"
           "    --bits, -b        On how many bits is the output font coded? [DEFAULT: %i]\n"
           "    --prefix, -p      Prefix to prepend to the generated functions [DEFAULT: NONE]\n"
           "    --attribute, -a   Add an attribute to the function [DEFAULT: NONE]\n"
           "\n",
           OPT_BITS_DEFAULT);
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
        c = getopt_long(argc, argv, "f:C:H:m:a:b:p:s:vhr", _options, &opt_idx);
        if (c == -1) break;

        switch (c)
          {
           case 'v':
              ++_opts.verbosity;
              break;

           case 'P':
              _opts.progmem = 1;
              break;

           case 'a':
              _opts.attribute = strdup(optarg);
              break;

           case 'f':
              _opts.font_file = strdup(optarg);
              break;

           case 'm':
              _opts.map_file = strdup(optarg);
              break;

           case 'C':
              _opts.c_file = strdup(optarg);
              break;

           case 'H':
              _opts.h_file = strdup(optarg);
              break;

           case 'b':
              _opts.font_bits = atoi(optarg);
              break;

           case 'p':
              _opts.prefix = strdup(optarg);
              break;

           case 's':
              _opts.font_size = atoi(optarg);
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

   /* Default arguments */
   if (_opts.font_bits <= 0)
     {
        if (_opts.verbosity >= 2)
          fprintf(stdout, "[--font-bits] was [%i] defaults to [%i]\n",
                  _opts.font_bits, OPT_BITS_DEFAULT);
        _opts.font_bits = OPT_BITS_DEFAULT;
     }
   if (_opts.prefix == NULL)
     {
        if (_opts.verbosity >= 2)
          fprintf(stdout, "[--prefix] was [] defaults to [\"\"]\n");
        _opts.prefix = strdup("");
     }
   if (_opts.attribute == NULL)
     {
        if (_opts.verbosity >= 2)
          fprintf(stdout, "[--attribute] was [] defaults to [\"\"]\n");
        _opts.attribute = strdup("");
     }
   // TODO Check memory is allocated

   return 0;
}

void
fm_opts_shutdown(void)
{
   free(_opts.font_file);
   free(_opts.c_file);
   free(_opts.h_file);
   free(_opts.map_file);
   free(_opts.prefix);
   free(_opts.attribute);
   memset(&_opts, 0, sizeof(Fm_Opts));
}

Fm_Opts *
fm_opts_get(void)
{
   return &_opts;
}

