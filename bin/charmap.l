%{
#include "fontmaker.h"

static unsigned int  *_glyphs        = NULL;
static unsigned int   _glyphs_count  = 0;

static inline int _add(unsigned int glyph);

%}

%%
##.*            ;
[ \t\n]         ;
[^\ \n\t]+      {
                  char utf8[8];
                  unsigned int code;

                  snprintf(utf8, sizeof(utf8), "%s", yytext);
                  code = fm_utf8_to_unicode(utf8);
                  if (code == 0)
                    {
                       fprintf(stderr, "*** Failed to convert utf8 [%s] to unicode\n", utf8);
                    }
                  else
                    {
                       if (!_add(code))
                         {
                            fprintf(stderr, "*** Failed to register codepoint [%u]\n", code);
                         }
                    }
                }
%%


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

/*============================================================================*
 *                                     API                                    *
 *============================================================================*/

int
fm_charmap_init(void)
{
   return 0;
}

void
fm_charmap_shutdown(void)
{
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

int
fm_charmap_generate(void)
{
   FILE *fh;
   Fm_Opts *opts;

   opts = fm_opts_get();
   fh = fopen(opts->map_file, "r");
   if (!fh)
     {
        fprintf(stderr,"*** Failed to open charmap file [%s]\n", opts->map_file);
        return 1;
     }
   yyin = fh;
   yylex();
   fclose(fh);

   return 0;
}

