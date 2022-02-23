//Deniz Yuksel

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 1000

#define TIMER_PERIOD  1000/50 // Period for the timer.
#define TIMER_ON         0 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

bool INTERSECTING;
int A1, A2, B1, B2, Xa, mouseclick_X, mouseclickB, clickmouse_y, mouseclick_t;

int counter = 0;

typedef struct {
    float x, y;
} mouse_P;

typedef struct {
    mouse_P startpoint, endpoint;
    float A, B, C;
} line_t;

typedef struct {
    mouse_P merkez;
    float radiusdegree;
} cycle_t;

typedef struct {
    bool hit;
    float dist;
    mouse_P near;
    mouse_P exert;
} result_t;

cycle_t circ = { {0, 0} , 50 };

line_t line;
int no_lines = 0;
line_t perp;
result_t res;

mouse_P mouse;
int A[2];
int B[2];
int FREE[21] = {};
int XP[21];
int XY[21];
//
bool control = false;
bool control2 = false;
bool VERCH = false;
bool VERCH2 = false;
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}
void firstpoints(int x, int y)
{
    line.startpoint.x = x;
    line.startpoint.y = y;
    glColor3f(1, 0, 0);
    circle(line.startpoint.x, line.startpoint.y, 8);
}

int min(int x, int y)
{
    return y ^ ((x ^ y) & -(x < y));
}

int max(int x, int y)
{
    return x ^ ((x ^ y) & -(x < y));
}

bool interCheckFunc(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
    int A1 = y2 - y1;
    int b1 = x1 - x2;
    int a2 = y4 - y3;
    int b2 = x3 - x4;
    
    int determinant = A1 * b2 - a2 * b1;

    if (determinant == 0)
    
        return false;  
    else
    {
        int c1 = A1 * (x1)+b1 * (y1);
        int c2 = a2 * (x3)+b2 * (y3);
        int x = (b2 * c1 - b1 * c2) / determinant;
        int y = (A1 * c2 - a2 * c1) / determinant;
        if (((y < y1 && y < y2 || y > y1 && y > y2) && (x < x1 && x < x2 || x > x1 && x > x2)) || ((y < y4 && y < y3 || y > y4 && y > y3) && (x < x4 && x < x3 || x > x4 && x > x3)))
       
            return false;
      
        else                 
            return true;
        

    }
}

bool checkInter(int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4)
{
    if (max(x1, x2) < min(x3, x4))
    
        return false;
    

    A1 = (y1 - y2) / (x1 - x2);
    A2 = (y3 - y4) / (x3 - x4);
    B1 = y1 - A1 * x1;
    B2 = y3 - A2 * x3;

    if (A1 == A2)
    
        return false;

    Xa = (B2 - B1) / (A1 - A2);
    if ((Xa < max(min(x1, x2), min(x3, x4))) || (Xa > min(max(x1, x2), max(x3, x4))))
 
        return false;
else   
        return true;
}

void LINES(mouse_P s, mouse_P e, float size)
{
    glLineWidth(size);
    glBegin(GL_LINES);
    glVertex2f(s.x, s.y);
    glVertex2f(e.x, e.y);
    glEnd();
    glLineWidth(1.0f);
}

//2 ana nokta aras�ndaki �izgi
void DRAWSEG(int x1, int x2, int y1, int y2)
{
    //circle(line.end.x, line.end.y, 5);
    glColor3f(0, 0, 1);
    line.startpoint.x = x1;
    line.startpoint.y = y1;
    line.endpoint.x = x2;
    line.endpoint.y = y2;
    LINES(line.startpoint, line.endpoint, 3);
}

void DRAW1(int x1, int x2, int y1, int y2)
{
    //circle(line.end.x, line.end.y, 5);
    glColor3f(1, 0, 0);
    line.startpoint.x = x1;
    line.startpoint.y = y1;
    line.endpoint.x = x2;
    line.endpoint.y = y2;
    LINES(line.startpoint, line.endpoint, 3);
}


mouse_P  findPoint(mouse_P start, mouse_P end, float t) {
    mouse_P p = { start.x + (end.x - start.x) * t, start.y + (end.y - start.y) * t };
    return p;
}

void MAIN1(int x1, int x2, int y1, int y2)
{
    line.startpoint.x = x1;
    line.startpoint.y = y1;
    mouse_P s = findPoint(line.startpoint, line.endpoint, -100);
    line.startpoint.x = x2;
    line.startpoint.y = y2;
    mouse_P e = findPoint(line.startpoint, line.endpoint, 100);
    glColor3f(1, 0, 0.8);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00F0);
    LINES(s, e, 1);
    s = findPoint(circ.merkez, res.exert, 100);
    e = findPoint(circ.merkez, res.exert, -100);
    LINES(s, e, 1);
    glDisable(GL_LINE_STIPPLE);
}

void CIRCLE(int x, int y, int r)
{

    glColor4f(1, 0.3, 0.5, 1);
    if (res.hit)
        glColor4f(1, 0, 0, 0.7);
    circle(x, y, r);
    glColor3f(0, 0, 0);
    vprint(x-5, y-5, GLUT_BITMAP_HELVETICA_18, "S");
}

void CIRCLEOBJE(int x, int y, int r)
{

    glColor4f(1, 0.3, 0.5, 1);
    if (res.hit)
        glColor4f(1, 0, 0, 0.7);
    circle(x, y, r);
    glColor3f(0, 0, 0);
    vprint(x-5, y-5, GLUT_BITMAP_HELVETICA_18, "E");
}

void UST() {

    glColor3f(0, 0, 0);
    glRectf(-600, 450, 600, 500);

    glColor3f(1, 1, 1);
    vprint(-480, 468, GLUT_BITMAP_HELVETICA_18, "Deniz Yuksel - 21803358");
    glColor3f(1, 1, 1);

    glColor3f(0, 1, 0);
    vprint(-100, 475, GLUT_BITMAP_HELVETICA_18, "HOMEWORK 2 SPRING");
    glColor3f(1, 1, 1);
}

void ALT() {
    glColor3f(0, 0, 0);
    glRectf(-600, -500, 600, -450);


    glColor3f(1, 1, 1);
    vprint(-550, -480, GLUT_BITMAP_TIMES_ROMAN_24, "F1:Set Start Position");
    glColor3f(1, 1, 1);

    glColor3f(1, 1, 1);
    vprint(-300, -480, GLUT_BITMAP_TIMES_ROMAN_24, "F2:Set End Position");
    glColor3f(1, 1, 1);

    glColor3f(1, 1, 1);
    vprint(-0, -480, GLUT_BITMAP_TIMES_ROMAN_24, "F3:Add Lines");
    glColor3f(1, 1, 1);
    if (no_lines == 0) {
        glColor4f(1, 0, 0, 0.7);
       
        vprint(-550, -480, GLUT_BITMAP_TIMES_ROMAN_24, "F1:Set Start Position");
        glColor3f(1, 1, 1);
    }
    else if (no_lines == 1) {
        glColor4f(1, 0, 0, 0.7);
        vprint(-300, -480, GLUT_BITMAP_TIMES_ROMAN_24, "F2:Set End Position");
        glColor3f(1, 1, 1);
    }
    else if (no_lines == 2) {
        glColor4f(1, 0, 0, 0.7);
        vprint(-0, -480, GLUT_BITMAP_TIMES_ROMAN_24, "F3:Add Lines");
        glColor3f(1, 1, 1);
    }
    
}

void FON() {
    int c;
    for (c = -480; c < 480; c += 50) {
        glLineWidth(1);
        glColor3f(1, 0, 0);
        glBegin(GL_LINES);
        glVertex2f(-600, c);
        glVertex2f(600, c);
        glEnd();
        glLineWidth(1);
    }

    for (c = -600; c < 600; c += 50) {
        glLineWidth(1);
        glColor3f(1, 0, 0);
        glBegin(GL_LINES);
        glVertex2f(c, -480);
        glVertex2f(c, 480);
        glEnd();
        glLineWidth(1);
    }

}


void display() {


    //
    // clear window to black
    //
    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);

   
    
    FON();

    UST();
    ALT();


    if (no_lines == 0) {
        //drawFirstPoint();
        CIRCLE(A[0], B[0], 40);
        if (VERCH)
        {
            if (control)
            {
                CIRCLEOBJE(A[1], B[1], 40);
                DRAWSEG(A[0], A[1], B[0], B[1]);
            }
        }
        if (VERCH2)
        {
            for (int k = 0; k < counter; k++)
            {
                firstpoints(XP[k], XY[k]);
            }

            for (int j = 1; j < counter; j = j + 2)
            {
                DRAWSEG(XP[j - 1], XP[j], XY[j - 1], XY[j]);
                if (counter > 1)
                {
                    INTERSECTING = interCheckFunc(A[0], B[0], A[1], B[1], XP[j - 1], XY[j - 1], XP[j], XY[j]);
                }
                if (INTERSECTING)
                {
                    DRAW1(XP[j - 1], XP[j], XY[j - 1], XY[j]);
                }

            }
        }
    }

    if (no_lines == 1)
    {
        CIRCLE(A[0], B[0], 40);
        if (control)
        {
            CIRCLEOBJE(A[1], B[1], 40);
            DRAWSEG(A[0], A[1], B[0], B[1]);
        }

        if (VERCH2)
        {
            for (int k = 0; k < counter; k++)
            {
                firstpoints(XP[k], XY[k]);
            }

            for (int j = 1; j < counter; j = j + 2)
            {
                DRAWSEG(XP[j - 1], XP[j], XY[j - 1], XY[j]);
                if (counter > 1)
                {
                    INTERSECTING = interCheckFunc(A[0], B[0], A[1], B[1], XP[j - 1], XY[j - 1], XP[j], XY[j]);
                }
                if (INTERSECTING)
                {
                    DRAW1(XP[j - 1], XP[j], XY[j - 1], XY[j]);
                }

            }
        }
    }
    if (no_lines == 2)
    {
        CIRCLE(A[0], B[0], 40);
        if (control)
        {
            CIRCLEOBJE(A[1], B[1], 40);
            DRAWSEG(A[0], A[1], B[0], B[1]);
        }

        for (int k = 0; k < counter; k++)
        {
            firstpoints(XP[k], XY[k]);
        }

        for (int j = 1; j < counter; j = j + 2)
        {
            DRAWSEG(XP[j - 1], XP[j], XY[j - 1], XY[j]);
            if (counter > 1)
            {
                INTERSECTING = interCheckFunc(A[0], B[0], A[1], B[1], XP[j - 1], XY[j - 1], XP[j], XY[j]);
            }
            if (INTERSECTING)
            {
                DRAW1(XP[j - 1], XP[j], XY[j - 1], XY[j]);
            }

        }

    }

    glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    if (key == GLUT_KEY_F1)
        no_lines = 0;  

    if (key == GLUT_KEY_F2)
  
        no_lines = 1;
    if (key == GLUT_KEY_F3)  
        no_lines = 2;

    glutPostRedisplay();
}

void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    mouse_P click = { x - winWidth / 2 , winHeight / 2 - y };

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN)
    {
        if (no_lines == 0)
        { 
            mouseclick_X = click.x;
            clickmouse_y = click.y;
            A[0] = mouseclick_X;
            B[0] = clickmouse_y;
         
        }
        else if (no_lines == 1)
        { 
            mouseclickB = click.x;
            mouseclick_t = click.y;
            A[1] = mouseclickB;
            B[1] = mouseclick_t;
            control = true;
            VERCH = true;
        }

        else if (no_lines == 2)
        { 
            control2 = true;
            mouseclickB = click.x;
            mouseclick_t = click.y;
            XP[counter] = mouseclickB;
            XY[counter] = mouseclick_t;
            counter++;

            if (counter == 21)
            {
                counter = 0;
                for (int i = 0; i < 21; i++)
                {
                    XP[i] = FREE[i];
                    XY[i] = FREE[i];
                }
            }
            VERCH2 = true;

        }


    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.
    mouse.x = x - winWidth / 2;
    mouse.y = winHeight / 2 - y;


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("Template File");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}