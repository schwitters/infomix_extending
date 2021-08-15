-----------------------------------------------------------------
-- NAME
--     regexp_replace.sql
--
-- DESCRIPTION
--     Functional tests for regexp_replace() UDR.
--
--     Generated from the U of Toronto regexp tests like this:
--
--     + removed lines with a blank field from the 'tests' file
--     + ran this awk command again the 'tests' files:
--
--       BEGIN {FS="\t"; total=1}
--       {print "-- " total++ ". result: '" $5 \
--         "'\nexecute function regexp_replace('" $2 "','" $1 "','" $4 "');\n"}
--
--     + Hand mangled the script further to produce more meaningful
--       tests for SQL. In general, any regular expression that was
--       solely '&' became '.&.' to make the replacement in the
--       original string more visible.
--
--     + Toronto substitution output is just the sub string. This
--       output was modified to reflect the replace of the entire
--       string.
-----------------------------------------------------------------

-- execute procedure traceset_regexp("/tmp/re_replace.trc", 30);

--
-- 0. Boundary checks
--

-- Begin-line           : should return '..abcdefgabcdefg'
execute function regexp_replace ('abcdefgabcdefg', '^', '.&.');

-- First character      : should return '.a.bcdefg.a.bcdefg'
execute function regexp_replace ('abcdefgabcdefg', 'a', '.&.');

-- Just first character : should return '.a.bcdefgabcdefg'
execute function regexp_replace ('abcdefgabcdefg', '^a', '.&.');

-- First two characters : should return '.ab.cdefg.ab.cdefg'
execute function regexp_replace ('abcdefgabcdefg', 'ab', '.&.');

-- Just first two chars : should return '.ab.cdefgabcdefg'
execute function regexp_replace ('abcdefgabcdefg', '^ab', '.&.');

-- Second character     : should return 'a.b.cdefga.b.cdefg'
execute function regexp_replace ('abcdefgabcdefg', 'b', '.&.');

-- Last character       : should return 'abcdef.g.abcdef.g.'
execute function regexp_replace ('abcdefgabcdefg', 'g', '.&.');

-- Just last character  : should return 'abcdefgabcdef.g.'
execute function regexp_replace ('abcdefgabcdefg', 'g$', '.&.');

-- End-of-line          : should return 'abcdefgabcdefg..'
execute function regexp_replace ('abcdefgabcdefg', '$', '.&.');

-- 1. result: '.abc.'
execute function regexp_replace('abc','abc','.&.');

-- 2. result: 'xbc'
execute function regexp_replace('xbc','abc','-');

-- 3. result: 'axc'
execute function regexp_replace('axc','abc','-');

-- 4. result: 'abx'
execute function regexp_replace('abx','abc','-');

-- 5. result: 'x.abc.y'
execute function regexp_replace('xabcy','abc','.&.');

-- 6. result: 'ab.abc.'
execute function regexp_replace('ababc','abc','.&.');

-- 7. result: '.abc.'
execute function regexp_replace('abc','ab*c','.&.');

-- 8. result: '.abc.'
execute function regexp_replace('abc','ab*bc','.&.');

-- 9. result: '.abbc.'
execute function regexp_replace('abbc','ab*bc','.&.');

-- 10. result: '.abbbbc.'
execute function regexp_replace('abbbbc','ab*bc','.&.');

-- 11. result: '.abbc.'
execute function regexp_replace('abbc','ab+bc','.&.');

-- 12. result: 'abc'
execute function regexp_replace('abc','ab+bc','-');

-- 13. result: 'abq'
execute function regexp_replace('abq','ab+bc','-');

-- 14. result: '.abbbbc.'
execute function regexp_replace('abbbbc','ab+bc','.&.');

-- 15. result: '.abbc.'
execute function regexp_replace('abbc','ab?bc','.&.');

-- 16. result: '.abc.'
execute function regexp_replace('abc','ab?bc','.&.');

-- 17. result: 'abbbbc'
execute function regexp_replace('abbbbc','ab?bc','-');

-- 18. result: '.abc.'
execute function regexp_replace('abc','ab?c','.&.');

-- 19. result: '.abc.'
execute function regexp_replace('abc','^abc$','.&.');

-- 20. result: 'abcc'
execute function regexp_replace('abcc','^abc$','-');

-- 21. result: '.abc.c.'
execute function regexp_replace('abcc','^abc','.&.');

-- 22. result: 'aabc'
execute function regexp_replace('aabc','^abc$','-');

-- 23. result: 'a.abc.'
execute function regexp_replace('aabc','abc$','.&.');

-- 24. result: '..abc'
execute function regexp_replace('abc','^','.&.');

-- 25. result: 'abc..'
execute function regexp_replace('abc','$','.&.');

-- 26. result: '.abc.'
execute function regexp_replace('abc','a.c','.&.');

-- 27. result: '.axc.'
execute function regexp_replace('axc','a.c','.&.');

-- 28. result: '.axyzc.'
execute function regexp_replace('axyzc','a.*c','.&.');

-- 29. result: 'axyzd'
execute function regexp_replace('axyzd','a.*c','-');

-- 30. result: 'abc'
execute function regexp_replace('abc','a[bc]d','-');

-- 31. result: '.abd.'
execute function regexp_replace('abd','a[bc]d','.&.');

-- 32. result: 'abd'
execute function regexp_replace('abd','a[b-d]e','-');

-- 33. result: '.ace.'
execute function regexp_replace('ace','a[b-d]e','.&.');

-- 34. result: 'a.ac.'
execute function regexp_replace('aac','a[b-d]','.&.');

-- 35. result: '.a-.'
execute function regexp_replace('a-','a[-b]','.&.');

-- 36. result: '.a-.'
execute function regexp_replace('a-','a[b-]','.&.');

-- 37. result: 'ab'
execute function regexp_replace('ab','[k]','-');

-- 38. result: '.a].'
execute function regexp_replace('a]','a]','.&.');

-- 39. result: '.a]b.'
execute function regexp_replace('a]b','a[]]b','.&.');

-- 40. result: 'aed'
execute function regexp_replace('aed','a[^bc]d','.&.');

-- 41. result: 'abd'
execute function regexp_replace('abd','a[^bc]d','-');

-- 42. result: '.adc.'
execute function regexp_replace('adc','a[^-b]c','.&.');

-- 43. result: 'a-c'
execute function regexp_replace('a-c','a[^-b]c','-');

-- 44. result: 'a]c'
execute function regexp_replace('a]c','a[^]b]c','-');

-- 45. result: '.adc.'
execute function regexp_replace('adc','a[^]b]c','.&.');

-- 46. result: '.ab.c'
execute function regexp_replace('abc','ab|cd','.&.');

-- 47. result: q1 '.ab..cd.'  
--             q2 '.ab.cd'
execute function regexp_replace('abcd','ab|cd','.&.');
execute function regexp_replace('abcd','ab|cd','.&.', 1);

-- 48. result: 'd.ef-.'
execute function regexp_replace('def','()ef','.&-\1.');

-- 49. result: 'b'
execute function regexp_replace('b','$b','-');

-- 50. result: '.a(b-.'
execute function regexp_replace('a(b','a\(b','.&-\1.');

-- 51. result: '.ab.'
execute function regexp_replace('ab','a\(*b','.&.');

-- 52. result: '.a((b.'
execute function regexp_replace('a((b','a\(*b','.&.');

-- 53. result: '.a\b.'
execute function regexp_replace('a\b','a\\b','.&.');

-- 54. result: '.a-a-a.bc'
execute function regexp_replace('abc','((a))','.&-\1-\2.');

-- 55. result: '.abc-a-c.'
execute function regexp_replace('abc','(a)b(c)','.&-\1-\2.');

-- 56. result: 'aabb.abc.'
execute function regexp_replace('aabbabc','a+b+c','.&.');

-- 57. result: q1 '.ab-b..-.'
--             q2 '.ab-b.'
execute function regexp_replace('ab','(a+|b)*','.&-\1.');
execute function regexp_replace('ab','(a+|b)*','.&-\1.', 1);

-- 58. result: '.ab-b.'
execute function regexp_replace('ab','(a+|b)+','.&-\1.');

-- 59. result: q1 '.a-a..b-b..-.'
--             q2 '.a-a.b'
execute function regexp_replace('ab','(a+|b)?','.&-\1.');
execute function regexp_replace('ab','(a+|b)?','.&-\1.', 1);

-- 60. result: q1 '.cde...'
--             q2 '.cde.'
execute function regexp_replace('cde','[^ab]*','.&.');
execute function regexp_replace('cde','[^ab]*','.&.', 1);

-- 61. result: 'abcd-&-\abcd'
execute function regexp_replace('abcd','abcd','&-\&-\\&');

-- 62. result: 'bc-\1-\bc'
execute function regexp_replace('abcd','a(bc)d','\1-\\1-\\\1');

-- 63. result: 'abbbcd-c'
execute function regexp_replace('abbbcd','([abc])*d','&-\1');

-- 64. result: 'abcd-a'
execute function regexp_replace('abcd','([abc])*bcd','&-\1');

-- 65. result: 'e'
execute function regexp_replace('e','a|b|c|d|e','&');

-- 66. result: 'ef-e'
execute function regexp_replace('ef','(a|b|c|d|e)f','&-\1');

-- 67. result: 'abcdefg'
execute function regexp_replace('abcdefg','abcd*efg','&');

-- 68. result: q1 'x.ab.y.abbb.z'
--             q2 'x.ab.yabbbz'
execute function regexp_replace('xabyabbbz','ab*','.&.');
execute function regexp_replace('xabyabbbz','ab*','.&.', 1);

-- 69. result: q1 'x.a.y.abbb.z'
--             q2 'x.a.yabbbz'
execute function regexp_replace('xayabbbz','ab*','.&.');
execute function regexp_replace('xayabbbz','ab*','.&.', 1);

-- 70. result: 'ab.cde-cd.'
execute function regexp_replace('abcde','(ab|cd)e','.&-\1.');

-- 71. result: '.hij.'
execute function regexp_replace('hij','[abhgefdc]ij','.&.');

-- 72. jta suspects the Toronto result for this one test might be incorrect.
--     The 'tests' file indicates that the substring replace should
--     be 'xy'; however, it also lists that the match should return
--     'n'. regexp_match() agrees with the 'n' (returns 'false').
--     In that case, the replace and substitute columns should be -.
--
--     result: 'abcde'
execute function regexp_match('abcde','^(ab|cd)e');

execute function regexp_replace('abcde','^(ab|cd)e','x\1y');

-- 73. result: 'abcd.ef-.'
execute function regexp_replace('abcdef','(abc|)ef','.&-\1.');

-- 74. result: 'a.bcd-b.'
execute function regexp_replace('abcd','(a|b)c*d','.&-\1.');

-- 75. result: '.abc-a.'
execute function regexp_replace('abc','(ab|ab*)bc','.&-\1.');

-- 76. result: '.abc-bc.'
execute function regexp_replace('abc','a([bc]*)c*','.&-\1.');

-- 77. result: '.abcd-bc-d.'
execute function regexp_replace('abcd','a([bc]*)(c*d)','.&-\1-\2.');

-- 78. result: '.abcd-bc-d.'
execute function regexp_replace('abcd','a([bc]+)(c*d)','.&-\1-\2.');

-- 79. result: '.abcd-b-cd.'
execute function regexp_replace('abcd','a([bc]*)(c+d)','.&-\1-\2.');

-- 80. result: '.adcdcde.'
execute function regexp_replace('adcdcde','a[bcd]*dcdcde','.&.');

-- 81. result: 'adcdcde'
execute function regexp_replace('adcdcde','a[bcd]+dcdcde','-');

-- 82. result: '.abc-ab.'
execute function regexp_replace('abc','(ab|a)b*c','.&-\1.');

-- 83. result: 'abc-a-b-d'
execute function regexp_replace('abcd','((a)(b)c)(d)','.\1-\2-\3-\4.');

-- 84. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc','[ -~]*','.&.');
execute function regexp_replace('abc','[ -~]*','.&.', 1);

-- 85. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc','[ -~ -~]*','.&.');
execute function regexp_replace('abc','[ -~ -~]*','.&.', 1);

-- 86. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc','[ -~ -~ -~]*','.&.');
execute function regexp_replace('abc','[ -~ -~ -~]*','.&.', 1);

-- 87. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc','[ -~ -~ -~ -~]*','.&.');
execute function regexp_replace('abc','[ -~ -~ -~ -~]*','.&.', 1);

-- 88. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc','[ -~ -~ -~ -~ -~]*','.&.');
execute function regexp_replace('abc','[ -~ -~ -~ -~ -~]*','.&.',1);

-- 89. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc','[ -~ -~ -~ -~ -~ -~]*','.&.');
execute function regexp_replace('abc','[ -~ -~ -~ -~ -~ -~]*','.&.',1);

-- 90. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc','[ -~ -~ -~ -~ -~ -~ -~]*','.&.');
execute function regexp_replace('abc','[ -~ -~ -~ -~ -~ -~ -~]*','.&.',1);

-- 91. result: '.alpha.'
execute function regexp_replace('alpha','[a-zA-Z_][a-zA-Z0-9_]*','.&.');

-- 92. result: 'a.bh-.'
execute function regexp_replace('abh','^a(bc+|b[eh])g|.h$','.&-\1.');

-- 93. result: '.effgz-effgz-.'
execute function regexp_replace('effgz','(bc+d$|ef*g.|h?i(j|k))','.&-\1-\2.');

-- 94. result: '.ij-ij-j.'
execute function regexp_replace('ij','(bc+d$|ef*g.|h?i(j|k))','.&-\1-\2.');

-- 95. result: 'effg'
execute function regexp_replace('effg','(bc+d$|ef*g.|h?i(j|k))','-');

-- 96. result: 'bcdd'
execute function regexp_replace('bcdd','(bc+d$|ef*g.|h?i(j|k))','-');

-- 97. result: 'r.effgz-effgz-.'
execute function regexp_replace('reffgz','(bc+d$|ef*g.|h?i(j|k))','.&-\1-\2.');

-- 98. result: '.a.'
execute function regexp_replace('a','(((((((((a)))))))))','.&.');

-- 99. result: 'uh-uh'
execute function regexp_replace('uh-uh','multiple words of text','-');

-- 100. result: '.multiple words., yeah'
execute function regexp_replace('multiple words, yeah','multiple words','.&.');

-- 101. result: '.abcde-ab-de.'
execute function regexp_replace('abcde','(.*)c(.*)','.&-\1-\2.');

-- 102. result: '(b, a)'
execute function regexp_replace('(a, b)','\((.*), (.*)\)','(\2, \1)');

