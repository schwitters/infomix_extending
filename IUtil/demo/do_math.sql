-- radians to degrees
EXECUTE FUNCTION degrees("1"::double precision);
EXECUTE FUNCTION degrees("1"::decimal);
-- degrees to radians
EXECUTE FUNCTION radians("57.3"::double precision);
EXECUTE FUNCTION radians("57.3"::decimal);
-- ceiling and floor
EXECUTE FUNCTION ceil("57"::integer);
EXECUTE FUNCTION ceil("57.3"::double precision);
EXECUTE FUNCTION ceil("57.3"::decimal);
EXECUTE FUNCTION floor("57"::integer);
EXECUTE FUNCTION floor("57.3"::double precision);
EXECUTE FUNCTION floor("57.3"::decimal);
-- Sign
EXECUTE FUNCTION sign("57"::integer);
EXECUTE FUNCTION sign("57.3"::double precision);
EXECUTE FUNCTION sign("57.3"::decimal);
EXECUTE FUNCTION sign("-57"::integer);
EXECUTE FUNCTION sign("-57.3"::double precision);
EXECUTE FUNCTION sign("-57.3"::decimal);
EXECUTE FUNCTION sign("0"::integer);
EXECUTE FUNCTION sign("0"::double precision);
EXECUTE FUNCTION sign("0"::decimal);
