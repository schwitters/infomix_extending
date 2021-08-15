CREATE TABLE Parts (
  PartNumber	integer,
  Name		varchar(30),
  Provider	varchar(30),
  Price		money
);

CREATE TABLE Component (
  ComponentId	Node,
  Sequence	integer,
  Quantity	integer,
  Name		varchar(30),
  PartNumber	integer
);

