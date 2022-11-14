# line to skip
10 for kk=0 to 15 :gosub 200 :next kk:? "Aga!"
20 rem stop
#50 gosub 250
#80 ' stop
110 ? "Merry christmass !!!"
120 for i = 0 to 10;
130 for j = 0 to 10-i:print "_";:next j ' space
140 for j = 0 to i*2: print "*";:next j ' tree
143 for j = 0 to 10-i:print "_";:next j ' space
145 print ' next line
150 next i
160 stop
200 ? "SUB A : ";kk
220 return
250 ? "SUB B"
260 return
800 stop