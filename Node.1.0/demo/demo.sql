
-- ----------------------------------------------------------------------
-- DEMO INPUT/OUTPUT FUNCTIONS
-- ----------------------------------------------------------------------

drop table nodes;

create table nodes ( n node, valid boolean );

--
-- Try to insert valid nodes
--

insert into nodes values ('1', 't');
insert into nodes values ('1.1', 't');
insert into nodes values ('1.1.1', 't');
insert into nodes values ('1.1.2', 't');
insert into nodes values ('1.2.1', 't');

insert into nodes values ('2.0', 't');
insert into nodes values ('2.1', 't');

--
-- Try to insert invalid nodes
--

insert into nodes values ('1.', 'f');
insert into nodes values ('1.0.', 'f');
insert into nodes values ('1.1.', 'f');
insert into nodes values ('1.1.0', 'f');

insert into nodes values ('0', 'f');
insert into nodes values ('0.0', 'f');
insert into nodes values ('0.1', 'f');

insert into nodes values ('', 'f');
insert into nodes values (' ', 'f');
insert into nodes values ('.', 'f');

insert into nodes values ('1.-1.2', 'f');
insert into nodes values ('1.A.2', 'f');

--
-- Check for invalid nodes
--

select n from nodes where not valid;

--
-- Demo unloading and loading the data in ASCII format
--

create temp table tnodes (n node, valid boolean);
unload to 'nodes.unl' select * from nodes where valid;
load from 'nodes.unl' insert into tnodes;
select * from nodes n, tnodes t where n.n = t.n;
drop table tnodes;

--
-- Demo chaining of input/output functions
--

select nodeimpb(nodeexpb(n)) from nodes where valid;
select nodeimpt(nodeexpt(n)) from nodes where valid;
select noderecv(nodesend(n)) from nodes where valid;
select nodein(nodeout(n)) from nodes where valid;

-- ----------------------------------------------------------------------
-- DEMO FUNCTIONS w/ 1 NODE PARAMETERS
-- ----------------------------------------------------------------------

--
-- Demo the 'length' function
--

select
	a.n a,
	length(a.n) len_a
from
	nodes a
where
	valid;

--
-- Demo the 'getparent' function
--

select
	a.n a,
	getparent(a.n) getpar_a
from
	nodes a
where
	valid;

--
-- Demo the 'newlevel' function
--

select
	a.n a,
	newlevel(a.n) newlvl_a
from
	nodes a
where
	valid;

--
-- Demo the 'getmember' function
--

select
	a.n a,
	getmember(a.n, 0) getmem_a_0
from
	nodes a
where
	valid;

select
	a.n a,
	getmember(a.n, 1) getmem_a_1
from
	nodes a
where
	valid;

select
	a.n a,
	getmember(a.n, 2) getmem_a_2
from
	nodes a
where
	valid;

select
	a.n a,
	getmember(a.n, 3) getmem_a_3
from
	nodes a
where
	valid;

--
-- Demo the 'increment' functions
--

select
	a.n a,
	increment(a.n) incr_a
from
	nodes a
where
	valid;

select
	a.n a,
	increment(a.n,0) incr_a_0
from
	nodes a
where
	valid;

select
	a.n a,
	increment(a.n,1) incr_a_1
from
	nodes a
where
	valid;

select
	a.n a,
	increment(a.n,2) incr_a_2
from
	nodes a
where
	valid;

select
	a.n a,
	increment(a.n,3) incr_a_3
from
	nodes a
where
	valid;

--
-- Demo the 'ancestors' iterator function
--

execute function ancestors('1.2.3.4.5.6.7.8.9');

-- ----------------------------------------------------------------------
-- DEMO FUNCTIONS w/ 2 NODE PARAMETERS
-- ----------------------------------------------------------------------

--
-- Demo the 'compare' function and associated relational operators
--

select
	a.n a,
	b.n b,
	compare(a.n,b.n) compare_a_b,
	a.n = b.n eq_a_b,
	a.n != b.n neq_a_b,
	a.n < b.n lt_a_b,
	a.n <= b.n lte_a_b,
	a.n > b.n gt_a_b,
	a.n >= b.n gte_a_b
from
	nodes a,
	nodes b
where
	a.valid and b.valid;

--
-- Demo the 'isparent' and 'ischild' functions
--

select
	a.n a,
	b.n b,
	isparent(a.n,b.n) isparent_a_b,
	ischild(a.n,b.n) ischild_a_b
from
	nodes a,
	nodes b
where
	a.valid and b.valid;


--
-- Demo the 'isancestor' and 'isdescendant' functions
--

select
	a.n a,
	b.n b,
	isancestor(a.n,b.n) isanc_a_b,
	isdescendant(a.n,b.n) isdesc_a_b
from
	nodes a,
	nodes b
where
	a.valid and b.valid;

-- ----------------------------------------------------------------------
-- DEMO FUNCTIONS w/ 3 NODE PARAMETERS
-- ----------------------------------------------------------------------

--
-- Demo the 'graft' function
--

select
	a.n a,
	b.n b,
	c.n c,
	graft(a.n,b.n,c.n) graft_a_b_c
from
	nodes a,
	nodes b,
	nodes c
where
	a.valid and b.valid and c.valid
	and isancestor(a.n, c.n);

