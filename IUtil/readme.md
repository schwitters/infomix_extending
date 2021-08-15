# IUtil Component

* * *

Like the SqlLib DataBlade module, the <span class="datablade">IUtil</span> Component implements SQL routines that the Informix Dynamic Server does not support natively but that are supported by some other database vendors. IUtil is implemented using the C language, and the source code is freely available for download.

## <a name="contents">Contents</a>

*   [Overview](#overview)

*   [Required Software](#requirements)

*   [Getting Started](#start_c)

    *   [Download the Distribution](#download_c)
    *   [Build the executable](#build_c)
    *   [Install](#install_c)
    *   [Register](#register_c)
    *   [Restrictions](#restrictions_c)
*   [Supported Data Types](#types)

*   [User-Defined Routines (UDRs)](#source)

    *   [ascii](#ascii)
    *   [ceil](#ceil)
    *   [chr](#chr)
    *   [iday](#iday)
    *   [dayname](#dayname)
    *   [dayofweek](#dayofweek)
    *   [dayofyear](#dayofyear)
    *   [degrees](#degrees)
    *   [floor](#floor)
    *   [radians](#radians)
    *   [sign](#sign)
*   [Revision History](#history)

* * *

## <a name="overview">Overview</a>

Differences in database vendor SQL support make it difficult to migrate applications between different database vendor products, especially if an application relies on a SQL feature in one database that is not available in the other.

The <span class="datablade">IUtil</span> component was developed to help a customer migrate an application from IBM DB2 to Informix IDS. It implements a specific subset of the proprietary functions that are built into DB2, and it can be extended to include virtually any others that you may require.

* * *

## A Note On Terminology

User-implemented extensions to IDS have variously been called "DataBlades", "DataBlade modules", "Bladelets", "Components", "Plug-ins", and probably other things as well. For the purposes of this discussion, all of these expressions mean the same thing.

Since the author of IUtil chose to refer to it as a "component", we'll use that term for it in this writeup.

[Contents](#contents)

* * *

## <a name="requirements">Required Software</a>

To run <span class="datablade">IUtil</span>, you need IDS 9.21 installed on your system. To rebuild the executable, you'll also need a C compiler.

The IUtil distribution comes with a pre-built shared object for Solaris and NT.

<span class="datablade">IUtil</span> was tested with the followint software releases:

> <span class="underline">SUN:</span>
> 
> *   Solaris 2.7
> *   IDS 9.21.UC1
> *   SUNpro C Compiler
> 
> <span class="underline">NT:</span>
> 
> *   Windows NT 4.0 with Service Pack 5
> *   IDS 9.21.TC1
> *   Microsoft Visual C++ 6.0

[Contents](#contents)

* * *

## <a name="start_c">Getting Started</a>

### <a name="download_c">Download the Distribution</a>

Download [IUtil.zip](/idn-secure/foundation/Bladelets/SWDownload/IUtil.zip), which contains the IUtil distribution, and extract the files from the zip archive.

The distribution is organized in a hierarchy under a directory named <span class="filename">IUtil</span>. The directory contents are summarized in the table below.

<table border="1" cellpadding="5">

<tbody>

<tr>

<th colspan="2">IUtil</th>

</tr>

<tr>

<td valign="top"><span class="filename">IUtil.htm</span></td>

<td valign="top"><span class="tbl-nop">This file.</span></td>

</tr>

<tr>

<td valign="top"><span class="filename">demo/</span></td>

<td valign="top"><span class="tbl-nop">Subdirectory with test scripts and sample output.</span></td>

</tr>

<tr>

<td valign="top"><span class="filename">scripts/</span></td>

<td valign="top"><span class="tbl-nop">SQL registration scripts. The contents of this directory should be copied to:</span>

<pre class="code-block">$INFORMIXDIR/extend/IUtil </pre>

</td>

</tr>

<tr>

<td valign="top"><span class="filename">src/</span>  
      IUtil_NT.mak  
      SOL.mak  
      WinNT-i386/  
      bin/  
      SOL/  
      Debug/  
      c/  
</td>

<td valign="top"><span class="tbl-nop">Source code for the UDRs.</span>  
      Makefile for NT  
      Makefile for Solaris  
      Build directory for NT  
      Build directory for Solaris  
      Working directory for Solaris  
      Working direcotory for NT  
      Source code directory  
</td>

</tr>

</tbody>

</table>

### <a name="build_c">Build the shared object</a>

This distribution includes the component built for Solaris 2.7 and for NT 4.0:

*   The Solaris shared object is in the <span class="filename">src/bin</span> subdirectory.
*   The NT dynamic link library (DLL) is in the <span class="filename">src/WinNT-i386</span> subdirectory.

If you are not on one of those platforms or if you modify the source code, recompile the DataBlade module using the instructions below.

<dl>

<dt><span class="underline">UNIX</span></dt>

<dd>

<span class="filename">SOL.mak</span> builds the component shared object under UNIX. Build the shared object with this command:

<pre class="code-block">make -f SOL.mak
</pre>

Detailed instructions for building a UNIX shared object library are documented in the <span class="title">DataBlade Developers Kit User Guide</span>, which is available for download from the Informix [online documentation web site](/answers).

</dd>

<dt><span class="underline">NT</span></dt>

<dd>

<span class="filename">IUtil_NT.mak</span> builds the DataBlade DLL at the DOS prompt. You can build a debug or release version (debug is the default):

<pre class="code-block">nmake /f IUtil_NT.mak CFG="SqlLibC - Win32 Debug"
nmake /f IUtil_NT.mak CFG="SqlLibC - Win32 Release"
</pre>

Note that it may be necessary to edit IUtil_NT.mak to include the correct path for the DevStudio header files and libraries on your machine.

The DLL gets output to <span class="filename">Debug\IUtil.bld</span>

You can also load <span class="datablade">IUtil</span> source code into Microsoft DevStudio by following the instructions in the [<span class="title">Building a DataBlade DLL using MS DevStudio</span>](/idn-secure/DataBlade/Library/nt_dll.htm) technical note. It explains how to create a new project and add the <span class="filename">*.c</span>, <span class="filename">*.h</span>, and <span class="filename">*.def</span> files to that project.

</dd>

</dl>

### <a name="install_c">Install</a>

<span class="obj-name">CURRENTDIR</span> in the instructions below refers to the root location for this component distribution.

> Login as user _informix_, or, on NT, as a user who is a member of the Informix-Admin group.
> 
> <span class="underline">Unix:</span>
> 
> <pre class="code-block">mkdir $INFORMIXDIR/extend/IUtil
> cd $CURRENTDIR
> cp ./src/bin/IUtil.bld $INFORMIXDIR/extend/IUtil/
> cp ./scripts/* $INFORMIXDIR/extend/IUtil/
> </pre>
> 
> <span class="underline">NT (using MKS):</span>
> 
> <pre class="code-block">mkdir $INFORMIXDIR/extend/IUtil
> cd $CURRENTDIR
> cp ./src/WinNT-i386/IUtil.bld $INFORMIXDIR/extend/IUtil
> cp ./scripts/* $INFORMIXDIR/extend/IUtil
> </pre>

### <a name="register_c">Register</a>

Register the bladelet at the UNIX prompt:

<pre class="code-block">    dbaccess -e < $INFORMIXDIR/extend/IUtil/register.sql 
</pre>

Or at the DOS prompt:

<pre class="code-block">    dbaccess -e < %INFORMIXDIR%\extend\IUtil\register.sql
</pre>

### <a name="restrictions_c">Restrictions</a>

IUtil cannot be registered in a database that already has SqlLibC or SqlLibJ installed because all three create some objects with identical names.

[Contents](#contents)

* * *

## <a name="types">Supported Data Types</a>

The <span class="datablade">IUtil</span> component does not create any new types. It supports the built-in date and datetime SQL types, as well as decimal and double precision.

[Contents](#contents)

* * *

## <a name="source">User-Defined Routines (UDRs)</a>

## <a name="ascii">ascii</a>

### Description

<span class="code-inline">ascii()</span> takes a single character as input and returns the ASCII value, in decimal, that corresponds to that character.

### Syntax

<span class="syntax">ascii (char) returns integer</span>

### Example

The next query returns the ASCII value of the letter "a":

<pre class="code-block">> execute function ascii('a');

(expression)

          97

1 row(s) retrieved.
</pre>

The next query returns the ASCII value for the space character:

<pre class="code-block">> execute function ascii(' ');

(expression)

          32

1 row(s) retrieved.
</pre>

### See Also

See the description of [chr](#chr).

[Contents](#contents)

* * *

## <a name="ceil">ceil</a>

### Description

<span class="code-inline">ceil()</span> takes a single numeric value as input and returns the smallest integer that is either equal to or greater than the input value.

### Syntax

<span class="syntax">ceil (numeric value) returns integer</span>

### Example

The queries below demonstrate the basic functionality:

<pre class="code-block">> execute function ceil(123.456);

(expression)

         124

1 row(s) retrieved.

> execute function ceil(-123.456);

(expression)

        -123

1 row(s) retrieved.

> execute function ceil(0.00);

(expression)

           0

1 row(s) retrieved.
</pre>

You can pass <span class="code-inline">ceil()</span> any numeric value, but if the value exceeds the size of an integer, you get error 1215:

<pre class="code-block">> execute function ceil(1234567890123456::decimal);

(expression)

 1215: Value exceeds limit of INTEGER precision
</pre>

### See Also

See the description of [floor](#floor).

[Contents](#contents)

* * *

## <a name="chr">chr</a>

### Description

<span class="code-inline">chr()</span> takes an integer value and returns the character that is represented by the ASCII value (in decimal) of that integer.

The integer value must be between 0 and 127.

### Syntax

<span class="syntax">chr (integer) returns char</span>

### Example

The next query returns the character value for ASCII code 97:

<pre class="code-block">> execute function chr(97);

(expression)

          a

1 row(s) retrieved.
</pre>

The next query returns the character value for ASCII code 32 (the space character):

<pre class="code-block">> execute function chr(32);

(expression)

1 row(s) retrieved.
</pre>

Since <span class="code-inline">ascii()</span> and <span class="code-inline">chr()</span> are symmetric, nesting calls sometimes make test results more visible:

<pre class="code-block">> execute function ascii(chr(32));

(expression)

          32

1 row(s) retrieved.
</pre>

### See Also

See the description of [ascii](#ascii).

[Contents](#contents)

* * *

## <a name="dayname">dayname</a>

### Description

<span class="code-inline">dayname()</span> takes a date or datetime argument and returns the day of the week for the input argument.

### Syntax

<span class="syntax">dayname (date) returns varchar(100)</span> <span class="syntax">dayname (datetime) returns varchar(100)</span>

### Example

Return the day of the week for the date September 2, 1992:

<pre class="code-block">> execute function dayname ("9/2/1992"::date)

(expression)

           Wednesday

1 row(s) retrieved.
</pre>

[Contents](#contents)

* * *

## <a name="dayofweek">dayofweek</a>

### Description

<span class="code-inline">dayofweek()</span> takes a date or datetime argument and returns an integer representing the day of the week for the input date.

### Syntax

<span class="syntax">dayofweek (date) returns integer</span> <span class="syntax">dayofweek (datetime) returns integer</span>

### Example

Return the day of the week for the date September 2, 1992:

<pre class="code-block">> execute function dayofweek ("9/2/1992"::date)

(expression)

         4

1 row(s) retrieved.
</pre>

[Contents](#contents)

* * *

## <a name="dayofyear">dayofyear</a>

### Description

<span class="code-inline">dayofyear()</span> takes a date or datetime argument and returns an integer representing the day of the year for the input date.

### Syntax

<span class="syntax">dayofyear (date) returns integer</span> <span class="syntax">dayofyear (datetime) returns integer</span>

### Example

Return the day of the year for the date September 2, 1992:

<pre class="code-block">> execute function dayofyear ("9/2/1992"::date)

(expression)

         246

1 row(s) retrieved.
</pre>

[Contents](#contents)

* * *

## <a name="degrees">degrees</a>

### Description

<span class="code-inline">degrees()</span> takes a numeric argument representing radians and converts it to degrees.

### Syntax

<span class="syntax">degrees (decimal) returns double precision</span> <span class="syntax">iday (double precision) returns double precision</span>

### Example

Convert the radian value "1" to degrees:

<pre class="code-block">> execute function degrees (1::double precision);

(expression)

 57.29577951308

1 row(s) retrieved.
</pre>

[Contents](#contents)

* * *

## <a name="floor">floor</a>

### Description

<span class="code-inline">floor()</span> takes a numeric value as input and returns the largest integer that is equal to or less than the input value.

### Syntax

<span class="syntax">floor (numeric value) returns integer</span>

### Examples

<pre class="code-block">> execute function floor (123.456);

(expression)

           123

1 row(s) retrieved.
</pre>

<pre class="code-block">> execute function floor (-123.456);

(expression)

           -124

1 row(s) retrieved.
</pre>

[Contents](#contents)

* * *

## <a name="iday">iday</a>

### Description

<span class="code-inline">iday()</span> takes a date or datetime argument and returns its day component.

### Syntax

<span class="syntax">iday (date) returns integer</span> <span class="syntax">iday (datetime) returns integer</span>

### Example

Return the day component for the date September 2, 1992:

<pre class="code-block">> execute function iday ("9/2/1992"::date)

(expression)

           2

1 row(s) retrieved.
</pre>

[Contents](#contents)

* * *

## <a name="radians">radians</a>

### Description

<span class="code-inline">radians()</span> takes a value representing a measurement in degrees and converts its value to radians.

### Syntax

<span class="syntax">radians (decimal) returns double precision</span> <span class="syntax">radians (double precision) returns double precision</span>

### Example

Convert 57.3 degrees to radians:

<pre class="code-block">> execute function radians ("57.3"::double precision);

(expression)

 1.000073661393

1 row(s) retrieved.
</pre>

[Contents](#contents)

* * *

## <a name="sign">sign</a>

### Description

<span class="code-inline">sign()</span> takes a numeric argument. It returns -1 if the value of the argument is less than 0, 0 if the value is 0, or +1 if the value is greater than 0.

### Syntax

<span class="syntax">sign (decimal) returns integer</span> <span class="syntax">sign (double precision) returns integer</span>

### Examples

<pre class="code-block">> execute function sign (195);

(expression)

           1

1 row(s) retrieved.
</pre>

<pre class="code-block">> execute function sign (-1.6);

(expression)

          -1

1 row(s) retrieved.
</pre>

[Contents](#contents)

* * *

## <a name="history">Revision History</a>

This is the first release of IUtil.

[Contents](#contents)

* * *

Last modified on . Please send your comments or questions concerning this page to ["><nobr>idn-datablades@informix.com</nobr>](mailto:idn-datablades@informix.com?subject=Comments on 
<!--#echo var=).

* * *