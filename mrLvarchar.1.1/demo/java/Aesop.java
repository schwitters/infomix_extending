import java.io.*;
import java.util.*;
import java.sql.*;
import com.informix.jdbc.*;

/** Aesop
* 
*  Find fables that match a regular expression
*/


public class Aesop {

   /** printException outputs exception information.
    *
    * @param Input Exception
    * @param Input Name of the function in which error occurred
    * @param Input Name of the JDBC call that failed
   */
   public static void printException (Exception e, String func, String call) {

      System.out.println("\nERROR: " + func + "->" + call + " failed.");
      System.out.println("Exception thrown of type " + 
         e.getClass().getName() + "\n" + 
         "Message is '" + e.getMessage() + "'");

      e.printStackTrace();

      return;
   }

   /** printSqlError outputs SQL error information, including 
    * multiple messages if there are more than one.
    *
    * @param Input SQLException
    * @param Input Name of the function in which error occurred
    * @param Input Name of the JDBC call that failed
   */
   public static void printSqlError (SQLException e, String func, String call) {

      System.out.println("\nERROR: " + func + "->" + call + " failed.");

      do {
         System.out.println();
         System.out.println("  SQLState = " + e.getSQLState());
         System.out.println("  SQLCODE  = " + e.getErrorCode());
         System.out.println("  Message  = " + e.getMessage());
         e = e.getNextException() ;
      } while (e != null);

      return;
   }

   /** initIfx initializes Informix JDBC driver.
    *
    * @param Input none
   */
   public static void initIfx () {

      try {
         Class.forName("com.informix.jdbc.IfxDriver");
         System.out.println("Informix JDBC driver loaded.");
      } 
      catch (Exception e) {
         System.out.println("ERROR: initIfx: failed to load Informix JDBC driver.");
         System.out.println("Exception thrown of type " + 
	   e.getClass().getName() + "\n" + 
           "Message is '" + e.getMessage() + "'");
         e.printStackTrace();
      }

      return;
   } // END initIfx

   /** getDbConn establishes database connection.
    *
    * @param Input database url
   */
   public static Connection getDbConn (String url) {

      Connection conn = null;

      try {
         conn = DriverManager.getConnection(url);
         System.out.println("Database connection established.");
      } catch (SQLException e) {
            printSqlError (e, "getDbConn", "getConnection()");
      } 

      return conn;
   } // END getDbConn

   /** Closes database connection.
    * @param Input Connection handle
   */
   public static void releaseDbConn (Connection conn) {

      if (conn == null)
         return;

      try {
         conn.close();
         System.out.println("Database connection closed.");
      } 
      catch (SQLException e) {
         printSqlError(e, "releaseDbConn", "close()");
      }
      return;
   } // END releaseDbConn


   /** Command line entry point. Accepts regular expression,
    *  executes a query that looks for all fables that match that
    *  regular expression, then outputs results.
    *  @param Input database
   */

   public static void main(String[] args) {

      String            re;
      String            prompt;
      int               done=0;
      Connection        conn = null;

      if (args.length == 0) {
         System.out.println("ERROR: database URL required.");
         return;
      }

      String url = args[0];

      // Initialize Informix JDBC driver and get database connection.
      initIfx ();
      conn = getDbConn (url);
      if(conn == null)
         return;
      
      // Get the name of the database to use in the prompt
      try {
         prompt=conn.getCatalog();
      } 
      catch(Exception e) {
	printException(e, "Main", "getCatalog");
        return; // this will abort
      }

      // Set up the type mapping for IdnMrLvarchar
      try {
          java.util.Map customTypeMap = conn.getTypeMap();
          customTypeMap.put("idn_mrLvarchar", Class.forName("IdnMrLvarchar"));
        }
        catch (Exception e) {
          System.out.println("Failed to set custom typemap.");
          System.out.println(e.getMessage());
          return;
        }

      // Main guts of the application that executes queries
      try {

         // Read standard input
         BufferedReader stdin =
           new BufferedReader(
             new InputStreamReader(System.in));

         System.out.println("Enter 'help' (without quotes) to get started.");

         while(done == 0) {

            System.out.print(prompt + "> ");
            re = stdin.readLine();

            if(re.equals("quit"))
               done=1;
            else if (re.equals("help")){
               System.out.println(" ");
               System.out.println("Enter a regular expression (without quotes)"
                  + " to find a fable that matches.");
               System.out.println("For example, this application searches for"
                  + " 'Crow', 'crow', 'Raven', or ");
               System.out.println("'raven' with the following regular expression:");
               System.out.println(" ");
               System.out.println("        [Cc]row|[Rr]aven");
               System.out.println(" ");
               System.out.println("Enter 'quit' to exit this application.");
               System.out.println(" ");
            }
            else {
               // Execute the query and print the results
               // System.out.println(re);

            try
                {
                PreparedStatement pstmt = conn.prepareStatement(
                      "select title, fable from aesop "
                    + "where regexp_match(fable, ?);");
                pstmt.setString(1, re);
                ResultSet r = pstmt.executeQuery();

                System.out.println(" ");

                int count = 0;
                while(r.next())
                    {
                    String col1 = r.getString(1);
                    System.out.println("Title: " + col1);

                    IdnMrLvarchar col2 = (IdnMrLvarchar)r.getObject(2);
                    System.out.println(col2.toString());

                    System.out.println(" ");
                    count++;
                    }
                   
                System.out.println("Number of fables found: " + count);

                r.close();
                pstmt.close();
                } catch (SQLException e) {
                     printSqlError(e, "main", "execute query");
                }
            }
         }
      }
      
      catch(Exception e) {
	printException(e, "Main", "while loop");
        return; // this will abort
      }

      // Close database connection 
      releaseDbConn(conn);
      System.out.println("bye");

   } // END 
}
