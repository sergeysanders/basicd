8 list:?:?
5 cnt#=0
10 rem BasicD all functions test
12  ? cnt#
13 cnt# = cnt#+1
20 print "BasicD v0.3a":? "Test EVERYTHING":? "******************************"
30 ?:print "------ Variables' types:"
40 let string$ = "\"Abba\"" + " is the best group of 79' :)": let float = 1.23: integer# = 0x12300321: byte_ = 254
50 ? "string = ";string$,"float(number) = ";float,"integer = 0x";hex$(integer#),"byte = 0x";hex$(byte_)
55 'goto 10
# ------- MATH
60 ?:? "------ Math functions"
70 ? " 123*12/2^3 = "; 123*12/2^3
80 ? " 123.0*(12/2)^3 = "; 123*(12/2)^3
82 ? " (123*12/2)^3 = "; (123*12/2)^3
84 ? " cos(123*(12/2)^3) = "; cos(123*(12/2)^3)
85 'goto 10
# ------- LOOP
100 ?:? "------ for-next loop"
110 for i = 10 to 0 step -1
120 ? int(i)
130 next i
# ------- Input
150 ?:? "------ input test"
160 rndNum = rnd(11);
170 input "Guess a number (0-10):",num#
180 if num#=rndNum then print " BINGO! ":goto 200
182 if num#>rndNum then print " My number is lower :(":goto 200
184 if num#<rndNum then print " My number is higher :(":goto 200
200 ?:?
1000 stop