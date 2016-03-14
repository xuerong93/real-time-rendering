////////////////////////////////////////////////////////////////////
//////////////////Xuerong Hu's LAB5 for rt-rending//////////////////
////////////////////////////////////////////////////////////////////
set the compiler environment according to the tutorial.
In my code,I changed teacher's glm include from (#include<glm/glm.hpp>,#include<glm/gtx/transform.hpp>) to (#include<glm.hpp>;#include<gtx/transform.hpp> )
The white sphere means where the light is.

key control:
/////////////light position control//////////////
'R':light position move to right by 0.5 each time
'L':light position move to left by 0.5 each time
'U':light position move upward by 0.5 each time
'D':light position move downward by 0.5 each time
'F':light position move forward by 0.5 each time
'B':light position move backward by 0.5 each time
/////////////light intensity control//////////////
'o':shine larger
'p':shine smaller
'O':diffuse and specular become strong
'P':diffuse and specular become weak

'w':polygon mode fill or not
/////////////other control//////////////
'q':quit and exit;
't':the dragon ply rotates 
'T':the dragon ply rotates
'x':the dragon ply rotates
'y':the dragon ply rotates about y axis;
'r':the dragon ply move to right by 0.1
'l':the dragon ply move to left by 0.1
'f':the dragon ply move forward by 0.1
'b':the dragon ply move backward by 0.1
'u':the dragon ply move up by 0.1
'd':the dragon ply move down by 0.1
'm':the cube environment map move automatically
's':the cube environment map stop from moving
'+':the dragon ply and the cube environment map become larger
'-':the dragon ply and the cube environment map become smaller
'c':set everything as default


mouse control:
press the left button and move the mouse, the entire scene is rotated. 
Press the right button and move the mouse to scale the scene. 

background:the red sphere shows like the red sun;

ply model: dragon.ply with normal. In my main function, my ply model is called "newdragon.ply"