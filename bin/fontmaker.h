#ifndef _FONTMAKER_H_
#define _FONTMAKER_H_

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include <unistd.h>
#include <stdint.h>
#include <ft2build.h>
#include <ftlcdfil.h>
#include FT_FREETYPE_H

typedef struct _Fm_Opts Fm_Opts;
typedef struct _Fm_Freetype Fm_Freetype;

struct _Fm_Opts
{
   char *font_file;
   char *c_file;
   char *h_file;
   char *hpp_file;
   char *map_file;
   char *prefix;
   char *attribute;
   char *namespace_name;

   int   font_size;
   int   verbosity;

   unsigned int progmem : 1;
   unsigned int pgm : 1;
};

struct _Fm_Freetype
{
   FT_Library   lib;
   FT_Face      face;

   int          width;
   int          height;
   int          bearing_y_up;
   int          bearing_y_down;
};

typedef int (*Fm_Charmap_Func)(FT_ULong code, unsigned int idx, const char *utf8);
typedef void (*Fm_Func)(FT_ULong code, FT_Bitmap bm, FT_Glyph_Metrics mx);

/*============================================================================*
 *                                  Functions                                 *
 *============================================================================*/


const char *main_exec_path_get(void);

int fm_opts_init(int argc, char **argv);
void fm_opts_shutdown(void);
Fm_Opts *fm_opts_get(void);

int fm_freetype_init(void);
void fm_freetype_shutdown(void);
FT_UInt fm_freetype_process_glyph(FT_ULong code, FT_GlyphSlot *gs);

int fm_charmap_init(void);
void fm_charmap_shutdown(void);
int fm_charmap_generate(void);
void fm_charmap_foreach(Fm_Charmap_Func func);
unsigned int fm_charmap_count_get(void);
unsigned int fm_charmap_error_char_index_get(void);

int fm_output_init(void);
void fm_output_shutdown(void);
void fm_output_size_adjust(void);
void fm_output_generate_c(void);
void fm_output_generate_h(void);
void fm_output_generate_hpp(void);
void fm_output_generate_pgm(void);

void fm_pgm_bitmap_to_pgm(const char *title, FT_Bitmap bm);

unsigned int fm_utf8_to_unicode(const char *str);

int fm_gperf_init(void);
void fm_gperf_shutdown(void);
void fm_gperf_config_gen(void);
int fm_gperf_compile(FILE *fh);


#ifdef UNUSED
# undef UNUSED
#endif
#define UNUSED(var_) (void)var_

#endif /* ! _FONTMAKER_H_ */

