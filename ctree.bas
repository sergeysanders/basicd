1 list:for i=0 to 20: ? "-";:next i:? :? 
10 input "How big is your tree (3-40)?",size
20 if (size<3) or (size>40) then print "oi oi oi!":stop
25 size = size-1
110 ? "Merry christmass !!!":? :? 
120 for i = 0 to size;
130 for j = 0 to size-i:print " ";:next j ' space
140 for j = 0 to i*2: print "*";:next j ' tree
143 'for j = 0 to size-i:print " ";:next j ' space
145 print 'next line
150 next i
160 stop
