-- --------------------------------------------------------------------------
-- demo_update.sql
--
-- Update some records in the "temperatures" table
-- --------------------------------------------------------------------------
--
-- Pittsburg just had a heat wave!
--
update temperatures set temp = 95 
where city = 'Pittsburg' and month = 'July';
--
-- The fog's been blowing into Berkeley all summer.
--
update temperatures set temp = 61 
where city = 'Berkeley' and month = 'July';
--
update temperatures set temp = 60 
where city = 'Berkeley' and month = 'August';
--
-- Oh! Did we say Walnut Creek? We meant Orinda, of course!
--
update temperatures set city = 'Orinda' where city = 'Walnut Creek';
--
-- done!
