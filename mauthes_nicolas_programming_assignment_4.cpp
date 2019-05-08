// Nick Mauthes

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>


#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/glut.h>
#endif


using namespace std;


#define X_RESOLUTION 640 
#define Y_RESOLUTION 480


// ############# COLOR & SHAPE #############

struct Color {
	float r, g, b;
};

const Color RED = {1.0, 0.0, 0.0};
const Color GREEN = {0.0, 1.0, 0.0};
const Color BLUE = {0.0, 0.0, 1.0};
const Color YELLOW = {1.0, 1.0, 0.0};
const Color PURPLE = {1.0, 0.0, 1.0};
const Color ORANGE = {1.0, 0.5, 0.0};
const Color GREY = {0.75, 0.75, 0.75};
const Color BLACK = {0.0, 0.0, 0.0};

struct Point {
	float x, y;
};

struct Rect {
	Point p1, p2;
	Color color;
	bool filled;
	
	void draw() {
		glColor3f(color.r, color.g, color.b);
		if(filled) {
			glBegin(GL_POLYGON);
		}
		else {
			glBegin(GL_LINE_LOOP);
		}
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glVertex2f(p1.x, p2.y);
		glEnd();
	}
};

struct Ellipse {
	Point p1, p2;
	Color color;
	bool filled;
	
	void draw() {
		glColor3f(color.r, color.g, color.b);
		if(filled) {
			glBegin(GL_POLYGON);
		}
		else {
			glBegin(GL_POINTS);
		}
		for(float i = 0; i < 2 * M_PI; i += 0.001) {
			glVertex2f(p1.x + (p2.x - p1.x)/2.0 + cos(i) * (p2.x - p1.x)/2, 
								p1. y + (p2.y - p1.y)/2.0 + sin(i) * (p2.y - p1.y)/2);
		}
		glEnd();
	}
};

struct Line {
	Point p1, p2;
	Color color;
	
	void draw() {
		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINES);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glEnd();
	}
};

float get_bezier_coordinate(float A, float B, float C, float D, float t) {
		float s = 1 - t;
    float AB = A*s + B*t;
    float BC = B*s + C*t;
    float CD = C*s + D*t;
    float ABC = AB*s + BC*t;
    float BCD = BC*s + CD*t;
    return ABC*s + BCD*t;
	
	return ABC*s + BCD*t;
}

struct Bezier {
	Point p1, p2, p3, p4;
	Color color;
	
	void draw() {
		glLineWidth(2);
		glColor3f(color.r, color.g, color.b);
		glBegin(GL_POINTS);
		for(float t = 0; t <= 1; t += 0.001) {
			glVertex2f(get_bezier_coordinate(p1.x, p2.x, p3.x, p4.x, t), get_bezier_coordinate(p1.y, p2.y, p3.y, p4.y, t));
		}
		glEnd();
	}
};

// #########################################


int selection;

vector<Point> points;

vector<Rect> rectangles;
vector<Ellipse> ellipses;
vector<Line> lines;
vector<Bezier> beziers;


void display (void)
{   
  glClearColor (1.0, 1.0, 1.0, 1.0);
  glClear (GL_COLOR_BUFFER_BIT);
  
  for(int i = 0; i < rectangles.size(); i++) {
  	rectangles[i].draw();
	}
  for(int i = 0; i < ellipses.size(); i++) {
  	ellipses[i].draw();
	}
  for(int i = 0; i < lines.size(); i++) {
  	lines[i].draw();
	}
  for(int i = 0; i < beziers.size(); i++) {
  	beziers[i].draw();
	}
  
  glLoadIdentity (); 
  glFlush ();
}





void reshape (int w, int h)
{   
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0.0, X_RESOLUTION, Y_RESOLUTION, 0, -1.0, 1.0);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
}



// ############# MOUSE/KBD INPUT #############

void keyboard(unsigned char key, int x, int y) {
	if(key == ' ') {
	  // CLEAR THE CANVAS
		rectangles.clear();
		ellipses.clear();
		lines.clear();
		beziers.clear();
	}
	
	cout << "Cleared\n";
	
	glutPostRedisplay();
}


void mouse (int mouse_button, int state, int x, int y)
{
	if(mouse_button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		Point p;
		p.x = x;
		p.y = y;
		points.push_back(p);
		
		cout << "Point ";
		cout << points.size();
		cout << ": ";
		cout << x;
		cout << ",";
		cout << y;
		cout << "\n";
		
		bool filled;
		Color color;
		
		if(selection < 8 || (selection >= 16 && selection < 24)) { // FILLED/OUTLINE
			filled = true;
		}
		else {
			filled = false;
		}
		
		if(selection % 8 == 0) { // COLOR
			color = RED;
		}
		else if(selection % 8 == 1) {
			color = GREEN;
		}
		else if(selection % 8 == 2) {
			color = BLUE;
		}
		else if(selection % 8 == 3) {
			color = YELLOW;
		}
		else if(selection % 8 == 4) {
			color = PURPLE;
		}
		else if(selection % 8 == 5) {
			color = ORANGE;
		}
		else if(selection % 8 == 6) {
			color = GREY;
		}
		else {
			color = BLACK;
		}
		
		
		if(selection < 16) { // DRAW RECTANGLE
			if(points.size() == 2) {
				Rect r;
				r.p1 = points[0];
				r.p2 = points[1];
				r.color = color;
				r.filled = filled;
				rectangles.push_back(r);
				
				points.clear();
			}
		}
		else if(selection < 32) { // DRAW ELLIPSE
			if(points.size() == 2) {
				Ellipse e;
				e.p1 = points[0];
				e.p2 = points[1];
				e.color = color;
				e.filled = filled;
				ellipses.push_back(e);
				
				points.clear();
			}
		}
		else if(selection < 40) { // DRAW LINE
			if(points.size() == 2) {
				Line l;
				l.p1 = points[0];
				l.p2 = points[1];
				l.color = color;
				lines.push_back(l);
				
				points.clear();
			}
		}
		else if(selection < 48) { // DRAW BEZIER
			if(points.size() == 4) {
				Bezier b;
				b.p1 = points[0];
				b.p2 = points[1];
				b.p3 = points[2];
				b.p4 = points[3];
				b.color = color;
				beziers.push_back(b);
				
				points.clear();
			}
		}
		else {
			cout << "INVALID SELECTION\n";
		}
		
	}
	glutPostRedisplay();
}

// ###########################################



// ############# MENU #############

void menu (int value)
{
	selection = value;
	cout << "Chose option:";
	cout << value;
	cout << "\n";
	
	points.clear();
}


void addColorEntries(int offset) {
  glutAddMenuEntry ("red", 0 + offset);
  glutAddMenuEntry ("green", 1 + offset);
  glutAddMenuEntry ("blue", 2 + offset);
  glutAddMenuEntry ("yellow", 3 + offset);
  glutAddMenuEntry ("purple", 4 + offset);
  glutAddMenuEntry ("orange", 5 + offset);
  glutAddMenuEntry ("grey", 6 + offset);
  glutAddMenuEntry ("black", 7 + offset);
}


void createMenus() {
  // CREATE SUBMENUS
  int rect_filled_colors = glutCreateMenu(menu);
  addColorEntries(8 * 0);
  
  int rect_outline_colors = glutCreateMenu(menu);
  addColorEntries(8 * 1);
  
  int rect_fill_menu = glutCreateMenu (menu);
  glutAddSubMenu ("filled", rect_filled_colors);
  glutAddSubMenu ("outline", rect_outline_colors);
  
  int ellipse_filled_colors = glutCreateMenu(menu);
  addColorEntries(8 * 2);
  
  int ellipse_outline_colors = glutCreateMenu(menu);
  addColorEntries(8 * 3);
  
  int ellipse_fill_menu = glutCreateMenu (menu);
  glutAddSubMenu ("filled", ellipse_filled_colors);
  glutAddSubMenu ("outline", ellipse_outline_colors);
  
  int line_colors = glutCreateMenu(menu);
  addColorEntries(8 * 4);
  
  int bezier_colors = glutCreateMenu(menu);
  addColorEntries(8 * 5);
  
  // CREATE TOP MENU
  glutCreateMenu (menu);
  glutAddSubMenu ("add rectangle", rect_fill_menu);
  glutAddSubMenu ("add ellipse", ellipse_fill_menu);
  glutAddSubMenu ("add line", line_colors);
  glutAddSubMenu ("add bezier curve", bezier_colors);
  glutAttachMenu (GLUT_LEFT_BUTTON);
}

// ################################



int main (int argc, char *argv[])
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize (X_RESOLUTION, Y_RESOLUTION);
  glutInitWindowPosition (0,0);
  glutCreateWindow ("Programming Assignment 4 (nmauthes)");
  
  selection = 0;
  
  createMenus();
  
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutMouseFunc (mouse);
  glutKeyboardFunc(keyboard);
  glutMainLoop ();
}

