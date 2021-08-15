-- -------------------------------------------------------------------------
-- Additional tests for the instr() function.
--
-- These focus on the case where we're searching for the Nth occurrence
-- of a string, scanning backward from the end.
-- -------------------------------------------------------------------------

execute function instr("abababababababa", "aba", -1, 1);
execute function instr("abababababababa", "aba", -1, 2);
execute function instr("abababababababa", "aba", -1, 3);
execute function instr("abababababababa", "aba", -1, 4);
execute function instr("abababababababa", "aba", -1, 5);
execute function instr("abababababababa", "aba", -1, 6);
execute function instr("abababababababa", "aba", -1, 7);
execute function instr("abababababababa", "aba", -1, 8);

execute function instr("abababababababa", "aba", -2, 1);
execute function instr("abababababababa", "aba", -3, 1);
execute function instr("abababababababa", "aba", -4, 1);
execute function instr("abababababababa", "aba", -5, 1);
execute function instr("abababababababa", "aba", -6, 1);
execute function instr("abababababababa", "aba", -7, 1);
execute function instr("abababababababa", "aba", -8, 1);
execute function instr("abababababababa", "aba", -9, 1);


