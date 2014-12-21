#include "fontmaker.h"

static int   _width          = 0;
static int   _height         = 0;
static int   _bearing_y_up   = 0;
static int   _bearing_y_down = 0;
static FILE *_out            = NULL;

#define W(f, ...) fprintf(_out, f "\n", ## __VA_ARGS__)
#define FCLOSE() do { fclose(_out); _out = NULL; } while (0)

static int
_size_autoset(FT_ULong     code,
              unsigned int idx)
{
   FT_GlyphSlot gs;
   FT_Glyph_Metrics mx;
   int w, h, bd, bu;

   fm_freetype_process_glyph(code, &gs);
   mx = gs->metrics;

   w = mx.width / 64;
   h = mx.height / 64;
   bu = mx.horiBearingY / 64;
   bd = h - bu;

   if (w > _width) _width = w;
   if (h > _height) _height = h;
   if (bu > _bearing_y_up) _bearing_y_up = bu;
   if (bd > _bearing_y_down) _bearing_y_down = bd;

   UNUSED(idx);

   return 1;
}

static int
_gen_char(FT_ULong     code,
          unsigned int idx)
{
   Fm_Opts *opts;
   FT_GlyphSlot gs;
   FT_Glyph_Metrics mx;
   FT_Bitmap bm;
   int i, x, y;
   int ex, ey, fx, gx, gy;
   int delta_x, delta_y;
   int width, height;

   opts = fm_opts_get();

   fm_freetype_process_glyph(code, &gs);
   bm = gs->bitmap;
   mx = gs->metrics;

   /*
    *       width
    *      <----->
    *       _width
    *   <----------->
    *  A             B
    *   X-----------X   ^   ^
    *   | E       F |   |   | delta_y
    *   |  X-----X  |   |   V        ^
    *   |  |     |  |   | _height    |
    *   |  |     |  |   |            | height
    *   |  |     |  |   |            |
    *   |  X-----X  |   |            v
    *   | H       G |   |
    *   X-----------X   V
    *  D             C
    *  <->       <->
    *  delta_x   delta_x
    *
    */

   width = bm.width;
   height = bm.rows;

   delta_x = (_width - width) / 2;
   delta_y = _bearing_y_up - (mx.horiBearingY / 64);

   ex = delta_x,         ey = delta_y;
   fx = width + delta_x;
   gx = fx,              gy = delta_y + height;

   if (opts->verbosity >= 1)
     printf("Generate switch for [%lu] (%u)\n", code, idx);

   if (idx == fm_charmap_count_get())
     W("       default:");
   else
     W("      case %u:", (unsigned int)code);
   W("         switch (pixel)");
   W("           {");

   i = 0;
   for (y = ey; y < gy; y++)
     {
        for (x = ex; x < gx; x++)
          {
             W("             case %i: return 0x%x;",
               y * _width + x,
               (uint8_t)((((float)bm.buffer[i]) / 256.0) * (64.0)));
             ++i;
          }
     }

   W("            default: return 0x00;");
   W("           }");
   W("         break;");

   return 1;
}

/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

int
fm_output_init(void)
{
   return 0;
}

void
fm_output_shutdown(void)
{
   _width = 0;
   _height = 0;
   _bearing_y_up = 0;
   _bearing_y_down = 0;
}

void
fm_output_size_adjust(void)
{
   fm_charmap_foreach(_size_autoset);
}

void
fm_output_generate_c(void)
{
   Fm_Opts *opts;

   opts = fm_opts_get();

   if (!opts->c_file) return;
   _out = fopen(opts->c_file, "w");
   if (!_out)
     {
        fprintf(stderr, "*** Failed to open C file [%s]\n", opts->c_file);
        return;
     }

   W("static %s const unsigned char _bitmap[%i][%i][%i] =",
     (opts->avr_rom) ? "__attribute__((__progmem__))" : "",
     fm_charmap_count_get(), _width, _height);
   W("{");
   W("};");
   W("");

   W("int");
   W("%ssize_get(void)", opts->prefix);
   W("{");
   W("   return %i;", opts->font_size);
   W("}");
   W("");

   W("void");
   W("%sbounding_box_get(int *w, int *h)", opts->prefix);
   W("{");
   W("   if (w) *w = %i;", _width);
   W("   if (h) *h = %i;", _height);
   W("}");
   W("");

   W("int");
   W("%sglyphs_available_get(void)", opts->prefix);
   W("{");
   W("   return %i;", fm_charmap_count_get());
   W("}");
   W("");

   W("unsigned int");
   W("%scolor_bits_get(void)", opts->prefix);
   W("{");
   W("   return %i;", opts->font_bits);
   W("}");
   W("");

   W("unsigned int");
   W("%sglyph_pixel_get(unsigned int codepoint,", opts->prefix);
   W("                  unsigned int x,");
   W("                  unsigned int y)");
   W("{");
   W("   unsigned int pixel = (y * %i) + x;", _width);
   W("   switch (codepoint)");
   W("     {");

   fm_charmap_foreach(_gen_char);

   W("     }");
   W("}");
   W("");

   FCLOSE();
}

void
fm_output_generate_h(void)
{
   Fm_Opts *opts;
   const char *f;

   opts = fm_opts_get();

   f = opts->h_file;
   if (!f) return;
   _out = fopen(opts->h_file, "w");
   if (!_out)
     {
        fprintf(stderr, "*** Failed to open C file [%s]\n", opts->c_file);
        return;
     }

   W("#ifndef __FONTMAKER__GENERATED_H__");
   W("#define __FONTMAKER__GENERATED_H__");
   W("");

   W("int");
   W("%ssize_get(void);", opts->prefix);
   W("");

   W("void");
   W("%sbounding_box_get(int *w, int *h);", opts->prefix);
   W("");

   W("int");
   W("%sglyphs_available_get(void);", opts->prefix);
   W("");

   W("unsigned int");
   W("%scolor_bits_get(void);", opts->prefix);
   W("");

   W("unsigned int");
   W("%sglyph_pixel_get(unsigned int codepoint,", opts->prefix);
   W("                  unsigned int x,");
   W("                  unsigned int y);");
   W("");

   W("#endif");
   W("");

   FCLOSE();
}
