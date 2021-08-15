-----------------------------------------------------------------
-- NAME
--     regexp_split.sql
--
-- DESCRIPTION
--     Functional tests for regexp_split() UDR.
--
--     Generated from the U of Toronto regexp tests like this:
--
--     + removed lines with a blank field from the 'tests' file and with 'c'
--
--     + ran this awk command again the 'tests' files:
--
--       BEGIN {FS="\t"; total=1}
--        {print "-- " total++ ". regexp_extract result: '" $3 \
--         "'\nexecute function regexp_split('" $2 "','" $1 "');\n"}
--
--     + Hand mangled what the result should really be, considering
--       The Toronto 'try' result.
--
--       Since regexp_slit and regexp_extract return opposite results,
--       regexp_extract results are also listed below.
--
--------------------------------------------------------------------------

-- execute procedure traceset_regexp("/tmp/re_split.trc", 30);

--
-- 0-a. Boundary checks
--

-- Begin-line           : should return 'abcdefgabcdefg');
execute function regexp_split ('abcdefgabcdefg', '^');

-- First character      : should return 'bcdefg' twice
execute function regexp_split ('abcdefgabcdefg', 'a');

-- Just first character : should return 'bcdefgabcdefg' once
execute function regexp_split ('abcdefgabcdefg', '^a');

-- First two characters : should return 'cdefg' twice
execute function regexp_split ('abcdefgabcdefg', 'ab');

-- Just first two chars : should return 'cdefgabcdefg' once
execute function regexp_split ('abcdefgabcdefg', '^ab');

-- Second character     : should return 'a', 'cdefga', and 'cdefg'
execute function regexp_split ('abcdefgabcdefg', 'b');

-- Last character       : should return 'abcdef' twice
execute function regexp_split ('abcdefgabcdefg', 'g');

-- Just last character  : should return 'abcdefgabcdef' just once
execute function regexp_split ('abcdefgabcdefg', 'g$');

-- End-of-line          : should return 'abcdefgabcdefg'
execute function regexp_split ('abcdefgabcdefg', '$');

-- Nothing to return    : should return 'No rows found'
execute function regexp_split ('abcdefgabcdefg', 'abcdefgabcdefg');

--
-- 0-b. Run queries that popped boundary problems.
--
execute function regexp_split (
    'Durable Construction Coupled with the Brightest Colors Available',
    ' ' );

execute function regexp_split (
    'Durable Construction Coupled with the Brightest Colors Available',
    'C[a-z]*' );

execute function regexp_split (
    'Durable Construction Coupled with the Brightest Colors Available',
    'C[a-z]*', 1 );

execute function regexp_split (
    'Durable Construction Coupled with the Brightest Colors Available',
    'C[a-z]*', 2 );

execute function regexp_split (
    'Durable Construction Coupled with the Brightest Colors Available',
    'C[a-z]*', 3 );

execute function regexp_split (
    'Durable Construction Coupled with the Brightest Colors Available',
    'C[a-z]*', 4 );

execute function regexp_split (
    'Durable Construction Coupled with the Brightest Colors Available',
    'C[a-z]*', 5 );

execute function regexp_split (
   'Minimum Chin Contact, Feather-Light, Maximum Protection Helmet',
   ' ');

execute function regexp_split (
   'Minimum Chin Contact, Feather-Light, Maximum Protection Helmet',
   'M[a-z]*m');

execute function regexp_split (
   'Minimum Chin Contact, Feather-Light, Maximum Protection Helmet',
   'M[a-z]*m', 1);

--
--  =================== Beginning of the Toronto Tests =================
--
-- 1. regexp_extract result: 'abc'
--    regexp_split result  : no rows found
execute function regexp_split('abc','abc');

-- 2. regexp_extract result: no rows found
--    regexp_split result  : 'xbc'
execute function regexp_split('xbc','abc');

-- 3. regexp_extract result: no rows found
--    regexp_split result  : 'axc'
execute function regexp_split('axc','abc');

-- 4. regexp_extract result: no rows found
--    regexp_split result  : 'abx'
execute function regexp_split('abx','abc');

-- 5. regexp_extract result: 'abc'
--    regexp_split result  : q1 'x' 
--                              'y'
--                           q2 'xabcy'
execute function regexp_split('xabcy','abc');
execute function regexp_split('xabcy','abc', 1);

-- 6. regexp_extract result: 'abc'
--    regexp_split result  : q1 'ab'
--                           q2 'ababc'
execute function regexp_split('ababc','abc');
execute function regexp_split('ababc','abc', 1);

-- 7. regexp_extract result: 'abc'
--    regexp_split result  : no rows found
execute function regexp_split('abc','ab*c');

-- 8. regexp_extract result: 'abc'
--    regexp_split result  : no rows found
execute function regexp_split('abc','ab*bc');

-- 9. regexp_extract result: 'abbc'
--    regexp_split result  : no rows found
execute function regexp_split('abbc','ab*bc');

-- 10. regexp_extract result: 'abbbbc'
--     regexp_split result  : no rows found
execute function regexp_split('abbbbc','ab*bc');

-- 11. regexp_extract result: 'abbc'
--     regexp_split result  : no rows found
execute function regexp_split('abbc','ab+bc');

-- 12. regexp_extract result: no rows found
--     regexp_split result  : 'abc'
execute function regexp_split('abc','ab+bc');

-- 13. regexp_extract result: no rows found
--     regexp_split result  : 'abq'
execute function regexp_split('abq','ab+bc');

-- 14. regexp_extract result: 'abbbbc'
--     regexp_split result  : no rows found
execute function regexp_split('abbbbc','ab+bc');

-- 15. regexp_extract result: 'abbc'
--     regexp_split result  : no rows found
execute function regexp_split('abbc','ab?bc');

-- 16. regexp_extract result: 'abc'
--     regexp_split result  : no rows found
execute function regexp_split('abc','ab?bc');

-- 17. regexp_extract result: no rows found
--     regexp_split result  : 'abbbbc'
execute function regexp_split('abbbbc','ab?bc');

-- 18. regexp_extract result: 'abc'
--     regexp_split result  : no rows found
execute function regexp_split('abc','ab?c');

-- 19. regexp_extract result: 'abc'
--     regexp_split result  : no rows found
execute function regexp_split('abc','^abc$');

-- 20. regexp_extract result: no rows found
--     regexp_split result  : 'abcc'
execute function regexp_split('abcc','^abc$');

-- 21. regexp_extract result: 'abc'
--     regexp_split   result: 'c'
execute function regexp_split('abcc','^abc');

-- 22. regexp_extract result: no rows found
--     regexp_split   result: 'aabc'
execute function regexp_split('aabc','^abc$');

-- 23. regexp_extract result: 'abc'
--     regexp_split   result: 'a'
execute function regexp_split('aabc','abc$');

-- 24. regexp_extract result: no rows found
--     regexp_split   result: 'abc'
execute function regexp_split('abc','^');

-- 25. regexp_extract result: no rows found
--
--     Explanation: regexp_match returns 't', but the match is
--     0 length, so the IDS server determines no result.
--
--     regexp_split   result: 'abc'
--
execute function regexp_split('abc','$');

-- 26. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','a.c');

-- 27. regexp_extract result: 'axc'
--     regexp_split   result: no rows found
execute function regexp_split('axc','a.c');

-- 28. regexp_extract result: 'axyzc'
--     regexp_split   result: no rows found
execute function regexp_split('axyzc','a.*c');

-- 29. regexp_extract result: no rows found
--     regexp_split   result: 'axyzd'
execute function regexp_split('axyzd','a.*c');

-- 30. regexp_extract result: no rows found
--     regexp_split   result: 'abc'
execute function regexp_split('abc','a[bc]d');

-- 31. regexp_extract result: 'abd'
--     regexp_split   result: no rows found
execute function regexp_split('abd','a[bc]d');

-- 32. regexp_extract result: no rows found
--     regexp_split   result: 'abd'
execute function regexp_split('abd','a[b-d]e');

-- 33. regexp_extract result: 'ace'
--     regexp_split   result: no rows found
execute function regexp_split('ace','a[b-d]e');

-- 34. regexp_extract result: 'ac'
--     regexp_split   result: 'a'
execute function regexp_split('aac','a[b-d]');

-- 35. regexp_extract result: 'a-'
--     regexp_split   result: no rows found
execute function regexp_split('a-','a[-b]');

-- 36. regexp_extract result: 'a-'
--     regexp_split   result: no rows found
execute function regexp_split('a-','a[b-]');

-- 37. regexp_extract result: no rows found
--     regexp_split   result: 'ab'
execute function regexp_split('ab','[k]');

-- 38. regexp_extract result: 'a]'
--     regexp_split   result: no rows found
execute function regexp_split('a]','a]');

-- 39. regexp_extract result: 'a]b'
--     regexp_split   result: no rows found
execute function regexp_split('a]b','a[]]b');

-- 40. regexp_extract result: 'aed'
--     regexp_split   result: no rows found
execute function regexp_split('aed','a[^bc]d');

-- 41. regexp_extract result: no rows found
--     regexp_split   result: 'abd'
execute function regexp_split('abd','a[^bc]d');

-- 42. regexp_extract result: 'adc'
--     regexp_split   result: no rows found
execute function regexp_split('adc','a[^-b]c');

-- 43. regexp_extract result: no rows found
--     regexp_split   result: 'a-c'
execute function regexp_split('a-c','a[^-b]c');

-- 44. regexp_extract result: no rows found
--     regexp_split   result: 'a]c'
execute function regexp_split('a]c','a[^]b]c');

-- 45. regexp_extract result: 'adc'
--     regexp_split   result: no rows found
execute function regexp_split('adc','a[^]b]c');

-- 46. regexp_extract result: 'ab'
--     regexp_split   result: q1 'c'
--                            q2 'abc'
execute function regexp_split('abc','ab|cd');
execute function regexp_split('abc','ab|cd', 1);

-- 47. regexp_extract result: q1 'ab'
--                               'cd'
--                            q2 'ab'
--
--     regexp_split   result: q1 no rows returned
--                            q2 'abcd'
--
execute function regexp_split('abcd','ab|cd');
execute function regexp_split('abcd','ab|cd',1);

-- 48. regexp_extract result: 'ef'
--     regexp_split   result: 'd'
execute function regexp_split('def','()ef');

-- 49. regexp_extract result: no rows found
--     regexp_split   result: 'b'
execute function regexp_split('b','$b');

-- 50. regexp_extract result: 'a(b'
--     regexp_split   result: no rows found
execute function regexp_split('a(b','a\(b');

-- 51. regexp_extract result: 'ab'
--     regexp_split   result: no rows found
execute function regexp_split('ab','a\(*b');

-- 52. regexp_extract result: 'a((b'
--     regexp_split   result: no rows found
execute function regexp_split('a((b','a\(*b');

-- 53. regexp_extract result: 'a\b'
--     regexp_split   result: no rows found
execute function regexp_split('a\b','a\\b');

-- 54. regexp_extract result: 'a'
--     regexp_split   result: 'bc' 
execute function regexp_split('abc','((a))');

-- 55. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','(a)b(c)');

-- 56. regexp_extract result: 'abc'
--     regexp_split   result: 'aabb'
execute function regexp_split('aabbabc','a+b+c');

-- 57. regexp_extract result: 'ab'
--     regexp_split   result: no rows found
execute function regexp_split('ab','(a+|b)*');

-- 58. regexp_extract result: 'ab'
--     regexp_split   result: no rows found
execute function regexp_split('ab','(a+|b)+');

-- 59. regexp_extract result: q1 'a'
--                               'b'
--                            q2 'a'
--
--     regexp_split   result: q1 no rows found
--                            q2 'ab'
--
execute function regexp_split('ab','(a+|b)?');
execute function regexp_split('ab','(a+|b)?', 1);

-- 60. regexp_extract result: 'cde'
--     regexp_split   result: no rows found
execute function regexp_split('cde','[^ab]*');

-- 61. regexp_extract result: 'abcd'
--     regexp_split   result: no rows found
execute function regexp_split('abcd','abcd');

-- 62. regexp_extract result: 'abcd'
--     regexp_split   result: no rows found
execute function regexp_split('abcd','a(bc)d');

-- 63. regexp_extract result: 'abbbcd'
--     regexp_split   result: no rows found
execute function regexp_split('abbbcd','([abc])*d');

-- 64. regexp_extract result: 'abcd'
--     regexp_split   result: no rows found
execute function regexp_split('abcd','([abc])*bcd');

-- 65. regexp_extract result: 'e'
--     regexp_split   result: no rows found
execute function regexp_split('e','a|b|c|d|e');

-- 66. regexp_extract result: 'ef'
--     regexp_split   result: no rows found
execute function regexp_split('ef','(a|b|c|d|e)f');

-- 67. regexp_extract result: 'abcdefg'
--     regexp_split   result: no rows found
execute function regexp_split('abcdefg','abcd*efg');

-- 68. regexp_extract result: q1 'ab'
--                               'abbb'
--                            q2 'ab'
--
--     regexp_split   result: q1 'x'
--                               'y'
--                               'z'
--                            q2 'xabyabbbz'
--                            q3 'x'
--                               'yabbbz'
--
execute function regexp_split('xabyabbbz','ab*');
execute function regexp_split('xabyabbbz','ab*',1);
execute function regexp_split('xabyabbbz','ab*',2);

-- 69. regexp_extract result: q1 'a'
--                               'abbb'
--                            q2 'a'
--
--     regexp_split   result: q1 'x', 'y', 'z'
--                            q2 'xayabbbz'
--                            q3 'x', 'yabbbz'
--
execute function regexp_split('xayabbbz','ab*');
execute function regexp_split('xayabbbz','ab*', 1);
execute function regexp_split('xayabbbz','ab*', 2);

-- 70. regexp_extract result: 'cde'
--     regexp_split   result: 'ab'
execute function regexp_split('abcde','(ab|cd)e');

-- 71. regexp_extract result: 'hij'
--     regexp_split   result: no rows found
execute function regexp_split('hij','[abhgefdc]ij');

-- 72. regexp_extract result: no rows found
--     regexp_split   result: 'abcde'
execute function regexp_split('abcde','^(ab|cd)e');

-- 73. regexp_extract result: 'ef'
--     regexp_split   result: 'abcd'
execute function regexp_split('abcdef','(abc|)ef');

-- 74. regexp_extract result: 'bcd'
--     regexp_split   result: 'a'
execute function regexp_split('abcd','(a|b)c*d');

-- 75. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','(ab|ab*)bc');

-- 76. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','a([bc]*)c*');

-- 77. regexp_extract result: 'abcd'
--     regexp_split   result: no rows found
execute function regexp_split('abcd','a([bc]*)(c*d)');

-- 78. regexp_extract result: 'abcd'
--     regexp_split   result: no rows found
execute function regexp_split('abcd','a([bc]+)(c*d)');

-- 79. regexp_extract result: 'abcd'
--     regexp_split   result: no rows found
execute function regexp_split('abcd','a([bc]*)(c+d)');

-- 80. regexp_extract result: 'adcdcde'
--     regexp_split   result: no rows found
execute function regexp_split('adcdcde','a[bcd]*dcdcde');

-- 81. regexp_extract result: no rows found
--     regexp_split   result: 'adcdcde'
execute function regexp_split('adcdcde','a[bcd]+dcdcde');

-- 82. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','(ab|a)b*c');

-- 83. regexp_extract result: 'abcd'
--     regexp_split   result: no rows found
execute function regexp_split('abcd','((a)(b)c)(d)');

-- 84. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','[ -~]*');

-- 85. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','[ -~ -~]*');

-- 86. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','[ -~ -~ -~]*');

-- 87. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','[ -~ -~ -~ -~]*');

-- 88. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','[ -~ -~ -~ -~ -~]*');

-- 89. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','[ -~ -~ -~ -~ -~ -~]*');

-- 90. regexp_extract result: 'abc'
--     regexp_split   result: no rows found
execute function regexp_split('abc','[ -~ -~ -~ -~ -~ -~ -~]*');

-- 91. regexp_extract result: 'alpha'
--     regexp_split   result: no rows found
execute function regexp_split('alpha','[a-zA-Z_][a-zA-Z0-9_]*');

-- 92. regexp_extract result: 'bh'
--     regexp_split   result: 'a'
execute function regexp_split('abh','^a(bc+|b[eh])g|.h$');

-- 93. regexp_extract result: 'effgz'
--     regexp_split   result: no rows found
execute function regexp_split('effgz','(bc+d$|ef*g.|h?i(j|k))');

-- 94. regexp_extract result: 'ij'
--     regexp_split   result: no rows found
execute function regexp_split('ij','(bc+d$|ef*g.|h?i(j|k))');

-- 95. regexp_extract result: no rows found
--     regexp_split   result: 'effg'
execute function regexp_split('effg','(bc+d$|ef*g.|h?i(j|k))');

-- 96. regexp_extract result: no rows found
--     regexp_split   result: 'bcdd'
execute function regexp_split('bcdd','(bc+d$|ef*g.|h?i(j|k))');

-- 97. regexp_extract result: 'effgz'
--     regexp_split   result: 'r'
execute function regexp_split('reffgz','(bc+d$|ef*g.|h?i(j|k))');

-- 98. regexp_extract result: 'a'
--     regexp_split   result: no rows found
execute function regexp_split('a','(((((((((a)))))))))');

-- 99. regexp_extract result: no rows found
--     regexp_split   result: 'uh-uh'
execute function regexp_split('uh-uh','multiple words of text');

-- 100. regexp_extract result: 'multiple words'
--      regexp_split   result: ', yeah'
execute function regexp_split('multiple words, yeah','multiple words');

-- 101. regexp_extract result: 'abcde'
--     regexp_split    result: no rows found
execute function regexp_split('abcde','(.*)c(.*)');

-- 102. regexp_extract result: '(a, b)'
--     regexp_split    result: no rows found
execute function regexp_split('(a, b)','\((.*), (.*)\)');


-- ======================= Negative tests =============================
--
-- Each of these should fail to compile the regular expression
--

EXECUTE FUNCTION regexp_split( 'abc' , 'a[b-a]' );
EXECUTE FUNCTION regexp_split( 'abc' , 'a[]b' );
EXECUTE FUNCTION regexp_split( 'abc' , 'a[' );
EXECUTE FUNCTION regexp_split( 'abc' , '()*' );
EXECUTE FUNCTION regexp_split( 'abc' , '*a' );
EXECUTE FUNCTION regexp_split( 'abc' , '^*' );
EXECUTE FUNCTION regexp_split( 'abc' , '$*' );
EXECUTE FUNCTION regexp_split( 'abc' , '(*)b' );
EXECUTE FUNCTION regexp_split( 'abc' , 'a\' );
EXECUTE FUNCTION regexp_split( 'abc' , 'abc)' );
EXECUTE FUNCTION regexp_split( 'abc' , '(abc' );
EXECUTE FUNCTION regexp_split( 'abc' , 'a**' );
EXECUTE FUNCTION regexp_split( 'abc' , 'a*?' );
EXECUTE FUNCTION regexp_split( 'abc' , '(a*)*' );
EXECUTE FUNCTION regexp_split( 'abc' , '(a*)+' );
EXECUTE FUNCTION regexp_split( 'abc' , '(a|)*' );
EXECUTE FUNCTION regexp_split( 'abc' , '(a*|b)*' );
EXECUTE FUNCTION regexp_split( 'abc' , '(^)*' );
EXECUTE FUNCTION regexp_split( 'abc' , '(ab|)*' );
EXECUTE FUNCTION regexp_split( 'abc' , ')(' );
EXECUTE FUNCTION regexp_split( 'abc' , '((a*|b))*' );
EXECUTE FUNCTION regexp_split( 'abc' , '((((((((((a))))))))))' );

