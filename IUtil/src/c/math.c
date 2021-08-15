/*
 * Math type functions: degrees, floor, radians, sign, truncate
 */
#include <mi.h>
#include <math.h>

/*----------------------------------------------------------*/
/*
 * Conversion functions between degrees and radians
 */
/* 2*PI radian in a circle */
/* 1 degree = PI/180 radians; 1 radian = 180/PI degrees */
#define PI 3.141592653589793

/* radians to degree */
mi_double_precision *degrees_dbl(mi_double_precision *in, MI_FPARAM *fp)
{
  mi_double_precision *ret;
  ret = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
  (*ret) = (*in) * 180.0 / PI;
  return(ret);
}

/* degree to radian */
mi_double_precision *radians_dbl(mi_double_precision *in, MI_FPARAM *fp)
{
  mi_double_precision *ret;
  ret = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
  (*ret) = (*in) * PI / 180.0;
  return(ret);
}
mi_double_precision *iutil_convert(mi_decimal *d)
{
  mi_double_precision *val;
  mi_integer ret;

  val = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
  ret = dectodbl(d, val);
  return(val);
}
mi_double_precision *degrees_dec(mi_decimal *in, MI_FPARAM *fp)
{
  return(degrees_dbl(iutil_convert(in), fp) );
}
mi_double_precision *radians_dec(mi_decimal *in, MI_FPARAM *fp)
{
  return(radians_dbl(iutil_convert(in), fp) );
}
/*----------------------------------------------------------*/
mi_integer ceil_int(mi_integer v, MI_FPARAM *fp)
{
  return(v);
}
mi_double_precision *ceil_dbl(mi_double_precision *v, MI_FPARAM *fp)
{
  mi_double_precision *ret;
  ret = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
  (*ret) = ceil(*v);
  return(ret);
}
mi_double_precision *ceil_dec(mi_decimal *v, MI_FPARAM *fp)
{
  mi_double_precision *val;
  mi_integer ret;

  val = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
  ret = dectodbl(v, val);
  return(ceil_dbl(val, fp));
}
mi_integer floor_int(mi_integer v, MI_FPARAM *fp)
{
  return(v);
}
mi_double_precision *floor_dbl(mi_double_precision *v, MI_FPARAM *fp)
{
  mi_double_precision *ret;
  ret = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
  (*ret) = floor(*v);
  return(ret);
}
mi_double_precision *floor_dec(mi_decimal *v, MI_FPARAM *fp)
{
  mi_double_precision *val;
  mi_integer ret;

  val = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));
  ret = dectodbl(v, val);
  return(floor_dbl(val, fp));
}
/*----------------------------------------------------------*/
mi_integer sign_int(mi_integer v, MI_FPARAM *fp)
{
  if (v == 0)
    return(0);
  if (v < 0)
    return(-1);
  return(1);
}
mi_double_precision *sign_dbl(mi_double_precision *v, MI_FPARAM *fp)
{
  mi_double_precision *ret;
  ret = (mi_double_precision *)mi_alloc(sizeof(mi_double_precision));

  if ((*v) == 0)
    (*ret) = 0;
  if ((*v) < 0)
    (*ret) = -1;
  if ((*v) > 0)
    (*ret) = 1;
  return(ret);
}
mi_double_precision *sign_dec(mi_decimal *v, MI_FPARAM *fp)
{
  return(sign_dbl(iutil_convert(v), fp) );
}
