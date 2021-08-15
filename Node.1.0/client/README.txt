The following directory contains client code using the Node type.
It provides a comparison between two employee hierarchy implementations.

The first step consist in compiling the programs after some minor modifications
as described below:

make -f Makefile

The programs consist of the following:

GenEmp.java:
  This program generates a load file for the employee table.
  The program takes 2 arguments: the depth of the hierarchy and the
  number of direct reports.

  ex: java GenEmp 6 6

  Once the load file (emp.unl) is created, you can use emp.sql to create
  and load the table.

GenEmp2.java:
  This program generates a load file for the employee2 table. This table used
  the Node type to represent the hierarchy.
  The program takes 2 arguments: the depth of the hierarchy and the
  number of direct reports.

  ex: java GenEmp2 6 6

  Once the load file (emp2.unl) is created, you can use emp2.sql to create
  and load the table.

GetEmpCount.java:
  This is a client program that times the time required to find the total
  number of employees under a manager for the emp and emp2 tables. The standard
  relational table supports 2 methods as discussed in chapter 2.

  You need to modify this program to reflect the appropriate connection URL.
  Change the URL at line 9.

Node.java:
  This Java class implements the Node type. It allows you to send and receive
  Nodes directly to and from the database server. This class is used in
  QueryNode.java.

QueryNode.ec:
  This is an ESQL/C program showing how to manipulate a Node as a variable -size
  opaque type.

QueryNode.java:
  This is a Java program demonstrating the use of the Node type. The Node type
  is sent and received as a class Node.

  You need to change the URL on line 11 to identify your server and your database.

QueryNodeS.java:
  This is similar to QueryNode.java except that it manipilates a Node as a
  String class.

  You need to change the URL on line 11 to identify your server and your database.

The rest of the files are as follows:

AncestorSet.sql:
  This SPL function takes a node and returnes a set of all its ancestors
  including itself.

BOM.sql:
  A simple example of table definitions for a bill-of-material
  
emp.sql:
  This SQL script creates the employee table and loads the rows from the 
  emp.unl file. This file is created with the GenEmp.java Java program.

emp_d.sql:
  This SQL script drops the employee table.

emp2.sql:
  This SQL script creates the employee2 table and loads the rows from the 
  emp2.unl file. This file is created with the GenEmp.java Java program.

emp2_d.sql:
  This SQL script drops the employee2 table.

Makefile:
  This is the makefile used to compile the programs included in this
  directory.

perftest.bat:
  Test script for different depth of employee hierarchies.

Policy.sql:
  A simple example of table definitions where the tables are related
  to each other based on a location hierarchy and a corporation ID.

Query.sql:
  Example of queries.

README.txt:
  This file.

XML.sql:
  Simple example of tables representing DTDs and XML documents.
