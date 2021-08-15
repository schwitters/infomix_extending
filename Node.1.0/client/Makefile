JAVA_CLASSPATH=-classpath .;/jdbc220/lib/ifxjdbc.jar
JAVA_OPTIONS=-O $(JAVA_CLASSPATH)

all: GenEmp.class GenEmp2.class GetEmpCount.class QueryNode.class QueryNodeS.class

GenEmp.class: GenEmp.java
	javac -O GenEmp.java

GenEmp2.class: GenEmp2.java
	javac -O GenEmp2.java

GetEmpCount.class: GetEmpCount.java
	javac -O GetEmpCount.java

QueryNodeS.class: QueryNodeS.java
	javac $(JAVA_OPTIONS) QueryNodeS.java

QueryNode.class: QueryNode.java Node.class
	javac $(JAVA_OPTIONS) QueryNode.java

Node.class: Node.java
	javac $(JAVA_OPTIONS) Node.java

QueryNode.exe: QueryNode.ec
	esql -o QueryNode.exe QueryNode.ec

init:
	dbaccess -e jroy emp_d
	dbaccess -e jroy emp2_d
	dbaccess -e jroy emp
	dbaccess -e jroy emp2

clean:
	rm *.class *.c *.c_ *.exe *.obj *.map

cleanall: clean
	rm *.unl
