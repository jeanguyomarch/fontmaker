#include "fontmaker.h"

static FILE *_gperf = NULL;

#undef W
#define W(f, ...) fprintf(_gperf, f "\n", ## __VA_ARGS__)

int
fm_gperf_init(void)
{
   _gperf = fopen(CMAKE_TMP_FILE, "w");
   if (!_gperf)
     {
        fprintf(stderr, "*** Failed to open \"" CMAKE_TMP_FILE "\"\n");
        return 1;
     }
   return 0;
}

void
fm_gperf_shutdown(void)
{
   if (_gperf)
     {
        fclose(_gperf);
        _gperf = NULL;
     }
}

static int
_kw_cb(FT_ULong      code,
       unsigned int  idx,
       const char   *utf8)
{
   UNUSED(code); UNUSED(idx);

   if (*utf8 == '"')
     W("\"\\\"\""); // "\""
   else
     W("\"%s\"", utf8);

   return 1;
}

void
fm_gperf_config_gen(void)
{
   W("%%{"); // %{
   W("#include <string.h>");
   W("%%}"); // %}
   W("%%%%"); // %%
   fm_charmap_foreach(_kw_cb);
   W("%%%%"); // %%
}

int
fm_gperf_compile(FILE *fh)
{
   Fm_Opts *opts;
   const char *lang;
   char prog[128];
   FILE *exe;
   char buf[4096];
   char *ptr;

   /* Important to sync */
   fflush(_gperf);

   /* Determine language */
   opts = fm_opts_get();
   lang = (opts->hpp_file) ? "C++" : "C";

   /* Gperf command */
   snprintf(prog, sizeof(prog),
            "gperf -L %s %s",
            lang, CMAKE_TMP_FILE);

   /* Execute gperf and read it */
   exe = popen(prog, "r");
   if (!exe)
     {
        fprintf(stderr, "*** Failed to popen() [%s]\n", prog);
        return 1;
     }
   while ((ptr = fgets(buf, sizeof(buf), exe)) != NULL)
     {
        if (!strncmp(ptr, "#line ", 6)) continue;
        fprintf(fh, "%s", ptr);
     }
   pclose(exe);
   return 0;
}

