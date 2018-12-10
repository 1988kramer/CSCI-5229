SlamViz
Author: Andrew Kramer
Date: 12/9/2018

Visualizer program for a sparse visual-inertial SLAM system. Shows the current 
and previous poses of the robot and the landmarks being actively tracked by 
the robot. It currently is only able to run from logged poses and landmarks. 
It provides the following options to the user:

- Reset the view scale, azimuth and elevation
- Change projection between perspective and orthogonal
- Change the light source location
  - can either orbit the origin or it can be placed distant from the 
    origin in the positive x and y
- Toggle the display of axes at the origin
- Switch the texture of the robot model between three options
- Switch between grid world or skybox environments
- Toggle whether inactive landmarks are displayed
  - inactive landmarks are those that are no longer used for tracking
- Set the lower threshold for the quality of landmarks being displayed
  - landmark quality is a function of the system's certainty on the 
    landmark's location
- Toggle whether the camera view is centered on the origin or centered on 
  the robot's current estimated location
- Toggle the display of previous poses, previous poses can be displayed as
  - Sets of axes if the "show axes" box is checked
  - A smoke trail that dissipates over time if "show axes" is unchecked
- Change the playback speed of the visualization
  - Positive speeds run the visualization forward
  - Zero speed pauses the visualization
  - Negative speed reverses the visualization

To Build:

Run qmake and then make.


To Run:

./SlamViz
No input files need to be specified as these are currently hardcoded.


Final Progress Assessment:

Since the project review I've accomplished the following:
 - Replaced the simple sphere landmarks with more complex objects
   - Landmarks are now represented by a textured star model
   - The star model is read from a .obj file using a function in Star.cpp
   - Each star should face the robot's current position in space
 - Added option to display the robot's previous poses as a smoke trail
   - The smoke trail is rendered as a series of transparent billboard 
     polygons with a smoke texture applied
   - The size of each polygon is a function of its age, so the smoke appears
     to dissipate over time
   - The functions used to render the smoke trail are found in SmokeBB.cpp
 - Added option to adjust the playback speed of the visualization
   - User can set zero playback speed to pause the visualization
   - User can also set negative values to the playback speed to reverse
     the visualization
 - Changed skybox to center on the robot's position when the camera tracks
   the robot's position
   - This prevents the robot from leaving the skybox when the camera is 
     tracking the robot's position and the view is zoomed in
 - Changed the timer function so the animation is now much smoother


Note on my original goals:

My initial goal with this project was to use a shader program to render
shadows. I had also intended to use a shader program to generate particles
for the robot's smoke trail. I was not successful in either of these efforts.
I spent roughly 20 hours trying to implement shadow mapping for my program
and I requested help from TAs twice. I was able to generate good depth maps 
from the light source's perspective, but I couldn't use those depth maps to
generate correct shadows in my scene. I eventually decided I should focus my 
efforts elsewhere so I could at least have some progress to show by the 
project's due date. This is why my project submission does not include shaders.
If you're interested I would be happy to share my work on shadow mapping so
you can see I did actually make some progress on it.


Attributions:

This program borrows heavily from the qt example presented in class. In a
couple of weeks it will also borrow from the shader examples.
