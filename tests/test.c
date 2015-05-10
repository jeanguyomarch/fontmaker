#include "font.h"
#include <stdio.h>

int
main(void)
{
   int w, h;

   bounding_box_get(&w, &h);
   printf("=== STATS ===\n"
          "size.........: %i\n"
          "w............: %i\n"
          "h............: %i\n"
          "glyphs.......: %i\n",
          size_get(),
          w,
          h,
          glyphs_count_get());

   return 0;
}

