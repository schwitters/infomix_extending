//(c) Copyright IBM Corp. 2003  All rights reserved.                 */
//                                                                   */
//This sample program is owned by International Business Machines    */
//Corporation or one of its subsidiaries ("IBM") and is copyrighted  */
//and licensed, not sold.                                            */
//                                                                   */
//You may copy, modify, and distribute this sample program in any    */
//form without payment to IBM,  for any purpose including developing,*/
//using, marketing or distributing programs that include or are      */
//derivative works of the sample program.                            */
//                                                                   */
//The sample program is provided to you on an "AS IS" basis, without */
//warranty of any kind.  IBM HEREBY  EXPRESSLY DISCLAIMS ALL         */
//WARRANTIES EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO*/
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTIC-*/
//ULAR PURPOSE. Some jurisdictions do not allow for the exclusion or */
//limitation of implied warranties, so the above limitations or      */
//exclusions may not apply to you.  IBM shall not be liable for any  */
//damages you suffer as a result of using, modifying or distributing */
//the sample program or its derivatives.                             */
//                                                                   */
//Each copy of any portion of this sample program or any derivative  */
//work,  must include a the above copyright notice and disclaimer of */
//warranty.                                                          */
//                                                                   */
//********************************************************************/
//
// Create a load file for the employee table
// Each level has N employees
//
import java.io.*;
import java.sql.*;
import com.informix.jdbc.*;

public class QueryGuid {
  private static String URL = 
  "jdbc:informix-sqli://ibm-jroy:1526/jroy:INFORMIXSERVER=ol_jroy";
  private static String sqlstmt = "SELECT col1 FROM tab "+
		  "WHERE col1 >= ?";

  public static void main(String[] args) throws SQLException {
    Connection conn;
    PreparedStatement pstmt;
    ResultSet rs;

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
      map.put("Guid", Class.forName("Guid"));
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
    Guid myGuid = new Guid("0ac07282-184e-4103-ad2a-ad8abdab8f0b");
    pstmt.setObject(1, myGuid);
    // pstmt.setString(1, "3ac07282-184e-4103-ad2a-ad8abdab8f0b");
    System.out.println("Guid value: " + myGuid.toString());
    // pstmt.setString(1, myNode.toString());

    // System.out.println("Class returned: " + whatis.getName());
    rs = pstmt.executeQuery();

    String stringGuid;
    while (true == rs.next()) {
      Guid col1Guid = (Guid)rs.getObject(1);
      System.out.println("Col1      : " + col1Guid.toString() );
    } // end while

    rs.close();
    conn.close();
    return;
  }
}
