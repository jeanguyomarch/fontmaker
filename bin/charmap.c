#include "fontmaker.h"

static unsigned int  *_glyphs        = NULL;
static unsigned int   _glyphs_count  = 0;


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

int
fm_charmap_generate(void)
{
   Fm_Opts *opts;
   int ret = 0;
   char *ptr, c;
   unsigned int code;
   unsigned int i;
   char utf8[8];

   memset(utf8, 0, sizeof(utf8));

   /* Built-in set (from ' ' to '~' (7-bits ASCII)) */
   for (i = 0x20; i <= 0x7e; i++)
     {
        utf8[0] = i; /* Can only be on one byte */
        code = fm_utf8_to_unicode(utf8);
        if (code == 0)
          {
             fprintf(stderr, "*** Failed to convert UTF-8 [%s]\n", utf8);
             return 1;
          }
        if (!_add(code))
          {
             fprintf(stderr, "*** Failed to register codepoint [%u]\n", code);
             return 1;
          }
     }

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

