#!/bin/sh
#
# install.sh
#
# Install the idn_LegacyC and idn_LegacyCPP demos
#
here=`pwd`
echo " "
echo "Installing the idn_LegacyC and idn_LegacyCPP demos"
echo " "

# Gotta run this as user informix...
if test ! `whoami` = 'informix'
then
  echo " "
  echo "This script must be run by the informix user."
  echo " "
  exit
fi

echo " "
echo "...idn_LegacyC..."
echo " "

# create the directory, if not already there
if test ! -d $INFORMIXDIR/extend/idn_LegacyC.1.2
then
  mkdir $INFORMIXDIR/extend/idn_LegacyC.1.2
fi

cd $here/C/scripts
cp objects.sql $INFORMIXDIR/extend/idn_LegacyC.1.2
cp prepare.sql $INFORMIXDIR/extend/idn_LegacyC.1.2
cp prepare.en_us.1252.sql $INFORMIXDIR/extend/idn_LegacyC.1.2
cd $here/C/src/solaris-sparc
cp idn_LegacyC.bld $INFORMIXDIR/extend/idn_LegacyC.1.2
echo " "
echo "...idn_LegacyCPP..."
echo " "

if test ! -d $INFORMIXDIR/extend/idn_LegacyCPP.1.2
then
  mkdir $INFORMIXDIR/extend/idn_LegacyCPP.1.2
fi

cd $here/C++/scripts
cp objects.sql $INFORMIXDIR/extend/idn_LegacyCPP.1.2
cp prepare.sql $INFORMIXDIR/extend/idn_LegacyCPP.1.2
cp prepare.en_us.1252.sql $INFORMIXDIR/extend/idn_LegacyCPP.1.2
cd $here/C++/src/solaris-sparc
cp idn_LegacyCPP.bld $INFORMIXDIR/extend/idn_LegacyCPP.1.2
echo " "
cd $here
echo "done."
echo " "
echo "*********************************************************************"
echo "IMPORTANT: If this is not the first time you have installed these"
echo "DataBlades, you will need to recycle - stop and restart - your server"
echo "before rerunning them."
echo "*********************************************************************"

