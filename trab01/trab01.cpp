#include <GL/glut.h>  

#include <iostream>
#include <GL/freeglut.h>
#include <vector>

struct Vector3
{
	float x, y, z;

	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3() : x(0), y(0), z(0) {}
};

using vertex = Vector3;
using color = Vector3;

class CubeFace {
public:
	std::vector<vertex> vertices;
	color faceColor;
	CubeFace(std::vector<vertex> vertices, color faceColor) {
		this->vertices = vertices;
		this->faceColor = faceColor;
	}
};

class Cube {
	std::vector<CubeFace> faces;
	vertex position;
	float size;
	double angle;
	double rotation_matrix[3][3];

public:
	Cube(float size, vertex position);
	void draw();
	void rotate(double angle, vertex axis);
	void translate(vertex position);
};

void display();
void idle();
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);

Cube cube = Cube(3.0, Vector3(0.0f, 0.0f, 0.0f));

int delay = 10;

void initGL() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Coloca a cor de background para preto e opaco
	glClearDepth(1.0f);                   // Define o buffer de profundidade para o mais distante possível
	//glOrtho(0, 256, 0, 256, -1, 1);
	glEnable(GL_DEPTH_TEST);   // Habilita o culling de profundidade
	//glDepthFunc(GL_LEQUAL);    // Define o tipo de teste de profundidade
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -15.0f);
	cube.translate(vertex(-6.0, -5.0, 0));
	cube.draw();

	glutSwapBuffers();
}


//Função de redesenhou prioriza o aspecto da projeção
void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 2.0, 50.0);
	glMatrixMode(GL_MODELVIEW);
}


int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);


	glutCreateWindow("3D Shapes");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	initGL();
	glutMainLoop();
	return 0;
}

Cube::Cube(float size, vertex position)
{
	this->size = size;
	this->angle = 0;
	this->position = position;

	float angle = 0;
	float passo_angulo = ((360 / float(4)) * 3.1416) / 180.0;

	float apothem = size / (2 * tan(3.1416 / float(4)));
	position.x -= size / 2.0;
	position.y -= apothem;
	
	//front
	std::vector<vertex> front_vertices;
	front_vertices.push_back(vertex(position.x, position.y, position.z));
	front_vertices.push_back(vertex(position.x + size, position.y, position.z));
	front_vertices.push_back(vertex(position.x + size, position.y + apothem, position.z));
	front_vertices.push_back(vertex(position.x, position.y + apothem, position.z));
	faces.push_back(CubeFace(front_vertices, color(1.0, 0.0, 0.0)));
	
	//back
	std::vector<vertex> back_vertices;
	back_vertices.push_back(vertex(position.x, position.y, position.z + size));
	back_vertices.push_back(vertex(position.x + size, position.y, position.z + size));
	back_vertices.push_back(vertex(position.x + size, position.y + apothem, position.z + size));
	back_vertices.push_back(vertex(position.x, position.y + apothem, position.z + size));
	faces.push_back(CubeFace(back_vertices, color(0.0, 1.0, 0.0)));
	
	//left
	std::vector<vertex> left_vertices;
	left_vertices.push_back(vertex(position.x, position.y, position.z));
	left_vertices.push_back(vertex(position.x, position.y, position.z + size));
	left_vertices.push_back(vertex(position.x, position.y + apothem, position.z + size));
	left_vertices.push_back(vertex(position.x, position.y + apothem, position.z));
	faces.push_back(CubeFace(left_vertices, color(0.0, 0.0, 1.0)));
	
	//right
	std::vector<vertex> right_vertices;
	right_vertices.push_back(vertex(position.x + size, position.y, position.z));
	right_vertices.push_back(vertex(position.x + size, position.y, position.z + size));
	right_vertices.push_back(vertex(position.x + size, position.y + apothem, position.z + size));
	right_vertices.push_back(vertex(position.x + size, position.y + apothem, position.z));
	faces.push_back(CubeFace(right_vertices, color(1.0, 1.0, 0.0)));
	
	//top
	std::vector<vertex> top_vertices;
	top_vertices.push_back(vertex(position.x, position.y + apothem, position.z));
	top_vertices.push_back(vertex(position.x + size, position.y + apothem, position.z));
	top_vertices.push_back(vertex(position.x + size, position.y + apothem, position.z + size));
	top_vertices.push_back(vertex(position.x, position.y + apothem, position.z + size));
	faces.push_back(CubeFace(top_vertices, color(1.0, 0.0, 1.0)));
	
	//bottom
	std::vector<vertex> bottom_vertices;
	bottom_vertices.push_back(vertex(position.x, position.y, position.z));
	bottom_vertices.push_back(vertex(position.x + size, position.y, position.z));
	bottom_vertices.push_back(vertex(position.x + size, position.y, position.z + size));
	bottom_vertices.push_back(vertex(position.x, position.y, position.z + size));
	faces.push_back(CubeFace(bottom_vertices, color(0.0, 1.0, 1.0)));
	
	
}

void Cube::draw()
{
	glTranslatef(0.0, 0.0, -5.0);

	// draw each face of cube
	for (auto face : this->faces)
	{
		glColor3f(face.faceColor.x,face.faceColor.y, face.faceColor.z);
		glBegin(GL_QUADS);
		for (auto vertex : face.vertices)
		{
			glVertex3f(vertex.x, vertex.y, vertex.z);
		}
		glEnd();
	}
}

void Cube::rotate(double angle, vertex axis)
{
	// rotate cube without glRotatef
	
	
}

void Cube::translate(vertex position)
{
	vertex newPosition;
	newPosition.x = this->position.x + position.x * cos(cube.angle);
	newPosition.y = this->position.y + position.y * cos(cube.angle);
	newPosition.z = this->position.z + position.z * cos(cube.angle);
	
	for (auto& face : this->faces)
	{
		for (auto& vertex : face.vertices)
		{
			vertex.x += position.x;
			vertex.y += position.y;
			vertex.z += position.z;
		}
	}

}
