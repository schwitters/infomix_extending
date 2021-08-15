-- run the ceil() routine against each column in the SQLNumbers table
--
select decimalColumn, ceil(decimalColumn) ceil
from SQLNumbers;

select integerColumn, ceil(integerColumn) ceil
from SQLNumbers;

select int8Column, ceil(int8Column) ceil
from SQLNumbers;

select realColumn, ceil(realColumn) ceil
from SQLNumbers;

select smallintColumn, ceil(smallintColumn) ceil
from SQLNumbers;

select doubleColumn, ceil(doubleColumn) ceil
from SQLNumbers;

