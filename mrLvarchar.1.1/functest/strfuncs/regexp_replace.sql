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

-- execute procedure traceset_mrLvarchar("/tmp/re_replace.trc", 30);
--
-- Light testing of a large value.
--
select regexp_replace(mrcol, ' [aA][sS][sS]([ :,.])', ' donkey\1')
from   test_mr
where  regexp_match(mrcol, ' [aA][sS][sS][ :,.]');

-- The first argument is explicitly cast to idn_mrLvarchar 
-- in case the regexp blade is also registered in the same database.

--
-- 0. Boundary checks
--

-- Begin-line           : should return '..abcdefgabcdefg'
execute function regexp_replace ('abcdefgabcdefg'::idn_mrLvarchar, '^', '.&.');

-- First character      : should return '.a.bcdefg.a.bcdefg'
execute function regexp_replace ('abcdefgabcdefg'::idn_mrLvarchar, 'a', '.&.');

-- Just first character : should return '.a.bcdefgabcdefg'
execute function regexp_replace ('abcdefgabcdefg'::idn_mrLvarchar, '^a', '.&.');

-- First two characters : should return '.ab.cdefg.ab.cdefg'
execute function regexp_replace ('abcdefgabcdefg'::idn_mrLvarchar, 'ab', '.&.');

-- Just first two chars : should return '.ab.cdefgabcdefg'
execute function regexp_replace ('abcdefgabcdefg'::idn_mrLvarchar, '^ab', '.&.');

-- Second character     : should return 'a.b.cdefga.b.cdefg'
execute function regexp_replace ('abcdefgabcdefg'::idn_mrLvarchar, 'b', '.&.');

-- Last character       : should return 'abcdef.g.abcdef.g.'
execute function regexp_replace ('abcdefgabcdefg'::idn_mrLvarchar, 'g', '.&.');

-- Just last character  : should return 'abcdefgabcdef.g.'
execute function regexp_replace ('abcdefgabcdefg'::idn_mrLvarchar, 'g$', '.&.');

-- End-of-line          : should return 'abcdefgabcdefg..'
execute function regexp_replace ('abcdefgabcdefg'::idn_mrLvarchar, '$', '.&.');

-- <<<<<<<<<<<<<< Toronto regexp tests >>>>>>>>>>>>>>>>>>>>>
-- 1. result: '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'abc','.&.');

-- 2. result: 'xbc'
execute function regexp_replace('xbc'::idn_mrLvarchar,'abc','-');

-- 3. result: 'axc'
execute function regexp_replace('axc'::idn_mrLvarchar,'abc','-');

-- 4. result: 'abx'
execute function regexp_replace('abx'::idn_mrLvarchar,'abc','-');

-- 5. result: 'x.abc.y'
execute function regexp_replace('xabcy'::idn_mrLvarchar,'abc','.&.');

-- 6. result: 'ab.abc.'
execute function regexp_replace('ababc'::idn_mrLvarchar,'abc','.&.');

-- 7. result: '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'ab*c','.&.');

-- 8. result: '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'ab*bc','.&.');

-- 9. result: '.abbc.'
execute function regexp_replace('abbc'::idn_mrLvarchar,'ab*bc','.&.');

-- 10. result: '.abbbbc.'
execute function regexp_replace('abbbbc'::idn_mrLvarchar,'ab*bc','.&.');

-- 11. result: '.abbc.'
execute function regexp_replace('abbc'::idn_mrLvarchar,'ab+bc','.&.');

-- 12. result: 'abc'
execute function regexp_replace('abc'::idn_mrLvarchar,'ab+bc','-');

-- 13. result: 'abq'
execute function regexp_replace('abq'::idn_mrLvarchar,'ab+bc','-');

-- 14. result: '.abbbbc.'
execute function regexp_replace('abbbbc'::idn_mrLvarchar,'ab+bc','.&.');

-- 15. result: '.abbc.'
execute function regexp_replace('abbc'::idn_mrLvarchar,'ab?bc','.&.');

-- 16. result: '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'ab?bc','.&.');

-- 17. result: 'abbbbc'
execute function regexp_replace('abbbbc'::idn_mrLvarchar,'ab?bc','-');

-- 18. result: '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'ab?c','.&.');

-- 19. result: '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'^abc$','.&.');

-- 20. result: 'abcc'
execute function regexp_replace('abcc'::idn_mrLvarchar,'^abc$','-');

-- 21. result: '.abc.c.'
execute function regexp_replace('abcc'::idn_mrLvarchar,'^abc','.&.');

-- 22. result: 'aabc'
execute function regexp_replace('aabc'::idn_mrLvarchar,'^abc$','-');

-- 23. result: 'a.abc.'
execute function regexp_replace('aabc'::idn_mrLvarchar,'abc$','.&.');

-- 24. result: '..abc'
execute function regexp_replace('abc'::idn_mrLvarchar,'^','.&.');

-- 25. result: 'abc..'
execute function regexp_replace('abc'::idn_mrLvarchar,'$','.&.');

-- 26. result: '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'a.c','.&.');

-- 27. result: '.axc.'
execute function regexp_replace('axc'::idn_mrLvarchar,'a.c','.&.');

-- 28. result: '.axyzc.'
execute function regexp_replace('axyzc'::idn_mrLvarchar,'a.*c','.&.');

-- 29. result: 'axyzd'
execute function regexp_replace('axyzd'::idn_mrLvarchar,'a.*c','-');

-- 30. result: 'abc'
execute function regexp_replace('abc'::idn_mrLvarchar,'a[bc]d','-');

-- 31. result: '.abd.'
execute function regexp_replace('abd'::idn_mrLvarchar,'a[bc]d','.&.');

-- 32. result: 'abd'
execute function regexp_replace('abd'::idn_mrLvarchar,'a[b-d]e','-');

-- 33. result: '.ace.'
execute function regexp_replace('ace'::idn_mrLvarchar,'a[b-d]e','.&.');

-- 34. result: 'a.ac.'
execute function regexp_replace('aac'::idn_mrLvarchar,'a[b-d]','.&.');

-- 35. result: '.a-.'
execute function regexp_replace('a-'::idn_mrLvarchar,'a[-b]','.&.');

-- 36. result: '.a-.'
execute function regexp_replace('a-'::idn_mrLvarchar,'a[b-]','.&.');

-- 37. result: 'ab'
execute function regexp_replace('ab'::idn_mrLvarchar,'[k]','-');

-- 38. result: '.a].'
execute function regexp_replace('a]'::idn_mrLvarchar,'a]','.&.');

-- 39. result: '.a]b.'
execute function regexp_replace('a]b'::idn_mrLvarchar,'a[]]b','.&.');

-- 40. result: 'aed'
execute function regexp_replace('aed'::idn_mrLvarchar,'a[^bc]d','.&.');

-- 41. result: 'abd'
execute function regexp_replace('abd'::idn_mrLvarchar,'a[^bc]d','-');

-- 42. result: '.adc.'
execute function regexp_replace('adc'::idn_mrLvarchar,'a[^-b]c','.&.');

-- 43. result: 'a-c'
execute function regexp_replace('a-c'::idn_mrLvarchar,'a[^-b]c','-');

-- 44. result: 'a]c'
execute function regexp_replace('a]c'::idn_mrLvarchar,'a[^]b]c','-');

-- 45. result: '.adc.'
execute function regexp_replace('adc'::idn_mrLvarchar,'a[^]b]c','.&.');

-- 46. result: '.ab.c'
execute function regexp_replace('abc'::idn_mrLvarchar,'ab|cd','.&.');

-- 47. result: q1 '.ab..cd.'  
--             q2 '.ab.cd'
execute function regexp_replace('abcd'::idn_mrLvarchar,'ab|cd','.&.');
execute function regexp_replace('abcd'::idn_mrLvarchar,'ab|cd','.&.', 1);

-- 48. result: 'd.ef-.'
execute function regexp_replace('def'::idn_mrLvarchar,'()ef','.&-\1.');

-- 49. result: 'b'
execute function regexp_replace('b'::idn_mrLvarchar,'$b','-');

-- 50. result: '.a(b-.'
execute function regexp_replace('a(b'::idn_mrLvarchar,'a\(b','.&-\1.');

-- 51. result: '.ab.'
execute function regexp_replace('ab'::idn_mrLvarchar,'a\(*b','.&.');

-- 52. result: '.a((b.'
execute function regexp_replace('a((b'::idn_mrLvarchar,'a\(*b','.&.');

-- 53. result: '.a\b.'
execute function regexp_replace('a\b'::idn_mrLvarchar,'a\\b','.&.');

-- 54. result: '.a-a-a.bc'
execute function regexp_replace('abc'::idn_mrLvarchar,'((a))','.&-\1-\2.');

-- 55. result: '.abc-a-c.'
execute function regexp_replace('abc'::idn_mrLvarchar,'(a)b(c)','.&-\1-\2.');

-- 56. result: 'aabb.abc.'
execute function regexp_replace('aabbabc'::idn_mrLvarchar,'a+b+c','.&.');

-- 57. result: q1 '.ab-b..-.'
--             q2 '.ab-b.'
execute function regexp_replace('ab'::idn_mrLvarchar,'(a+|b)*','.&-\1.');
execute function regexp_replace('ab'::idn_mrLvarchar,'(a+|b)*','.&-\1.', 1);

-- 58. result: '.ab-b.'
execute function regexp_replace('ab'::idn_mrLvarchar,'(a+|b)+','.&-\1.');

-- 59. result: q1 '.a-a..b-b..-.'
--             q2 '.a-a.b'
execute function regexp_replace('ab'::idn_mrLvarchar,'(a+|b)?','.&-\1.');
execute function regexp_replace('ab'::idn_mrLvarchar,'(a+|b)?','.&-\1.', 1);

-- 60. result: q1 '.cde...'
--             q2 '.cde.'
execute function regexp_replace('cde'::idn_mrLvarchar,'[^ab]*','.&.');
execute function regexp_replace('cde'::idn_mrLvarchar,'[^ab]*','.&.', 1);

-- 61. result: 'abcd-&-\abcd'
execute function regexp_replace('abcd'::idn_mrLvarchar,'abcd','&-\&-\\&');

-- 62. result: 'bc-\1-\bc'
execute function regexp_replace('abcd'::idn_mrLvarchar,'a(bc)d','\1-\\1-\\\1');

-- 63. result: 'abbbcd-c'
execute function regexp_replace('abbbcd'::idn_mrLvarchar,'([abc])*d','&-\1');

-- 64. result: 'abcd-a'
execute function regexp_replace('abcd'::idn_mrLvarchar,'([abc])*bcd','&-\1');

-- 65. result: 'e'
execute function regexp_replace('e'::idn_mrLvarchar,'a|b|c|d|e','&');

-- 66. result: 'ef-e'
execute function regexp_replace('ef'::idn_mrLvarchar,'(a|b|c|d|e)f','&-\1');

-- 67. result: 'abcdefg'
execute function regexp_replace('abcdefg'::idn_mrLvarchar,'abcd*efg','&');

-- 68. result: q1 'x.ab.y.abbb.z'
--             q2 'x.ab.yabbbz'
execute function regexp_replace('xabyabbbz'::idn_mrLvarchar,'ab*','.&.');
execute function regexp_replace('xabyabbbz'::idn_mrLvarchar,'ab*','.&.', 1);

-- 69. result: q1 'x.a.y.abbb.z'
--             q2 'x.a.yabbbz'
execute function regexp_replace('xayabbbz'::idn_mrLvarchar,'ab*','.&.');
execute function regexp_replace('xayabbbz'::idn_mrLvarchar,'ab*','.&.', 1);

-- 70. result: 'ab.cde-cd.'
execute function regexp_replace('abcde'::idn_mrLvarchar,'(ab|cd)e','.&-\1.');

-- 71. result: '.hij.'
execute function regexp_replace('hij'::idn_mrLvarchar,'[abhgefdc]ij','.&.');

-- 72. jta suspects the Toronto result for this one test might be incorrect.
--     The 'tests' file indicates that the substring replace should
--     be 'xy'; however, it also lists that the match should return
--     'n'. regexp_match() agrees with the 'n' (returns 'false').
--     In that case, the replace and substitute columns should be -.
--
--     result: 'abcde'
execute function regexp_match('abcde'::idn_mrLvarchar,'^(ab|cd)e');

execute function regexp_replace('abcde'::idn_mrLvarchar,'^(ab|cd)e','x\1y');

-- 73. result: 'abcd.ef-.'
execute function regexp_replace('abcdef'::idn_mrLvarchar,'(abc|)ef','.&-\1.');

-- 74. result: 'a.bcd-b.'
execute function regexp_replace('abcd'::idn_mrLvarchar,'(a|b)c*d','.&-\1.');

-- 75. result: '.abc-a.'
execute function regexp_replace('abc'::idn_mrLvarchar,'(ab|ab*)bc','.&-\1.');

-- 76. result: '.abc-bc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'a([bc]*)c*','.&-\1.');

-- 77. result: '.abcd-bc-d.'
execute function regexp_replace('abcd'::idn_mrLvarchar,'a([bc]*)(c*d)','.&-\1-\2.');

-- 78. result: '.abcd-bc-d.'
execute function regexp_replace('abcd'::idn_mrLvarchar,'a([bc]+)(c*d)','.&-\1-\2.');

-- 79. result: '.abcd-b-cd.'
execute function regexp_replace('abcd'::idn_mrLvarchar,'a([bc]*)(c+d)','.&-\1-\2.');

-- 80. result: '.adcdcde.'
execute function regexp_replace('adcdcde'::idn_mrLvarchar,'a[bcd]*dcdcde','.&.');

-- 81. result: 'adcdcde'
execute function regexp_replace('adcdcde'::idn_mrLvarchar,'a[bcd]+dcdcde','-');

-- 82. result: '.abc-ab.'
execute function regexp_replace('abc'::idn_mrLvarchar,'(ab|a)b*c','.&-\1.');

-- 83. result: 'abc-a-b-d'
execute function regexp_replace('abcd'::idn_mrLvarchar,'((a)(b)c)(d)','.\1-\2-\3-\4.');

-- 84. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~]*','.&.');
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~]*','.&.', 1);

-- 85. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~]*','.&.');
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~]*','.&.', 1);

-- 86. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~ -~]*','.&.');
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~ -~]*','.&.', 1);

-- 87. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~]*','.&.');
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~]*','.&.', 1);

-- 88. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~ -~]*','.&.');
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~ -~]*','.&.',1);

-- 89. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~ -~ -~]*','.&.');
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~ -~ -~]*','.&.',1);

-- 90. result: q1 '.abc...'
--             q2 '.abc.'
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~ -~ -~ -~]*','.&.');
execute function regexp_replace('abc'::idn_mrLvarchar,'[ -~ -~ -~ -~ -~ -~ -~]*','.&.',1);

-- 91. result: '.alpha.'
execute function regexp_replace('alpha'::idn_mrLvarchar,'[a-zA-Z_][a-zA-Z0-9_]*','.&.');

-- 92. result: 'a.bh-.'
execute function regexp_replace('abh'::idn_mrLvarchar,'^a(bc+|b[eh])g|.h$','.&-\1.');

-- 93. result: '.effgz-effgz-.'
execute function regexp_replace('effgz'::idn_mrLvarchar,'(bc+d$|ef*g.|h?i(j|k))','.&-\1-\2.');

-- 94. result: '.ij-ij-j.'
execute function regexp_replace('ij'::idn_mrLvarchar,'(bc+d$|ef*g.|h?i(j|k))','.&-\1-\2.');

-- 95. result: 'effg'
execute function regexp_replace('effg'::idn_mrLvarchar,'(bc+d$|ef*g.|h?i(j|k))','-');

-- 96. result: 'bcdd'
execute function regexp_replace('bcdd'::idn_mrLvarchar,'(bc+d$|ef*g.|h?i(j|k))','-');

-- 97. result: 'r.effgz-effgz-.'
execute function regexp_replace('reffgz'::idn_mrLvarchar,'(bc+d$|ef*g.|h?i(j|k))','.&-\1-\2.');

-- 98. result: '.a.'
execute function regexp_replace('a'::idn_mrLvarchar,'(((((((((a)))))))))','.&.');

-- 99. result: 'uh-uh'
execute function regexp_replace('uh-uh'::idn_mrLvarchar,'multiple words of text','-');

-- 100. result: '.multiple words., yeah'
execute function regexp_replace('multiple words, yeah'::idn_mrLvarchar,'multiple words','.&.');

-- 101. result: '.abcde-ab-de.'
execute function regexp_replace('abcde'::idn_mrLvarchar,'(.*)c(.*)','.&-\1-\2.');

-- 102. result: '(b, a)'
execute function regexp_replace('(a, b)'::idn_mrLvarchar,'\((.*), (.*)\)','(\2, \1)');

