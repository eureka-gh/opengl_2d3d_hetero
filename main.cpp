//
//  main.cpp
//  GLFW&GLEW Example for 2d&3d rendering in different window contexts
//
//  Created by Hao, Gao on 26/08/2016.
//  Copyright © 2016. All rights reserved.
//

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define VISUAL_OBJECT_WINDOW_TITLE_BAR_HEIGHT               (30)    /* if less than 30, weird things happening on mac os */
#define VISUAL_OBJECT_WINDOW_PROGRESS_WINDOW_HEIGHT         (35)
#define VISUAL_OBJECT_WINDOW_PROGRESS_WINDOW_WIDTH_OFFSET   (20)
#define FRAME_PER_SECOND (24.0f)
#define FRAME_RATE (1.0f / FRAME_PER_SECOND)
#define MAX_FRAMES  (20 * (int)FRAME_PER_SECOND)
#define JOC_MAX_OBJECT_NUM (16)
#define JOC_VOLUME_METER_RANGE (-100)

/* global variabls */
bool g_stop_signal = false;
int  g_rendering_scale = 2;
int  g_current_frame_num = 0;

void draw_local_coordinates()
{
    glBegin(GL_LINES);
    
    /* red for x-axis */
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    
    /* green for y-axis */
    glColor3f(0.0,1.0,0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    
    /* blue for z-axis */
    glColor3f(0.0,0.0,1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    
    glEnd();
}

void draw_2d_dynamic_bars()
{
    GLfloat start_pos = -0.5f;
    GLfloat end_pos = start_pos*(-1.0f);
    
    GLfloat offset = (end_pos - start_pos)/JOC_MAX_OBJECT_NUM;
    GLfloat gap = offset/4;
    GLfloat xpos = start_pos+gap/2;
    GLfloat bar_range = 0.6f;
    
    /* Draw 16 empty bars for representing 16 objects */
    int i=0;
    for (;i<JOC_MAX_OBJECT_NUM; ++i, xpos+=offset)
    {
        glBegin(GL_LINE_LOOP);
        glColor3f(0.0f, 1.0f, 0.0f);
        
        glVertex2f(xpos, bar_range);
        glVertex2f(xpos+offset-gap, bar_range);
        glVertex2f(xpos+offset-gap, -1.0f*bar_range);
        glVertex2f(xpos, -1.0f*bar_range);
        
        glEnd();
    }
    
    
    /* Draw solid bar for all valid objects */
    xpos = start_pos+gap/2;
    GLfloat bar_height;
    GLfloat ypos;
    for (size_t i = 0; i < JOC_MAX_OBJECT_NUM; ++i, xpos+=offset)
    {
        float r = -1.0*(float)(abs(rand()) % JOC_VOLUME_METER_RANGE);
        
        bar_height = (r/(float)JOC_VOLUME_METER_RANGE) * (bar_range * 2);
        ypos = bar_height-bar_range;
        
        glBegin(GL_POLYGON);
        glColor3f(0.87f, 0.96f, 0.02f);  // yellow bar
        glVertex2f(xpos, ypos);
        glVertex2f(xpos+offset-gap, ypos);
        glVertex2f(xpos+offset-gap, -1.0f*bar_range);
        glVertex2f(xpos, -1.0f*bar_range);
        glEnd();
        
        glBegin(GL_LINES);
        /* grey for x-axis */
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(xpos, ypos);
        glVertex2f(xpos+offset-gap, ypos);
        /* y-axis */
        glEnd();
    }
}

void draw_2d_progress_bar()
{
    if (g_current_frame_num > MAX_FRAMES)
    {
        g_stop_signal = true;
        return;
    }
    
    GLfloat start_pos = -1.0f;
    GLfloat end_pos = start_pos*(-1.0f);
    GLfloat ratio = g_current_frame_num++ / (GLfloat)MAX_FRAMES;
    GLfloat current_pos = (end_pos - start_pos)*ratio + start_pos;
    
    /* setup background */
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    
    glLineWidth(5);
    glBegin(GL_LINES);
    
    /* grey for x-axis */
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(start_pos, 0.0, 0.0);
    glVertex3f(end_pos, 0.0, 0.0);
    
    /* draw y-axis as progressing bar */
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(current_pos, -0.5, 0.0);
    glVertex3f(current_pos, 0.5, 0.0);
    
    glEnd();
}

void draw_floor()
{
    glEnable(GL_LINE_SMOOTH);
    
    glBegin(GL_LINES);
    
    /* grey for floor lattice */
    glColor3f(0.67f, 0.67f, 0.67f);
    
    
    GLfloat upper_boundary =  1.0f * (GLfloat)g_rendering_scale;
    GLfloat lower_boundary = -1.0f * (GLfloat)g_rendering_scale;
    GLfloat step = 0.2f * (GLfloat)g_rendering_scale;
    
    for (GLfloat i = lower_boundary; i <= upper_boundary; i+=step)
    {
        glVertex3f(i, lower_boundary, 0.0);
        glVertex3f(i, upper_boundary, 0.0);
        
        for (GLfloat j = lower_boundary; j <= upper_boundary; j+=step)
        {
            glVertex3f(lower_boundary, j, 0.0);
            glVertex3f(upper_boundary, j, 0.0);
        }
    }
    
    glVertex3f(upper_boundary, lower_boundary, 0.0);
    glVertex3f(upper_boundary, upper_boundary, 0.0);
    
    glVertex3f(lower_boundary, upper_boundary, 0.0);
    glVertex3f(upper_boundary, upper_boundary, 0.0);
    
    
    glEnd();
}

void draw_3d_objects()
{
    /* Based on inital view perspective, apply coornidate transform */
    glLoadIdentity();                   // Reset The Current Modelview Matrix
    
    glTranslatef(0.0f, 0.0f, -6.0f);    // Move the center of scenary towards inside
    //glTranslatef(0.0f, -1.0f, 0.0f);    // Move the center of scenary downwards a bit
    glTranslatef(0.2f, 0.0f, 0.0f);     // Move the center of scenary to right side a bit to compensate the rotation around Z axis
    /* Rotate around X axis so that Z axis is upwards */
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    /* rotate around Z axis a little bit to show angle between X and Y */
    //glRotatef(-10.0f, 0.0f, 0.0f, 1.0f);
    GLfloat angle = (g_current_frame_num%36/(GLfloat)36) * (GLfloat)360;
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    /* rotate around X axis a little bit to lift the camera a bit */
    glRotatef(15.0f, 1.0f, 0.0f, 0.0f);
    
    draw_floor();
    draw_local_coordinates();
}

typedef void (*GL_DRAWING_FUNC)(void) ;

int main(void)
{
    GLFWwindow         *windows[3];
    GL_DRAWING_FUNC    drawing_funcs[3];
    
    GLFWwindow* window;
    GLFWwindow* window1;
    GLFWwindow* window2;
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int width = 640, height = 480;
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    window1 = glfwCreateWindow(width, 40, "Hello World", NULL, NULL);
    window2 = glfwCreateWindow(width/2, height/2, "Hello World", NULL, NULL);
    
    if (!window || !window1 || !window2)
    {
        printf("unable to create glfw windows, terminate...\n");
        glfwTerminate();
        return -1;
    }
    
    windows[0] = window;
    windows[1] = window1;
    windows[2] = window2;
    
    drawing_funcs[0] = draw_3d_objects;
    drawing_funcs[1] = draw_2d_progress_bar;
    drawing_funcs[2] = draw_2d_dynamic_bars;
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    /* glew function seems to work with opengl 2d rendering ?? */
    /* To be noticed:
     * glewInit has to be called after glfwMakeContextCurrent(window) which setup opengl context 
     */
    GLenum ret = glewInit();
    if( ret != GLEW_OK ){
        printf("Failed to init GLEW, error: %s.\n", glewGetString(ret));
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
    
    /* layout all windows at the right position with proper size */
    int xpos, ypos;
    /* Get main windows postion and size */
    glfwGetWindowPos(window, &xpos, &ypos);
    glfwGetWindowSize(window, &width, &height);
    /* Set slave windows position only */
    glfwSetWindowPos(window1, xpos, ypos + VISUAL_OBJECT_WINDOW_TITLE_BAR_HEIGHT + height);
    glfwSetWindowSize(window1, width - VISUAL_OBJECT_WINDOW_PROGRESS_WINDOW_WIDTH_OFFSET, VISUAL_OBJECT_WINDOW_PROGRESS_WINDOW_HEIGHT);
    glfwSetWindowPos(window2, xpos + width, ypos);
    
    /* setup viewport for 1st window */
    {
        glViewport(0, 0, width, height);
        /* set initial veiw perspective */
        glMatrixMode (GL_PROJECTION);   // Select The Projection Matrix
        glLoadIdentity ();              // Reset The Projection Matrix
        
        // Calculate The Aspect Ratio Of The Window
        // gluXXXX are all deprecated, gluPerspective can be replaced by glFrustum
        gluPerspective(45.0f, (GLfloat)(width)/(GLfloat)(height), 1.0f, 100.0f);
        
        glMatrixMode (GL_MODELVIEW);     // Select The Modelview Matrix
        glLoadIdentity ();               // Reset The Modelview Matrix
    }
    
    double last_time, current_time;
    last_time = glfwGetTime();
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window) && !g_stop_signal)
    {
        current_time = glfwGetTime();
        if (current_time - last_time > FRAME_RATE)
        {
            for (int i=0; i<3; ++i)
            {
                GLFWwindow* window = windows[i];
                /* switch window context */
                glfwMakeContextCurrent(window);
                
                /* Render here */
                glClear(GL_COLOR_BUFFER_BIT);

                (*drawing_funcs[i])();
                
                //glFlush(); /* comment out dosen't hurt */
                
                /* Swap front and back buffers */
                glfwSwapBuffers(window);
            }
            last_time = current_time;
        }
        
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
