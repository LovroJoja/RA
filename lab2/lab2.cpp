#include <vector>
#include <math.h>
#include <GL/glut.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <cmath>
#include <fstream>
#include<iostream>

using namespace std;
using namespace glm;
#define PI 3.14159265

GLuint LoadTexture(const char * filename, int wrap);
void myIdle();
void draw_window();
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
void myDisplay();
void myReshape(int w, int h);
void myMouse(int button, int state, int x, int y);


GLuint window;
GLuint width = 1080;
GLuint height = 720;
const float delta = 0.1f;
int N_v;
int N_f;
vec3* aprox;
vec3* vrhovi;
ivec3* poligoni;
vec4* ravnine;
bool test_unutar = true;
float Global_t = 0.0;
int currentSegment = 0;
vec3 ociste;
vec3 glediste;
vec3 viewUp;

class Source{
public:
	float x, y, z;
	double size;
	float Red, Green, Blue;
	int card;
};

class Particle{
public:
	float x, y, z;
	float Red, Green, Blue;
	float v;
	int t;
	float dirX, dirY, dirZ;
	float aX, aY, aZ;
	double angle;
	double size;
};
float speed = 0.8;
void drawParticles();
void drawParticle(Particle p);
vector<Particle> particles;
Source source;
Source source2;
double maxH = 42.0;
int tempQ = -1;
GLuint tex;
int main(int argc, char ** argv)
{	
	
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

	source.x = 0.0;
	source.y = 0.0;
	source.z = 0.0;
	source.size = 0.4;
	source.Red = 1.0;
	source.Green = 0.0;
	source.Blue = 0.0;
	source.card = 10;

	source2.x = 0.0;
	source2.y = 0.0;
	source2.z = 0.0;
	source2.size = 0.4;
	source2.Red = 0.0;
	source2.Green = 0.0;
	source2.Blue = 1.0;
	source2.card = 10;
	srand (time(NULL));

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);

	window = glutCreateWindow("Glut OpenGL Sustav Cestica");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMouseFunc(myMouse);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);

	tex = LoadTexture("C:\\IRG\\IRGPrimjeri\\cestica.bmp", 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	
	
	
	glutMainLoop();
	return 0;
}


//void myDisplay(){
//	draw_window();
//	glFlush();
//}

void myMouse(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		//printf("ispilon %d\n", y);
		source.x = x;
		source.y = y;
	}
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

	if(theKey == '+') source.card += 1;
	if(theKey == '-') {
		if(source.card > 0) source.card -= 1;
	}

	if(theKey == '6') source.x -= delta;
	if(theKey == '4') source.x += delta;
	if(theKey == '8') source.y += delta;
	if(theKey == '5') source.y -= delta;
	if(theKey == '9') source.z += delta;
	if(theKey == '7') source.z -= delta;

	if(theKey == '1') speed -= 0.1;
	if(theKey == '2') speed += 0.1;
	myDisplay();
	glFlush();

}

int t = 0;
void myDisplay() {
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set the background color to white
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (GLfloat)width / (GLfloat)height, 0.1, 100.0); 

    // Set up the view matrix
    gluLookAt(ociste.x, ociste.y, ociste.z, glediste.x, glediste.y, glediste.z, viewUp.x, viewUp.y, viewUp.z);

    drawParticles();
    glutSwapBuffers();
}
int currentTime = 0;
int previousTime = 0;


void myIdle(){
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int delta = currentTime - previousTime;

	if(delta > 100){
		if(source.card > 0){
			int n = rand() % source.card + 1;
			for (int i = 0; i< n; i++){
				vec3 dir = vec3(rand() % 2000 - 1000 / (double)1000, rand() % 2000 - 1000 / (double)1000, rand() % 2000 - 1000 / (double)1000);
				dir = normalize(dir);
				Particle particle;
				Particle particle2;

				particle.x = source.x;
				particle.z = source.y;
				particle.y = source.z;
				
				particle2.x = source2.x;
				particle2.z = source2.y;
				particle2.y = source2.z;  

				particle.Red = source.Red;
				particle.Green = source.Green;
				particle.Blue = source.Blue;
				
				particle2.Red = source2.Red;
				particle2.Green = source2.Green;
				particle2.Blue = source2.Blue;

				particle.dirX = dir.x;
				particle.dirY = dir.y;
				particle.dirZ = dir.z;
				
				particle2.dirX = dir.x;
				particle2.dirY = dir.y;
				particle2.dirZ = dir.z;

				particle.v = speed;
				
				particle.t = 50 + (rand()%36 - 10);
				particle.size = source.size;
				particles.push_back(particle);

				
				particle2.v = speed;
				particle2.t = 50 + (rand()%36 - 10);
				particle2.size = source2.size;
				particles.push_back(particle2);
		}
		}
		for(int i = 0; i < particles.size(); i++){
			vec3 s = vec3(0.0, 0.0, 1.0);
			vec3 axis = vec3(0.0, 0.0, 0.0);
			vec3 e = vec3(0.0, 0.0, 0.0);

			e.x = particles.at(i).x - ociste.x;
			e.y = particles.at(i).y - ociste.y;
			e.z = particles.at(i).z - ociste.z;
			e = normalize(e);
			s = normalize(s);
			axis.x = s.y * e.z - e.y * s.z;
			axis.y = s.x * e.z - e.x * s.z;
			axis.z = s.x * e.y - e.y * s.x;
			//axis = normalize(axis);
			
			double se = dot(s, e);
			double angle = acos(se);
			particles.at(i).angle = angle / (2 * PI) * 360;
			particles.at(i).aX = axis.x;
			particles.at(i).aY = axis.y;
			particles.at(i).aZ = axis.z;

			particles.at(i).x += particles.at(i).v * particles.at(i).dirX;
			particles.at(i).y += particles.at(i).v * particles.at(i).dirY;
			particles.at(i).z += particles.at(i).v * particles.at(i).dirZ;

			particles.at(i).t--;

			if(particles.at(i).Red > 0) particles.at(i).Red -= 0.01;
			if(particles.at(i).Green < 1.0) particles.at(i).Green += 0.01;
			if(particles.at(i).Blue < 1.0) particles.at(i).Blue += 0.01;


			if(particles.at(i).t <= 0){
				particles.erase(particles.begin() + i);
			}

		}
		printf("Broj cestica: %d\n", particles.size());
		previousTime = currentTime;
		myDisplay();
	}
}




GLuint LoadTexture(const char * filename, int wrap){
	GLuint texture;
	int tex_width, tex_height;
	BYTE * data;
	FILE * file;

	file = fopen(filename, "rb");
	if (file == NULL){
		printf("Could not load texture - Opening the texture file failed.");
		return 0;
	}

	tex_width = 256;
	tex_height = 256;
	data = (BYTE*) malloc(tex_width * tex_height * 3);

	fread(data, tex_width * tex_height * 3, 1, file);
	fclose(file);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, tex_width, tex_height, GL_RGB, GL_UNSIGNED_BYTE, data);
	free(data);

	return texture;
}

void drawParticles(){
	for (int p = 0; p < particles.size(); p++){
		drawParticle(particles.at(p));
	}
}

void drawParticle(Particle p){
	glColor3f(p.Red, p.Green, p.Blue);
	glTranslatef(p.x, p.y, p.z);
	glRotatef(p.angle, p.aX, p.aY, p.aZ);
	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 0.0);
	glVertex3f(-p.size, -p.size, 0.0);

	glTexCoord2d(1.0, 0.0);
	glVertex3f(+p.size, -p.size, 0.0);

	glTexCoord2d(1.0, 1.0);
	glVertex3f(+p.size, +p.size, 0.0);

	glTexCoord2d(0.0, 1.0);
	glVertex3f(-p.size, +p.size, 0.0);

	glEnd();

	glRotatef(-p.angle, p.aX, p.aY, p.aZ);
	glTranslatef(-p.x, -p.y, -p.z);
}