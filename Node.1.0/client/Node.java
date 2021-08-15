// class representing A Node type
//
// The functions Ancestors(Node), Graft(Node, Node, Node), and
// Increment(Node, int) are not implemented in Java yet.
//
// Version 1.0, Jan 30, 2001

package com.informix.Node;

import java.sql.*;
import java.util.*;


public class Node implements SQLData {
  // There is a potential problem in using int type
  // for the values since the Node type defines the
  // values as unsigned int.
  // In practice, this should not cause a problem.
  public int count;
  public int[] values;

  private String sql_type = "Node";

  public Node() {
  }

  public Node(String v) {
    StringTokenizer tz = new StringTokenizer(v, ".");
    count = tz.countTokens();
    values = new int[count];
    for (int i = 0; i < count; i++)
      values[i] = new Integer(tz.nextToken()).intValue();
    }

  public String getSQLTypeName() {
	return sql_type;
  }

  public void readSQL(SQLInput stream, String type)
  throws SQLException {
    sql_type = type;
    count = stream.readInt();
    values = new int[count];
    for (int i = 0; i < count; i++)
      values[i] = stream.readInt();
  }

  public void writeSQL(SQLOutput stream)
	throws SQLException {

    stream.writeInt(count);
    for (int i = 0; i < count; i++)
      stream.writeInt(values[i]);
  }
  /*
   * Function not required by SQLData interface, but makes it easier
   * for displaying results.
   */
  public String toString() {
    StringBuffer s = new StringBuffer(20);
    for (int i = 0; i < count; i++) {
      if (i > 0)
        s.append(".");
      s.append(values[i]);
    }
    return s.toString();
  }
  public int length() {
    return (count);
  }
  public int getMember(int ix) {
    if (ix >= count)
      return (-1);
    return(values[ix]);
  }
  public int compare(Node v) {
    int c2, len, member;
    c2 = v.length();
    if (c2 > count)
      len = c2;
    else
      len = count;
    for (int i = 0; i < len; i++) {
      member = v.getMember(i);
      if (values[i] == member)
        continue;
      if (values[i] > member)
        return (1);
      return(-1);
    }
    if (count == c2)
      return(0);
    if (count > c2)
      return(1);
    return(-1);
  }
  public boolean equal(Node v) {
    return(0 == this.compare(v));
  }
  public boolean notEqual(Node v) {
    return(0 != this.compare(v));
  }
  public boolean greaterThan(Node v) {
    return(1 == this.compare(v));
  }
  public boolean greaterThanOrEqual(Node v) {
    return(0 <= this.compare(v));
  }
  public boolean lessThan(Node v) {
    return(-1 == this.compare(v));
  }
  public boolean lessThanOrEqual(Node v) {
    return(0 >= this.compare(v));
  }
  public boolean isParent(Node v) {
    int c2, member;
    c2 = v.length();
    if (c2 != (count - 1))
      return(false);
    for (int i = 0; i < c2 - 1; i++) {
      member = v.getMember(i);
      if (values[i] != member)
        return(false);
    }
    return(true);
  }
  public boolean isChild(Node v) {
    return(v.isParent(this));
  }
  public boolean isAncestor(Node v) {
    int c2, member;
    c2 = v.length();
    if (c2 < count)
      return(false);
    for (int i = 0; i < count - 1; i++) {
      member = v.getMember(i);
      if (values[i] != member)
        return(false);
    }
    return(true);
  }
  public boolean isDescendant(Node v) {
    return(v.isAncestor(this));
  }
  public Node increment(Node v) {
    Node ret = new Node();
    ret.count = v.count;
    ret.values = new int[ret.count];
    for (int i = 0; i < v.count; i++)
      ret.values[i] = v.values[i];
    ret.values[ret.count - 1] = ret.values[ret.count - 1] + 1;
    return(ret);
  }
  public Node newLevel(Node v) {
    Node ret = new Node();
    ret.count = v.count + 1;
    ret.values = new int[ret.count];
    for (int i = 0; i < v.count; i++)
      ret.values[i] = v.values[i];
    ret.values[ret.count - 1] = 1;
    return(ret);
  }
}
