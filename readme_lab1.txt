Xuerong Hu's lab 1 of 5542
(Windodws Visual studio)

1. Open lab1(VC++ Project),set all include directory,library directory, additional include directory and additional dependencies correctly accordinh to PDF given by Teacher.
   include:Cg\include\Cg;     glew\include\GL
   Library: glew\lib\Release\Win32;      Cg\lib
   additional include:glm
   additional dependencies: glew32.lib
2. Select properties->Configuration Properties->Debugging->Command Arguments,select Edit, type window width(in pixel),length(in pixel),and the file path(for example: 600 600 file_path).
   My lab1 reads file "iris.csv", which includes description of numbers.
3. Add existing item lab1.cpp. Then build, compile and run.