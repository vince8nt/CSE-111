// $Id: graphics.cpp,v 1.6 2019-05-15 18:02:12-07 - - $

#include <iostream>
using namespace std;

#include <GL/freeglut.h>

#include "graphics.h"
#include "util.h"

int window::width = 640; // in pixels
int window::height = 480; // in pixels
vector<object> window::objects;
size_t window::selected_obj = 0;
mouse window::mus;
rgbcolor window::borderColor = rgbcolor(255, 0, 0);
GLfloat window::borderSize = 4;
GLfloat window::moveSpeed = 4;

// Implementation of object functions.
object::object (shared_ptr<shape> pshape_, vertex center_,
      rgbcolor color_, rgbcolor borderColor_, GLfloat borderSize_,
      bool hasBorder_):
      pshape(pshape_), center(center_), color(color_),
      borderColor(borderColor_), borderSize(borderSize_),
      hasBorder(hasBorder_){
}

void object::draw(bool myHasBorder, int myOrder) {
   pshape->draw (center, color, borderColor, borderSize,
      myHasBorder, myOrder);
}

// edge rollover should work
void object::move (GLfloat delta_x, GLfloat delta_y) {
   center.xpos = center.xpos + delta_x;
   center.ypos = center.ypos + delta_y;
   if (center.xpos >= window::width)
      center.xpos = fmod (center.xpos, window::width);
   while (center.xpos < 0) center.xpos += window::width;
   if (center.ypos >= window::height)
      center.ypos = fmod (center.ypos, window::height);
   while (center.ypos < 0) center.ypos += window::height;
}

// Implementation of mouse functions.
void mouse::state (int button, int state) {
   switch (button) {
      case GLUT_LEFT_BUTTON: left_state = state; break;
      case GLUT_MIDDLE_BUTTON: middle_state = state; break;
      case GLUT_RIGHT_BUTTON: right_state = state; break;
   }
}

void mouse::draw() {
   static rgbcolor color ("green");
   ostringstream text;
   text << "(" << xpos << "," << window::height - ypos << ")";
   if (left_state == GLUT_DOWN) text << "L"; 
   if (middle_state == GLUT_DOWN) text << "M"; 
   if (right_state == GLUT_DOWN) text << "R"; 
   if (entered == GLUT_ENTERED) {
      void* font = GLUT_BITMAP_HELVETICA_18;
      glColor3ubv (color.ubvec);
      glRasterPos2i (10, 10);
      auto ubytes = reinterpret_cast<const GLubyte*>
                    (text.str().c_str());
      glutBitmapString (font, ubytes);
   }
}


// Executed when window system signals to shut down.
void window::close() {
   DEBUGF ('g', sys_info::execname() << ": exit ("
           << sys_info::exit_status() << ")");
   exit (sys_info::exit_status());
}

// Executed when mouse enters or leaves window.
void window::entry (int mouse_entered) {
   DEBUGF ('g', "mouse_entered=" << mouse_entered);
   window::mus.entered = mouse_entered;
   if (window::mus.entered == GLUT_ENTERED) {
      DEBUGF ('g', sys_info::execname() << ": width=" << window::width
           << ", height=" << window::height);
   }
   glutPostRedisplay();
}

// Called to display the objects in the window.
void window::display() {
   glClear (GL_COLOR_BUFFER_BIT);
   uint index = 0;
   for (auto& object: window::objects) {
      if (window::selected_obj != index)
         object.draw(false, index);
      ++index;
   }
   window::objects[window::selected_obj].draw(
      true, window::selected_obj);
   mus.draw();
   glutSwapBuffers();
}

// Called when window is opened and when resized.
void window::reshape (int width_, int height_) {
   DEBUGF ('g', "width=" << width_ << ", height=" << height_);
   window::width = width_;
   window::height = height_;
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0, window::width, 0, window::height);
   glMatrixMode (GL_MODELVIEW);
   glViewport (0, 0, window::width, window::height);
   glClearColor (0.25, 0.25, 0.25, 1.0);
   glutPostRedisplay();
}


// Executed when a regular keyboard key is pressed.
// TODO(v): do stuff when the buttons are pressed.
void window::keyboard (GLubyte key, int x, int y) {
   enum {BS = 8, TAB = 9, ESC = 27, SPACE = 32, DEL = 127};
   DEBUGF ('g', "key=" << unsigned (key) << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   switch (key) {
      case 'Q': case 'q': case ESC:
         window::close();
         break;
      case 'H': case 'h':
         window::objects[window::selected_obj].move(
            -window::moveSpeed, 0);
         break;
      case 'J': case 'j':
         window::objects[window::selected_obj].move(
            0, -window::moveSpeed);
         break;
      case 'K': case 'k':
         window::objects[window::selected_obj].move(
            0, window::moveSpeed);
         break;
      case 'L': case 'l':
         window::objects[window::selected_obj].move(
            window::moveSpeed, 0);
         break;
      case 'N': case 'n': case SPACE: case TAB:
         (++window::selected_obj) %= window::objects.size();
         break;
      case 'P': case 'p': case BS:
         
         if (window::selected_obj == 0)
            window::selected_obj = window::objects.size() - 1;
         else --window::selected_obj;
         break;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
         if(window::objects.size() > static_cast<uint8_t>(key - '0'))
            window::selected_obj = key - '0';
         break;
      default:
         cerr << unsigned (key) << ": invalid keystroke" << endl;
         break;
   }
   glutPostRedisplay();
}


// Executed when a special function key is pressed.
// TODO(v): do stuff when the buttons are pressed.
void window::special (int key, int x, int y) {
   DEBUGF ('g', "key=" << key << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   switch (key) {
      case GLUT_KEY_LEFT: //move selected object (-1, 0)
         window::objects[window::selected_obj].move(-1, 0);
         break;
      case GLUT_KEY_DOWN: //move selected object (0, -1)
         window::objects[window::selected_obj].move(0, -1);
         break;
      case GLUT_KEY_UP: //move selected object (0, +1)
         window::objects[window::selected_obj].move(0, 1);
         break;
      case GLUT_KEY_RIGHT: //move selected object (+1, 0)
         window::objects[window::selected_obj].move(1, 0);
         break;
      case GLUT_KEY_F1: //select object (1)
         if(window::objects.size() > 0) window::selected_obj = 1;
         break;
      case GLUT_KEY_F2: //select object (2)
         if(window::objects.size() > 1) window::selected_obj = 2;
         break;
      case GLUT_KEY_F3: //select object (3)
         if(window::objects.size() > 2) window::selected_obj = 3;
         break;
      case GLUT_KEY_F4: //select object (4)
         if(window::objects.size() > 3) window::selected_obj = 4;
         break;
      case GLUT_KEY_F5: //select object (5)
         if(window::objects.size() > 4) window::selected_obj = 5;
         break;
      case GLUT_KEY_F6: //select object (6)
         if(window::objects.size() > 5) window::selected_obj = 6;
         break;
      case GLUT_KEY_F7: //select object (7)
         if(window::objects.size() > 6) window::selected_obj = 7;
         break;
      case GLUT_KEY_F8: //select object (8)
         if(window::objects.size() > 7) window::selected_obj = 8;
         break;
      case GLUT_KEY_F9: //select object (9)
         if(window::objects.size() > 8) window::selected_obj = 9;
         break;
      case GLUT_KEY_F10: //select object (10)
         if(window::objects.size() > 9) window::selected_obj = 10;
         break;
      case GLUT_KEY_F11: //select object (11)
         if(window::objects.size() > 10) window::selected_obj = 11;
         break;
      case GLUT_KEY_F12: //select object (12)
         if(window::objects.size() > 11) window::selected_obj = 12;
         break;
      default:
         cerr << unsigned (key) << ": invalid function key" << endl;
         break;
   }
   glutPostRedisplay();
}


void window::motion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::passivemotion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::mousefn (int button, int state, int x, int y) {
   DEBUGF ('g', "button=" << button << ", state=" << state
           << ", x=" << x << ", y=" << y);
   window::mus.state (button, state);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::main () {
   static int argc = 0;
   glutInit (&argc, nullptr);
   glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize (window::width, window::height);
   glutInitWindowPosition (128, 128);
   glutCreateWindow (sys_info::execname().c_str());
   glutCloseFunc (window::close);
   glutEntryFunc (window::entry);
   glutDisplayFunc (window::display);
   glutReshapeFunc (window::reshape);
   glutKeyboardFunc (window::keyboard);
   glutSpecialFunc (window::special);
   glutMotionFunc (window::motion);
   glutPassiveMotionFunc (window::passivemotion);
   glutMouseFunc (window::mousefn);
   DEBUGF ('g', "Calling glutMainLoop()");
   glutMainLoop();
}

