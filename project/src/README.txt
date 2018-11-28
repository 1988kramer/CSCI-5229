SlamViz
Author: Andrew Kramer
Date: 11/28/2018

Visualizer program for a sparse visual-inertial SLAM system. Shows the current and previous poses of the robot and the landmarks being actively
tracked by the robot. It currently is only able to run from logged poses
and landmarks. It provides the following options to the user:

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


To Build:

Run qmake and then make.


To Run:

./SlamViz
No input files need to be specified as these are currently hardcoded.


Progress Assessment:

I've accomplished all the points presented in the "to do by review" section of my progress report. I've added the option for a gridworld display, the piper cub model tracks the robot's pose, and a subset of the robot's previos poses are displayed as axes. I've also made several developments from the "to do after review" section of my progress report. I've added display of landmarks with the option to display inactive as well as active landmarks and I've added the option for the camera to track the robot's pose.


Still To Do:

- Add accurate scene shadowing using a shader program
- Add smoke trail for previous poses using particles controlled by a shader
- Make each landmark a light source using a shader
  - Light intensity should be proportional to landmark quality
  - Light color should change between active and inactive landmarks


Optional To Do:

- Add option to adjust playback speed
- Add cockpit/first-person view
- Use shader to render landmarks
  - currently bogs down if all landmarks are displayed, even with low poly-count landmarks


Attributions:

This program borrows heavily from the qt example presented in class. In a
couple of weeks it will also borrow from the shader examples.
