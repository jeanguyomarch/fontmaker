#include "font.hxx"
#include <stdio.h>

int
main(void)
{
   int w, h;
   Font f;

   f.bounding_box_get(w, h);
   printf("=== STATS ===\n"
          "size.........: %i\n"
          "w............: %i\n"
          "h............: %i\n"
          "glyphs.......: %i\n",
          f.size_get(),
          w,
          h,
          f.glyphs_count_get());

   return 0;
}

