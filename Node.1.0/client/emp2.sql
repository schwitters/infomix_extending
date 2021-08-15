CREATE TABLE employee2 (
Employee_Id	Node,
Last_Name	varchar(30),
PRIMARY KEY(Employee_Id)
);

LOAD FROM "emp2.unl" INSERT INTO employee2;

SELECT COUNT(*) FROM employee2;
