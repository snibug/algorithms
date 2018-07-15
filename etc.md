Coordinate System
========================
Point (a,b,c) meaning (a/c, b/c)
Line (a,b,c) meaning ax+by+c = 0
Point x Point = Line
Line x Line = Point


Euler v,e,f
============
v-e+f = 2

Pick's Theorem
==============
area A, interior points I, boundary points B: A=I+B/2-1


Nim Game
========
nimber = mex{possible states}

Moore's nim: Remove something from at most k rows. Convert to 2-base digits, treat each digit as k+1-base digit. Add without carries. (eg. xor is add-without-carry in base 2)

misere nim: play exactly as if you were playing normal play nim, except if your winning move would lead to a position that consists of heaps of size one only. In that case, leave exactly one more or one fewer heaps of size one than the normal play strategy recommends.




Graph theory
============

Arboricity - partition edges into forests. minimum number of forests. Special case of matroid partitioning
Matroid Partitioning - max flow of edges as nodes


Ineqaulities to shortest path problem
=====================================

For x(i) <= x(j) + c, draw an edge from j to i with cost c. If there is negative loop, it's not possible
Start node becomes 0, so create an artifical start node. Then you can handle x(i) <= 0 + c, 0 <= x(i) + c
