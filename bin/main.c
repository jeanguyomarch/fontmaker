#include "fontmaker.h"

static char *_exe_cwd = NULL;

const char *
main_exec_path_get(void)
{
   return _exe_cwd;
}

int
main(int    argc,
     char **argv)
{
   int status;

   _exe_cwd = strdup(dirname(argv[0]));
   if (!_exe_cwd)
     {
        status = errno;
        fprintf(stderr, "*** Failed to strdup()\n");
        goto err_opts;
     }

   /* Getopt */
   status = fm_opts_init(argc, argv);
   if (status != 0) goto err_opts;

   /* Freetype setup */
   status = fm_freetype_init();
   if (status != 0) goto err_freetype;

   /* Charmap status */
   status = fm_charmap_init();
   if (status != 0) goto err_charmap;

   /* Output status */
   status = fm_output_init();
   if (status != 0) goto err_output;

   /* "Scripted" action */
   fm_output_size_adjust();
   fm_output_generate_c();
   fm_output_generate_h();

   status = 0;

   /* Shutdown */
   fm_output_shutdown();
err_output:
   fm_charmap_shutdown();
err_charmap:
   fm_freetype_shutdown();
err_freetype:
   fm_opts_shutdown();
err_opts:
   free(_exe_cwd);
   return status;
}

