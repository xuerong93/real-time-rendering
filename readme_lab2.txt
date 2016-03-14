////////////////////////////////////////////////////////////////////
//////////////////Xuerong Hu's LAB2 for rt-rending//////////////////
////////////////////////////////////////////////////////////////////
set the compiler environment according to the tutorial.
In my code,I changed teacher's glm include from (#include<glm/glm.hpp>,#include<glm/gtx/transform.hpp>) to (#include<glm.hpp>;#include<gtx/transform.hpp> )
My robot can move eyes,nose,upper arms,lower arms,hands, upper leg,lower leg and feet. control keys are as follows.
The background looks like a maze and at the exit of the maze there is treasure for robot to pick.

key control:
'q':quit and exit;
't':the whole body rotate counter clockwise by 15 degrees each time 
'T':the whole body rotate clockwise by 15 degrees each time
'r':the whole body move to right by 0.1
'l':the whole body move to left by 0.1
'f':the whole body move up by 0.1
'b':the whole body move down by 0.1
'e':two eye pupil move around by 15 degrees each time
'n':nose move from -Pi/4 to Pi/4 by 20 degrees each time
'm':the whole robot move automatically
's':the whole robot stop from moving
'c':clear, move to home position(the entrance of the maze)
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
