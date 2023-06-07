#define _USE_MATH_DEFINES
#define ANGLE_INCREMENT 6.0
#define PI 3.14159265358979323846

#include <GL/glut.h> // GLUT, includes glu.h and gl.h
#include <math.h>

using namespace std;
// function declarations
void draw_circle(float, float, float, int, bool);
void draw_hour_hand();
void draw_minute_hand();
void draw_second_hand();
void second_passed_handler(int);
void draw_clock();

// variables
// hour angle
GLfloat hour_angle = 0.0;
// minute angle
GLfloat minute_angle = 0.0;
// second angle
GLfloat second_angle = 0.0;


/** 
 * Handler for window-repaint event. Call back when the window first appears and
 * whenever the window needs to be re-painted. 
 */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // draw the clock
    draw_clock();

    glFlush(); // Render now
}

/**
 *  Handler for window re-size event. Called back when the window first appears and
 * whenever the window is re-sized with its new width and height 
 */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
 
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   // Set the aspect ratio of the clipping area to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();
   if (width >= height) {
     // aspect >= 1, set the height from -1 to 1, with larger width
      gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
   } else {
      // aspect < 1, set the width to -1 to 1, with larger height
     gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
   }
}


/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);        // Initialize GLUT
    glutInitWindowSize(640, 640); // Set the window's initial width & height

    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner

    glutCreateWindow("Offline 1: Clock"); // Create a window with the given title
    glutDisplayFunc(display);              // Register display callback handler for window re-paint

    glutReshapeFunc(reshape); // Register callback handler for window re-size event
    glutTimerFunc(0, second_passed_handler, 0); // First timer call immediately
    glutMainLoop(); // Enter the infinitely event-processing loop
    return 0;
}


/**
 * This function draws the hour hand of the clock
 * at the center of the screen the shifting and rotation of the hand is done 
 * by the caller function
*/
void draw_hour_hand(){
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.3f, -0.04f);
        glVertex2f(0.5f, 0.0f);
        glVertex2f(0.3f, 0.04f);
    glEnd();

    draw_circle(0.325f, 0.0f, 0.05f, 10, true);
}
/**
 * This function draws the minute hand of the clock
 * hifting and rotation of the hand is done by the caller function
*/
void draw_minute_hand(){
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.5f, -0.025f);
        glVertex2f(0.8f, 0.0f);
        glVertex2f(0.5f, 0.025f);
    glEnd();
}
/**
 * This function draws the second hand of the clock
 * the shifting and rotation of the hand is done by the caller function
*/
void draw_second_hand(){
    glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.8f, 0.0f);
    glEnd();
}


/**
 * function responsible for entire clock drawing
 * draws the hour hand minute hand and second hand seperately then calls the display function
 * uses local coordinate system so any shifting and rotating applied on the clock 
 * is done by the caller function
*/
void draw_clock(){
    // rotate the hour hand by the hour angle
    glRotatef(-hour_angle, 0.0f, 0.0f, 1.0f);
    // draw the hour hand
    draw_hour_hand();
    // rotate the hour hand back to the original position
    glRotatef(+hour_angle, 0.0f, 0.0f, 1.0f);


    // rotate for the minutes hand
    glRotatef(-minute_angle, 0.0f, 0.0f, 1.0f);
    // draw the minute hand
    draw_minute_hand();
    // rotate back to the original position
    glRotatef(+minute_angle, 0.0f, 0.0f, 1.0f);


    // rotate for the seconds hand
    glRotatef(-second_angle, 0.0f, 0.0f, 1.0f);
    // draw the seconds hand
    draw_second_hand();
    // rotate back to the original position
    glRotatef(+second_angle, 0.0f, 0.0f, 1.0f);

    // draw the middle bob
    draw_circle(0.0f, 0.0f, 0.04f, 10, true);

    // draw the outer circle
    draw_circle(0.0f, 0.0f, 0.9f, 100, false);
}

/**
 * This function draws a circle at the given center and radius
 * the number of segments determines the smoothness of the circle
*/
void draw_circle(float cx, float cy, float r, int num_segments, bool fill)
{
    float theta = PI * 2 / float(num_segments);
    float tangetial_factor = tanf(theta);//calculate the tangential factor 

    float radial_factor = cosf(theta);//calculate the radial factor 

    float x = r;//we start at angle = 0 

    float y = 0;
    glLineWidth(2);
    if (fill)
    {
        glBegin(GL_POLYGON);
    } else {
        glBegin(GL_LINE_LOOP);
    }
    for (int ii = 0; ii < num_segments; ii++)
    {
        glVertex2f(x + cx, y + cy);//output vertex 

        //calculate the tangential vector 
        //remember, the radial vector is (x, y) 
        //to get the tangential vector we flip those coordinates and negate one of them 

        float tx = -y;
        float ty = x;

        //add the tangential vector 

        x += tx * tangetial_factor;
        y += ty * tangetial_factor;

        //correct using the radial factor 

        x *= radial_factor;
        y *= radial_factor;
    }
    glEnd();
}


/**
 * This function is called every second to update the clock
 * This only changes the value then calls the display function
*/
void second_passed_handler(int value){
    // increment the second angle
    second_angle += ANGLE_INCREMENT;
    // reset the angle if it is greater than 360
    if(second_angle >= 360.0){
        second_angle = 0.0;
    }
    // increment the minute angle
    minute_angle += ANGLE_INCREMENT / 60.0;
    // reset 
    if(minute_angle >= 360.0){
        minute_angle = 0.0;
    }
    // increment the hour angle
    hour_angle += ANGLE_INCREMENT / 3600.0;
    // reset
    if(hour_angle >= 360.0){
        hour_angle = 0.0;
    }
    // call the display function
    glutPostRedisplay();
    // call the timer function again
    glutTimerFunc(1000, second_passed_handler, 0);
}