-- --------------------------------------------------------------------------
-- testchrs.sql
--
-- This script will test both ascii() and chr() for the whole range of
-- ascii values. chr() will also be tested with numbers that do not
-- represent ascii values.
--
-- The test works by piggybacking the two functions together, converting
-- from a chracter to an ascii value, then back again.
-- --------------------------------------------------------------------------
--
select 'a', chr(ascii('a')) from oneval;
select 'b', chr(ascii('b')) from oneval;
select 'c', chr(ascii('c')) from oneval;
select 'd', chr(ascii('d')) from oneval;
select 'e', chr(ascii('e')) from oneval;
select 'f', chr(ascii('f')) from oneval;
select 'g', chr(ascii('g')) from oneval;
select 'h', chr(ascii('h')) from oneval;
select 'i', chr(ascii('i')) from oneval;
select 'j', chr(ascii('j')) from oneval;
select 'k', chr(ascii('k')) from oneval;
select 'l', chr(ascii('l')) from oneval;
select 'm', chr(ascii('m')) from oneval;
select 'n', chr(ascii('n')) from oneval;
select 'o', chr(ascii('o')) from oneval;
select 'p', chr(ascii('p')) from oneval;
select 'q', chr(ascii('q')) from oneval;
select 'r', chr(ascii('r')) from oneval;
select 's', chr(ascii('s')) from oneval;
select 't', chr(ascii('t')) from oneval;
select 'u', chr(ascii('u')) from oneval;
select 'v', chr(ascii('v')) from oneval;
select 'w', chr(ascii('w')) from oneval;
select 'x', chr(ascii('x')) from oneval;
select 'y', chr(ascii('y')) from oneval;
select 'z', chr(ascii('z')) from oneval;

select 'A', chr(ascii('A')) from oneval;
select 'B', chr(ascii('B')) from oneval;
select 'C', chr(ascii('C')) from oneval;
select 'D', chr(ascii('D')) from oneval;
select 'E', chr(ascii('E')) from oneval;
select 'F', chr(ascii('F')) from oneval;
select 'G', chr(ascii('G')) from oneval;
select 'H', chr(ascii('H')) from oneval;
select 'I', chr(ascii('I')) from oneval;
select 'J', chr(ascii('J')) from oneval;
select 'K', chr(ascii('K')) from oneval;
select 'L', chr(ascii('L')) from oneval;
select 'M', chr(ascii('M')) from oneval;
select 'N', chr(ascii('N')) from oneval;
select 'O', chr(ascii('O')) from oneval;
select 'P', chr(ascii('P')) from oneval;
select 'Q', chr(ascii('Q')) from oneval;
select 'R', chr(ascii('R')) from oneval;
select 'S', chr(ascii('S')) from oneval;
select 'T', chr(ascii('T')) from oneval;
select 'U', chr(ascii('U')) from oneval;
select 'V', chr(ascii('V')) from oneval;
select 'W', chr(ascii('W')) from oneval;
select 'X', chr(ascii('X')) from oneval;
select 'Y', chr(ascii('Y')) from oneval;
select 'Z', chr(ascii('Z')) from oneval;

select '1', chr(ascii('1')) from oneval;
select '2', chr(ascii('2')) from oneval;
select '3', chr(ascii('3')) from oneval;
select '4', chr(ascii('4')) from oneval;
select '5', chr(ascii('5')) from oneval;
select '6', chr(ascii('6')) from oneval;
select '7', chr(ascii('7')) from oneval;
select '8', chr(ascii('8')) from oneval;
select '9', chr(ascii('9')) from oneval;
select '0', chr(ascii('0')) from oneval;

select '~', chr(ascii('~')) from oneval;
select '`', chr(ascii('`')) from oneval;
select '!', chr(ascii('!')) from oneval;
select '@', chr(ascii('@')) from oneval;
select '#', chr(ascii('#')) from oneval;
select '$', chr(ascii('$')) from oneval;
select '%', chr(ascii('%')) from oneval;
select '^', chr(ascii('^')) from oneval;
select '&', chr(ascii('&')) from oneval;
select '*', chr(ascii('*')) from oneval;
select '(', chr(ascii('(')) from oneval;
select ')', chr(ascii(')')) from oneval;
select '-', chr(ascii('-')) from oneval;
select '_', chr(ascii('_')) from oneval;
select '+', chr(ascii('+')) from oneval;
select '=', chr(ascii('=')) from oneval;
select '|', chr(ascii('|')) from oneval;
select '\', chr(ascii('\')) from oneval;
select ']', chr(ascii(']')) from oneval;
select '}', chr(ascii('}')) from oneval;
select '{', chr(ascii('{')) from oneval;
select '[', chr(ascii('[')) from oneval;
select '"', chr(ascii('"')) from oneval;
select "'", chr(ascii("'")) from oneval;
select ':', chr(ascii(':')) from oneval;
select ';', chr(ascii(';')) from oneval;
select '.', chr(ascii('.')) from oneval;
select ',', chr(ascii(',')) from oneval;
select '/', chr(ascii('/')) from oneval;
select '?', chr(ascii('?')) from oneval;

-- These should fail:
execute function chr(999);
execute function chr(128);
execute function chr(-1);

