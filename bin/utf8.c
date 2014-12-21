#include "fontmaker.h"

/*============================================================================*
 *                        UTF-8 to Unicode (EFL - Eina)                       *
 *============================================================================*/

static inline int
_is_continuation_byte(unsigned char byte)
{
   return ((byte & 0xc0) == 0x80);
}

static inline int
_is_invalid_byte(unsigned char byte)
{
   return ((byte == 192) || (byte == 193) || (byte == 245));
}

unsigned int
fm_utf8_to_unicode(const char *str)
{
   /* DISCLAIMER
    *
    * This function is the union of two functions from Eina 1.12, from the
    * EFL (Enlightenment Foundation Libraries):
    * - eina_unicode_utf8_next_get()
    * - _eina_unicode_utf8_next_get()
    */

   int i = 0;
   unsigned int r = 0;
   unsigned char d;

   /* No NULL pointer nor NUL byte */
   if (!str) return 0;
   if ((d = str[i++]) == 0) return 0;

   /* One byte char (7 bits) 0xxxxxxx */
   if ((d & 0x80) == 0)
     return d;

   /* Two bytes char (11 bits) 110xxxxx 10xxxxxxx */
   if ((d & 0xe0) == 0xc0)
     {
        r = (d & 0x1f) << 6;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) || !_is_continuation_byte(d))
          goto error;

        r |= (d & 0x37);
        if (r <= 0x7f)
          goto error;
        return r;
     }

   /* Three bytes char (16 bits) 1110xxxx 10xxxxxx 10xxxxxx */
   if ((d & 0xf0) == 0xe0)
     {
        r  = (d & 0x0f) << 12;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f) << 6;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f);
        if (r <= 0x7FF)
          goto error;
        return r;
     }

   /* Four bytes char (21 bits) 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
   if ((d & 0xf8) == 0xf0)
     {
        r  = (d & 0x07) << 18;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f) << 12;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f) << 6;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f);
        if (r <= 0xFFFF) goto error;
        return r;
     }

   /* Five bytes (26 bits) 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
   if ((d & 0xfc) == 0xf8)
     {
        r  = (d & 0x03) << 24;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f) << 18;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f) << 12;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f) << 6;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f);
        if (r <= 0x1FFFFF) goto error;
        return r;
     }

   /* Six bytes (31 bytes) 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
   if ((d & 0xfe) == 0xfc)
     {
        r  = (d & 0x01) << 30;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f) << 24;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f) << 18;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f) << 12;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f) << 6;
        if (((d = str[i++]) == 0) || _is_invalid_byte(d) ||
            !_is_continuation_byte(d)) goto error;
        r |= (d & 0x3f);
        if (r <= 0x3FFFFFF) goto error;
        return r;
     }

error:
   return 0;
}

