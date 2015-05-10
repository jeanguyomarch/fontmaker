#include "fontmaker.h"

static int   _width          = 0;
static int   _height         = 0;
static int   _bearing_y_up   = 0;
static int   _bearing_y_down = 0;
static FILE *_out            = NULL;

#define FCLOSE(out_) do { fclose(out_); out_ = NULL; } while (0)

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
   unsigned char cp;

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

#define W(f, ...) fprintf(_out, f, ## __VA_ARGS__)

   width = bm.width;
   height = bm.rows;

   delta_x = (_width - width) / 2;
   delta_y = _bearing_y_up - (mx.horiBearingY / 64);

   ex = delta_x,         ey = delta_y;
   fx = width + delta_x;
   gx = fx,              gy = delta_y + height;

   if (opts->verbosity >= 2)
     fprintf(stdout, "Generate switch for [%lu] (%u)\n", code, idx);


   W("     {\n");
   i = 0;
   for (y = 0; y < _height; y++)
     {
        W("          { ");
        for (x = 0; x < _width; x++)
          {
             if ((y >= ey) && (y < gy) && (x >= ex) && (x < gx))
               cp = (uint8_t)((((float)bm.buffer[i++]) / 256.0) * (64.0));
             else
               cp = 0;

             W("0x%02x", cp);
             if (x < _width - 1)
               W(", ");
          }
        W(" }");
        if (y < _height - 1) W(",");
        W("\n");
     }
   W("     }");
   if (idx < fm_charmap_count_get()) W(",");
   W("\n");

   /* Code below is to generate a single string with all codepoints */
   //       i = 0;
   //       for (y = 0; y < _height; y++)
   //         {
   //            for (x = 0; x < _width; x++)
   //              {
   //                 if ((y >= ey) && (y < gy) && (x >= ex) && (x < gx))
   //                   cp = (uint8_t)((((float)bm.buffer[i++]) / 256.0) * (64.0));
   //                 else
   //                   cp = 0;
   //                 W("\\x%02x", cp);
   //              }
   //         }

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

#undef W
#define W(f, ...) fprintf(_out, f "\n", ## __VA_ARGS__)

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

   if (opts->progmem)
     {
        W("#include <avr/pgmspace.h>");
        W("");
     }
   W("static const unsigned char _bitmap[%i][%i][%i] %s=",
     fm_charmap_count_get(), _height, _width, (opts->progmem) ? "PROGMEM " : "");

   W("{");
   fm_charmap_foreach(_gen_char);
   W("};");
   W("");

   W("%s int", opts->attribute);
   W("%ssize_get(void)", opts->prefix);
   W("{");
   W("   return %i;", opts->font_size);
   W("}");
   W("");

   W("%s void", opts->attribute);
   W("%sbounding_box_get(int *w, int *h)", opts->prefix);
   W("{");
   W("   if (w) *w = %i;", _width);
   W("   if (h) *h = %i;", _height);
   W("}");
   W("");

   W("%s int", opts->attribute);
   W("%sglyphs_available_get(void)", opts->prefix);
   W("{");
   W("   return %i;", fm_charmap_count_get());
   W("}");
   W("");

   W("%s unsigned int", opts->attribute);
   W("%scolor_bits_get(void)", opts->prefix);
   W("{");
   W("   return %i;", opts->font_bits);
   W("}");
   W("");

   W("%s unsigned char", opts->attribute);
   W("%sglyph_pixel_get(unsigned int codepoint,", opts->prefix);
   W("                  unsigned int x,");
   W("                  unsigned int y)");
   W("{");
   if (opts->progmem)
     {
        W("   return pgm_read_byte_near(&(_bitmap[codepoint - 0x20][y][x]));");
        // W("   return _bitmap[(codepoint - 0x20) * (%i * %i) + (y * %i) + x];",
        //   _width, _height, _width);
     }
   else
     {
        W("   return _bitmap[codepoint - 0x20][y][x];");
     }
   W("}");
   W("");

   FCLOSE(_out);
}

static const char *
_to_upper(const char *in)
{
   static char buf[512];
   char *ptr = (char *)in;
   char c;
   int i = 0;

   while ((c = *(ptr++)))
     {
        if (c >= 'a' && c <= 'z')
          c -= 0x20;
        buf[i++] = c;
     }
   buf[i] = 0;

   return buf;
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

   W("#ifdef __cplusplus");
   W("extern \"C\" {");
   W("#endif /* __cplusplus */");
   W("");

   W("#define %sCHAR_WIDTH %i", _to_upper(opts->prefix), _width);
   W("#define %sCHAR_HEIGHT %i", _to_upper(opts->prefix), _height);

      W("%s int", opts->attribute);
   W("%ssize_get(void);", opts->prefix);
   W("");

   W("%s void", opts->attribute);
   W("%sbounding_box_get(int *w, int *h);", opts->prefix);
   W("");

   W("%s int", opts->attribute);
   W("%sglyphs_available_get(void);", opts->prefix);
   W("");

   W("%s unsigned int", opts->attribute);
   W("%scolor_bits_get(void);", opts->prefix);
   W("");

   W("%s unsigned char", opts->attribute);
   W("%sglyph_pixel_get(unsigned int codepoint,", opts->prefix);
   W("                  unsigned int x,");
   W("                  unsigned int y);");
   W("");

   W("#ifdef __cplusplus");
   W("}");
   W("#endif /* __cplusplus */");
   W("");

   W("#endif");
   W("");

   FCLOSE(_out);
}

static int
_gen_pgm(FT_ULong     code,
         unsigned int idx)
{
   FT_Bitmap bm;
   FT_GlyphSlot gs;
   char title[16];

   fm_freetype_process_glyph(code, &gs);
   bm = gs->bitmap;

   snprintf(title, sizeof(title), "glyph_%lu.pgm", code);
   printf("Generating [%s]...\n", title);
   fm_pgm_bitmap_to_pgm(title, bm);

   return 1;
}

void
fm_output_generate_pgm(void)
{
   Fm_Opts *opts;

   opts = fm_opts_get();
   if (!opts->pgm) return;

   fm_charmap_foreach(_gen_pgm);
}

