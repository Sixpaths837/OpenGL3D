/* CG_Assignment2
* handScene_CPP_FILE
-------------------------------------------------------------*/

/***************************************************************
*                       DEPENDENCIES                           *
****************************************************************/
#include <iostream>   // Io stream included for cpp
#include <vector>     // vector lib
#include <map>        // map lib
#include <string>     // string lib
#include <fstream>    // fstream for .obj
#include <sstream>    // sstream for .obj
#include <GL/freeglut.h>  // GLUT, include glu.h and gl.h

std::map<std::string, std::vector<float>> materials_map; //!< Initializing material map that helps coloring the.obj rendered object

/*! Structs for parsing .obj !*/
/*! Special Struct for holding a obj vertex !*/
struct vertex {
    std::vector<float> v; //!< Vertex spatial values stored in this vector
    void normalize() {
        float magnitude = 0.0f;
        for (int i = 0; i < v.size(); i++)
            magnitude += pow(v[i], 2.0f);
        magnitude = sqrt(magnitude);
        for (int i = 0; i < v.size(); i++)
            v[i] /= magnitude;
    } //!< Function  to normalize the co-ordinates
    vertex operator-(vertex v2) {
        vertex v3;
        if (v.size() != v2.v.size()) {
            v3.v.push_back(0.0f);
            v3.v.push_back(0.0f);
            v3.v.push_back(0.0f);
        }
        else {
            for (int i = 0; i < v.size(); i++)
                v3.v.push_back(v[i] - v2.v[i]);
        }
        return v3;
    } //!< Operator Overloading to perform operations
    vertex cross(vertex v2) {
        vertex v3;
        if (v.size() != 3 || v2.v.size() != 3) {
            v3.v.push_back(0.0f);
            v3.v.push_back(0.0f);
            v3.v.push_back(0.0f);
        }
        else {
            v3.v.push_back(v[1] * v2.v[2] - v[2] * v2.v[1]);
            v3.v.push_back(v[2] * v2.v[0] - v[0] * v2.v[2]);
            v3.v.push_back(v[0] * v2.v[1] - v[1] * v2.v[0]);
        }
        return v3;
    } //!< Performs cross product of two vertices
};

/*! Face descriptor in an obj file !*/
struct face {
    std::vector<int> vertex;
    std::vector<int> texture;
    std::vector<int> normal;
    std::string material;
};

/*! Class description for the Obj file parser !*/
class cObj {
private:
    std::vector<vertex> vertices; //!< Stores all the vertex values
    std::vector<vertex> texcoords; 
    std::vector<vertex> normals; //!< Stores the vertex of the normals of the faces
    std::vector<vertex> parameters; 
    std::vector<face> faces; //!< Makes a vector of all the faces in the blender folder
    GLuint list; //!< Id specifier for glGenList
protected:
public:
    cObj(std::string filename); //!< Reference to the Consturctor with the path to the .obj file
    ~cObj(); //!< Reference to the default destructor of the class
    void compileList(); //!< Reference the glGenList Compile function
    void render(); //!< Reference to the glGenList call
};

/*! Contructor implementation to Obj file class*/
cObj::cObj(std::string filename) {
    std::ifstream ifs(filename.c_str(), std::ifstream::in);
    std::string line, key;
    std::string mat;
    while (ifs.good() && !ifs.eof() && std::getline(ifs, line)) {
        key = "";
        std::stringstream stringstream(line);
        stringstream >> key >> std::ws;

        if (key == "v") { //!< vertex
            vertex v; float x;
            while (!stringstream.eof()) {
                stringstream >> x >> std::ws;
                v.v.push_back(x);
            }
            vertices.push_back(v);
        }
        else if (key == "vp") { //!< parameter
            vertex v; float x;
            while (!stringstream.eof()) {
                stringstream >> x >> std::ws;
                v.v.push_back(x);
            }
            parameters.push_back(v);
        }
        else if (key == "vt") { //!< texture coordinate
            vertex v; float x;
            while (!stringstream.eof()) {
                stringstream >> x >> std::ws;
                v.v.push_back(x);
            }
            texcoords.push_back(v);
        }
        else if (key == "vn") { //!< normal
            vertex v; float x;
            while (!stringstream.eof()) {
                stringstream >> x >> std::ws;
                v.v.push_back(x);
            }
            v.normalize();
            normals.push_back(v);
        }
        else if (key == "usemtl") { //!< material
            while (!stringstream.eof()) {
                stringstream >> mat >> std::ws;
            }
        }
        else if (key == "f") { //!< face
            face f; int v, t, n;
            f.material = mat;
            while (!stringstream.eof()) {
                stringstream >> v >> std::ws;
                f.vertex.push_back(v - 1);
                if (stringstream.peek() == '/') {
                    stringstream.get();
                    if (stringstream.peek() == '/') {
                        stringstream.get();
                        stringstream >> n >> std::ws;
                        f.normal.push_back(n - 1);
                    }
                    else {
                        stringstream >> t >> std::ws;
                        f.texture.push_back(t - 1);
                        if (stringstream.peek() == '/') {
                            stringstream.get();
                            stringstream >> n >> std::ws;
                            f.normal.push_back(n - 1);
                        }
                    }
                }
            }
            faces.push_back(f);
        }
    }
    ifs.close();
    list = glGenLists(1);
    compileList();
    vertices.clear();
    texcoords.clear();
    normals.clear();
    faces.clear();
}

/*! glGenList compiling function implementation*/
void cObj::compileList() {
    glNewList(list, GL_COMPILE);
    for (int i = 0; i < faces.size(); i++) {
        if (faces[i].vertex.size() == 3) { //!< triangle
            if (faces[i].normal.size() == 3) { //!< with normals
                glBegin(GL_TRIANGLES);
                glColor3f(materials_map[faces[i].material][0], materials_map[faces[i].material][1], materials_map[faces[i].material][2]);
                glNormal3f(normals[faces[i].normal[0]].v[0], normals[faces[i].normal[0]].v[1], normals[faces[i].normal[0]].v[2]);
                glVertex3f(vertices[faces[i].vertex[0]].v[0], vertices[faces[i].vertex[0]].v[1], vertices[faces[i].vertex[0]].v[2]);
                glNormal3f(normals[faces[i].normal[1]].v[0], normals[faces[i].normal[1]].v[1], normals[faces[i].normal[1]].v[2]);
                glVertex3f(vertices[faces[i].vertex[1]].v[0], vertices[faces[i].vertex[1]].v[1], vertices[faces[i].vertex[1]].v[2]);
                glNormal3f(normals[faces[i].normal[2]].v[0], normals[faces[i].normal[2]].v[1], normals[faces[i].normal[2]].v[2]);
                glVertex3f(vertices[faces[i].vertex[2]].v[0], vertices[faces[i].vertex[2]].v[1], vertices[faces[i].vertex[2]].v[2]);
                glEnd();
            }
            else { //!< without normals -- evaluate normal on triangle
                vertex v = (vertices[faces[i].vertex[1]] - vertices[faces[i].vertex[0]]).cross(vertices[faces[i].vertex[2]] - vertices[faces[i].vertex[0]]);
                v.normalize();
                glBegin(GL_TRIANGLES);
                glColor3f(materials_map[faces[i].material][0], materials_map[faces[i].material][1], materials_map[faces[i].material][2]);
                glNormal3f(v.v[0], v.v[1], v.v[2]);
                glVertex3f(vertices[faces[i].vertex[0]].v[0], vertices[faces[i].vertex[0]].v[1], vertices[faces[i].vertex[0]].v[2]);
                glVertex3f(vertices[faces[i].vertex[1]].v[0], vertices[faces[i].vertex[1]].v[1], vertices[faces[i].vertex[1]].v[2]);
                glVertex3f(vertices[faces[i].vertex[2]].v[0], vertices[faces[i].vertex[2]].v[1], vertices[faces[i].vertex[2]].v[2]);
                glEnd();
            }
        }
        else if (faces[i].vertex.size() == 4) { //!< quad
            if (faces[i].normal.size() == 4) { //!< with normals
                glBegin(GL_QUADS);
                glColor3f(materials_map[faces[i].material][0], materials_map[faces[i].material][1], materials_map[faces[i].material][2]);
                glNormal3f(normals[faces[i].normal[0]].v[0], normals[faces[i].normal[0]].v[1], normals[faces[i].normal[0]].v[2]);
                glVertex3f(vertices[faces[i].vertex[0]].v[0], vertices[faces[i].vertex[0]].v[1], vertices[faces[i].vertex[0]].v[2]);
                glNormal3f(normals[faces[i].normal[1]].v[0], normals[faces[i].normal[1]].v[1], normals[faces[i].normal[1]].v[2]);
                glVertex3f(vertices[faces[i].vertex[1]].v[0], vertices[faces[i].vertex[1]].v[1], vertices[faces[i].vertex[1]].v[2]);
                glNormal3f(normals[faces[i].normal[2]].v[0], normals[faces[i].normal[2]].v[1], normals[faces[i].normal[2]].v[2]);
                glVertex3f(vertices[faces[i].vertex[2]].v[0], vertices[faces[i].vertex[2]].v[1], vertices[faces[i].vertex[2]].v[2]);
                glNormal3f(normals[faces[i].normal[3]].v[0], normals[faces[i].normal[3]].v[1], normals[faces[i].normal[3]].v[2]);
                glVertex3f(vertices[faces[i].vertex[3]].v[0], vertices[faces[i].vertex[3]].v[1], vertices[faces[i].vertex[3]].v[2]);
                glEnd();
            }
            else { //!< without normals -- evaluate normal on quad
                vertex v = (vertices[faces[i].vertex[1]] - vertices[faces[i].vertex[0]]).cross(vertices[faces[i].vertex[2]] - vertices[faces[i].vertex[0]]);
                v.normalize();
                glBegin(GL_QUADS);
                glColor3f(materials_map[faces[i].material][0], materials_map[faces[i].material][1], materials_map[faces[i].material][2]);
                glNormal3f(v.v[0], v.v[1], v.v[2]);
                glVertex3f(vertices[faces[i].vertex[0]].v[0], vertices[faces[i].vertex[0]].v[1], vertices[faces[i].vertex[0]].v[2]);
                glVertex3f(vertices[faces[i].vertex[1]].v[0], vertices[faces[i].vertex[1]].v[1], vertices[faces[i].vertex[1]].v[2]);
                glVertex3f(vertices[faces[i].vertex[2]].v[0], vertices[faces[i].vertex[2]].v[1], vertices[faces[i].vertex[2]].v[2]);
                glVertex3f(vertices[faces[i].vertex[3]].v[0], vertices[faces[i].vertex[3]].v[1], vertices[faces[i].vertex[3]].v[2]);
                glEnd();
            }
        }
    }
    glEndList();
}

/*! Caller function for glGenList*/
void cObj::render() {
    glCallList(list);
}

/*! Default Destructor */
cObj::~cObj() {
    glDeleteLists(list, 1);
}

/***************************************************************
*                           FUNCTIONS                          *
****************************************************************/

/**@brief 
*Global variables 
*/
GLdouble upx = 0.0, upy = 1.0, upz = 0.0, nx = 0.0, ny = 0.0, nz = 1.0;
GLdouble r = 70.00, x = 0.0, y = 10.0, z = 0.0, xc = 0.0, yc = 10.0, zc = 70.0;
float xdeg = 0.0, ydeg = 0.0, zdeg = 0.0;
cObj* obj = NULL;

/**@ 
*References for drawing scene 
*/
GLfloat base[6][4][3] = {
    {{-6.0f, 0.0f,-3.0f},{-6.0f, 0.0f, 3.0f},{ 6.0f, 0.0f, 3.0f},{ 6.0f, 0.0f,-3.0f}}, //top
    {{-6.0f,-2.0f,-3.0f},{ 6.0f,-2.0f,-3.0f},{ 6.0f,-2.0f, 3.0f},{-6.0f,-2.0f, 3.0f}}, //bottom
    {{-6.0f, 0.0f, 3.0f},{-6.0f, 0.0f,-3.0f},{-6.0f,-2.0f,-3.0f},{-6.0f,-2.0f, 3.0f}}, //left
    {{ 6.0f, 0.0f, 3.0f},{-6.0f, 0.0f, 3.0f},{-6.0f,-2.0f, 3.0f},{ 6.0f,-2.0f, 3.0f}}, //front
    {{ 6.0f, 0.0f,-3.0f},{ 6.0f,-2.0f,-3.0f},{-6.0f,-2.0f,-3.0f},{-6.0f, 0.0f,-3.0f}}, //back
    {{ 6.0f, 0.0f, 3.0f},{ 6.0f,-2.0f, 3.0f},{ 6.0f,-2.0f,-3.0f},{ 6.0f, 0.0f,-3.0f}} //right
};

GLfloat school[6][4][3] = {
    {{-2.0f, 2.7f,-3.0f},{-2.0f, 2.7f,-1.0f},{ 2.0f, 2.7f,-1.0f},{ 2.0f, 2.7f,-3.0f}}, //top
    {{-2.0f, 0.0f,-3.0f},{ 2.0f, 0.0f,-3.0f},{ 2.0f, 0.0f,-1.0f},{-2.0f, 0.0f,-1.0f}}, //bottom
    {{-2.0f, 2.7f,-1.0f},{-2.0f, 2.7f,-3.0f},{-2.0f, 0.0f,-3.0f},{-2.0f, 0.0f,-1.0f}}, //left
    {{ 2.0f, 2.7f,-1.0f},{-2.0f, 2.7f,-1.0f},{-2.0f, 0.0f,-1.0f},{ 2.0f, 0.0f,-1.0f}}, //front
    {{ 2.0f, 2.7f,-3.0f},{ 2.0f, 0.0f,-3.0f},{-2.0f, 0.0f,-3.0f},{-2.0f, 2.7f,-3.0f}}, //back
    {{ 2.0f, 2.7f,-1.0f},{ 2.0f, 0.0f,-1.0f},{ 2.0f, 0.0f,-3.0f},{ 2.0f, 2.7f,-3.0f}} //right
};

GLfloat schoolside[6][4][3] = {
    {{-3.0f, 1.8f,-3.0f},{-3.0f, 1.8f,-1.0f},{-2.0f, 1.8f,-1.0f},{-2.0f, 1.8f,-3.0f}}, //top
    {{-3.0f, 0.0f,-3.0f},{-2.0f, 0.0f,-3.0f},{-2.0f, 0.0f,-1.0f},{-3.0f, 0.0f,-1.0f}}, //bottom
    {{-3.0f, 1.8f,-1.0f},{-3.0f, 1.8f,-3.0f},{-3.0f, 0.0f,-3.0f},{-3.0f, 0.0f,-1.0f}}, //left
    {{-2.0f, 1.8f,-1.0f},{-3.0f, 1.8f,-1.0f},{-3.0f, 0.0f,-1.0f},{-2.0f, 0.0f,-1.0f}}, //front
    {{-2.0f, 1.8f,-3.0f},{-2.0f, 0.0f,-3.0f},{-3.0f, 0.0f,-3.0f},{-3.0f, 1.8f,-3.0f}}, //back
    {{-2.0f, 1.8f,-1.0f},{-2.0f, 0.0f,-1.0f},{-2.0f, 0.0f,-3.0f},{-2.0f, 1.8f,-3.0f}} //right
};

GLfloat schooltop[4][3][3] = {
    {{ 0.0f, 3.6f,-2.0f},{-1.3f, 2.7f,-1.3f},{ 1.3f, 2.7f,-1.3f}}, //left
    {{ 0.0f, 3.6f,-2.0f},{ 1.3f, 2.7f,-1.3f},{ 1.3f, 2.7f,-3.0f}}, //front
    {{ 0.0f, 3.6f,-2.0f},{ 1.3f, 2.7f,-3.0f},{-1.3f, 2.7f,-3.0f}}, //back
    {{ 0.0f, 3.6f,-2.0f},{-1.3f, 2.7f,-3.0f},{-1.3f, 2.7f,-1.3f}} //right
};

GLfloat windowpane[12][2][4][3];
GLfloat windowref[2][4][3] = {
    {{-2.8f, 0.1f,-1.0f}, {-2.2f, 0.1f,-1.0f}, {-2.2f, 0.8f,-1.0f}, {-2.8f, 0.8f,-1.0f}}, // screen
    {{-2.8f, 0.1f,-1.0f}, {-2.6f, 0.1f,-1.0f}, {-2.2f, 0.8f,-1.0f}, {-2.4f, 0.8f,-1.0f}}  // glare
};

GLfloat road[4][3] = {
    {-6.0f,1e-3f, 0.0f},{-6.0f,1e-3f, 1.5f},{ 6.0f,1e-3f, 1.5f},{ 6.0f,1e-3f, 0.0f}
};

GLfloat roadline[8][4][3];
GLfloat roadlineref[4][3] = {
    {-6.0f,2e-3f, 0.6f},{-6.0f,2e-3f, 0.8f},{-5.2f,2e-3f, 0.8f},{-5.2f,2e-3f, 0.6f}
};

GLfloat door[4][3] = {
    { 0.65f, 1.2f,-1.0f + 1e-3f},{-0.65f, 1.2f,-1.0f + 1e-3f},{-0.65f, 0.2f,-1.0f + 1e-3f},{ 0.65f, 0.2f,-1.0f + 1e-3f}
};
GLfloat doorstep[8][4][3] = {
    {{-0.8f, 0.1f,-1.0f},{-0.8f, 0.1f,-0.8f},{ 0.8f, 0.1f,-0.8f},{ 0.8f, 0.1f,-1.0f}}, // 1st top
    {{-0.8f, 0.1f,-0.8f},{-0.8f, 0.1f,-1.0f},{-0.8f, 0.0f,-1.0f},{-0.8f, 0.0f,-0.8f}}, // 1st left
    {{ 0.8f, 0.1f,-0.8f},{-0.8f, 0.1f,-0.8f},{-0.8f, 0.0f,-0.8f},{ 0.8f, 0.0f,-0.8f}}, // 1st front
    {{ 0.8f, 0.1f,-0.8f},{ 0.8f, 0.0f,-0.8f},{ 0.8f, 0.0f,-1.0f},{ 0.8f, 0.1f,-1.0f}}, // 1st right
    {{-0.7f, 0.2f,-1.0f},{-0.7f, 0.2f,-0.9f},{ 0.7f, 0.2f,-0.9f},{ 0.7f, 0.2f,-1.0f}}, // 2nd top
    {{-0.7f, 0.2f,-0.9f},{-0.7f, 0.2f,-1.0f},{-0.7f, 0.1f,-1.0f},{-0.7f, 0.1f,-0.9f}}, // 2nd left
    {{ 0.7f, 0.2f,-0.9f},{-0.7f, 0.2f,-0.9f},{-0.7f, 0.1f,-0.9f},{ 0.7f, 0.1f,-0.9f}}, // 2nd front
    {{ 0.7f, 0.2f,-0.9f},{ 0.7f, 0.1f,-0.9f},{ 0.7f, 0.1f,-1.0f},{ 0.7f, 0.2f,-1.0f}}  // 2nd right
};

/**@brief
*Make the window panes 
*/
void makePanes() {
    int w = 0;
    for (int i = 0; i < 18; i++) {
        if (i % 6 == 2 and (i / 6 == 0 or i / 6 == 1)) continue;
        if (i % 6 == 3 and (i / 6 == 0 or i / 6 == 1)) continue;
        if (i / 6 == 2 and (i % 6 == 0 or i % 6 == 5)) continue;
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 4; k++) {
                windowpane[w][j][k][0] = windowref[j][k][0] + 1.0f * (i % 6);
                windowpane[w][j][k][1] = windowref[j][k][1] + 0.9f * (i / 6);
                windowpane[w][j][k][2] = windowref[j][k][2] + (j + 1) * 1e-3f;
            }
        w++;
    }
}

/**@brief
*Make the road lines 
* 
* @retval void
*/
void makeRoad(void) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 4; j++) {
            roadline[i][j][0] = roadlineref[j][0] + 1.6f * i;
            roadline[i][j][1] = roadlineref[j][1];
            roadline[i][j][2] = roadlineref[j][2];
        }
}

/**@brief
*Map materials for blender object with corresponding rgb values 
* 
* @retval void
*/
void mapMaterials(void) {
    materials_map["CGlass"] = std::vector<float>{ 0.800000, 0.800000, 0.800000 };
    materials_map["Glass"] = std::vector<float>{ 0.060719,0.060719,0.060719 },
    materials_map["Lights"] = std::vector<float>{ 0.800000,0.800000,0.800000 },
    materials_map["Material"] = std::vector<float>{ 1.000000,1.000000,1.000000 },
    materials_map["Material.001"] = std::vector<float>{ 0.846873,0.428691,0.000000 },
    materials_map["Material.002"] = std::vector<float>{ 0.191275,0.375787,0.698168 },
    materials_map["Material.004"] = std::vector<float>{ 0.191275,0.375787,0.698168 },
    materials_map["Mirror"] = std::vector<float>{ 0.800000,0.800000,0.800000 },
    materials_map["OrangeLights"] = std::vector<float>{ 0.800000,0.273689,0.018567 },
    materials_map["Red"] = std::vector<float>{ 0.800000,0.000000,0.001381 },
    materials_map["Silver"] = std::vector<float>{ 0.295692, 0.295692, 0.295692 };
}

/**@brief
*Initialize OpenGL Graphics 
* 
* @retval void
*/
void initGL(void) {
    obj = new cObj("bus.obj");
    glClearColor(0.639f, 0.875f, 0.937f, 1.0f); //!< Set background color to light blue and opaque
    glClearDepth(1.0f);                   //!< Set background depth to farthest
    glEnable(GL_DEPTH_TEST);   //!< Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);    //!< Set the type of depth-test
    glShadeModel(GL_SMOOTH);   //!< Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //!< Nice perspective corrections
}

/**@brief
*Handler for window-repaint event. Called back when the window first appears and whenever the window needs to be re-painted.
* 
* @retval void
*/
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //!< Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);     //!< To operate on model-view matrix

    //! The Scene being loaded as a new item
    glLoadIdentity(); //!< Reset the model-view matrix
    gluLookAt(xc, yc, zc, x, y, z, 0.0, 1.0, 0.0);
    glRotatef(xdeg, 1.0, 0.0, 0.0);
    glRotatef(ydeg, 0.0, 1.0, 0.0);
    glRotatef(zdeg, 0.0, 0.0, 1.0);

    glScalef(4.0, 4.0, 4.0);
    obj->render();
    glScalef(0.25, 0.25, 0.25);
    glScalef(10.0, 10.0, 10.0);

    //! Start of quads
    glBegin(GL_QUADS);
    //!< drawing the base of the scene
    glColor3f(0.674f, 0.788f, 0.294f);
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 4; j++)
            glVertex3fv(base[i][j]);


    //!< drawing the school main block
    glColor3f(0.635f, 0.29f, 0.133f);
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 4; j++)
            glVertex3fv(school[i][j]);

    //!< drawing the sides of the  school
    glColor3f(0.635f, 0.29f, 0.133f);
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 4; j++)
            glVertex3fv(schoolside[i][j]);

    glColor3f(0.635f, 0.29f, 0.133f);
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 4; j++)
            glVertex3f(-schoolside[i][j][0], schoolside[i][j][1], schoolside[i][j][2]);

    //!< draw the windows
    for (int i = 0; i < 12; i++) {
        glColor3f(0.627f, 0.835f, 0.823f);
        for (int j = 0; j < 4; j++)
            glVertex3fv(windowpane[i][0][j]);
        glColor3f(0.871f, 0.953f, 0.953f);
        for (int j = 0; j < 4; j++)
            glVertex3fv(windowpane[i][1][j]);
    }

    //!< draw the road
    glColor3f(0.482f, 0.482f, 0.482f);
    for (int i = 0; i < 4; i++)
        glVertex3fv(road[i]);

    //!< draw the road lines
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 4; j++)
            glVertex3fv(roadline[i][j]);

    //!< draw the door
    glColor3f(0.886f, 0.886f, 0.886f);
    for (int i = 0; i < 4; i++) {
        if (i == 0) glColor3f(0.482f, 0.482f, 0.482f);
        glVertex3fv(door[i]);
        if (i == 0) glColor3f(0.886f, 0.886f, 0.886f);
    }
    //!< draw the doorsteps
    glColor3f(0.886f, 0.886f, 0.886f);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 4; j++)
            glVertex3fv(doorstep[i][j]);

    glEnd();  //! End of drawing quads

    glBegin(GL_TRIANGLES); //! Start of drawing triangles

    //!< drawing the school top as a pyramid
    glColor3f(0.859f, 0.910f, 0.949f);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 3; j++)
            glVertex3fv(schooltop[i][j]);

    glEnd(); //! End of drawing quads

    glLineWidth(6.0f); //!< set line width

    //! Start drawing lines
    glBegin(GL_LINES);
    //!< border for base
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 4; i++) {
        glVertex3fv(base[0][i]);
        glVertex3fv(base[0][(i + 1) % 4]);
    }
    for (int j = 0; j < 6; j++)
        for (int i = 0; i < 4; i++) {
            glVertex3fv(school[j][i]);
            glVertex3fv(school[j][(i + 1) % 4]);
        }
    for (int j = 0; j < 6; j++)
        for (int i = 0; i < 4; i++) {
            glVertex3fv(schoolside[j][i]);
            glVertex3fv(schoolside[j][(i + 1) % 4]);
        }
    for (int j = 0; j < 6; j++)
        for (int i = 0; i < 4; i++) {
            glVertex3f(-schoolside[j][i][0], schoolside[j][i][1], schoolside[j][i][2]);
            glVertex3f(-schoolside[j][(i + 1) % 4][0], schoolside[j][(i + 1) % 4][1], schoolside[j][(i + 1) % 4][2]);
        }
    glColor3f(0.396f, 0.439f, 0.451f);
    for (int i = 0; i < 4; i++) {
        glVertex3fv(schooltop[i][0]);
        glVertex3fv(schooltop[i][1]);
    }
    glEnd(); //! End drawing lines
    glLineWidth(1.0f);

    glutSwapBuffers();  //!< Swap the front and back frame buffers (double buffering)
}

/**@brief
*Handler for window re-size event. Called back when the window first appears and whenever the window is re-sized with its new width and height
* 
* @retval void
* 
* @param width
*   width of the viewport in pixels
* @param height
*   height of the viewport in pixels
*/
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(60.0f, aspect, 0.1, 400.0);
}

/**@brief
*Iteractive Camera Action - Handler 
* 
* @retval void
* 
* @param key
*   the key used for interrupting the render
* @param xcb
*   the first value returned for error
* @param ycb
*   the second value returned for error
*/
void keyInput(unsigned char key, int xcb, int ycb)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;
    case 'm': //!< case to zoom out of the scene
        r += 1.0;
        if (r > 300)r = 300.0;
        xc = x + r * nx;
        yc = y + r * ny;
        zc = z + r * nz;
        glutPostRedisplay();
        break;
    case 'M': //!< case to zoom out of the scene
        r -= 1.0;
        if (r < 1.0)r = 1.0;
        xc = x + r * nx;
        yc = y + r * ny;
        zc = z + r * nz;
        glutPostRedisplay();
        break;
    case 'x': //!< case to rotate ccw about x
        xdeg += 5.0;
        if (xdeg > 360.0) xdeg -= 360.0;
        glutPostRedisplay();
        break;
    case 'X': //!< case to rotate cw about x
        xdeg -= 5.0;
        if (xdeg < 0.0) xdeg += 360.0;
        glutPostRedisplay();
        break;
    case 'y': //!< case to rotate ccw about y
        ydeg += 5.0;
        if (ydeg > 360.0) ydeg -= 360.0;
        glutPostRedisplay();
        break;
    case 'Y': //!< case to rotate cw about y
        ydeg -= 5.0;
        if (ydeg < 0.0) ydeg += 360.0;
        glutPostRedisplay();
        break;
    case 'z': //!< case to rotate ccw about z
        zdeg += 5.0;
        if (zdeg > 360.0) zdeg -= 360.0;
        glutPostRedisplay();
        break;
    case 'Z': //!< case to rotate cw about z 
        zdeg -= 5.0;
        if (zdeg < 0.0) zdeg += 360.0;
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

/**@brief
*Callback routine for non - ASCII key entry.
* 
* @retval void
* 
* @param key
*   the key used for interrupting the render
* @param xcb
*   the first value returned for error
* @param ycb
*   the second value returned for error
*/
void specialKeyInput(int key, int xcb, int ycb)
{
    if (key == GLUT_KEY_LEFT)
        x -= 1.0, xc -= 1.0;
    if (key == GLUT_KEY_RIGHT)
        x += 1.0, xc += 1.0;
    if (key == GLUT_KEY_DOWN)
        y -= 1.0, yc -= 1.0;
    if (key == GLUT_KEY_UP)
        y += 1.0, yc += 1.0;

    glutPostRedisplay(); //!< Calls for the screen to be refreshed
}

/**@brief
*Print available interactive controls to console
* 
* @retval void
*/
void printInteract(void)
{
    std::cout << "Interaction:" << std::endl
        << "Press left/right and up/down arrow keys to pan left/right and up/down scene" << std::endl
        << "Press x, X, y, Y, z, Z to turn the scene." << std::endl
        << "Press m,M to zoom in and out of the scene." << std::endl;
}

/**@brief
*Main function: GLUT runs as a console application starting at main() 
* 
* @retval int
* 
* @param argc
*   the number of command line args used to run the compiled binary
* @param argv
*   the values passed as command line args
*/
int main(int argc, char** argv) {
    printInteract();
    makePanes();                      //!< Call helper functions
    makeRoad();
    mapMaterials();
    

    glutInit(&argc, argv);            //!< Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE); //!< Enable double buffered mode
    glutInitWindowSize(1920, 1080);   //!< Set the window's initial width & height
    glutInitWindowPosition(0, 0); //!< Position the window's initial top-left corner
    glutCreateWindow("handScene");          //!< Create window with the given title

    glutDisplayFunc(display);       //!< Register callback handler for window re-paint event
    glutReshapeFunc(reshape);       //!< Register callback handler for window re-size event
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);

    initGL();                       //!< Our own OpenGL initialization
    glutMainLoop();                 //!< Enter the infinite event-processing loop
    return 0;
}
