CREATE TABLE policies (
  policyNumber	serial,
  Location	Node,
  Corp_Id	integer,
  rating	char(1)
);

CREATE TABLE Hotel_Resource (
  Location	Node,
  Corp_Id	integer,
  Name		varchar(50),
  Address	varchar(75),
  Phone		varchar(10)
);

