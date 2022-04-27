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

std::vector < std::vector <ivec3> > faces;
unsigned int _model;
float rot;

vec3 diferrence(vec3 a, vec3 b) {
    return vec3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

vec3 sum(vec3 a, vec3 b) {
    return vec3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

void calcNormals() {
    for (int i = 0; i < faces.size(); i++) {
        vector<ivec3> face = faces[i];
        vec3 v1 = vertices[face[0].v];
        vec3 v2 = vertices[face[1].v];
        vec3 v3 = vertices[face[2].v];
        vec3 edge = diferrence(v2, v1);
        vec3 edge2 = diferrence(v3, v1);
        vec3 normal = vec3{ edge.y * edge2.z - edge.z * edge2.y, edge.z * edge2.x - edge.x * edge2.z, edge.x * edge2.y - edge.y * edge2.x };
        normal = vec3{ normal.x / sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z), normal.y / sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z), normal.z / sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z) };
		
        //cout << "face :" << v1.x << ", " << v1.y << ", " << v1.z <<"; "<< v2.x << ", " << v2.y << ", " << v2.z <<"; "<< v3.x << ", " << v3.y << ", " << v3.z << endl;
        for (int j = 0; j < face.size(); j++) {
            ivec3 vertex = face[j];
          /*  cout << vertex.n << endl;
            cout<<"normal: "<<normals[vertex.n].x << ", " << normals[vertex.n].y << ", " << normals[vertex.n].z << endl;*/
        }
    }
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
    cout << "Loaded" << endl;
	
    //calcNormals();

    cout << "normals calculated" << endl;
    glNewList(_model, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < faces.size(); i++) {
            vec3 v1 = vertices[faces[i][0].v];
			vec3 v2 = vertices[faces[i][1].v];
			vec3 v3 = vertices[faces[i][2].v];
			glVertex3f(v1.x, v1.y, v1.z);
			glVertex3f(v2.x, v2.y, v2.z);
			glVertex3f(v3.x, v3.y, v3.z);
        }
        glEnd();
		glPopMatrix();
    }
    glEndList();
	cout<< "list created" << endl;
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
    glClear(GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(0, -40.00, -105);
    glColor3f(1.0, 0.23, 0.27);
    glScalef(0.4, 0.4, 0.4);
    glRotatef(rot, 0, 1, 0);
    glCallList(_model);
    glPopMatrix();
    rot = rot + 0.6;
    if (rot > 360) rot = rot - 360;
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

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glutInitWindowSize(800, 450);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Carregar OBJ");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutTimerFunc(10, timer, 0);
    loadObj("data/mba1.obj");
    glutMainLoop();
    return 0;
}