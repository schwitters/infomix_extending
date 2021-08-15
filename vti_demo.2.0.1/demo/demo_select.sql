-- --------------------------------------------------------------------------
-- demo_select.sql
--
-- Issue a few select statements against the "temperatures" table
-- --------------------------------------------------------------------------
--
-- Turn on tracing.
--
execute function vti_demo_traceon(20);
--
-- What cities are in the database?
--
select distinct city from temperatures order by city;
--
-- What's the range of temperatures in Berkeley?
--
select city, month, temp from temperatures where city = 'Berkeley'
order by temp;
--
-- What's the temperature range for each city in the database?
--
select city, max(temp), avg(temp), min(temp) from temperatures
group by city;
--
-- What's the range of temperatures by month across all cities?
--
select month, max(temp) max_temp, avg(temp) avg_temp, min(temp) min_temp
from temperatures
group by month
order by max_temp;
--
-- What are the hotest and coldest places in the Bay Area?
--
select city, month, temp from temperatures
where temp = (select max(temp) from temperatures)
or    temp = (select min(temp) from temperatures);
--
-- And what places are just about average?
--
select city, month, temp from temperatures
where temp + 1 >= (select avg(temp) from temperatures)
and temp -1 <= (select avg(temp) from temperatures);
--
-- What data do we have in the table that we put in the extspace?
--
select * from seasons;
--
-- Now let's join this table with our temperatures data:
--
-- What's the average summer temperature in Berkeley?
--
select city, temperatures.month, temperatures.temp
from temperatures, seasons
where city = 'Berkeley' 
and  temperatures.month = seasons.month
and temperatures.month in 
(select month from seasons where season = 'summer')
group by city, temperatures.month, temperatures.temp;

select distinct city, season, avg(temp) avg_temp
from seasons, temperatures
where temperatures.city = 'Berkeley'
and temperatures.month = seasons.month
and temperatures.month in 
(select month from seasons where season = 'summer')
group by city, season;
--
-- How do typical winter temperatures compare for the cities?
--
select distinct city, season, avg(temp) avg_temp
from seasons, temperatures
where temperatures.month = seasons.month
and temperatures.month in 
(select month from seasons where season = 'winter')
group by city, season;

--
-- Is it true that San Francisco has a year-round spring climate?
--
select city, season, avg(temp)
from temperatures, seasons
where city = 'San Francisco'
and temperatures.month = seasons.month
group by city, season;

--
-- I bet Pittsburg doesn't!
--
select city, season, avg(temp)
from temperatures, seasons
where city = 'Pittsburg'
and temperatures.month = seasons.month
group by city, season;

-- done!
