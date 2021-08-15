import java.util.*;
import java.sql.*;
import com.informix.jdbc.*;

/** <p>
* The IdnMrLvarchar class implements the SQLData interface
* for the idn_mrLvarchar multi-representational opaque type.
* </p>
* <p>
* IdnMrLvarchar is just a string, but it can be up to 2 gigabytes;
* in other words, it can be larger than the Informix lvarchar type.
* </p>
* 
* Complete information about the Informix UDT is available at the 
* Informix Developers Corner: 
* http://www.informix.com/idn-secure/DataBlade/Library/mrLvarchar.htm
*
* @author jean.anderson@informix.com
* @version 1.0
*/

public class IdnMrLvarchar implements SQLData {

   private String  mrData;
   private String  type;

   // =================================================================
   // Constructors
   // =================================================================

   public IdnMrLvarchar () {
      mrData = null;
   }

   public IdnMrLvarchar (String s) {
      mrData=s;
   }

   // =================================================================
   // toString()
   // =================================================================

   /** Converts an IdnMrLvarchar object to its string representation.
    * @param none
    * @return String
    *
   */
   public String toString() {
      String s = mrData;
      return s;
   }

   // =================================================================
   // SQLData methods
   // =================================================================

   /** Returns the type name.
    * @param none
    * @return String containing the type.
    *
   */
    public String getSQLTypeName() {
        return type;
    }

   /** Converts the Java class to the SQL data output stream. 
    * The IfmxUDTSQLOutput available() method says how many bytes
    * are in the tuple.
    * @param Output stream
    *
   */
    public void writeSQL (SQLOutput stream)
        throws SQLException {
       IfmxUDTSQLOutput out = (IfmxUDTSQLOutput) stream;
       out.writeString(mrData, out.available());
    }

   /** Converts the SQL data in the input stream to the Java class
    * and sets the data type.
    * The IfmxUDTSQLInput available() method says how many bytes
    * are in the tuple.
    * @param Input string
    * @param Type name
    *
   */
    public void readSQL (SQLInput stream, String typeName)
        throws SQLException {

       type = typeName;
       IfmxUDTSQLInput in = (IfmxUDTSQLInput) stream;
       mrData = in.readString(in.available());
    }


   // =================================================================
   // main
   // =================================================================
   /** Entry point for external unit tests that exercise IdnMrLvarchar class.
    * @param args Takes one string argument that contains the ascii 
    * representation for a IdnMrLvarchar object
    * @return No return value
    * @exception exceptions Needs to throw exceptions, once I figure out 
    * what exactly needs to be done.
   */
   public static void main(String[] args) {

      IdnMrLvarchar s;

      if(args.length == 0) {
         s = new IdnMrLvarchar();     
      }
      else {
         s = new IdnMrLvarchar(args[0]);
      }

      if(s.toString() == null)
         System.out.println("IdnMrLvarchar: null");
      else 
        System.out.println("IdnMrLvarchar: " + s.toString());

   } // END main
}
