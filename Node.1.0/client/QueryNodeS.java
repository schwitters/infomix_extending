//
// Create a load file for the employee table
// Each level has N employees
//
import java.io.*;
import java.sql.*;
import com.informix.jdbc.*;

public class QueryNodeS {
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

    // Prepare the statement
    pstmt = conn.prepareStatement(sqlstmt);
    pstmt.setString(1, "1.7.43.256.1537");

    rs = pstmt.executeQuery();

    String stringNode;
    while (true == rs.next()) {
      stringNode = rs.getString(1);
      lastName = rs.getString(2);
      System.out.println("Node: " + stringNode + ", name: " + lastName);
    } // end while

    rs.close();
    conn.close();
    return;
  }
}
