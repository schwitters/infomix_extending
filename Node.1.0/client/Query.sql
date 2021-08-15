SET EXPLAIN ON;

SELECT * FROM employee2
WHERE isAncestor(employee_Id, "1.7.43.256.1537")
ORDER BY employee_Id Desc;

{
CREATE FUNCTION AncestorsSet( Arg1 NODE )
RETURNS SET(NODE NOT NULL)
	DEFINE 	setRetVal	SET(NODE NOT NULL);
	DEFINE  nNode		NODE;
	
	FOREACH EXECUTE FUNCTION Ancestors(Arg1) INTO nNode
		INSERT INTO TABLE(setRetVal) VALUES ( nNode );
	END FOREACH;

	RETURN setRetVal;

END FUNCTION;
}

SELECT * FROM employee2
WHERE employee_Id IN AncestorsSet("1.7.43.256.1537")
ORDER BY employee_Id Desc;
