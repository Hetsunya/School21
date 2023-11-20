./s21_grep CC makefile grep >> s21_grep.log
grep CC makefile grep >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -e CC -e count makefile grep >> s21_grep.log
grep -e CC -e count makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -i CC makefile grep.h >> s21_grep.log
grep -i CC makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -v CC makefile grep.h >> s21_grep.log
grep -v CC makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -c CC makefile grep.h >> s21_grep.log
grep -c CC makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -l CC makefile grep.h >> s21_grep.log
grep -l CC makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -n CC makefile grep.h >> s21_grep.log
grep -n CC makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -s CC nofile.txt >> s21_grep.log
grep -s CC nofile.txt >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -f makefile grep.h makefile grep.h >> s21_grep.log
grep -f makefile grep.h makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -o gc makefile grep.h >> s21_grep.log
grep -o gc makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -iv CC makefile grep.h >> s21_grep.log
grep -iv CC makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -in CC makefile grep.h >> s21_grep.log
grep -in CC makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -hl CC makefile grep.h >> s21_grep.log
grep -hl CC makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -is CC nofile.txt >> s21_grep.log
grep -is CC nofile.txt >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -e CC -e count -v makefile grep.h >> s21_grep.log
grep -e CC -e count -v makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep CC -nl makefile grep.h >> s21_grep.log
grep CC -nl makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -e CC -f makefile grep.h makefile grep.h >> s21_grep.log
grep -e CC -f makefile grep.h makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -ov CC makefile grep.h >> s21_grep.log
grep -ov CC makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -oi CC makefile grep.h >> s21_grep.log
grep -oi CC makefile grep.h >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -ic CC makefile grep.h >> s21_grep.log
grep -ic CC makefile grep >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -ih CC makefile grep >> s21_grep.log
grep -ih CC makefile grep >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -on CC makefile grep >> s21_grep.log
grep -on CC makefile grep >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -il CC makefile grep >> s21_grep.log
grep -il CC makefile grep >> grep.log
diff -s s21_grep.log grep.log


./s21_grep -vn CC makefile grep >> s21_grep.log
grep -vn CC makefile grep >> grep.log
diff -s s21_grep.log grep.log

