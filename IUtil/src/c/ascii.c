/*
 * Return an integer representing the leftmost character of the
 * argument and vice-versa
 */
#include <mi.h>

mi_integer ascii(mi_lvarchar *lvar, MI_FPARAM *fp)
{
  mi_string *str;
  mi_integer ret;

  str = mi_lvarchar_to_string(lvar);
  ret = (mi_integer)str[0];
  return(ret);
}

mi_lvarchar *chr(mi_integer val, MI_FPARAM *fp)
{
  mi_string buffer[2];
  mi_lvarchar *ret = 0;

  if (val > 255) {
    mi_fp_setreturnisnull(fp, 0, MI_TRUE);
  } else {
    buffer[0] = val | 256;
    buffer[1] = 0;
    ret = mi_string_to_lvarchar(buffer);
  }
  return(ret);
}
