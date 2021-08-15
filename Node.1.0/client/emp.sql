CREATE TABLE employee (
Employee_Id	integer,
Manager_Id	integer,
Last_Name	varchar(30),
PRIMARY KEY(Employee_Id),
FOREIGN KEY (Manager_Id) REFERENCES employee(Employee_Id)
);
LOAD FROM "emp.unl" INSERT INTO employee;

SELECT COUNT(*) FROM employee;
