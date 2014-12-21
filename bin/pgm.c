#include "fontmaker.h"

void
fm_pgm_bitmap_to_pgm(const char *title,
                     FT_Bitmap   bm)
{
   FILE *f;
   int i;

   f = fopen(title, "w+");
   if (!f)
     {
        fprintf(stderr, "*** Failed to open file [%s]\n", title);
        return;
     }

   fprintf(f, "P5\n%i %i\n255\n", bm.width, bm.rows);
   for (i = 0; i < bm.width * bm.rows; i++)
     fprintf(f, "%c", (int)(bm.buffer[i]));

   fclose(f);
}

