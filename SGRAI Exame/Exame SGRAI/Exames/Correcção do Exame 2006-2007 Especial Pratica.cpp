#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define RAD(x) (M_PI*(x)/180)
#define GRAUS(x) (180*(x)/M_PI)

#define DEBUG 1

/* VARIAVEIS GLOBAIS */

typedef struct {
	GLboolean doubleBuffer;
	GLint delayMovimento;
	GLUquadricObj *qobj;
} Estado;

typedef struct {
	GLfloat angh, angv, dist;
} Camara;

typedef struct {
	GLfloat amb[4], diff[4], spec[4], pos[4];
} Luz;

typedef struct {
	GLfloat amb[4], diff[4], spec[4], shine[1];
} Material;

typedef struct {
	GLfloat largura, altura;
} Dente;

typedef struct {
	GLint nSlices, nLoops, nStacks;
	GLfloat raio, espessura;
	GLint nDentes;
	Dente dente;
} Cremalheira;

typedef struct {
	GLfloat comprimento, largura, espessura;
} Cranque;

typedef struct {
	GLfloat comprimento, largura, altura;
} Pedal;

typedef struct {
	Cremalheira cremalheira;
	Cranque cranque;
	Pedal pedal;
	GLfloat aRotacao, vRotacao;
} Pedaleira;

Estado estado = {
	GL_TRUE, 50
};
Camara camara = {
	-45.0, 45.0, 200.0
};
Luz luz = {
	{1.0, 1.0, 1.0, 1.0},
	{1.0, 1.0, 1.0, 1.0},
	{1.0, 1.0, 1.0, 1.0},
	{1.0, 1.0, 1.0, 0.0}, /* Fonte de luz direccional */
};
Material material = {
	{0.1, 0.1, 0.1, 1.0},
	{0.6, 0.6, 0.6, 1.0},
	{0.4, 0.4, 0.4, 1.0},
	{100.0}
};
Pedaleira pedaleira = {
	32, 1, 1, 37.5, 3.0, 24, {5.0, 2.5}, /* Cremalheira */
	75.0, 5.0, 3.0, /* Cranque */
	40.0, 20.0, 10.0,
	0.0, 0.0 /* Ângulo e velocidade de rotação */
};

/* Inicialização do ambiente OPENGL */
void Init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE); /* por causa da utilização de glScalef... */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, luz.amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luz.diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, luz.spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material.amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material.diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material.spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material.shine);
	estado.qobj = gluNewQuadric();
	gluQuadricDrawStyle(estado.qobj, GLU_FILL);
	gluQuadricNormals(estado.qobj, GLU_SMOOTH);
}

/* CALLBACK PARA REDIMENSIONAR JANELA */
void Reshape(int width, int height)
{
	glViewport(0, 0, (GLint) width, (GLint) height);  
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble) width / (GLdouble) height, 10.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* ... definicao das rotinas auxiliares de desenho ... */
void desenha_cremalheira(void)
{
	int i;

	glPushMatrix();
		glTranslatef(0.0, 0.0, -pedaleira.cremalheira.espessura / 2.0);
		glRotatef(180.0, 1.0, 0.0, 0.0);
		gluDisk(estado.qobj, 0.0, pedaleira.cremalheira.raio, pedaleira.cremalheira.nSlices, pedaleira.cremalheira.nLoops);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0.0, 0.0, -pedaleira.cremalheira.espessura / 2.0);
		gluCylinder(estado.qobj, pedaleira.cremalheira.raio, pedaleira.cremalheira.raio, pedaleira.cremalheira.espessura, pedaleira.cremalheira.nSlices, pedaleira.cremalheira.nStacks);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0.0, 0.0, pedaleira.cremalheira.espessura / 2.0);
		gluDisk(estado.qobj, 0.0, pedaleira.cremalheira.raio, pedaleira.cremalheira.nSlices, pedaleira.cremalheira.nLoops);
	glPopMatrix();
	for (i = 0; i < pedaleira.cremalheira.nDentes; i++)
	{
		glPushMatrix();
			glRotatef(360.0 * (GLfloat) i / (GLfloat) pedaleira.cremalheira.nDentes, 0.0, 0.0, 1.0);
			glTranslatef(pedaleira.cremalheira.raio, 0.0, 0.0);
			glScalef(2.0 * pedaleira.cremalheira.dente.altura, pedaleira.cremalheira.dente.largura, pedaleira.cremalheira.espessura);
			glutSolidCube(1.0);
		glPopMatrix();
	}
}

void desenha_cranque_esquerdo(void)
{
	glPushMatrix();
		glTranslatef(0.0, -pedaleira.cranque.comprimento / 2.0 + pedaleira.cranque.largura / 2.0, pedaleira.cremalheira.espessura / 2.0 + pedaleira.cranque.espessura / 2.0);
		glScalef(pedaleira.cranque.largura, pedaleira.cranque.comprimento, pedaleira.cranque.espessura);
		glutSolidCube(1.0);
	glPopMatrix();
}

void desenha_cranque_direito(void)
{
	glPushMatrix();
		glTranslatef(0.0, pedaleira.cranque.comprimento / 2.0 - pedaleira.cranque.largura / 2.0, -pedaleira.cremalheira.espessura / 2.0 - pedaleira.cranque.espessura / 2.0);
		glScalef(pedaleira.cranque.largura, pedaleira.cranque.comprimento, pedaleira.cranque.espessura);
		glutSolidCube(1.0);
	glPopMatrix();
}

void desenha_pedal_esquerdo(void)
{
	glPushMatrix();
		glTranslatef(0.0, -pedaleira.cranque.comprimento + pedaleira.cranque.largura / 2.0, pedaleira.cremalheira.espessura / 2.0 + pedaleira.cranque.espessura + pedaleira.pedal.comprimento / 2.0);
		glRotatef(-pedaleira.aRotacao, 0.0, 0.0, 1.0);
		glScalef(pedaleira.pedal.largura, pedaleira.pedal.altura, pedaleira.pedal.comprimento);
		glutSolidCube(1.0);
	glPopMatrix();
}

void desenha_pedal_direito(void)
{
	glPushMatrix();
		glTranslatef(0.0, pedaleira.cranque.comprimento - pedaleira.cranque.largura / 2.0, -pedaleira.cremalheira.espessura / 2.0 - pedaleira.cranque.espessura - pedaleira.pedal.comprimento / 2.0);
		glRotatef(-pedaleira.aRotacao, 0.0, 0.0, 1.0);
		glScalef(pedaleira.pedal.largura, pedaleira.pedal.altura, pedaleira.pedal.comprimento);
		glutSolidCube(1.0);
	glPopMatrix();
}

void desenha_pedaleira(void)
{
	glPushMatrix();
		glRotatef(pedaleira.aRotacao, 0.0, 0.0, 1.0);
		desenha_cremalheira();
		desenha_cranque_esquerdo();
		desenha_cranque_direito();
		desenha_pedal_esquerdo();
		desenha_pedal_direito();
	glPopMatrix();
}

/* Callback de desenho */
void Draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLightfv(GL_LIGHT0, GL_POSITION, luz.pos);
/* ... chamada das rotinas auxiliares de desenho ... */
	glPushMatrix();
		glTranslatef(0.0, 0.0, -camara.dist);
		glRotatef(camara.angv, 1.0, 0.0, 0.0);
		glRotatef(camara.angh, 0.0, 1.0, 0.0);
		desenha_pedaleira();
	glPopMatrix();
	if (estado.doubleBuffer)
		glutSwapBuffers();
	else
		glFlush();
}

/* Callbacks para interaccao via teclado */
void Key(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
			exit(0);
			break;
		case 'q':
			pedaleira.vRotacao += 1.0;
			break;
		case 'w':
			pedaleira.vRotacao -= 1.0;
			break;
	}
}

void SpecialKey(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_LEFT:
			camara.angh += 5.0;
			break;
		case GLUT_KEY_RIGHT:
			camara.angh -= 5.0;
			break;
		case GLUT_KEY_DOWN:
			camara.angv += 5.0;
			break;
		case GLUT_KEY_UP:
			camara.angv -= 5.0;
			break;
		case GLUT_KEY_PAGE_DOWN:
			camara.dist *= 1.1;
			break;
		case GLUT_KEY_PAGE_UP:
			camara.dist /= 1.1;
			break;
	}
}

/* Callback de temporizador */
void Timer(int value)
{
	glutTimerFunc(estado.delayMovimento, Timer, 0);
/* ... accoes do temporizador ... */
		pedaleira.aRotacao += pedaleira.vRotacao;
		while (pedaleira.aRotacao <= -180.0)
			pedaleira.aRotacao += 360.0;
		while (pedaleira.aRotacao > 180.0)
			pedaleira.aRotacao -= 360.0;
/* redesenhar o ecra */
	glutPostRedisplay();
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(400, 400);
	glutInitDisplayMode((estado.doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	if (glutCreateWindow("Pedaleira") == GL_FALSE)
		exit(1);
	Init();
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Draw);
	glutKeyboardFunc(Key);
	glutSpecialFunc(SpecialKey);
	glutTimerFunc(estado.delayMovimento, Timer, 0);
	glutMainLoop();
}
