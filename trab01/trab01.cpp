/*
author: Eduardo Loback, Enzo Redivo

TODO:
    * camera movement.
	* texture load.
    * other lights
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <GL/freeglut.h>
#include <string>
using namespace std;

struct vec3 {
    float x, y, z;
};

struct vec2 {
    float x, y;
};

struct ivec3 {
    int v, t, n;
};

//globals
std::vector <vec3> vertices;
std::vector <vec2> textCoords;
std::vector <vec3> normals;
float scale = 0.4;
vec3 mov{ 0, -20, -105 };
vec3 cam{ 0, 0, 0 };
vec3 vr{ 0, 1, 0 };
enum mode { perpective=0, movement=1, rotation=2 };
mode actual = movement;
bool rotating = true;

std::vector < std::vector <ivec3> > faces;
unsigned int _model;
float rot;

vec3 diferrence(vec3 a, vec3 b) {
    return vec3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

vec3 sum(vec3 a, vec3 b) {
    return vec3{ a.x + b.x, a.y + b.y, a.z + b.z };
}


void calcNormals();
std::vector<string> split(string str, char delimiter);
void createFace(string line);
vec2 parseVector2(string line);
vec3 parseVector3(string line);
void createList();
void loadObj(std::string fileName);
void reshape(int w, int h);
void draw();
void display(void);
void timer(int value);
void Initialize();

void keyboard(unsigned char key, int x, int y);

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 450);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Carregar OBJ");
    Initialize();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(10, timer, 0);
    loadObj("data/mba1.obj");
    glutMainLoop();
    return 0;
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 3000.0);
    glMatrixMode(GL_MODELVIEW);
}
void draw()
{	
    glPushMatrix();
    glTranslatef(mov.x, mov.y, mov.z);
    glColor3f(1.0, 0.23, 0.27);
    glScalef(scale, scale, 0.4);
    if (!(vr.x == 0 && vr.y == 0 && vr.z == 0)) 
    {
        glRotatef(rot, vr.x, vr.y, vr.z);
        if(rotating) rot = rot + 0.6;
        if (rot > 360) rot = rot - 360;
    }
    //gluLookAt(0, 0, 0, 0 + cam.x, 0 + cam.y, 100 + cam.z, 0, 40, 0
    glCallList(_model);
    glPopMatrix();
    glEnd();

}
void display(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    draw();
    glutSwapBuffers();
}

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}

void Initialize() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    //Ativa o uso de luz
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    

    // determina a intensidade e cor da luz
    //GLfloat lightAmbient[] = { 0.2, 0.2, 0.2, 1.0 }; // cinza
    //GLfloat lightDiffuse[] = { 0.8, 0.8, 0.8, 1.0 }; // cinza claro
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 0.0f, 1.0f };    //yellow diffuse : color where light hit directly the object's surface
    GLfloat lightAmbient[] = { 1.0f, 0.0f, 0.0f, 1.0f };    //red ambient : color applied everywhere
    GLfloat luz_especular[] = { 1.0, 1.0, 1.0, 1.0 }; // branco

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luz_especular);

    GLfloat l1LightAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat l1LightDiffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat l1LightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat l1LightPosition[] = { -2.0, 2.0, 1.0, 1.0 };
	GLfloat l1LightDirection[] = { -1.0, -1.0, 0.0 };
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, l1LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1LightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1LightSpecular);
	glLightfv(GL_LIGHT1, GL_POSITION, l1LightPosition);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l1LightDirection);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0.0);
	

    //determina a posição da luz
    GLfloat posicao_luz[] = { .5, .5, 0.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, posicao_luz);
    

    
}



std::vector<string> split(string str, char delimiter) {
    std::vector<string> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;

    while (getline(ss, tok, delimiter)) {
        // evades delimiter if it is used in sequence
        if (tok.size() > 0) {
            internal.push_back(tok);
        }
    }

    return internal;
}

void createFace(string line) {
    //std::cout << line << endl;
    std::vector<std::string> tokens = split(line, ' ');
    const int size = tokens.size();
    vector<ivec3> face;
    for (int i = 1; i < size; i++) {
        //std::cout << tokens[i] << endl;
        std::vector<std::string> sVertex = split(tokens[i], '/');
        int sVertexSize = sVertex.size();
        if (sVertexSize == 2) {
            face.push_back(ivec3{ stoi(sVertex[0]) - 1, 0, stoi(sVertex[1]) - 1 });
        }
        else face.push_back(ivec3{ stoi(sVertex[0]) - 1, stoi(sVertex[1]) - 1, stoi(sVertex[2]) - 1 });
        //std::cout << "vertex: " << face[i-1].v << ", "<<face[i-1].t<<", " <<face[i - 1].n << endl;
    }
    //std::cout << endl;
    faces.push_back(face);
}


vec2 parseVector2(string line) {
    std::vector<std::string> tokens = split(line, ' ');
    return vec2{ stof(tokens[1]), stof(tokens[2]) };
}

vec3 parseVector3(string line) {
    std::vector<std::string> tokens = split(line, ' ');
    return vec3{ stof(tokens[1]), stof(tokens[2]), stof(tokens[3]) };
}



void createList() {
    glNewList(_model, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < faces.size(); i++) {
            for (int j = 0; j < 3; j++) {
				vec3 vertex = vertices[faces[i][j].v];
				vec3 normal = normals[faces[i][j].n];
				glVertex3f(vertex.x, vertex.y, vertex.z);
                glNormal3f(normal.x, normal.y, normal.z);

				//texture can not 
                if (faces[i][j].t != 0) {
                    vec2 texture = textCoords[faces[i][j].t];
                    glTexCoord2f(texture.x, texture.y);
                }
			}
        }
        glEnd();
        glPopMatrix();
    }
    glEndList();
}


void loadObj(std::string fileName) {
    _model = glGenLists(1);
    glPointSize(2.0);
    std::ifstream file;
    file.open(fileName.c_str());

    std::string line;
    if (file.is_open())
    {
        while (file.good())
        {
            getline(file, line);

            unsigned int lineLength = line.length();

            //cout << line << endl;

            if (lineLength < 2)
                continue;

            const char* lineCStr = line.c_str();
            switch (lineCStr[0])
            {
            case 'v':
                if (lineCStr[1] == 't')
                    textCoords.push_back(parseVector2(line));
                else if (lineCStr[1] == 'n') {
                    normals.push_back(parseVector3(line));
                }
                else if (lineCStr[1] == ' ' || lineCStr[1] == '\t')
                    vertices.push_back(parseVector3(line));
                break;
            case 'f':
                createFace(line);
                break;
            default: break;
            };
        }
    }
    else
    {
        std::cerr << "Unable to load mesh: " << fileName << std::endl;
    }

    cout << "normals calculated" << endl;
    createList();
    cout << "list created" << endl;
}


void keyboard(unsigned char key, int x, int y) {
    //std::cout << key;
    switch (key) {
    case 27:
        exit(0);
        break;
    case 's':
        if (actual == mode(0)) cam.y = cam.y - 10;
        else if(actual == mode(1)) mov.y = mov.y - 10;
        else if (actual == mode(2)) vr.y = vr.y > 1 ? vr.y : vr.y + 0.1;
        break;
    case 'w':
        if (actual == mode(0)) cam.y = cam.y + 10;
        else if (actual == mode(1)) mov.y = mov.y + 10;
		else if (actual == mode(2)) vr.y = vr.y < -1 ? vr.y : vr.y - 0.1;
        break;
    case 'a':
        if (actual == mode(0)) cam.x = cam.x - 10;
        else if (actual == mode(1)) mov.x = mov.x - 10;
		else if (actual == mode(2)) vr.x = vr.x > 1 ? vr.x : vr.x + 0.1;
        break;
    case 'd':
        if (actual == mode(0)) cam.x = cam.x + 10;
        else if (actual == mode(1)) mov.x = mov.x + 10;
		else if (actual == mode(2)) vr.x = vr.x < -1 ? vr.x : vr.x - 0.1;
        break;
    case 'f':
		if (actual == mode(0)) cam.z = cam.z - 10;
		else if(actual == mode(1)) mov.z = mov.z - 10;
		else if (actual == mode(2)) vr.z = vr.z > 1 ? vr.z : vr.z + 0.1;
		break;
    case 'g':
		if (actual == mode(0)) cam.z = cam.z + 10;
		else if (actual == mode(1)) mov.z = mov.z + 10;
		else if (actual == mode(2)) vr.z = vr.z < -1 ? vr.z : vr.z - 0.1;
		break;
    case 'q':
        if (actual == mode(0)) actual = mode(1);
        else if (actual == mode(1)) actual = mode(2);
		else actual = mode(1);
        cout <<"actual mode is "<< actual << endl;
        break;
    case 'r':
		rotating = !rotating;
        break;
    case ' ':
        scale = scale + 0.2;
        break;
    }
}
