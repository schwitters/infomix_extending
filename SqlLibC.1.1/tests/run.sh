#!/bin/sh
# run.sh
#
# Simple script to run the test scripts and save the output.
#
# You must run the setup script before you run this.
#
echo "ceil()..."
dbaccess -e SqlLibDB < testceil.sql > run.out 2>&1
echo "char() and ascii()..."
dbaccess -e SqlLibDB < testchr.sql >> run.out 2>&1
echo "instr()..."
dbaccess -e SqlLibDB < testinstr.sql >> run.out 2>&1
echo "instrb()..."
dbaccess -e SqlLibDB < testinstrb.sql >> run.out 2>&1
echo "sign()..."
dbaccess -e SqlLibDB < testsign.sql >> run.out 2>&1
echo "to_decimal()..."
dbaccess -e SqlLibDB < testto_decimal.sql >> run.out 2>&1
echo "to_float()..."
dbaccess -e SqlLibDB < testto_float.sql >> run.out 2>&1
echo "to_integer()..."
dbaccess -e SqlLibDB < testto_integer.sql >> run.out 2>&1
echo "done."
