//
// Create a load file for the employee table
// Each level has N employees
//
import java.io.*;
import java.sql.*;
import com.informix.jdbc.*;

public class QueryNode {
  private static String URL = 
  "jdbc:informix-sqli://cpx0027:1526/jroy:INFORMIXSERVER=ol_jroy";
  private static String sqlstmt = 
  "SELECT Employee_Id, Last_Name FROM Employee2 " +
  "WHERE isAncestor(Employee_ID, ?) ORDER BY Employee_Id Desc";

  public static void main(String[] args) throws SQLException {
    Connection conn;
    PreparedStatement pstmt;
    ResultSet rs;
    String lastName;

    // Connect to the server
    try {
      Class.forName("com.informix.jdbc.IfxDriver");
    } catch (Exception e) {
        throw new SQLException("Can't load driver!", "U0U01");
    }
    conn = DriverManager.getConnection(URL);
    // Register a Java class for the Node opaque type
    try {
      System.out.println("Register customized type mapping");
      java.util.Map map = conn.getTypeMap();
      map.put("Node", Class.forName("Node"));
      conn.setTypeMap(map);
      System.out.println("mapping ...ok");
    }
    catch(SQLException e) {
      System.out.println("\n***ERROR: " + e.getErrorCode() +
                         " " + e.getMessage());
      e.printStackTrace();
      return;
    }
    catch (Exception ee) {
      // Node ClassNotFound
      System.out.println("\n***ERROR: " + ee.toString() + " " +
                         ee.toString());
      ee.printStackTrace();
      return;
    }

    // Prepare the statement
    pstmt = conn.prepareStatement(sqlstmt);
    Node myNode = new Node("1.7.43.256.1537");
    pstmt.setObject(1, myNode);
    System.out.println("Node value: " + myNode.toString());
    // pstmt.setString(1, myNode.toString());

    // System.out.println("Class returned: " + whatis.getName());
    rs = pstmt.executeQuery();

    String stringNode;
    while (true == rs.next()) {
      // myNode = (Node)rs.getObject(1);
      stringNode = rs.getString(1);
      lastName = rs.getString(2);
      System.out.println("Node: " + myNode.toString() + ", name: " + lastName);
    } // end while

    rs.close();
    conn.close();
    return;
  }
}
