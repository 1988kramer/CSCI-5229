hw2.c
Author: Andrew Kramer
Date: 9/19/2018

Example program plots a lorenz attractor using user-defined 
parameters. Line color smoothly transitions shifts from pink, 
to purple, to blue. User can make the colors cycle over the
length of the line.

Controls:

- right arrow: rotate view to the right
-  left arrow: rotate view to the left
-    up arrow: rotate view up
-  down arrow: rotate view down

- y: increase r parameter by 0.5
- p: decrease r parameter by 0.5
- i: increase b parameter by 0.05
- u: decrease b parameter by 0.05
- x: increase s parameter by 0.5
- k: decrease s parameter by 0.5
- g: start and stop cycling colors

Attributions:
 - this program borrows heavily from ex6.c, presented in class, for supporting functions
   such as special(), reshape(), and Print().
 - also borrows from lorenz.c, included in the assignment, for the logic 
   used to generate the vertices of the lorenz attractor graph

Time spent on assignment: 4 hours