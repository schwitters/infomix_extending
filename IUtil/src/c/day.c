/*
 * Date conversion functions
 */
#include <mi.h>

/* Function to convert a datetime to a date */
mi_date dt2date(mi_datetime *dt)
{
  dtime_t mydt;
  mi_date newdate;
  int ret;
  char buffer[30];

  mydt.dt_qual = TU_DTENCODE(TU_YEAR, TU_DAY);
  ret = dtextend(dt, &mydt);

  ret = dttoasc(&mydt, buffer);
  ret = rdefmtdate(&newdate, "yyyy-mm-dd", buffer);

  return(newdate);
}
/* return the day part of the argument.
 * The argument can ve: date, datetime, interval, string
 */
mi_integer day_date(mi_date date, MI_FPARAM *fp)
{
  short mdy[3];
  int ret;

  ret = rjulmdy(date, mdy);
  return((mi_integer)mdy[1]);
}

mi_integer day_dt(mi_datetime *dt, MI_FPARAM *fp)
{
  mi_date newdate;

  newdate=dt2date(dt);
  return(day_date(newdate, fp));
}

/* return the name of the day passed as argument.
 * The argument can be: date, timestamp, string representing a date
 * or timestamp.
 * Returns varchar(100)
 */
static char *days_names[] = {"Sunday", "Monday", "Tuesday", 
	"Wednesday", "Thursday", "Friday", "Saturday"};

mi_lvarchar *dayname_date(mi_date date, MI_FPARAM *fp)
{
  int ret;
  ret = rdayofweek(date);
  return(mi_string_to_lvarchar(days_names[ret]));
}

mi_lvarchar *dayname_dt(mi_datetime *dt, MI_FPARAM *fp)
{
  mi_date newdate;

  newdate = dt2date(dt);
  return(dayname_date(newdate, fp));
}
/* return the day of the week
 * Argument: date, datetime, string
 */
mi_integer dayofweek_date(mi_date date, MI_FPARAM *fp)
{
  int ret;
  ret = rdayofweek(date);
  return(ret + 1);
}
mi_integer dayofweek_dt(mi_datetime *dt, MI_FPARAM *fp)
{
  mi_date newdate;

  newdate = dt2date(dt);
  return(dayofweek_date(newdate, fp));
}
/* return the day of the year (1-366)
 * Argument: date, datetime, string
 */
mi_integer dayofyear_date(mi_date date, MI_FPARAM *fp)
{
  mi_date jan1;
  int ret;
  short mdy[3];

  ret = rjulmdy(date, mdy);
  mdy[0] = 1;
  mdy[1] = 1;
  ret = rmdyjul(mdy, &jan1);
  ret = 1 + (date - jan1);
  return(ret);
}

mi_integer dayofyear_dt(mi_datetime *dt, MI_FPARAM *fp)
{
  mi_date newdate;

  newdate = dt2date(dt);
  return(dayofyear_date(newdate, fp));
}
