-- run the sign() routine against each column in the SQLNumbers table
--
select decimalColumn, sign(decimalColumn) sign
from SQLNumbers;

select integerColumn, sign(integerColumn) sign
from SQLNumbers;

select int8Column, sign(int8Column) sign
from SQLNumbers;

select realColumn, sign(realColumn) sign
from SQLNumbers;

select smallintColumn, sign(smallintColumn) sign
from SQLNumbers;

select doubleColumn, sign(doubleColumn) sign
from SQLNumbers;

