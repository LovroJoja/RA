#include <vector>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <cmath>
using namespace std;
using namespace glm;
#define PI 3.14159265


void myIdle();
void updateBSplineMotion();
void draw_window();
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
void myDisplay();
void myReshape(int w, int h);
void myMouse(int button, int state, int x, int y){

}
GLuint window;
GLuint width = 1080;
GLuint height = 720;
const float delta = 0.1f;
int N_v;
int N_f;
int animated = 0;
vec3* aprox;
vec3* vrhovi;
vec3 pos;
vec3 tangenta;
ivec3* poligoni;
vec4* ravnine;
bool test_unutar = true;
float Global_t = 0.0;
int currentSegment = 0;
vec3 ociste;
vec3 glediste;
vec3 viewUp;
vec3 objectOrientation;
vec3 os_rotacije;


struct BSplineResult {
    glm::vec3 point;
    glm::vec3 derivative;
};

//mat4 Bmat = mat4(-1.0f, 3.0f, -3.0f, 1.0f, 3.0f, -6.0f, 3.0f, 0.0f, -3.0f, 0.0f, 3.0f, 0.0f, 1.0f, 4.0f, 1.0f, 0.0f);
mat4 Bmat = mat4(-1.0f, 3.0f, -3.0f, 1.0f, 3.0f, -6.0f, 0.0f, 4.0f, -3.0f, 3.0f, 3.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
mat4x3 Bdev = mat4x3(-1.0f, 2.0f, -1.0f, 3.0f, -4.0f, 0.0f, -3.0f, 2.0f, 1.0f, 1.0f, 0.0f, 0.0f);
//mat4 Bmat = mat4(-1.0f/6.0f, 3.0f/6.0f, -3.0f/6.0f, 1.0f/6.0f, 3.0f/6.0f, -6.0f/6.0f, 3.0f/6.0f, 0.0f/6.0f, -3.0f/6.0f, 0.0f/6.0f, 3.0f/6.0f, 0.0f/6.0f, 1.0f/6.0f, 4.0f/6.0f, 1.0f/6.0f, 0.0f/6.0f);
//mat4 Bmat = mat4(-1.0f/6.0f, 3.0f/6.0f, -3.0f/6.0f, 1.0f/6.0f, 3.0f/6.0f, -6.0f/6.0f, 0.0f/6.0f, 4.0f/6.0f, -3.0f/6.0f, 3.0f/6.0f, 3.0f/6.0f, 1.0f/6.0f, 1.0f/6.0f, 0.0f/6.0f, 0.0f/6.0f, 0.0f/6.0f);


BSplineResult BSpline(float t, int segment) {

	vec4 T = vec4(t * t * t, t * t, t, 1.0f);
	//vec4 tdev = vec4( 3 * t * t, 2 * t, 1, 1.0f);
	vec3 tdev = vec3(t * t, t, 1);
	//mat4 r = mat4(aprox[segment].x, aprox[segment].y, aprox[segment].z, 1.0f,
	//	aprox[segment+1].x, aprox[segment+1].y, aprox[segment+1].z, 1.0f,
	//	aprox[segment+2].x, aprox[segment+2].y, aprox[segment+2].z, 1.0f,
	//	aprox[segment+3].x, aprox[segment+3].y, aprox[segment+3].z, 1.0f);

	mat4 r = mat4(aprox[segment].x, aprox[segment + 1].x, aprox[segment + 2].x, aprox[segment + 3].x,
		aprox[segment].y, aprox[segment + 1].y, aprox[segment + 2].y, aprox[segment + 3].y,
		aprox[segment].z, aprox[segment + 1].z, aprox[segment + 2].z, aprox[segment + 3].z,
		1.0f, 1.0f, 1.0f, 1.0f);
	

	//vec4 p = dot(dot(T, Bmat), r);
	vec4 p = T * Bmat * r / 6.0f;
	vec4 pdev = tdev * Bdev * r / 2.0f;
	//printf("Tocka:%f %f %f %f\n", pdev.x, pdev.y, pdev.z, pdev.w);
	vec3 result = vec3(p.x, p.y, p.z);
	vec3 deriv = vec3(pdev.x, pdev.y, pdev.z);

	BSplineResult final;
    final.point = vec3(result.x, result.y, result.z);
    final.derivative = vec3(deriv.x, deriv.y, deriv.z);
    //delete[] p;
    //delete[] pdev;
    return final;
    
}

int main(int argc, char ** argv)
{	
	string path;
	path.append("C:\\IRG\\IRGPrimjeri\\Debug\\");
    path.append("bird.obj");
	FILE* file = fopen(path.c_str(), "rt");

	if(file == 0){
		printf("Unable to load file.\n");
		return 1;
	}
	
	float inX, inY, inZ;
	printf("Unesite koordinate tocke ocista:\n");
	scanf("%f %f %f", &inX, &inY, &inZ);
	ociste = vec3(inX, inY, inZ);

	printf("Unesite koordinate tocke gledista:\n");
	scanf("%f %f %f", &inX, &inY, &inZ);
	glediste = vec3(inX, inY, inZ);

	printf("Unesite koordinate viewUp vektora:\n");
	scanf("%f %f %f", &inX, &inY, &inZ);
	viewUp = vec3(inX, inY, inZ);
	pos = vec3(0.0, 0.0, 0.0);
	tangenta = vec3(0.0, 0.0, 0.0);
	objectOrientation = vec3(0.0, 0.0, 1.0);
	os_rotacije = vec3(0.0, 0.0, 0.0);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);

	window = glutCreateWindow("Glut OpenGL Transformacija Pogleda i Perspektivna Projekcija");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);


	char buffer[1000];
	N_v = 0;
	int v_i = 0;
	N_f = 0;
	int f_i = 0;

	while(fgets(buffer, sizeof(buffer), file)){
		if(buffer[0] == 'v') N_v++;
		else if(buffer[0] == 'f') N_f++;
	}
	fseek(file, 0, SEEK_SET);

	vrhovi = new vec3[N_v];
	poligoni = new ivec3[N_f];
	
	aprox = new vec3[12];
	aprox[0] = vec3(0.0, 0.0, 0.0);
	aprox[1] = vec3(0.0, 10.0, 5.0);
	aprox[2] = vec3(10.0, 10.0, 10.0);
	aprox[3] = vec3(10.0, 0.0, 15.0);
	aprox[4] = vec3(0.0, 0.0, 20.0);
	aprox[5] = vec3(0.0, 10.0, 25.0);
	aprox[6] = vec3(10.0, 10.0, 30.0);
	aprox[7] = vec3(10.0, 0.0, 35.0);
	aprox[8] = vec3(0.0, 0.0, 40.0);
	aprox[9] = vec3(0.0, 10.0, 45.0);
	aprox[10] = vec3(10.0, 10.0, 50.0);
	aprox[11] = vec3(10.0, 0.0, 55.0);


	while(fgets(buffer, sizeof(buffer), file)){
		if (buffer[0] == 'v'){
			float x, y, z;
			sscanf(buffer, "v %f %f %f", &x, &y, &z);
			vrhovi[v_i] = vec3(x, y, z);
			v_i++;
		}
		else if (buffer[0] == 'f'){
			int x, y, z;
			sscanf(buffer, "f %d %d %d", &x, &y, &z);
			poligoni[f_i] = vec3(x, y, z);
			f_i++;
		}
	}
	
	
	glutMainLoop();
	return 0;
}
void myDisplay(){
	draw_window();
	glFlush();
}
void myReshape(int w, int h){

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	width = w;
	height = h;	
	//draw_window();
	//glFlush();
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY){
	if(theKey == 'q') ociste.x += delta;
	if(theKey == 'w') ociste.x -= delta;
	if(theKey == 'a') ociste.y += delta;
	if(theKey == 's') ociste.y -= delta;
	if(theKey == 'y') ociste.z += delta;
	if(theKey == 'x') ociste.z -= delta;
	if(theKey == 'r') glediste.x += delta;
	if(theKey == 't') glediste.x -= delta;
	if(theKey == 'f') glediste.y += delta;
	if(theKey == 'g') glediste.y -= delta;
	if(theKey == 'v') glediste.z += delta;
	if(theKey == 'b') glediste.z -= delta;
	if(theKey == 'p'){
		if(animated == 0) animated = 1;
		else animated = 0;
	}
	if(theKey == 'o'){
		updateBSplineMotion();
	}
	
	printf("Transformacija: \n");	
	printf("Ociste:%f %f %f\n", ociste.x, ociste.y, ociste.z);
    printf("Glediste:%f %f %f\n", glediste.x, glediste.y, glediste.z); 
	draw_window();
	glFlush();

}

void draw_window() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set the background color to white
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (GLfloat)width / (GLfloat)height, 0.1, 100.0); 

    // Set up the view matrix
    gluLookAt(ociste.x, ociste.y, ociste.z, glediste.x, glediste.y, glediste.z, viewUp.x, viewUp.y, viewUp.z);

    
   
    //glRotatef(cos_theta,os_rotacije.x, os_rotacije.y, os_rotacije.z);
    // Određivanje kuta i osi rotacije
    os_rotacije.x = objectOrientation.y * tangenta.z - objectOrientation.z * tangenta.y;
    os_rotacije.y = -(objectOrientation.x * tangenta.z - objectOrientation.z * tangenta.x);
    os_rotacije.z = objectOrientation.x * tangenta.y - objectOrientation.y * tangenta.x;
    os_rotacije = normalize(os_rotacije);
    float cos_theta = dot(objectOrientation, tangenta);
    float theta = acos(cos_theta) * 180.0 / PI;
    //float sin_theta = sin(theta);
    printf("Os Rotacije:%f %f %f\n", os_rotacije.x, os_rotacije.y, os_rotacije.z);
    
    
    glPushMatrix();
    //glTranslatef(pos.x, pos.y, pos.z);
    //glLoadIdentity();
    
    
    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(theta, os_rotacije.x, os_rotacije.y, os_rotacije.z);
    // Crtanje objekta, transformacije se provode redoslijedom odozdo prema gore
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 0.0f);
    for (int i = 0; i < N_f; i++) {
        //glVertex3f(vrhovi[poligoni[i].x - 1].x + pos.x, vrhovi[poligoni[i].x - 1].y + pos.y, vrhovi[poligoni[i].x - 1].z + pos.z);
        //glVertex3f(vrhovi[poligoni[i].y - 1].x + pos.x, vrhovi[poligoni[i].y - 1].y + pos.y, vrhovi[poligoni[i].y - 1].z + pos.z);
        //glVertex3f(vrhovi[poligoni[i].z - 1].x + pos.x, vrhovi[poligoni[i].z - 1].y + pos.y, vrhovi[poligoni[i].z - 1].z + pos.z);

        glVertex3f(vrhovi[poligoni[i].x - 1].x, vrhovi[poligoni[i].x - 1].y , vrhovi[poligoni[i].x - 1].z);
        glVertex3f(vrhovi[poligoni[i].y - 1].x, vrhovi[poligoni[i].y - 1].y, vrhovi[poligoni[i].y - 1].z);
        glVertex3f(vrhovi[poligoni[i].z - 1].x, vrhovi[poligoni[i].z - 1].y, vrhovi[poligoni[i].z - 1].z);
    }
    glEnd();
    glLineWidth(1.0);
    glColor3f(0.0f, 0.0f, 0.0f);  // Wireframe
    glBegin(GL_LINES);
    for (int i = 0; i < N_f; i++) {
        //glVertex3f(vrhovi[poligoni[i].x - 1].x  + pos.x, vrhovi[poligoni[i].x - 1].y + pos.y, vrhovi[poligoni[i].x - 1].z + pos.z);
        //glVertex3f(vrhovi[poligoni[i].y - 1].x + pos.x, vrhovi[poligoni[i].y - 1].y + pos.y, vrhovi[poligoni[i].y - 1].z + pos.z);

        //glVertex3f(vrhovi[poligoni[i].y - 1].x + pos.x, vrhovi[poligoni[i].y - 1].y + pos.y, vrhovi[poligoni[i].y - 1].z + pos.z);
        //glVertex3f(vrhovi[poligoni[i].z - 1].x + pos.x, vrhovi[poligoni[i].z - 1].y + pos.y, vrhovi[poligoni[i].z - 1].z + pos.z);

        //glVertex3f(vrhovi[poligoni[i].z - 1].x + pos.x, vrhovi[poligoni[i].z - 1].y + pos.y, vrhovi[poligoni[i].z - 1].z + pos.z);
        //glVertex3f(vrhovi[poligoni[i].x - 1].x + pos.x, vrhovi[poligoni[i].x - 1].y + pos.y, vrhovi[poligoni[i].x - 1].z + pos.z);

        glVertex3f(vrhovi[poligoni[i].x - 1].x, vrhovi[poligoni[i].x - 1].y, vrhovi[poligoni[i].x - 1].z);
        glVertex3f(vrhovi[poligoni[i].y - 1].x, vrhovi[poligoni[i].y - 1].y, vrhovi[poligoni[i].y - 1].z);

        glVertex3f(vrhovi[poligoni[i].y - 1].x, vrhovi[poligoni[i].y - 1].y, vrhovi[poligoni[i].y - 1].z);
        glVertex3f(vrhovi[poligoni[i].z - 1].x, vrhovi[poligoni[i].z - 1].y, vrhovi[poligoni[i].z - 1].z);

        glVertex3f(vrhovi[poligoni[i].z - 1].x, vrhovi[poligoni[i].z - 1].y, vrhovi[poligoni[i].z - 1].z);
        glVertex3f(vrhovi[poligoni[i].x - 1].x, vrhovi[poligoni[i].x - 1].y, vrhovi[poligoni[i].x - 1].z);
    }
    glEnd();

    glPopMatrix();
    // Koordinatne osi
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);  // X-axis (red)
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);

    glColor3f(0.0f, 1.0f, 0.0f);  // Y-axis (green)
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);

    glColor3f(0.0f, 0.0f, 1.0f);  // Z-axis (blue)
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glEnd();




    glPointSize(5.0);  
    glColor3f(1.0f, 0.0f, 1.0f);  

    glBegin(GL_POINTS);

    // Crtaj točke krivulje
    //segment = 0;
    printf("POCETAK");
    for (int segment = 0; segment < 8; segment++){
    	for (float t = 0.0; t <= 1.0; t += 0.01) { 
    		//vec4 r = vec4(aprox[segment], aprox[segment+1], aprox[segment+2], aprox[segment+3]);
        	BSplineResult rez = BSpline(t, segment);
        	printf("%f %f %f\n", rez.point.x, rez.point.y, rez.point.z);
        	glVertex3f(rez.point.x, rez.point.y, rez.point.z);
    	}
	}

    glEnd();
    printf("KRAJ");

    glPointSize(5.0); 
    glColor3f(0.0f, 0.0f, 1.0f);  

    glBegin(GL_POINTS);

    // Kontrolne točke
    //segment = 0;
    for (int i=0; i < 12; i++){
    	vec3 point = aprox[i];
    	//printf("Aprox:%f %f %f\n", point.x, point.y, point.z);
    	glVertex3f(point.x, point.y, point.z);
    	
	}

    glEnd();
    glPointSize(10.0);  // Set point size to make the B-spline points more visible
    glColor3f(1.0f, 0.0f, 0.0f);  

    glBegin(GL_POINTS);

    // Nacrtaj tangentu i os rotacije na trenutnoj poziciji
    //segment = 0;
    glVertex3f(pos.x, pos.y, pos.z);

    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(pos.x, pos.y, pos.z);
    glVertex3f(tangenta.x * 2.0 + pos.x, tangenta.y * 2.0 + pos.y, tangenta.z * 2.0 + pos.z);

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(pos.x, pos.y, pos.z);
    glVertex3f(os_rotacije.x * 2.0 + pos.x, os_rotacije.y * 2.0 + pos.y, os_rotacije.z * 2.0 + pos.z);
    glEnd();
    


    glutSwapBuffers();
}
void updateBSplineMotion(){
	Global_t += 0.001;

	if(Global_t >= 1.0){
		Global_t = 0.0;
		currentSegment += 1;
	}
	if(currentSegment == 8) currentSegment = 0;

	BSplineResult rez = BSpline(Global_t, currentSegment);
	pos.x = rez.point.x;
	pos.y = rez.point.y;
	pos.z = rez.point.z;
	tangenta.x = rez.derivative.x;
	tangenta.y = rez.derivative.y;
	tangenta.z = rez.derivative.z;
	tangenta = normalize(tangenta);


}
void myIdle() {
	if(animated == 1) {
		updateBSplineMotion();
    	myDisplay();
    }
}
