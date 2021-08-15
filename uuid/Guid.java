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
// Provide na implementation for a globally unique identifier
//
import java.sql.*;

public class Guid implements SQLData {
  private boolean initialized = false;
  public byte[] value;
  private String sql_type = "Guid";

  public Guid() {
  }
  public Guid(String v) throws NumberFormatException {
	int j = 0;

	value = new byte[16];
	for(int i = 0; i < 36; i += 2) {
	  switch (i) {
	    case 8:
	    case 13:
	    case 18:
	    case 23:
		  // skip the dash
		  i ++;
	    default:
          value[j++] = (byte)Integer.parseInt(v.substring(i, i+2), 16);
	  } // end switch
	} // end for
  }
  public String getSQLTypeName() {
    return sql_type;
  }
  public void readSQL(SQLInput stream, String type)
  throws SQLException {
    sql_type = type;
	value = new byte[16];
	for (int i = 0; i < 16; i++)
      value[i] = stream.readByte();
  }

  public void writeSQL(SQLOutput stream)
	throws SQLException {

    stream.writeBytes(value);
  }
  /*
   * Function not required by SQLData interface, but makes it easier
   * for displaying results.
   */
  public String toString() {
	String b;
    StringBuffer s = new StringBuffer(36);
    for (int i = 0; i < 16; i++) {
	  switch (i) {
		case 4:
		case 6:
		case 8:
		case 10:
			s.append("-");
		default:
          b = Integer.toHexString(0xff & (int)value[i]);
		  if (1 == b.length())
			s.append("0");
          s.append(b);
	  } // end switch
    } // end for
    return s.toString();
  }
  public boolean valid() {
    return (initialized);
  }
  public int compare(Guid v) {
	// Not sure what to do when comparing values that are not valid
    int c2, len, member;
    for (int i = 0; i < 16; i++) {
      if (value[i] == v.value[i])
        continue;
      if (value[i] > v.value[i])
        return (1);
      return(-1);
    }
    return(0);
  }
  public boolean equal(Guid v) {
    return(0 == this.compare(v));
  }
  public boolean notEqual(Guid v) {
    return(0 != this.compare(v));
  }
  public boolean greaterThan(Guid v) {
    return(1 == this.compare(v));
  }
  public boolean greaterThanOrEqual(Guid v) {
    return(0 <= this.compare(v));
  }
  public boolean lessThan(Guid v) {
    return(-1 == this.compare(v));
  }
  public boolean lessThanOrEqual(Guid v) {
    return(0 >= this.compare(v));
  }
}
