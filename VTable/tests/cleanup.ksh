#
# remove temp files, drop the database.
#
rm -f run.out register.out unregister.out run1.out run2.out

dbaccess - - <<END
drop database vtabdb;
END

