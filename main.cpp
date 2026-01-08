#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define STAR_COUNT 500
#define METEOR_COUNT 10

float starX[STAR_COUNT];
float starY[STAR_COUNT];
float starZ[STAR_COUNT];

float angleMercury = 0.0f;
float angleVenus   = 0.0f;
float angleEarth   = 0.0f;
float angleMars    = 0.0f;
float angleJupiter = 0.0f;
float angleSaturn  = 0.0f;
float angleUranus  = 0.0f;
float angleNeptune = 0.0f;

float speedMercury = 4.0f;
float speedVenus   = 3.0f;
float speedEarth   = 2.5f;
float speedMars    = 2.0f;
float speedJupiter = 1.6f;
float speedSaturn  = 1.3f;
float speedUranus  = 1.0f;
float speedNeptune = 0.8f;

float angleMoon = 0.0f;
float speedMoon = 8.0f;

float zoom         = 1.0f;
int   paused       = 1;
float angle        = 0.0f;
float sceneRotateY = 0.0f;

int sunPOV     = 0;
int earthPOV   = 0;
int neptunePOV = 0;

float camYaw   = 0.0f;
float camPitch = 0.0f;

int   meteorsActive = 0;
float meteorSpeedFast = 0.35f;
float meteorSpeedSlow = 0.15f;
float meteorLenFast   = 6.0f;
float meteorLenSlow   = 3.0f;

float meteorX[METEOR_COUNT];
float meteorY[METEOR_COUNT];
float meteorZ[METEOR_COUNT];

float meteorDirX[METEOR_COUNT];
float meteorDirY[METEOR_COUNT];

float meteorSpeed = 0.35f;
float meteorLen   = 4.0f;

void drawLineDDA(float x1, float y1, float x2, float y2, float z)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float steps = fabs(dx);
    if (fabs(dy) > steps) steps = fabs(dy);
    if (steps == 0) return;

    float xInc = dx / steps;
    float yInc = dy / steps;

    float x = x1;
    float y = y1;

    glPointSize(4.0f);
    glBegin(GL_POINTS);

    int k = 0;
    while (k <= (int)steps) {
        glVertex3f(x, y, z);
        x += xInc;
        y += yInc;
        k++;
    }
    glEnd();
}





void drawCircleMidpoint(int R, float scale, float z)
{
    int x = 0;
    int y = R;
    int d = 1 - R;

    glBegin(GL_POINTS);
    while (y >= x) {
        float xs = x * scale;
        float ys = y * scale;

        glVertex3f( xs,  ys, z);
        glVertex3f( ys,  xs, z);
        glVertex3f(-xs,  ys, z);
        glVertex3f(-ys,  xs, z);
        glVertex3f( xs, -ys, z);
        glVertex3f( ys, -xs, z);
        glVertex3f(-xs, -ys, z);
        glVertex3f(-ys, -xs, z);

        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
    }
    glEnd();
}

void bezierPoint(
    float t,
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    float x3, float y3,
    float* x, float* y)
{
    float u  = 1.0f - t;
    float uu = u * u;
    float uuu = uu * u;
    float tt  = t * t;
    float ttt = tt * t;

    *x = uuu * x0
       + 3 * uu * t  * x1
       + 3 * u  * tt * x2
       + ttt * x3;

    *y = uuu * y0
       + 3 * uu * t  * y1
       + 3 * u  * tt * y2
       + ttt * y3;
}










void circle(float radius, float x, float y,
            float red, float green, float blue)
{
    int stacks = 40;
    int slices = 40;

    float lx = 0.7f;
    float ly = 0.7f;
    float lz = 0.5f;

    glPushMatrix();
    glTranslatef(x, y, 0.0f);

    int i = 0;
    while (i < stacks) {
        float phi1 = (float)i     / stacks * 3.1416f - 3.1416f / 2.0f;
        float phi2 = (float)(i+1) / stacks * 3.1416f - 3.1416f / 2.0f;

        glBegin(GL_TRIANGLE_STRIP);
        int j = 0;
        while (j <= slices) {
            float theta = (float)j / slices * 2.0f * 3.1416f;

            float x1 = radius * cosf(phi1) * cosf(theta);
            float y1 = radius * cosf(phi1) * sinf(theta);
            float z1 = radius * sinf(phi1);

            float nx1 = cosf(phi1) * cosf(theta);
            float ny1 = cosf(phi1) * sinf(theta);
            float nz1 = sinf(phi1);

            float dot1 = nx1*lx + ny1*ly + nz1*lz;
            if (dot1 < 0.0f) dot1 = 0.0f;
            float k1 = 0.2f + 0.8f * dot1;
            glColor3f(red * k1, green * k1, blue * k1);
            glVertex3f(x1, y1, z1);

            float x2 = radius * cosf(phi2) * cosf(theta);
            float y2 = radius * cosf(phi2) * sinf(theta);
            float z2 = radius * sinf(phi2);

            float nx2 = cosf(phi2) * cosf(theta);
            float ny2 = cosf(phi2) * sinf(theta);
            float nz2 = sinf(phi2);

            float dot2 = nx2*lx + ny2*ly + nz2*lz;
            if (dot2 < 0.0f) dot2 = 0.0f;
            float k2 = 0.2f + 0.8f * dot2;
            glColor3f(red * k2, green * k2, blue * k2);
            glVertex3f(x2, y2, z2);

            j++;
        }
        glEnd();

        i++;
    }

    glPopMatrix();
}



void drawOrbit(float radius)
{
    glColor3f(0.25f, 0.25f, 0.30f);
    int R = 120;
    float scale = radius / (float)R;
    drawCircleMidpoint(R, scale, 0.0f);
}





void drawStars()
{
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    int i = 0;
    while (i < STAR_COUNT) {
        glVertex3f(starX[i], starY[i], starZ[i]);
        i++;
    }
    glEnd();
}

void timer(int t)
{
    if (!paused) {
        angleMercury += speedMercury;
        angleVenus   += speedVenus;
        angleEarth   += speedEarth;
        angleMars    += speedMars;
        angleJupiter += speedJupiter;
        angleSaturn  += speedSaturn;
        angleUranus  += speedUranus;
        angleNeptune += speedNeptune;
        angleMoon    += speedMoon;

        if (angleMercury >= 360) angleMercury -= 360;
        if (angleVenus   >= 360) angleVenus   -= 360;
        if (angleEarth   >= 360) angleEarth   -= 360;
        if (angleMars    >= 360) angleMars    -= 360;
        if (angleJupiter >= 360) angleJupiter -= 360;
        if (angleSaturn  >= 360) angleSaturn  -= 360;
        if (angleUranus  >= 360) angleUranus  -= 360;
        if (angleNeptune >= 360) angleNeptune -= 360;
        if (angleMoon    >= 360) angleMoon    -= 360;

        angle += 0.5f;
        if (angle > 360.0f) angle -= 360.0f;

        if (meteorsActive) {
            for (int i = 0; i < METEOR_COUNT; i++) {
                float spd = (i < METEOR_COUNT/2) ? meteorSpeedFast : meteorSpeedSlow;
                meteorX[i] += meteorDirX[i] * spd;
                meteorY[i] += meteorDirY[i] * spd;

                if (meteorX[i] < -25.0f || meteorX[i] > 25.0f ||
                    meteorY[i] < -15.0f || meteorY[i] > 15.0f) {

                    meteorX[i] = ((rand() % 200) / 100.0f - 1.0f) * 18.0f;
                    meteorY[i] = ((rand() % 200) / 100.0f - 1.0f) * 12.0f;
                    meteorZ[i] = - (float)(rand() % 20 + 20);

                    float dx = (float)(rand() % 200 - 100) / 100.0f;
                    float dy = (float)(rand() % 200 - 100) / 100.0f;
                    if (dx == 0 && dy == 0) dx = 1.0f;
                    float len = sqrtf(dx*dx + dy*dy);
                    meteorDirX[i] = dx / len;
                    meteorDirY[i] = dy / len;
                }
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(60, timer, 0);
}






void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (sunPOV==0 && earthPOV==0 && neptunePOV==0) {
        glTranslatef(0.0f, 0.0f, -15.0f);
        glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(sceneRotateY, 0.0f, 1.0f, 0.0f);
        glRotatef(angle, 1.0f, 1.0f, 0.0f);
    }
    else if (sunPOV == 1) {
        glTranslatef(0.0f, 0.0f, -2.0f);
        glRotatef(camPitch, 1.0f, 0.0f, 0.0f);
        glRotatef(camYaw,   0.0f, 1.0f, 0.0f);
    }
    else if (earthPOV == 1) {
        float rEarth = 3.4f;
        glTranslatef(0.0f, 0.0f, -2.0f);
        glRotatef(camPitch, 1.0f, 0.0f, 0.0f);
        glRotatef(camYaw,   0.0f, 1.0f, 0.0f);
        glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(-rEarth, 0.0f, 0.0f);
        glRotatef(-angleEarth, 0.0f, 0.0f, 1.0f);
    }
    else if (neptunePOV == 1) {
        float rNeptune = 9.0f;
        glTranslatef(0.0f, 0.0f, -2.0f);
        glRotatef(camPitch, 1.0f, 0.0f, 0.0f);
        glRotatef(camYaw,   0.0f, 1.0f, 0.0f);
        glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(-rNeptune, 0.0f, 0.0f);
        glRotatef(-angleNeptune, 0.0f, 0.0f, 1.0f);
    }

    glScalef(zoom, zoom, zoom);

    drawStars();

    circle(1.2f, 0.0f, 0.0f, 1.0f, 0.7f, 0.0f);

    drawOrbit(1.8f);
    glPushMatrix();
        glRotatef(angleMercury, 0.0f, 0.0f, 1.0f);
        glTranslatef(1.8f, 0.0f, 0.0f);
        circle(0.20f, 0.0f, 0.0f, 0.6f, 0.4f, 0.3f);
    glPopMatrix();

    drawOrbit(2.6f);
    glPushMatrix();
        glRotatef(angleVenus, 0.0f, 0.0f, 1.0f);
        glTranslatef(2.6f, 0.0f, 0.0f);
        circle(0.23f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
    glPopMatrix();

    drawOrbit(3.4f);
    glPushMatrix();
        glRotatef(angleEarth, 0.0f, 0.0f, 1.0f);
        glTranslatef(3.4f, 0.0f, 0.0f);
        circle(0.25f, 0.0f, 0.0f, 0.0f, 0.6f, 0.2f);

        glPushMatrix();
            float moonOrbit = 0.6f;
            glRotatef(angleMoon, 0.0f, 0.0f, 1.0f);
            glTranslatef(moonOrbit, 0.0f, 0.0f);
            circle(0.08f, 0.0f, 0.0f, 0.8f, 0.8f, 0.9f);
        glPopMatrix();
    glPopMatrix();

    drawOrbit(4.2f);
    glPushMatrix();
        glRotatef(angleMars, 0.0f, 0.0f, 1.0f);
        glTranslatef(4.2f, 0.0f, 0.0f);
        circle(0.22f, 0.0f, 0.0f, 0.9f, 0.3f, 0.2f);
    glPopMatrix();

    drawOrbit(5.3f);
    glPushMatrix();
        glRotatef(angleJupiter, 0.0f, 0.0f, 1.0f);
        glTranslatef(5.3f, 0.0f, 0.0f);
        circle(0.40f, 0.0f, 0.0f, 0.7f, 0.4f, 0.2f);
    glPopMatrix();

    drawOrbit(6.6f);
    glPushMatrix();
        glRotatef(angleSaturn, 0.0f, 0.0f, 1.0f);
        glTranslatef(6.6f, 0.0f, 0.0f);

        glColor3f(0.8f, 0.7f, 0.5f);
        int   R1     = 80;
        int   R2     = 70;
        float scale1 = (0.34f * 2.0f) / (float)R1;
        float scale2 = (0.34f * 1.6f) / (float)R2;
        drawCircleMidpoint(R1, scale1, 0.0f);
        drawCircleMidpoint(R2, scale2, 0.0f);

        circle(0.34f, 0.0f, 0.0f, 0.9f, 0.6f, 0.2f);
    glPopMatrix();

    drawOrbit(7.8f);
    glPushMatrix();
        glRotatef(angleUranus, 0.0f, 0.0f, 1.0f);
        glTranslatef(7.8f, 0.0f, 0.0f);
        circle(0.32f, 0.0f, 0.0f, 0.3f, 0.5f, 1.0f);
    glPopMatrix();

    drawOrbit(9.0f);
    glPushMatrix();
        glRotatef(angleNeptune, 0.0f, 0.0f, 1.0f);
        glTranslatef(9.0f, 0.0f, 0.0f);
        circle(0.30f, 0.0f, 0.0f, 0.1f, 0.1f, 0.5f);
    glPopMatrix();

    if (meteorsActive) {
        glColor3f(1.0f, 0.8f, 0.0f);
        for (int i = 0; i < METEOR_COUNT/2; i++) {
            float x1 = meteorX[i];
            float y1 = meteorY[i];
            float z  = meteorZ[i];
            float x2 = x1 + meteorDirX[i] * meteorLenFast;
            float y2 = y1 + meteorDirY[i] * meteorLenFast;
            drawLineDDA(x1, y1, x2, y2, z);
        }

        glColor3f(1.0f, 0.8f, 0.0f);
        for (int i = METEOR_COUNT/2; i < METEOR_COUNT; i++) {
            float x1 = meteorX[i];
            float y1 = meteorY[i];
            float z  = meteorZ[i];
            float x2 = x1 + meteorDirX[i] * meteorLenSlow;
            float y2 = y1 + meteorDirY[i] * meteorLenSlow;
            drawLineDDA(x1, y1, x2, y2, z);
        }
    }

    glutSwapBuffers();
}





void keyboard(unsigned char key, int x, int y)
{
    if (key == 'p' || key == 'P') {
        if (paused == 0) paused = 1;
        else paused = 0;
    }
    if (key == '+') {
        zoom = zoom + 0.1f;
    }
    if (key == '-') {
        zoom = zoom - 0.1f;
        if (zoom < 0.3f) zoom = 0.3f;
    }
    if (key == 'q' || key == 'Q') {
        if (sunPOV) {
            sunPOV = earthPOV = neptunePOV = 0;
        } else {
            sunPOV = 1; earthPOV = 0; neptunePOV = 0;
        }
        camYaw = camPitch = 0.0f;
    }
    if (key == 'e' || key == 'E') {
        if (earthPOV) {
            sunPOV = earthPOV = neptunePOV = 0;
        } else {
            sunPOV = 0; earthPOV = 1; neptunePOV = 0;
        }
        camYaw = camPitch = 0.0f;
    }
    if (key == 'n' || key == 'N') {
        if (neptunePOV) {
            sunPOV = earthPOV = neptunePOV = 0;
        } else {
            sunPOV = 0; earthPOV = 0; neptunePOV = 1;
        }
        camYaw = camPitch = 0.0f;
    }
    if (key == 'a' || key == 'A') {
        if (sunPOV==0 && earthPOV==0 && neptunePOV==0)
            sceneRotateY -= 5.0f;
        else
            camYaw -= 3.0f;
    }
    if (key == 'd' || key == 'D') {
        if (sunPOV==0 && earthPOV==0 && neptunePOV==0)
            sceneRotateY += 5.0f;
        else
            camYaw += 3.0f;
    }
    if (key == 'w' || key == 'W') {
        if (sunPOV==0 && earthPOV==0 && neptunePOV==0)
            angle -= 3.0f;
        else
            camPitch -= 3.0f;
    }
    if (key == 's' || key == 'S') {
        if (sunPOV==0 && earthPOV==0 && neptunePOV==0)
            angle += 3.0f;
        else
            camPitch += 3.0f;
    }
    else if (key == 27) {
        exit(0);
    }

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (!meteorsActive) {
            meteorsActive = 1;
            for (int i = 0; i < METEOR_COUNT; i++) {
                meteorX[i] = ((rand() % 200) / 100.0f - 1.0f) * 18.0f;
                meteorY[i] = ((rand() % 200) / 100.0f - 1.0f) * 12.0f;
                meteorZ[i] = - (float)(rand() % 20 + 20);

                float dx = (float)(rand() % 200 - 100) / 100.0f;
                float dy = (float)(rand() % 200 - 100) / 100.0f;
                if (dx == 0 && dy == 0) dx = 1.0f;
                float len = sqrtf(dx*dx + dy*dy);
                meteorDirX[i] = dx / len;
                meteorDirY[i] = dy / len;
            }
        } else {
            meteorsActive = 0;
        }
    }
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.02f, 0.02f, 0.06f, 1.0f);

    int i = 0;
    while (i < STAR_COUNT) {
        starX[i] = ((rand() % 200) / 100.0f - 1.0f) * 18.0f;
        starY[i] = ((rand() % 200) / 100.0f - 1.0f) * 18.0f;
        starZ[i] = - (float)(rand() % 40 + 20);
        i++;
    }
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Solar System Simulation (3D View)");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
