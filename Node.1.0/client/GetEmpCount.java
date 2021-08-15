//
// Test query performance on standard employee implementation
// versus a Node implementation
//
import java.sql.*;

public class GetEmpCount {
  private static String URL = 
    "jdbc:informix-sqli://cpx0027:1526/jroy:INFORMIXSERVER=ol_jroy";
  private static String sqlstmt =
  "SELECT Employee_Id, Manager_Id " +
  "FROM Employee " +
  "WHERE Manager_Id = ? ";
  private static String sqlstmt2a =
  "SELECT COUNT(*) FROM Employee2 " +
  "WHERE Employee_Id > ";
  private static String sqlstmt2b = " AND Employee_Id < ";

  //--------------------------------------------------------------
  public static
    int getCount(Connection conn, int mgr) throws SQLException
  {
    PreparedStatement pstmt;
    ResultSet rs;
    int count;
    int emp;

    count = 1;
    pstmt = conn.prepareStatement(sqlstmt);
    pstmt.setInt(1, mgr);

    // Execute the query and get the result set back
    rs = pstmt.executeQuery();

    // First check if we got something. If not, Exception!
    while (true == rs.next()) {
      emp = rs.getInt(1);
      count += getCount(conn, emp);
    }
    rs.close();
    return(count);
  }
  public static
    int countIt(Connection conn, int mgr) throws SQLException
  {
    int count;

    // Prepare the statement
    long start = System.currentTimeMillis();
    count = GetEmpCount.getCount(conn, mgr);
    long stop = System.currentTimeMillis();
    System.out.println("Timing (ms): " + Long.toString(stop - start));
    return(count);
  }
  public static
    int countIt2(Connection conn, String mgr) throws SQLException
  {
    PreparedStatement pstmt;
    ResultSet rs;
    int count = 0;

    // Prepare the statement
    pstmt = conn.prepareStatement(sqlstmt2a + "'" + mgr + "'" +
                                  sqlstmt2b + "increment('" + mgr + "')");
    long start = System.currentTimeMillis();
    // Execute the query and get the result set back
    rs = pstmt.executeQuery();

    // First check if we got something. If not, Exception!
    if (true == rs.next()) {
      count = rs.getInt(1);
    }
    rs.close();
    long stop = System.currentTimeMillis();
    System.out.println("Timing (ms): " + Long.toString(stop - start));
    return(count);
  }
  public static
    int countIt3(Connection conn, int mgr) throws SQLException
  {
    StringBuffer buffer;
    PreparedStatement pstmt;
    ResultSet rs;
    int count = 0, totCount = 0;

    long start = System.currentTimeMillis();
    for (int lvl = 1;; lvl++) {
      // Create the statement
      buffer = new StringBuffer(120);
      buffer.append("SELECT COUNT(*) FROM ");
      for (int i = 1; i <= lvl; i++) {
        buffer.append(" Employee e" + i + ",");
      }
      buffer.deleteCharAt(buffer.length() - 1);
      buffer.append(" WHERE e" + lvl + ".Manager_Id = " + mgr);
      for (int i = lvl - 1; i > 0; i--) {
        buffer.append(" AND e" + i + ".Manager_Id = e" +
	              (i+1) + ".Employee_Id");
      }
      // System.out.println(buffer.toString());
      // Prepare the statement
      pstmt = conn.prepareStatement(buffer.toString());
      // Execute the query and get the result set back
      rs = pstmt.executeQuery();
      if (true == rs.next()) {
        count = rs.getInt(1);
      }
      rs.close();
      // System.out.println("Count: " + count);
      totCount += count;
      if (count == 0)
        break;
    } // end while(true)
    long stop = System.currentTimeMillis();
    System.out.println("Timing (ms): " + Long.toString(stop - start));
    return(totCount);
  }
  public static void main( String[] args) throws SQLException {
    Connection conn;
    int ret;
    // Connect to the server
    try {
      Class.forName("com.informix.jdbc.IfxDriver");
    } catch (Exception e) {
        throw new SQLException("Can't load driver!", "U0U01");
    }
    conn = DriverManager.getConnection(URL);

    ret = GetEmpCount.countIt(conn, 1); 
    System.out.println("Count: " + ret);
    ret = GetEmpCount.countIt2(conn, "1.0"); 
    System.out.println("Count: " + ret);
    ret = GetEmpCount.countIt3(conn, 1); 
    System.out.println("Count: " + ret);
    return;
  }
}
