CREATE TABLE DTD (
  dtd_Number  Node PRIMARY KEY,
  name        varchar(20),
  value       varchar(254)
);
CREATE TABLE DOCUMENT (
  doc_Number Node Primary Key,
  dtd_entry  Node,
  value      lvarchar
);
