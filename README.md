# opengl_2d3d_hetero
A sample code that presents a way to draw 2d &amp; 3d scenes in different window context with GLFW &amp; GLEW

Make sure GLFW and GLEW has been installed.

example compile command with gcc:
prerequisite: 
-    glfw&amp;glew header files are installed in /opt/local/include
-    glfw&amp;glew lib files are installed in /opt/local/lib
-    glfw version: 3.2.1, downloaded from http://www.glfw.org/download.html
-    glew version: 2.0.0, downloaded from http://glew.sourceforge.net/
gcc main.cpp -I/opt/local/include -o out -L/opt/local/lib -lglfw.3 -L/opt/local/lib -lGLEW.2.0 -framework OpenGL
