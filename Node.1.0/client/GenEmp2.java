//
// Create a load file for the employee table
// Each level has N employees
//
import java.io.*;

// file output example:
// 1||AAA|
// 2|1|AAAB|
public class GenEmp2 {

  public static String genNode(int empnb, int nbemp) {
    // Generate a Node string based on the number of employee per mgr
    // and the current employee number
    StringBuffer result = new StringBuffer();

    for (int i = empnb; i > 1;) {
      i = (int)Math.ceil((i - 1.0) / nbemp);
      if (result.length() > 0) {
        result.insert(0, '.');
      }
      result.insert(0, i);
    }
    result.append('.');
    return(result.toString());
  }
  public static void main(String[] args) {
    if (args.length != 2) {
      System.err.println("Usage: GenEmp nblevels nbemps");
      return;
    }
    String initEmp = "1.0|Joe|\n";
    String lastmgr = null;
    int nblevels = new Integer(args[0]).intValue();
    nblevels--;
    int nbemp = new Integer(args[1]).intValue();
    int i, empnb, count;
    BufferedOutputStream out;
    try {
    out = new BufferedOutputStream(new FileOutputStream("emp2.unl"));
    } catch(FileNotFoundException e) {
      System.out.println("File not found!");
      return;
    }
    try {
      // Insert the top level and go from there
      out.write(initEmp.getBytes());
      empnb = 2;
      for (int lvl = 0; lvl < nblevels; lvl++) {
        count = 1;
        for (i = 0; i <= lvl; i++) {
	count *= nbemp;
	}
        for (int j = 0; j < count;) {
	  lastmgr = GenEmp2.genNode(empnb, nbemp);
          for (i = 0; i < nbemp; i++) {
	    out.write((lastmgr + empnb + "|Joe" + empnb + "|\n").getBytes());
	    empnb++;
	    j++;
          }
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
