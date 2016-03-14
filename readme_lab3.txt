////////////////////////////////////////////////////////////////////
//////////////////Xuerong Hu's LAB3 for rt-rending//////////////////
////////////////////////////////////////////////////////////////////
set the compiler environment according to the tutorial.
In my code,I changed teacher's glm include from (#include<glm/glm.hpp>,#include<glm/gtx/transform.hpp>) to (#include<glm.hpp>;#include<gtx/transform.hpp> )
My robot can move eyes,nose,upper arms,lower arms,hands, upper leg,lower leg and feet. control keys are as follows.
The background looks like a maze and at the exit of the maze there is treasure for robot to pick.

key control:
'q':quit and exit;
't':the whole body rotate counter clockwise by 15 degrees each time 
'T':the whole body rotate clockwise by 15 degrees each time
'x':the whole body rotate about x axis;
'y':the whole body ratate about y axis;
'r':the whole body move to right by 0.1
'l':the whole body move to left by 0.1
'f':the whole body move forward by 0.1
'b':the whole body move backward by 0.1
'u':the whole body move up by 0.1
'd':the whole body move down by 0.1
'm':the whole robot move automatically
's':the whole robot stop from moving
'c':clear, move to home position(the entrance of the maze),and turn to the default camera position
'1':right lower arm rotates by 15 degrees each time(not the whole circle,looks more comfortable)
'2':right upper arm rotates by 15 degrees each time(not the whole circle,looks more comfortable)
'3':left lower arm rotates by 15 degrees each time(not the whole circle,looks more comfortable)
'4':left upper arm rotates by 15 degrees each time(not the whole circle,looks more comfortable)
'5':right lower leg rotates by 15 degrees each time(not the whole circle,looks more comfortable)
'6':right upper leg rotates by 15 degrees each time(not the whole circle,looks more comfortable)
'7':left lower leg rotates by 15 degrees each time(not the whole circle,looks more comfortable)
'8':left upper leg rotates by 15 degrees each time(not the whole circle,looks more comfortable)
'9':rotate hands and feet at the same time by 30 degrees each time
'+':the whole robot becomes bigger
'-':the whole robot becomes smaller


mouse control:
press the left button and move the mouse, the entire scene is rotated. 
Press the right button and move the mouse to scale the scene. 

background:
the white one is the maze made of several cubes; the yellow sphere means treasure in the exit; the red sphere 
shows like the red sun;

the robot:
the body, upper arm, lower arm, hand, upper leg, lower leg, foot are all made of cubes;
the eye circles are made of cylinders;
the top of eye circle is circle;
the eye pupils and nose are made of spheres; 