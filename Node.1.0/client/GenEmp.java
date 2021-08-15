//
// Create a load file for the employee table
// Each level has N employees
//
import java.io.*;

// file output example:
// 1||AAA|
// 2|1|AAAB|
public class GenEmp {
  public static void main(String[] args) {
    if (args.length != 2) {
      System.err.println("Usage: GenEmp nblevels nbemps");
      return;
    }
    String initEmp = "1||Joe|\n";
    int nblevels = new Integer(args[0]).intValue();
    nblevels--;
    int nbemp = new Integer(args[1]).intValue();
    int i, empnb, count, lastmgr;
    BufferedOutputStream out;
    try {
    out = new BufferedOutputStream(new FileOutputStream("emp.unl"));
    } catch(FileNotFoundException e) {
      System.out.println("File not found!");
      return;
    }
    try {
      // Insert the top level and go from there
      out.write(initEmp.getBytes());
      empnb = 2;
      lastmgr = 1;
      for (int lvl = 0; lvl < nblevels; lvl++) {
        count = 1;
        for (i = 0; i <= lvl; i++) {
	count *= nbemp;
	}
        for (int j = 0; j < count;) {
          for (i = 0; i < nbemp; i++) {
	    out.write((empnb + "|" + lastmgr + "|Joe" + empnb + "|\n").getBytes());
	    empnb++;
	    j++;
          }
	  lastmgr++;
        }
      }
      out.close();
    } catch (IOException e) {
      System.out.println("IO Exception");
      return;
    }
    return;
  }
}
