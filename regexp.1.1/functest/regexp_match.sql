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

-- execute procedure traceset_regexp("/tmp/re_match.trc", 30);

-- 1. result: 't'
execute function regexp_match('abc','abc');

-- 2. result: 'f'
execute function regexp_match('xbc','abc');

-- 3. result: 'f'
execute function regexp_match('axc','abc');

-- 4. result: 'f'
execute function regexp_match('abx','abc');

-- 5. result: 't'
execute function regexp_match('xabcy','abc');

-- 6. result: 't'
execute function regexp_match('ababc','abc');

-- 7. result: 't'
execute function regexp_match('abc','ab*c');

-- 8. result: 't'
execute function regexp_match('abc','ab*bc');

-- 9. result: 't'
execute function regexp_match('abbc','ab*bc');

-- 10. result: 't'
execute function regexp_match('abbbbc','ab*bc');

-- 11. result: 't'
execute function regexp_match('abbc','ab+bc');

-- 12. result: 'f'
execute function regexp_match('abc','ab+bc');

-- 13. result: 'f'
execute function regexp_match('abq','ab+bc');

-- 14. result: 't'
execute function regexp_match('abbbbc','ab+bc');

-- 15. result: 't'
execute function regexp_match('abbc','ab?bc');

-- 16. result: 't'
execute function regexp_match('abc','ab?bc');

-- 17. result: 'f'
execute function regexp_match('abbbbc','ab?bc');

-- 18. result: 't'
execute function regexp_match('abc','ab?c');

-- 19. result: 't'
execute function regexp_match('abc','^abc$');

-- 20. result: 'f'
execute function regexp_match('abcc','^abc$');

-- 21. result: 't'
execute function regexp_match('abcc','^abc');

-- 22. result: 'f'
execute function regexp_match('aabc','^abc$');

-- 23. result: 't'
execute function regexp_match('aabc','abc$');

-- 24. result: 't'
execute function regexp_match('abc','^');

-- 25. result: 't'
execute function regexp_match('abc','$');

-- 26. result: 't'
execute function regexp_match('abc','a.c');

-- 27. result: 't'
execute function regexp_match('axc','a.c');

-- 28. result: 't'
execute function regexp_match('axyzc','a.*c');

-- 29. result: 'f'
execute function regexp_match('axyzd','a.*c');

-- 30. result: 'f'
execute function regexp_match('abc','a[bc]d');

-- 31. result: 't'
execute function regexp_match('abd','a[bc]d');

-- 32. result: 'f'
execute function regexp_match('abd','a[b-d]e');

-- 33. result: 't'
execute function regexp_match('ace','a[b-d]e');

-- 34. result: 't'
execute function regexp_match('aac','a[b-d]');

-- 35. result: 't'
execute function regexp_match('a-','a[-b]');

-- 36. result: 't'
execute function regexp_match('a-','a[b-]');

-- 37. result: 'f'
execute function regexp_match('ab','[k]');

-- 38. result: 't'
execute function regexp_match('a]','a]');

-- 39. result: 't'
execute function regexp_match('a]b','a[]]b');

-- 40. result: 't'
execute function regexp_match('aed','a[^bc]d');

-- 41. result: 'f'
execute function regexp_match('abd','a[^bc]d');

-- 42. result: 't'
execute function regexp_match('adc','a[^-b]c');

-- 43. result: 'f'
execute function regexp_match('a-c','a[^-b]c');

-- 44. result: 'f'
execute function regexp_match('a]c','a[^]b]c');

-- 45. result: 't'
execute function regexp_match('adc','a[^]b]c');

-- 46. result: 't'
execute function regexp_match('abc','ab|cd');

-- 47. result: 't'
execute function regexp_match('abcd','ab|cd');

-- 48. result: 't'
execute function regexp_match('def','()ef');

-- 49. result: 'f'
execute function regexp_match('b','$b');

-- 50. result: 't'
execute function regexp_match('a(b','a\(b');

-- 51. result: 't'
execute function regexp_match('ab','a\(*b');

-- 52. result: 't'
execute function regexp_match('a((b','a\(*b');

-- 53. result: 't'
execute function regexp_match('a\b','a\\b');

-- 54. result: 't'
execute function regexp_match('abc','((a))');

-- 55. result: 't'
execute function regexp_match('abc','(a)b(c)');

-- 56. result: 't'
execute function regexp_match('aabbabc','a+b+c');

-- 57. result: 't'
execute function regexp_match('ab','(a+|b)*');

-- 58. result: 't'
execute function regexp_match('ab','(a+|b)+');

-- 59. result: 't'
execute function regexp_match('ab','(a+|b)?');

-- 60. result: 't'
execute function regexp_match('cde','[^ab]*');

-- 61. result: 't'
execute function regexp_match('abcd','abcd');

-- 62. result: 't'
execute function regexp_match('abcd','a(bc)d');

-- 63. result: 't'
execute function regexp_match('abbbcd','([abc])*d');

-- 64. result: 't'
execute function regexp_match('abcd','([abc])*bcd');

-- 65. result: 't'
execute function regexp_match('e','a|b|c|d|e');

-- 66. result: 't'
execute function regexp_match('ef','(a|b|c|d|e)f');

-- 67. result: 't'
execute function regexp_match('abcdefg','abcd*efg');

-- 68. result: 't'
execute function regexp_match('xabyabbbz','ab*');

-- 69. result: 't'
execute function regexp_match('xayabbbz','ab*');

-- 70. result: 't'
execute function regexp_match('abcde','(ab|cd)e');

-- 71. result: 't'
execute function regexp_match('hij','[abhgefdc]ij');

-- 72. result: 'f'
execute function regexp_match('abcde','^(ab|cd)e');

-- 73. result: 't'
execute function regexp_match('abcdef','(abc|)ef');

-- 74. result: 't'
execute function regexp_match('abcd','(a|b)c*d');

-- 75. result: 't'
execute function regexp_match('abc','(ab|ab*)bc');

-- 76. result: 't'
execute function regexp_match('abc','a([bc]*)c*');

-- 77. result: 't'
execute function regexp_match('abcd','a([bc]*)(c*d)');

-- 78. result: 't'
execute function regexp_match('abcd','a([bc]+)(c*d)');

-- 79. result: 't'
execute function regexp_match('abcd','a([bc]*)(c+d)');

-- 80. result: 't'
execute function regexp_match('adcdcde','a[bcd]*dcdcde');

-- 81. result: 'f'
execute function regexp_match('adcdcde','a[bcd]+dcdcde');

-- 82. result: 't'
execute function regexp_match('abc','(ab|a)b*c');

-- 83. result: 't'
execute function regexp_match('abcd','((a)(b)c)(d)');

-- 84. result: 't'
execute function regexp_match('abc','[ -~]*');

-- 85. result: 't'
execute function regexp_match('abc','[ -~ -~]*');

-- 86. result: 't'
execute function regexp_match('abc','[ -~ -~ -~]*');

-- 87. result: 't'
execute function regexp_match('abc','[ -~ -~ -~ -~]*');

-- 88. result: 't'
execute function regexp_match('abc','[ -~ -~ -~ -~ -~]*');

-- 89. result: 't'
execute function regexp_match('abc','[ -~ -~ -~ -~ -~ -~]*');

-- 90. result: 't'
execute function regexp_match('abc','[ -~ -~ -~ -~ -~ -~ -~]*');

-- 91. result: 't'
execute function regexp_match('alpha','[a-zA-Z_][a-zA-Z0-9_]*');

-- 92. result: 't'
execute function regexp_match('abh','^a(bc+|b[eh])g|.h$');

-- 93. result: 't'
execute function regexp_match('effgz','(bc+d$|ef*g.|h?i(j|k))');

-- 94. result: 't'
execute function regexp_match('ij','(bc+d$|ef*g.|h?i(j|k))');

-- 95. result: 'f'
execute function regexp_match('effg','(bc+d$|ef*g.|h?i(j|k))');

-- 96. result: 'f'
execute function regexp_match('bcdd','(bc+d$|ef*g.|h?i(j|k))');

-- 97. result: 't'
execute function regexp_match('reffgz','(bc+d$|ef*g.|h?i(j|k))');

-- 98. result: 't'
execute function regexp_match('a','(((((((((a)))))))))');

-- 99. result: 'f'
execute function regexp_match('uh-uh','multiple words of text');

-- 100. result: 't'
execute function regexp_match('multiple words, yeah','multiple words');

-- 101. result: 't'
execute function regexp_match('abcde','(.*)c(.*)');

-- 102. result: 't'
execute function regexp_match('(a, b)','\((.*), (.*)\)');


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

