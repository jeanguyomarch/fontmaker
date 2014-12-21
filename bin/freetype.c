#include "fontmaker.h"

static Fm_Freetype _ft = { NULL, NULL, -1, -1, -1, -1 };

/*============================================================================*
 *                                 Public API                                 *
 *============================================================================*/

int
fm_freetype_init(void)
{
   int err;
   Fm_Opts *opts;

   opts = fm_opts_get();

   /* Init freetype */
   err = FT_Init_FreeType(&_ft.lib);
   if (err != 0)
     {
        fprintf(stderr, "*** Failed to initialize FreeType2\n");
        return err;
     }

   /* Create face from font */
   err = FT_New_Face(_ft.lib, opts->font_file, 0, &_ft.face);
   if (err != 0)
     {
        fprintf(stderr, "*** Failed to create face from file [%s]\n",
                opts->font_file);
        return err;
     }

   /* Check it has fixed width */
   if (!FT_IS_FIXED_WIDTH(_ft.face))
     {
        fprintf(stderr, "*** Font [%s] is not monospaced!\n", opts->font_file);
        return 1;
     }

   /* Configure char size */
   err = FT_Set_Char_Size(_ft.face, 0, opts->font_size * 64, 72, 72);
   if (err != 0)
     {
        fprintf(stderr, "*** Failed to set size [%i] for DPI 72x72\n",
                opts->font_size);
        return err;
     }

   /* Set LCD filter is enabled in build */
   err = FT_Library_SetLcdFilter(_ft.lib, FT_LCD_FILTER_LIGHT);
   if (err != 0)
     {
        if (err == FT_Err_Unimplemented_Feature)
          {
             fprintf(stderr, "WARNING: FT_Library_SetLcdFilter() is not enabled "
                     "in your build of FreeType2. Font size will be alterated!\n");
          }
        else
          {
             fprintf(stderr, "*** FT_Library_SetLcdFilter() failed\n");
             return err;
          }
     }

   return 0;
}

void
fm_freetype_shutdown(void)
{
   FT_Done_Face(_ft.face);
   FT_Done_FreeType(_ft.lib);
   memset(&_ft, 0, sizeof(Fm_Freetype));
}

FT_UInt
fm_freetype_process_glyph(FT_ULong      code,
                          FT_GlyphSlot *gs)
{
   FT_UInt glyph;
   int chk;

   glyph = FT_Get_Char_Index(_ft.face, code);
   if (glyph == 0) return 0;

   chk = FT_Load_Glyph(_ft.face, glyph, FT_LOAD_DEFAULT);
   if (chk != 0) return 0;

   chk = FT_Render_Glyph(_ft.face->glyph, FT_RENDER_MODE_NORMAL);
   if (chk != 0) return 0;

   if (_ft.face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) return 0;
   if (gs) *gs = _ft.face->glyph;

   return glyph;
}

