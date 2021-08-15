# do it for 2 levels at 6 employees
dbaccess -e jroy emp_d
dbaccess -e jroy emp2_d
java GenEmp 2 6
java GenEmp2 2 6
dbaccess -e jroy emp
dbaccess -e jroy emp2
java GetEmpCount
# do it for 3 levels at 6 employees
dbaccess -e jroy emp_d
dbaccess -e jroy emp2_d
java GenEmp 3 6
java GenEmp2 3 6
dbaccess -e jroy emp
dbaccess -e jroy emp2
java GetEmpCount
# do it for 4 levels at 6 employees
dbaccess -e jroy emp_d
dbaccess -e jroy emp2_d
java GenEmp 4 6
java GenEmp2 4 6
dbaccess -e jroy emp
dbaccess -e jroy emp2
java GetEmpCount
# do it for 5 levels at 6 employees
dbaccess -e jroy emp_d
dbaccess -e jroy emp2_d
java GenEmp 5 6
java GenEmp2 5 6
dbaccess -e jroy emp
dbaccess -e jroy emp2
java GetEmpCount
