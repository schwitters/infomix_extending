-----------------------------------------------------------------
-- NAME
--     regexp_match.sql
--
-- DESCRIPTION
--     Functional tests for regexp_match() UDR.
--
--     Generated from the U of Toronto regexp tests like this:
--
--     + removed lines with a blank field from the 'tests' file and with 'c'
--
--     + ran this awk command again the 'tests' files:
--
--       BEGIN {FS="\t"; total=1}
--        {print "-- " total++ ". result: '" $3 \
--         "'\nexecute function regexp_match('" $2 "','" $1 "');\n"}
--
--     + Hand mangled the script further. Change toronto 'n' to 'f'
--       and 'y' to 't', to match SQL output. 
--
--     + negative tests are at the bottom of this file.
-----------------------------------------------------------------

-- execute procedure traceset_mrLvarchar("/tmp/re_match.trc", 30);

--
-- Light testing that includes big values 
--

-- should return 1-5 and 9-10
select id from test_mr where regexp_match(mrcol, '^');
select id from test_mr where regexp_match(mrcol, '$');

-- should return both id 9 & 10
select id from test_mr where regexp_match(mrcol, '^A fable');

-- should return id 9
select id from test_mr where regexp_match(mrcol, ' pig ');
select id from test_mr where regexp_match(mrcol, 'are\."$');

-- should return id 10
select id from test_mr where regexp_match(mrcol, ' [aA][sS][sS] ');
select id from test_mr where regexp_match(mrcol, 'bargain\.$');

-- <<<<<<<<<<<<<< Toronto regexp tests >>>>>>>>>>>>>>>>>>>>>
-- The first argument is explicitly cast to idn_mrLvarchar 
-- in case the regexp blade is also registered in the same database.

-- 1. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'abc');

-- 2. result: 'f'
execute function regexp_match('xbc'::idn_mrLvarchar,'abc');

-- 3. result: 'f'
execute function regexp_match('axc'::idn_mrLvarchar,'abc');

-- 4. result: 'f'
execute function regexp_match('abx'::idn_mrLvarchar,'abc');

-- 5. result: 't'
execute function regexp_match('xabcy'::idn_mrLvarchar,'abc');

-- 6. result: 't'
execute function regexp_match('ababc'::idn_mrLvarchar,'abc');

-- 7. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'ab*c');

-- 8. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'ab*bc');

-- 9. result: 't'
execute function regexp_match('abbc'::idn_mrLvarchar,'ab*bc');

-- 10. result: 't'
execute function regexp_match('abbbbc'::idn_mrLvarchar,'ab*bc');

-- 11. result: 't'
execute function regexp_match('abbc'::idn_mrLvarchar,'ab+bc');

-- 12. result: 'f'
execute function regexp_match('abc'::idn_mrLvarchar,'ab+bc');

-- 13. result: 'f'
execute function regexp_match('abq'::idn_mrLvarchar,'ab+bc');

-- 14. result: 't'
execute function regexp_match('abbbbc'::idn_mrLvarchar,'ab+bc');

-- 15. result: 't'
execute function regexp_match('abbc'::idn_mrLvarchar,'ab?bc');

-- 16. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'ab?bc');

-- 17. result: 'f'
execute function regexp_match('abbbbc'::idn_mrLvarchar,'ab?bc');

-- 18. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'ab?c');

-- 19. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'^abc$');

-- 20. result: 'f'
execute function regexp_match('abcc'::idn_mrLvarchar,'^abc$');

-- 21. result: 't'
execute function regexp_match('abcc'::idn_mrLvarchar,'^abc');

-- 22. result: 'f'
execute function regexp_match('aabc'::idn_mrLvarchar,'^abc$');

-- 23. result: 't'
execute function regexp_match('aabc'::idn_mrLvarchar,'abc$');

-- 24. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'^');

-- 25. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'$');

-- 26. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'a.c');

-- 27. result: 't'
execute function regexp_match('axc'::idn_mrLvarchar,'a.c');

-- 28. result: 't'
execute function regexp_match('axyzc'::idn_mrLvarchar,'a.*c');

-- 29. result: 'f'
execute function regexp_match('axyzd'::idn_mrLvarchar,'a.*c');

-- 30. result: 'f'
execute function regexp_match('abc'::idn_mrLvarchar,'a[bc]d');

-- 31. result: 't'
execute function regexp_match('abd'::idn_mrLvarchar,'a[bc]d');

-- 32. result: 'f'
execute function regexp_match('abd'::idn_mrLvarchar,'a[b-d]e');

-- 33. result: 't'
execute function regexp_match('ace'::idn_mrLvarchar,'a[b-d]e');

-- 34. result: 't'
execute function regexp_match('aac'::idn_mrLvarchar,'a[b-d]');

-- 35. result: 't'
execute function regexp_match('a-'::idn_mrLvarchar,'a[-b]');

-- 36. result: 't'
execute function regexp_match('a-'::idn_mrLvarchar,'a[b-]');

-- 37. result: 'f'
execute function regexp_match('ab'::idn_mrLvarchar,'[k]');

-- 38. result: 't'
execute function regexp_match('a]'::idn_mrLvarchar,'a]');

-- 39. result: 't'
execute function regexp_match('a]b'::idn_mrLvarchar,'a[]]b');

-- 40. result: 't'
execute function regexp_match('aed'::idn_mrLvarchar,'a[^bc]d');

-- 41. result: 'f'
execute function regexp_match('abd'::idn_mrLvarchar,'a[^bc]d');

-- 42. result: 't'
execute function regexp_match('adc'::idn_mrLvarchar,'a[^-b]c');

-- 43. result: 'f'
execute function regexp_match('a-c'::idn_mrLvarchar,'a[^-b]c');

-- 44. result: 'f'
execute function regexp_match('a]c'::idn_mrLvarchar,'a[^]b]c');

-- 45. result: 't'
execute function regexp_match('adc'::idn_mrLvarchar,'a[^]b]c');

-- 46. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'ab|cd');

-- 47. result: 't'
execute function regexp_match('abcd'::idn_mrLvarchar,'ab|cd');

-- 48. result: 't'
execute function regexp_match('def'::idn_mrLvarchar,'()ef');

-- 49. result: 'f'
execute function regexp_match('b'::idn_mrLvarchar,'$b');

-- 50. result: 't'
execute function regexp_match('a(b'::idn_mrLvarchar,'a\(b');

-- 51. result: 't'
execute function regexp_match('ab'::idn_mrLvarchar,'a\(*b');

-- 52. result: 't'
execute function regexp_match('a((b'::idn_mrLvarchar,'a\(*b');

-- 53. result: 't'
execute function regexp_match('a\b'::idn_mrLvarchar,'a\\b');

-- 54. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'((a))');

-- 55. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'(a)b(c)');

-- 56. result: 't'
execute function regexp_match('aabbabc'::idn_mrLvarchar,'a+b+c');

-- 57. result: 't'
execute function regexp_match('ab'::idn_mrLvarchar,'(a+|b)*');

-- 58. result: 't'
execute function regexp_match('ab'::idn_mrLvarchar,'(a+|b)+');

-- 59. result: 't'
execute function regexp_match('ab'::idn_mrLvarchar,'(a+|b)?');

-- 60. result: 't'
execute function regexp_match('cde'::idn_mrLvarchar,'[^ab]*');

-- 61. result: 't'
execute function regexp_match('abcd'::idn_mrLvarchar,'abcd');

-- 62. result: 't'
execute function regexp_match('abcd'::idn_mrLvarchar,'a(bc)d');

-- 63. result: 't'
execute function regexp_match('abbbcd'::idn_mrLvarchar,'([abc])*d');

-- 64. result: 't'
execute function regexp_match('abcd'::idn_mrLvarchar,'([abc])*bcd');

-- 65. result: 't'
execute function regexp_match('e'::idn_mrLvarchar,'a|b|c|d|e');

-- 66. result: 't'
execute function regexp_match('ef'::idn_mrLvarchar,'(a|b|c|d|e)f');

-- 67. result: 't'
execute function regexp_match('abcdefg'::idn_mrLvarchar,'abcd*efg');

-- 68. result: 't'
execute function regexp_match('xabyabbbz'::idn_mrLvarchar,'ab*');

-- 69. result: 't'
execute function regexp_match('xayabbbz'::idn_mrLvarchar,'ab*');

-- 70. result: 't'
execute function regexp_match('abcde'::idn_mrLvarchar,'(ab|cd)e');

-- 71. result: 't'
execute function regexp_match('hij'::idn_mrLvarchar,'[abhgefdc]ij');

-- 72. result: 'f'
execute function regexp_match('abcde'::idn_mrLvarchar,'^(ab|cd)e');

-- 73. result: 't'
execute function regexp_match('abcdef'::idn_mrLvarchar,'(abc|)ef');

-- 74. result: 't'
execute function regexp_match('abcd'::idn_mrLvarchar,'(a|b)c*d');

-- 75. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'(ab|ab*)bc');

-- 76. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'a([bc]*)c*');

-- 77. result: 't'
execute function regexp_match('abcd'::idn_mrLvarchar,'a([bc]*)(c*d)');

-- 78. result: 't'
execute function regexp_match('abcd'::idn_mrLvarchar,'a([bc]+)(c*d)');

-- 79. result: 't'
execute function regexp_match('abcd'::idn_mrLvarchar,'a([bc]*)(c+d)');

-- 80. result: 't'
execute function regexp_match('adcdcde'::idn_mrLvarchar,'a[bcd]*dcdcde');

-- 81. result: 'f'
execute function regexp_match('adcdcde'::idn_mrLvarchar,'a[bcd]+dcdcde');

-- 82. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'(ab|a)b*c');

-- 83. result: 't'
execute function regexp_match('abcd'::idn_mrLvarchar,'((a)(b)c)(d)');

-- 84. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'[ -~]*');

-- 85. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'[ -~ -~]*');

-- 86. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'[ -~ -~ -~]*');

-- 87. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~]*');

-- 88. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~ -~]*');

-- 89. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~ -~ -~]*');

-- 90. result: 't'
execute function regexp_match('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~ -~ -~ -~]*');

-- 91. result: 't'
execute function regexp_match('alpha'::idn_mrLvarchar,'[a-zA-Z_][a-zA-Z0-9_]*');

-- 92. result: 't'
execute function regexp_match('abh'::idn_mrLvarchar,'^a(bc+|b[eh])g|.h$');

-- 93. result: 't'
execute function regexp_match('effgz'::idn_mrLvarchar,'(bc+d$|ef*g.|h?i(j|k))');

-- 94. result: 't'
execute function regexp_match('ij'::idn_mrLvarchar,'(bc+d$|ef*g.|h?i(j|k))');

-- 95. result: 'f'
execute function regexp_match('effg'::idn_mrLvarchar,'(bc+d$|ef*g.|h?i(j|k))');

-- 96. result: 'f'
execute function regexp_match('bcdd'::idn_mrLvarchar,'(bc+d$|ef*g.|h?i(j|k))');

-- 97. result: 't'
execute function regexp_match('reffgz'::idn_mrLvarchar,'(bc+d$|ef*g.|h?i(j|k))');

-- 98. result: 't'
execute function regexp_match('a'::idn_mrLvarchar,'(((((((((a)))))))))');

-- 99. result: 'f'
execute function regexp_match('uh-uh'::idn_mrLvarchar,'multiple words of text');

-- 100. result: 't'
execute function regexp_match('multiple words, yeah'::idn_mrLvarchar,'multiple words');

-- 101. result: 't'
execute function regexp_match('abcde'::idn_mrLvarchar,'(.*)c(.*)');

-- 102. result: 't'
execute function regexp_match('(a, b)'::idn_mrLvarchar,'\((.*), (.*)\)');


-- ======================= Negative tests =============================
--
-- Each of these should fail to compile the regular expression
--

EXECUTE FUNCTION regexp_match( 'abc' , 'a[b-a]' );
EXECUTE FUNCTION regexp_match( 'abc' , 'a[]b' );
EXECUTE FUNCTION regexp_match( 'abc' , 'a[' );
EXECUTE FUNCTION regexp_match( 'abc' , '()*' );
EXECUTE FUNCTION regexp_match( 'abc' , '*a' );
EXECUTE FUNCTION regexp_match( 'abc' , '^*' );
EXECUTE FUNCTION regexp_match( 'abc' , '$*' );
EXECUTE FUNCTION regexp_match( 'abc' , '(*)b' );
EXECUTE FUNCTION regexp_match( 'abc' , 'a\' );
EXECUTE FUNCTION regexp_match( 'abc' , 'abc)' );
EXECUTE FUNCTION regexp_match( 'abc' , '(abc' );
EXECUTE FUNCTION regexp_match( 'abc' , 'a**' );
EXECUTE FUNCTION regexp_match( 'abc' , 'a*?' );
EXECUTE FUNCTION regexp_match( 'abc' , '(a*)*' );
EXECUTE FUNCTION regexp_match( 'abc' , '(a*)+' );
EXECUTE FUNCTION regexp_match( 'abc' , '(a|)*' );
EXECUTE FUNCTION regexp_match( 'abc' , '(a*|b)*' );
EXECUTE FUNCTION regexp_match( 'abc' , '(^)*' );
EXECUTE FUNCTION regexp_match( 'abc' , '(ab|)*' );
EXECUTE FUNCTION regexp_match( 'abc' , ')(' );
EXECUTE FUNCTION regexp_match( 'abc' , '((a*|b))*' );
EXECUTE FUNCTION regexp_match( 'abc' , '((((((((((a))))))))))' );

