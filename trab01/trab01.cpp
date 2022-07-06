/*
author: Eduardo Loback, Enzo Redivo
repo: https://github.com/eloback/comp-grafica-T1


TODO:
    * camera movement.
    * texture load.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glew.h>
#include <GL/freeglut.h>    
#include <string>
#include <chrono>

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

struct face {
    vector<ivec3> vertices;
    string material;
    GLuint vao;
};

struct geometry {
    vector<vec3> vertices;
    vector<vec3> normals;
    vector<vec2> texcoords;
    GLuint verticesBuffer;
    GLuint normalsBuffer;
    GLuint texcoordsBuffer;
    string material;
    GLuint vao;
};

struct ObjMaterial {
    string name;
    float shininess;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float transparency;
    float illum;
    vec3 emissive;
    float dissolve;
    string ambientMap;
};

//globals
std::vector <vec3> vertices;
std::vector <vec2> textCoords;
std::vector <vec3> normals;
std::vector <ObjMaterial> materials;
// float scale = 0.4;
float scale = 0.4;
vec3 mov{ 0, -20, -105 };
vec3 cam{ 0, 0, 0 };
vec3 vr{ 0, 1, 0 };
enum mode { perpective = 0, movement = 1, rotation = 2, lightDir = 3, lightPos = 4 };
mode actual = movement;
bool rotating = true;
bool spotlight = false;
bool activeLights[3] = { true, false, false };
GLfloat _light_position[4];
GLfloat _spotlight_position[3];

GLuint v, f; // vertex shader and fragment shader
GLuint p;

std::vector < face > faces;
string currentMaterial = "default";
ObjMaterial defaultMaterial = { "default", 0, { 0.2, 0.2, 0.2 }, { 0.8, 0.2, 0.2 }, { 0.8, 0.2, 0.2 }, 0, 2, { 0, 0, 0 }, 1, "default" };

vector<geometry> geometries;

unsigned int _model;
float rot;

vec3 diferrence(vec3 a, vec3 b) {
    return vec3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

vec3 sum(vec3 a, vec3 b) {
    return vec3{ a.x + b.x, a.y + b.y, a.z + b.z };
}


std::vector<string> split(string str, char delimiter);
void createFace(string line);
vec2 parseVector2(string line);
vec3 parseVector3(string line);
void loadObj(std::string fileName);
void loadMtl(std::string fileName);
void reshape(int w, int h);
void createVAOFromBufferInfo();
vector<geometry> convertFacesIntoGeometry();
void setShaders();
void draw();
void light();
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
    loadObj("data/chair.obj"); // filename
    geometries = convertFacesIntoGeometry();
    setShaders();
    glutMainLoop();
    return 0;
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
}



char* readStringFromFile(char* fn) {

    FILE* fp;
    char* content = NULL;
    int count = 0;

    if (fn != NULL) {
        fopen_s(&fp, fn, "rt");

        if (fp != NULL) {

            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);

            if (count > 0) {
                content = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

ObjMaterial findMaterial(string name) {
    for (int i = 0; i < materials.size(); i++) {
        if (materials[i].name == name) {
            return materials[i];
        }
    }
    return defaultMaterial;
}

vector<geometry> convertFacesIntoGeometry() {
    vector<geometry> geometries;
    cout << faces.size() << endl;
    for (int i = 0; i < faces.size(); i++) {
        geometry g;
        g.material = faces[i].material;
        for (int j = 0; j < faces[i].vertices.size(); j++) {
            g.vertices.push_back(vertices[faces[i].vertices[j].v]);
            g.normals.push_back(normals[faces[i].vertices[j].n]);
            if (faces[i].vertices[j].t != 0)g.texcoords.push_back(textCoords[faces[i].vertices[j].t]);
        }
        geometries.push_back(g);
    }
    return geometries;
}

void createVAOForGeometries() {
    for (int i = 0; i < geometries.size(); i++) {
        glGenVertexArrays(1, &geometries[i].vao);
        glBindVertexArray(geometries[i].vao);
        glGenBuffers(1, &geometries[i].verticesBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, geometries[i].verticesBuffer);
        glBufferData(GL_ARRAY_BUFFER, geometries[i].vertices.size() * sizeof(vec3), &geometries[i].vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        glGenBuffers(1, &geometries[i].normalsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, geometries[i].normalsBuffer);
        glBufferData(GL_ARRAY_BUFFER, geometries[i].normals.size() * sizeof(vec3), &geometries[i].normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        glGenBuffers(1, &geometries[i].texcoordsBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, geometries[i].texcoordsBuffer);
        glBufferData(GL_ARRAY_BUFFER, geometries[i].texcoords.size() * sizeof(vec2), &geometries[i].texcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(2);
    }
}
//void createVAOFromBufferInfo()
//{	
//    glGenVertexArrays(1, &VAO);
//	glBindVertexArray(VAO);
//	glGenBuffers(1, &verticesBuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, verticesBuffer);
//	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(0);
//	
//    if (textCoords.size() > 0) {
//        glGenBuffers(1, &textCoordsBuffer);
//        glBindBuffer(GL_ARRAY_BUFFER, textCoordsBuffer);
//        glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(vec2), &textCoords[0], GL_STATIC_DRAW);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
//        glEnableVertexAttribArray(1);
//    }
//	
//	glGenBuffers(1, &normalsBuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
//	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
//	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(2);
//	glBindVertexArray(0);
//
//}

void setShaders()
{
    cout << "setting shaders" << endl;
    /*

layout (location = 0) in vec3 entrada_pos;
layout (location = 1) in vec3 entrada_cor;
layout (location = 2) in vec3 entrada_normal;
layout (location = 3) in vec2 entrada_textura;

    */
    char* vs = NULL, * fs = NULL, * fs2 = NULL;


    glewInit();

    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);

    char vertex_shader[] = "simples.vert";
    char fragment_shader[] = "simples.frag";
    vs = readStringFromFile(vertex_shader);
    fs = readStringFromFile(fragment_shader);
    cout << "shader loaded" << endl;

    const char* vv = vs;
    const char* ff = fs;

    glShaderSource(v, 1, &vv, NULL);
    glShaderSource(f, 1, &ff, NULL);

    free(vs); free(fs);

    glCompileShader(v);
    glCompileShader(f);

    p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glLinkProgram(p);

    cout << " shader compiled and attached" << endl;

    createVAOForGeometries();

    cout << "shaders set" << endl;
}



void draw()
{
    glPushMatrix();
    glTranslatef(mov.x, mov.y, mov.z);
    glColor3f(1, 0, 0);
    glScalef(scale, scale, scale);
    if (!(vr.x == 0 && vr.y == 0 && vr.z == 0))
    {
        glRotatef(rot, vr.x, vr.y, vr.z);
        if (rotating) rot = rot + 0.6;
        if (rot > 360) rot = rot - 360;
    }

    //glCallList(_model);
    for (int i = 0; i < geometries.size(); i++)
    {
        glBindVertexArray(geometries[i].vao);
        glDrawArrays(GL_TRIANGLES, 0, geometries[i].vertices.size());
    }


    glPopMatrix();
    glEnd();

}
void light() {

    static bool prevSpotlight = spotlight;

    if (prevSpotlight != spotlight)
    {
        prevSpotlight = spotlight;
        if (!spotlight) printf("Entering LIGHT mode\r\n");
        else printf("Entering SPOTLIGHT mode\r\n");
    }

    for (uint8_t i = 0; i < sizeof(activeLights); i++)
    {
        if (activeLights[i]) {
            glEnable(GL_LIGHT0 + i);
            glLightfv(GL_LIGHT0 + i, GL_POSITION, _light_position);
        }
        else {
            glDisable(GL_LIGHT0 + i);
        }
    }
}


void display(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    light();
    draw();
    glFlush();
    glutSwapBuffers();
}

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}

void Initialize() {
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat diffuseLight[] = { 0.7, 0.7, 0.7, 1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glEnable(GL_LIGHT0);

    GLfloat ambientLight[] = { 0.05, 0.05, 0.05, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

    GLfloat specularLight[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat spectre[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spectre);
    glMateriali(GL_FRONT, GL_SHININESS, 128);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_LIGHTING);

    materials.push_back(defaultMaterial);
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
    face face;
    face.material = currentMaterial;
    for (int i = 1; i < size; i++) {
        //std::cout << tokens[i] << endl;
        std::vector<std::string> sVertex = split(tokens[i], '/');
        int sVertexSize = sVertex.size();
        if (sVertexSize == 2) {
            face.vertices.push_back(ivec3{ stoi(sVertex[0]) - 1, 0, stoi(sVertex[1]) - 1 });
        }
        else face.vertices.push_back(ivec3{ stoi(sVertex[0]) - 1, stoi(sVertex[1]) - 1, stoi(sVertex[2]) - 1 });
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
            case 'm':
                if (lineCStr[1] == 't') {
                    //cout << "mtllib " << line.substr(7) << endl;
                    loadMtl(line.substr(7));
                }
            case 'u':
                if (lineCStr[1] == 's') {
                    //cout << "usemtl " << line.substr(7) << endl;
                    currentMaterial = line.substr(7);
                }
                break;
            default: break;
            };
        }
    }
    else
    {
        std::cerr << "Unable to load mesh: " << fileName << std::endl;
    }

    cout << "obj parsed" << endl;
}

void loadMtl(string filename) {
    std::ifstream file;
    file.open(("data/" + filename).c_str());
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
            case 'n':
                if (lineCStr[1] == 'e' && lineCStr[2] == 'w') {
                    string name = line.substr(7);
                    ObjMaterial material;
                    material.name = name;
                    materials.push_back(material);
                }
                break;
            case 'K':
                if (lineCStr[1] == 'a') {
                    materials.back().ambient = parseVector3(line);
                }
                else if (lineCStr[1] == 'd') {
                    materials.back().diffuse = parseVector3(line);
                }
                else if (lineCStr[1] == 's') {
                    materials.back().specular = parseVector3(line);
                }
                else if (lineCStr[1] == 'e') {
                    materials.back().emissive = parseVector3(line);
                }
            case 'd':
                if (lineCStr[1] == ' ') {
                    materials.back().dissolve = stof(line.substr(2));
                }
                break; break;

            case 'T':
                if (lineCStr[1] == 'r') {
                    string value = line.substr(4);
                    materials.back().transparency = stof(value);
                }
                break;
            case 'i':
                if (lineCStr[1] == 'l') {
                    string value = line.substr(6);
                    materials.back().illum = stoi(value);
                }
                break;
            case 'm':
                if (lineCStr[1] == 'a') {
                    materials.back().ambientMap = line.substr(7);
                }
                break;
            default:
                break;
            };
        }
    }
    else
    {
        std::cerr << "Unable to load mesh: " << filename << std::endl;
    }
}


void keyboard(unsigned char key, int x, int y) {
    //std::cout << key;

    static constexpr float modifier = 0.01;

    switch (key) {
    case 27:
        exit(0);
        break;
    case 's':
        if (actual == mode(0)) cam.y = cam.y - 10;
        else if (actual == mode(1)) mov.y = mov.y - 10;
        else if (actual == mode(2)) vr.y = vr.y > 1 ? vr.y : vr.y + modifier;
        else if (actual == mode(3)) _spotlight_position[1] -= modifier;
        else if (actual == mode(4)) _light_position[1] -= modifier;
        break;
    case 'w':
        if (actual == mode(0)) cam.y = cam.y + 10;
        else if (actual == mode(1)) mov.y = mov.y + 10;
        else if (actual == mode(2)) vr.y = vr.y < -1 ? vr.y : vr.y - modifier;
        else if (actual == mode(3)) _spotlight_position[1] += modifier;
        else if (actual == mode(4)) _light_position[1] += modifier;
        break;
    case 'a':
        if (actual == mode(0)) cam.x = cam.x - 10;
        else if (actual == mode(1)) mov.x = mov.x - 10;
        else if (actual == mode(2)) vr.x = vr.x > 1 ? vr.x : vr.x + modifier;
        else if (actual == mode(3)) _spotlight_position[0] -= modifier;
        else if (actual == mode(4)) _light_position[0] -= modifier;
        break;
    case 'd':
        if (actual == mode(0)) cam.x = cam.x + 10;
        else if (actual == mode(1)) mov.x = mov.x + 10;
        else if (actual == mode(2)) vr.x = vr.x < -1 ? vr.x : vr.x - modifier;
        else if (actual == mode(3)) _spotlight_position[0] += modifier;
        else if (actual == mode(4)) _light_position[0] += modifier;
        break;
    case 'f':
        if (actual == mode(0)) cam.z = cam.z - 10;
        else if (actual == mode(1)) mov.z = mov.z - 10;
        else if (actual == mode(2)) vr.z = vr.z > 1 ? vr.z : vr.z + modifier;
        else if (actual == mode(3)) _spotlight_position[2] -= modifier;
        else if (actual == mode(4)) _light_position[2] -= modifier;
        break;
    case 'g':
        if (actual == mode(0)) cam.z = cam.z + 10;
        else if (actual == mode(1)) mov.z = mov.z + 10;
        else if (actual == mode(2)) vr.z = vr.z < -1 ? vr.z : vr.z - modifier;
        else if (actual == mode(3)) _spotlight_position[2] += modifier;
        else if (actual == mode(4)) _light_position[2] += modifier;
        break;
    case 'k':
        if (actual == mode(4)) _light_position[3] -= modifier;
        break;
    case 'i':
        if (actual == mode(4)) _light_position[3] += modifier;
        break;
    case 'q':
        if (actual == mode(0)) actual = mode(1);
        else if (actual == mode(1)) actual = mode(2);
        else if (actual == mode(2)) actual = mode(3);
        else if (actual == mode(3)) actual = mode(4);
        else actual = mode(1);
        cout << "actual mode is " << actual << endl;
        break;
    case 'r':
        rotating = !rotating;
        break;
    case '1':
        activeLights[0] = !activeLights[0];
        break;
    case '2':
        activeLights[1] = !activeLights[1];
        break;
    case '3':
        activeLights[2] = !activeLights[2];
        break;
    case ' ':
        scale = scale + 0.2;
        break;
    }
    if (key == '1' || key == '2' || key == '3') cout << "Active Lights: " << activeLights[0] << ", " << activeLights[1] << ", " << activeLights[2] << "." << endl;
    if (actual == mode(3)) printf("x: %f | y: %f | z: %f\r\n", _spotlight_position[0], _spotlight_position[1], _spotlight_position[2]);
    if (actual == mode(4)) printf("x: %f | y: %f | z: %f | ???: %f\r\n", _light_position[0], _light_position[1], _light_position[2], _light_position[3]);
}

//void createList() {
//    glNewList(_model, GL_COMPILE);
//    {
//        glPushMatrix();
//        glBegin(GL_TRIANGLES);
//        for (int i = 0; i < faces.size(); i++) {
//            face face = faces[i];
//            ObjMaterial material = findMaterial(face.material);
//            glMaterialfv(GL_FRONT, GL_AMBIENT, &material.ambient.x);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, &material.diffuse.x);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, &material.specular.x);
//            glMaterialf(GL_FRONT, GL_SHININESS, material.shininess);
//            glMaterialf(GL_FRONT, GL_ALPHA, material.transparency);
//            glMaterialf(GL_FRONT, GL_EMISSION, material.illum);
//            /*glMaterialfv(GL_FRONT, GL_AMBIENT, &materials[i].ambient.x);
//            glMaterialfv(GL_FRONT, GL_DIFFUSE, &materials[i].diffuse.x);
//            glMaterialfv(GL_FRONT, GL_SPECULAR, &materials[i].specular.x);
//            glMaterialf(GL_FRONT, GL_SHININESS, materials[i].shininess);
//            glMaterialf(GL_FRONT, GL_ALPHA, materials[i].transparency);
//            glMaterialf(GL_FRONT, GL_EMISSION, materials[i].illum);*/
//
//            for (int j = 0; j < 3; j++) {
//
//                if (face.vertices[j].t != 0) {
//                    vec2 texture = textCoords[face.vertices[j].t];
//                    glTexCoord2f(texture.x, texture.y);
//                }
//                vec3 normal = normals[face.vertices[j].n];
//                vec3 vertex = vertices[face.vertices[j].v];
//                glNormal3f(normal.x, normal.y, normal.z);
//                glVertex3f(vertex.x, vertex.y, vertex.z);
//
//
//            }
//        }
//        glEnd();
//        glPopMatrix();
//    }
//    glEndList();
//}