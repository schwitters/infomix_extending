-----------------------------------------------------------------
-- NAME
--     regexp_exgract.sql
--
-- DESCRIPTION
--     Functional tests for regexp_extract() UDR.
--
--     Generated from the U of Toronto regexp tests like this:
--
--     + removed lines with a blank field from the 'tests' file and with 'c'
--
--     + ran this awk command again the 'tests' files:
--
--       BEGIN {FS="\t"; total=1}
--        {print "-- " total++ ". result: '" $3 \
--         "'\nexecute function regexp_extract('" $2 "','" $1 "');\n"}
--
-----------------------------------------------------------------

-- execute procedure traceset_regexp("/tmp/re_extract.trc", 30);

--
-- 0. Boundary checks
--

-- Begin-line           : should return 'No rows found'
execute function regexp_extract ('abcdefgabcdefg', '^');

-- First character      : should return 'a' twice
execute function regexp_extract ('abcdefgabcdefg', 'a');

-- Just first character : should return 'a' once
execute function regexp_extract ('abcdefgabcdefg', '^a');

-- First two characters : should return 'ab' twice
execute function regexp_extract ('abcdefgabcdefg', 'ab');

-- Just first two chars : should return 'ab' once
execute function regexp_extract ('abcdefgabcdefg', '^ab');

-- Second character     : should return 'b' twice
execute function regexp_extract ('abcdefgabcdefg', 'b');

-- Last character       : should return 'g' twice
execute function regexp_extract ('abcdefgabcdefg', 'g');

-- Just last character  : should return 'g' just once
execute function regexp_extract ('abcdefgabcdefg', 'g$');

-- End-of-line          : should return 'No rows found'
execute function regexp_extract ('abcdefgabcdefg', '$');

-- 1. result: 'abc'
execute function regexp_extract('abc','abc');

-- 2. result: no rows found
execute function regexp_extract('xbc','abc');

-- 3. result: no rows found
execute function regexp_extract('axc','abc');

-- 4. result: no rows found
execute function regexp_extract('abx','abc');

-- 5. result: 'abc'
execute function regexp_extract('xabcy','abc');

-- 6. result: 'abc'
execute function regexp_extract('ababc','abc');

-- 7. result: 'abc'
execute function regexp_extract('abc','ab*c');

-- 8. result: 'abc'
execute function regexp_extract('abc','ab*bc');

-- 9. result: 'abbc'
execute function regexp_extract('abbc','ab*bc');

-- 10. result: 'abbbbc'
execute function regexp_extract('abbbbc','ab*bc');

-- 11. result: 'abbc'
execute function regexp_extract('abbc','ab+bc');

-- 12. result: no rows found
execute function regexp_extract('abc','ab+bc');

-- 13. result: no rows found
execute function regexp_extract('abq','ab+bc');

-- 14. result: 'abbbbc'
execute function regexp_extract('abbbbc','ab+bc');

-- 15. result: 'abbc'
execute function regexp_extract('abbc','ab?bc');

-- 16. result: 'abc'
execute function regexp_extract('abc','ab?bc');

-- 17. result: no rows found
execute function regexp_extract('abbbbc','ab?bc');

-- 18. result: 'abc'
execute function regexp_extract('abc','ab?c');

-- 19. result: 'abc'
execute function regexp_extract('abc','^abc$');

-- 20. result: no rows found
execute function regexp_extract('abcc','^abc$');

-- 21. result: 'abc'
execute function regexp_extract('abcc','^abc');

-- 22. result: no rows found
execute function regexp_extract('aabc','^abc$');

-- 23. result: 'abc'
execute function regexp_extract('aabc','abc$');

-- 24. result: no rows found
--
--     Explanation: regexp_match returns 't', but the match is
--     0 length, so the IDS server determines no result.
--
execute function regexp_extract('abc','^');

-- 25. result: no rows found
--
--     Explanation: regexp_match returns 't', but the match is
--     0 length, so the IDS server determines no result.
--
execute function regexp_extract('abc','$');

-- 26. result: 'abc'
execute function regexp_extract('abc','a.c');

-- 27. result: 'axc'
execute function regexp_extract('axc','a.c');

-- 28. result: 'axyzc'
execute function regexp_extract('axyzc','a.*c');

-- 29. result: no rows found
execute function regexp_extract('axyzd','a.*c');

-- 30. result: no rows found
execute function regexp_extract('abc','a[bc]d');

-- 31. result: 'abd'
execute function regexp_extract('abd','a[bc]d');

-- 32. result: no rows found
execute function regexp_extract('abd','a[b-d]e');

-- 33. result: 'ace'
execute function regexp_extract('ace','a[b-d]e');

-- 34. result: 'ac'
execute function regexp_extract('aac','a[b-d]');

-- 35. result: 'a-'
execute function regexp_extract('a-','a[-b]');

-- 36. result: 'a-'
execute function regexp_extract('a-','a[b-]');

-- 37. result: no rows found
execute function regexp_extract('ab','[k]');

-- 38. result: 'a]'
execute function regexp_extract('a]','a]');

-- 39. result: 'a]b'
execute function regexp_extract('a]b','a[]]b');

-- 40. result: 'aed'
execute function regexp_extract('aed','a[^bc]d');

-- 41. result: no rows found
execute function regexp_extract('abd','a[^bc]d');

-- 42. result: 'adc'
execute function regexp_extract('adc','a[^-b]c');

-- 43. result: no rows found
execute function regexp_extract('a-c','a[^-b]c');

-- 44. result: no rows found
execute function regexp_extract('a]c','a[^]b]c');

-- 45. result: 'adc'
execute function regexp_extract('adc','a[^]b]c');

-- 46. result: 'ab'
execute function regexp_extract('abc','ab|cd');

-- 47. result: q1 'ab'
--                'cd'
--             q2 'ab'
execute function regexp_extract('abcd','ab|cd');
execute function regexp_extract('abcd','ab|cd',1);

-- 48. result: 'ef'
execute function regexp_extract('def','()ef');

-- 49. result: no rows found
execute function regexp_extract('b','$b');

-- 50. result: 'a(b'
execute function regexp_extract('a(b','a\(b');

-- 51. result: 'ab'
execute function regexp_extract('ab','a\(*b');

-- 52. result: 'a((b'
execute function regexp_extract('a((b','a\(*b');

-- 53. result: 'a\b'
execute function regexp_extract('a\b','a\\b');

-- 54. result: 'a'
execute function regexp_extract('abc','((a))');

-- 55. result: 'abc'
execute function regexp_extract('abc','(a)b(c)');

-- 56. result: 'abc'
execute function regexp_extract('aabbabc','a+b+c');

-- 57. result: 'ab'
execute function regexp_extract('ab','(a+|b)*');

-- 58. result: 'ab'
execute function regexp_extract('ab','(a+|b)+');

-- 59. result: q1 'a'
--                'b'
--             q2 'a'
execute function regexp_extract('ab','(a+|b)?');
execute function regexp_extract('ab','(a+|b)?', 1);

-- 60. result: 'cde'
execute function regexp_extract('cde','[^ab]*');

-- 61. result: 'abcd'
execute function regexp_extract('abcd','abcd');

-- 62. result: 'abcd'
execute function regexp_extract('abcd','a(bc)d');

-- 63. result: 'abbbcd'
execute function regexp_extract('abbbcd','([abc])*d');

-- 64. result: 'abcd'
execute function regexp_extract('abcd','([abc])*bcd');

-- 65. result: 'e'
execute function regexp_extract('e','a|b|c|d|e');

-- 66. result: 'ef'
execute function regexp_extract('ef','(a|b|c|d|e)f');

-- 67. result: 'abcdefg'
execute function regexp_extract('abcdefg','abcd*efg');

-- 68. result: q1 'ab'
--                'abbb'
--             q2 'ab'
execute function regexp_extract('xabyabbbz','ab*');
execute function regexp_extract('xabyabbbz','ab*',1);

-- 69. result: q1 'a'
--                'abbb'
--             q2 'a'
execute function regexp_extract('xayabbbz','ab*');
execute function regexp_extract('xayabbbz','ab*', 1);

-- 70. result: 'cde'
execute function regexp_extract('abcde','(ab|cd)e');

-- 71. result: 'hij'
execute function regexp_extract('hij','[abhgefdc]ij');

-- 72. result: no rows found
execute function regexp_extract('abcde','^(ab|cd)e');

-- 73. result: 'ef'
execute function regexp_extract('abcdef','(abc|)ef');

-- 74. result: 'bcd'
execute function regexp_extract('abcd','(a|b)c*d');

-- 75. result: 'abc'
execute function regexp_extract('abc','(ab|ab*)bc');

-- 76. result: 'abc'
execute function regexp_extract('abc','a([bc]*)c*');

-- 77. result: 'abcd'
execute function regexp_extract('abcd','a([bc]*)(c*d)');

-- 78. result: 'abcd'
execute function regexp_extract('abcd','a([bc]+)(c*d)');

-- 79. result: 'abcd'
execute function regexp_extract('abcd','a([bc]*)(c+d)');

-- 80. result: 'adcdcde'
execute function regexp_extract('adcdcde','a[bcd]*dcdcde');

-- 81. result: no rows found
execute function regexp_extract('adcdcde','a[bcd]+dcdcde');

-- 82. result: 'abc'
execute function regexp_extract('abc','(ab|a)b*c');

-- 83. result: 'abcd'
execute function regexp_extract('abcd','((a)(b)c)(d)');

-- 84. result: 'abc'
execute function regexp_extract('abc','[ -~]*');

-- 85. result: 'abc'
execute function regexp_extract('abc','[ -~ -~]*');

-- 86. result: 'abc'
execute function regexp_extract('abc','[ -~ -~ -~]*');

-- 87. result: 'abc'
execute function regexp_extract('abc','[ -~ -~ -~ -~]*');

-- 88. result: 'abc'
execute function regexp_extract('abc','[ -~ -~ -~ -~ -~]*');

-- 89. result: 'abc'
execute function regexp_extract('abc','[ -~ -~ -~ -~ -~ -~]*');

-- 90. result: 'abc'
execute function regexp_extract('abc','[ -~ -~ -~ -~ -~ -~ -~]*');

-- 91. result: 'alpha'
execute function regexp_extract('alpha','[a-zA-Z_][a-zA-Z0-9_]*');

-- 92. result: 'bh'
execute function regexp_extract('abh','^a(bc+|b[eh])g|.h$');

-- 93. result: 'effgz'
execute function regexp_extract('effgz','(bc+d$|ef*g.|h?i(j|k))');

-- 94. result: 'ij'
execute function regexp_extract('ij','(bc+d$|ef*g.|h?i(j|k))');

-- 95. result: no rows found
execute function regexp_extract('effg','(bc+d$|ef*g.|h?i(j|k))');

-- 96. result: no rows found
execute function regexp_extract('bcdd','(bc+d$|ef*g.|h?i(j|k))');

-- 97. result: 'effgz'
execute function regexp_extract('reffgz','(bc+d$|ef*g.|h?i(j|k))');

-- 98. result: 'a'
execute function regexp_extract('a','(((((((((a)))))))))');

-- 99. result: no rows found
execute function regexp_extract('uh-uh','multiple words of text');

-- 100. result: 'multiple words'
execute function regexp_extract('multiple words, yeah','multiple words');

-- 101. result: 'abcde'
execute function regexp_extract('abcde','(.*)c(.*)');

-- 102. result: '<a, b)'
execute function regexp_extract('(a, b)','\((.*), (.*)\)');


-- ======================= Negative tests =============================
--
-- Each of these should fail to compile the regular expression
--

EXECUTE FUNCTION regexp_extract( 'abc' , 'a[b-a]' );
EXECUTE FUNCTION regexp_extract( 'abc' , 'a[]b' );
EXECUTE FUNCTION regexp_extract( 'abc' , 'a[' );
EXECUTE FUNCTION regexp_extract( 'abc' , '()*' );
EXECUTE FUNCTION regexp_extract( 'abc' , '*a' );
EXECUTE FUNCTION regexp_extract( 'abc' , '^*' );
EXECUTE FUNCTION regexp_extract( 'abc' , '$*' );
EXECUTE FUNCTION regexp_extract( 'abc' , '(*)b' );
EXECUTE FUNCTION regexp_extract( 'abc' , 'a\' );
EXECUTE FUNCTION regexp_extract( 'abc' , 'abc)' );
EXECUTE FUNCTION regexp_extract( 'abc' , '(abc' );
EXECUTE FUNCTION regexp_extract( 'abc' , 'a**' );
EXECUTE FUNCTION regexp_extract( 'abc' , 'a*?' );
EXECUTE FUNCTION regexp_extract( 'abc' , '(a*)*' );
EXECUTE FUNCTION regexp_extract( 'abc' , '(a*)+' );
EXECUTE FUNCTION regexp_extract( 'abc' , '(a|)*' );
EXECUTE FUNCTION regexp_extract( 'abc' , '(a*|b)*' );
EXECUTE FUNCTION regexp_extract( 'abc' , '(^)*' );
EXECUTE FUNCTION regexp_extract( 'abc' , '(ab|)*' );
EXECUTE FUNCTION regexp_extract( 'abc' , ')(' );
EXECUTE FUNCTION regexp_extract( 'abc' , '((a*|b))*' );
EXECUTE FUNCTION regexp_extract( 'abc' , '((((((((((a))))))))))' );

