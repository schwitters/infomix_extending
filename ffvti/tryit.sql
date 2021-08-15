EXECUTE FUNCTION set_tracing("ffvti", 80, "/tmp/trace.out");

CREATE TABLE tab 
(
  a BOOLEAN,
  b VARCHAR(20),
  c DATE,
  d DATETIME year to second,
  e INTERVAL hour to second,
  f DECIMAL,
  g DOUBLE PRECISION,
  h SMALLFLOAT,
  i INT8,
  j INT
) USING FFAccess (path='/tmp/tab.txt', delim=';');

SELECT * FROM tab;

DROP TABLE tab;
