#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

#define SCALING_FACTOR 25

int th=0;         //  Angle in x-y plane
int ph=0;         //  Angle in y-z plane

// Current states of the arrow keys
int leftKeyDown = 0;
int rightKeyDown = 0;
int upKeyDown = 0;
int downKeyDown = 0;

// Current states of plus/minus keys
int s_mode = 0;
int b_mode = 0;
int r_mode = 0;

/*  Lorenz Parameters  */
double s = 5;
double b = 0.5;
double r = 15;

int auto_run = 1;

typedef struct
{
    double x, y, z;   // Position of first vertex
    float r, g, b;   // RGB color values for the line
} line_t;

line_t lorenz_attractor[50000] = {{0}};

// lorenz function taken from lorenz.c
void lorenz() 
{
    /*  Coordinates  */
    double x = 1;
    double y = 1;
    double z = 1;
    /*  Time step  */
    double dt = 0.001;

    //printf("%5d %8.3f %8.3f %8.3f\n",0,x,y,z);

    /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
    for (int i = 0; i < 50000; i++)
    {
        double dx = s*(y-x);
        double dy = x*(r-z)-y;
        double dz = x*y - b*z;
        x += dt*dx;
        y += dt*dy;
        z += dt*dz;

        // Determine the color of the vertex
        float r,g,b;

        if (i <= 500)
        {
            r = 0;
            g = 1;
            b = 1 - (i / 500);
        }
        else if (i <= 1000)
        {
            r = ((i - 500) / 500);
            g = 1;
            b = 0;
        }
        else if (i <= 10000)
        {
            r = 1 - ((i - 1000) / 9000);
            g = 1;
            b = ((i - 20000) / 10000);
        }
        else if (i <= 25000)
        {
            r = 0;
            g = 1 - ((i - 10000) / 15000);
            b = 1;
        }
        else
        {
            r = (i - 25000) / 25000;
            g = 0;
            b = 1;
        }

        line_t temp = {x / SCALING_FACTOR, y / SCALING_FACTOR, z / SCALING_FACTOR, r, g, b};
        lorenz_attractor[i] = temp;

        //printf("%5d %8.3f %8.3f %8.3f\n",i+1,x,y,z);
    }
}

// Print function taken from ex5.c
#define LEN 8192  //  Maximum amount of text
void Print(const char* format , ...)
{
   char    buf[LEN]; // Text storage
   char*   ch=buf;   // Text pointer
   //  Create text to be display
   va_list args;
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display text string
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

// Error function taken from ex5.c
void ErrCheck(char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

void line_draw(line_t l)
{
    glColor3f(l.r, l.g, l.b);
    glVertex3d(l.x, l.y, l.z);
}

void draw()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset transformation matrix
    glLoadIdentity();

    glRotatef(ph,1,0,0);
    glRotatef(th,0,1,0);

    // Draw axis lines
    glBegin(GL_LINES);  
    // X-axis
    glColor3f(1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);
    // Y- axis
    glColor3f(0,1,0);
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
    // Z- axis
    glColor3f(0,0,1);
    glVertex3f(0,0,0);
    glVertex3f(0,0,1);
    glEnd();

    glBegin(GL_LINE_STRIP);

    for (int i = 0; i < 50000; i++)
        line_draw(lorenz_attractor[i]);
    
    glEnd();

    glColor3f(1,1,1);
    glWindowPos2i(5,30);
    Print("Angle theta = %d, Angle phi = %d", th, ph);

    glWindowPos2i(5,5);
    Print("Parameters - S: %.2f, B: %.2f, R: %.2f", s, b, r);

    ErrCheck("display");

    glFlush();
    //glutSwapBuffers();
}

void keyDown(unsigned char key, int x, int y)
{
    // Exit program if ESC is pressed
    if (key == 27)
        exit(0);
    
    // Toggle automatic parameter change
    if (key == 'a')
    {
        if (auto_run == 1)
            auto_run = 0;
        else
            auto_run = 1;
    }


    // Check which variable the user currently wants to change
    if (key == 's')
    {
        s_mode = 1;
        b_mode = 0;
        r_mode = 0;
    }
    else if (key == 'b')
    {
        s_mode = 0;
        b_mode = 1;
        r_mode = 0;
    }
    else if (key == 'r')
    {
        s_mode = 0;
        b_mode = 0;
        r_mode = 1;
    }
    // If no variable is selected, don't change any
    else if (!(key == '+' || key == '=' || key == '-' || key == '_'))
    {
        s_mode = 0;
        b_mode = 0;
        r_mode = 0;
    }

    if ((key == '+' || key == '='))
    {
        if (s_mode == 1)
            s+=0.25;
        else if (b_mode == 1)
            b+=0.25;
        else if (r_mode == 1)
            r+=0.25;
        
        lorenz();
    }
    else if ((key == '-' || key == '_'))
    {
        if (s_mode == 1)
            s-=0.25;
        else if (b_mode == 1)
            b-=0.25;
        else if (r_mode == 1)
            r-=0.25;
        
        lorenz();
    }

    glutPostRedisplay();
}

void specialKeyDown(int key,int x,int y)
{
    if (key == GLUT_KEY_RIGHT)
        rightKeyDown = 1;
    else if (key == GLUT_KEY_LEFT)
        leftKeyDown = 1;
    if (key == GLUT_KEY_UP)
        upKeyDown = 1;
    else if (key == GLUT_KEY_DOWN)
        downKeyDown = 1;
   
    glutPostRedisplay();
}

void specialKeyUp(int key,int x,int y)
{
    if (key == GLUT_KEY_RIGHT)
        rightKeyDown = 0;
    else if (key == GLUT_KEY_LEFT)
        leftKeyDown = 0;
    if (key == GLUT_KEY_UP)
        upKeyDown = 0;
    else if (key == GLUT_KEY_DOWN)
        downKeyDown = 0;
        
    glutPostRedisplay();
}

/* This function is used for smooth rotation;
*  Checks for key presses every 10 milliseconds and updates rotation accordingly. */
void timer(int val)
{
    /* Check if keys are pressed down 
    *  If yes, continue rotating. */
    if (!rightKeyDown || !leftKeyDown)
    {
        if (rightKeyDown)
            th += 1;
        else if (leftKeyDown)
            th -= 1;
    }
    if (!upKeyDown || !downKeyDown)
    {
        if (upKeyDown)
            ph += 1;
        else if (downKeyDown)
            ph -= 1;
    }
    
    th %= 360;
    ph %= 360;

    // If auto_run is enabled, automatically scale parameters
    if (auto_run == 1)
    {
        s += 0.1;
        b += 0.01;
        r += 0.2;

        if (s >= 40)
            s = 1;
        if (b >= 20)
            b = 1;
        if (r >= 50)
            r = 1;

        lorenz();
    }

    glutPostRedisplay();

    // Call this function again in 10 milliseconds
    glutTimerFunc(10, timer, 0);
}

// Reshape function taken from ex6.c
void reshape(int width,int height)
{
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   double asp = (height>0) ? (double)width/height : 1;
   glOrtho(-asp*3, asp*3, -3, 3, -3, 3);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

int main(int argc, char* argv[]) 
{
    // Initialize glut
    glutInit(&argc, argv);
    //glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowSize(1300,900);
    glutCreateWindow("OwenAllison_Homework2");

    #ifdef USEGLEW
    //  Initialize GLEW
    if (glewInit()!=GLEW_OK) fprintf(stderr, "Error initializing GLEW\n");
    #endif

    // Function specifications
    glutDisplayFunc(draw);
    glutKeyboardFunc(keyDown);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    glutReshapeFunc(reshape);

    lorenz();

    // Enable Depth-Buffer
    glEnable(GL_DEPTH_TEST);

    // Start timer
    glutTimerFunc(10, timer, 0);
    
    glutMainLoop();
    return 0;
}