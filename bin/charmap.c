#include "fontmaker.h"

static unsigned int  *_glyphs        = NULL;
static unsigned int   _glyphs_count  = 0;
static char         **_strings       = NULL;
static unsigned int   _strings_count = 0;


/*============================================================================*
 *                          Dynamic Array Management                          *
 *============================================================================*/

static inline int
_add(unsigned int glyph)
{
   static unsigned int size_max = 0;
   void *tmp;
   unsigned int len;

#define _REALLOC_ELEMS_STEP 8
   if (size_max == _glyphs_count)
     {
        len = (_glyphs_count + _REALLOC_ELEMS_STEP) * sizeof(unsigned int);
        tmp = realloc(_glyphs, len);
        if (!tmp) return 0;
        size_max += _REALLOC_ELEMS_STEP;
        _glyphs = tmp;
     }
#undef _REALLOC_ELEMS_STEP

   _glyphs[_glyphs_count++] = glyph;
   return 1;
}

static inline int
_string_add(const char *str)
{
   static unsigned int size_max = 0;
   void *tmp;
   unsigned int len;

#define _REALLOC_ELEMS_STEP 8
   if (size_max == _strings_count)
     {
        len = (_strings_count + _REALLOC_ELEMS_STEP) * sizeof(char *);
        tmp = realloc(_strings, len);
        if (!tmp) return 0;
        size_max += _REALLOC_ELEMS_STEP;
        _strings = tmp;
     }
#undef _REALLOC_ELEMS_STEP

   tmp = strdup(str);
   if (!tmp) return 0;
   _strings[_strings_count++] = tmp;

   return 1;
}

/*============================================================================*
 *                                     API                                    *
 *============================================================================*/

int
fm_charmap_init(void)
{
   Fm_Opts *opts;
   FILE *exe;
   int ret = 0;
   char buf[1024], path[PATH_MAX];
   char *ptr;
   unsigned int code;
   unsigned int i = 1;

   opts = fm_opts_get();

   snprintf(path, sizeof(path), "%s/fontmaker_parse", main_exec_path_get());
   if (access(path, F_OK) == -1)
     {
        fprintf(stderr, "*** Failed to find [%s]\n", path);
        return 127;
     }
   snprintf(buf, sizeof(buf), "%s %s", path, opts->map_file);
   exe = popen(buf, "r");
   if (!exe)
     {
        fprintf(stderr, "*** Failed to popen() [%s]\n", buf);
        return 1;
     }

   while ((ptr = fgets(buf, sizeof(buf), exe)) != NULL)
     {
        /* Remove new line */
        ptr = strrchr(buf, '\n'); *ptr = 0;

        /* Do not treat error character (always at the end of the charmap) */
        if (!strcmp(opts->error_char, buf))
          continue;

        code = fm_utf8_to_unicode(buf);
        if (code == 0)
          {
             fprintf(stderr, "*** Failed to convert UTF-8 string [%s] "
                     "to unicode character\n", buf);
             ret = 3;
             break;
          }

        if (opts->verbosity >= 2)
          printf("adding to charmap [%s] -> %u at index %u\n", buf, code, i);
        if (!_add(code))
          {
             fprintf(stderr, "*** Failed to register code [%u]\n", code);
             ret = 3;
             break;
          }
        if (!_string_add(buf))
          {
             fprintf(stderr, "*** Failed to register string [%s]\n", buf);
             ret = 3;
             break;
          }
        i++;
     }
   if (ferror(exe))
     {
        fprintf(stderr, "*** An error occured while reading STDOUT of [%s]\n", buf);
        ret = 2;
     }
   pclose(exe);

   /* Add whitespace */
   code = fm_utf8_to_unicode(" ");
   if (code == 0)
     {
        fprintf(stderr, "*** Failed to convert UTF-8 string [\" \"] "
                "to unicode character\n");
        ret = 4;
     }
   if (!_add(code))
     {
        fprintf(stderr, "*** Failed to register code [%u]\n", code);
        ret = 4;
     }
   if (!_string_add(" "))
     {
        fprintf(stderr, "*** Failed to registring whitespace\n");
        ret = 4;
     }

   /* Add error char to the map */
   code = fm_utf8_to_unicode(opts->error_char);
   if (code == 0)
     {
        fprintf(stderr, "*** Failed to convert UTF-8 string [%s] "
                "to unicode character\n", opts->error_char);
        ret = 5;
     }
   if (!_add(code))
     {
        fprintf(stderr, "*** Failed to register code [%u]\n", code);
        ret = 5;
     }
   if (!_string_add(opts->error_char))
     {
        fprintf(stderr, "*** Failed to register string [%s]\n",
                opts->error_char);
        ret = 5;
     }

   return ret;
}

void
fm_charmap_shutdown(void)
{
   unsigned int i;

   for (i = 0; i < _strings_count; i++)
     free(_strings[i]);
   free(_strings);
   _strings = NULL;
   _strings_count = 0;

   free(_glyphs);
   _glyphs = NULL;
   _glyphs_count = 0;
}

void
fm_charmap_foreach(Fm_Charmap_Func func)
{
   unsigned int i;

   if (!func) return;
   for (i = 0; i < _glyphs_count; i++)
     {
        if (!func(_glyphs[i], i + 1))
          break;
     }
}

unsigned int
fm_charmap_count_get(void)
{
   return _glyphs_count;
}

