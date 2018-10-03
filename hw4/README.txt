hw4.c
Author: Andrew Kramer
Date: 10/3/2018

Example program displays two model airplanes, loosely based
on the Piper J-3 Cub, flying in circles in opposite directions
and at different speeds above four other airplanes arranged 
above a green polygon representing the ground. 

The scene can be viewed in 3 modes:
- Orthogonal, outside the scene
- Perspective, outside the scene
- First person, within the scene

Controls:

- in all modes:
  - right arrow: rotate view to the right
  -  left arrow: rotate view to the left
  -    up arrow: rotate view up
  -  down arrow: rotate view down
  -           0: reset view
  -           +: increase field of view
  -           -: decrease field of view
  -           m: switch view mode

- in first-person mode:
  - w: move forward
  - s: move backward
  - a: strafe left
  - d: strafe right


Attributions:
 - this program borrows heavily from ex9.c, presented in class, for supporting functions such as special(), reshape(), and Print(), Sin(), Cos(), and Project().

Time spent on assignment: 4 hours