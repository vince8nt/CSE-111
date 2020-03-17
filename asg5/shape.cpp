// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

// equal to sqrt(3)/2, useful for equilateral triangle
#define PI 3.14159265
const float sq3d2 = 0.8660254;

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (const string& fontName, const string& textdata_):
      glut_bitmap_font(fontcode.find(fontName)->second),
      textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
      polygon({{width / 2, height / 2}, {-width / 2, height / 2},
      {-width / 2, -height / 2}, {width / 2, -height / 2}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height):
      polygon({{0, height}, {width, 0}, {0, -height}, {-width, 0}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

triangle::triangle (const vertex_list& vertices_): polygon(vertices_) {
   DEBUGF ('c', this);
}

// TODO(v): check that center is correct.
equilateral::equilateral (GLfloat width):
      triangle ({{0, 0}, {width, 0}, {width / 2, width * sq3d2}}) {
   DEBUGF ('c', this);
}

void text::draw (const vertex& center, const rgbcolor& color,
      const rgbcolor& borderColor, GLfloat borderSize,
      bool hasBorder, int order) const {
   DEBUGF ('d', this << "(" << center << "," << color << borderColor
      << "," << borderSize << "," << hasBorder << "," << order << ")");
   const GLubyte* toPrint = reinterpret_cast<const GLubyte*> (
      textdata.c_str());
   const GLubyte tCol[] {
      color.ubvec[0], color.ubvec[1], color.ubvec[2]
   };
   glColor3ubv (tCol);
   glRasterPos2f (center.xpos, center.ypos);
   
   glutBitmapString (glut_bitmap_font, toPrint);
}

void ellipse::draw (const vertex& center, const rgbcolor& color,
      const rgbcolor& borderColor, GLfloat borderSize,
      bool hasBorder, int order) const {
   DEBUGF ('d', this << "(" << center << "," << color << borderColor
      << "," << borderSize << "," << hasBorder << "," << order << ")");

   // print the shape
   const GLubyte sCol[] {
      color.ubvec[0], color.ubvec[1], color.ubvec[2]};
   glColor3ubv (sCol);
   glBegin (GL_POLYGON);
   for (GLfloat degree = 0; degree <= 360; degree += 0.5) {
      GLfloat xpos =
         center.xpos + dimension.xpos * cos(degree * PI/180);
      GLfloat ypos =
         center.ypos + dimension.ypos * sin(degree * PI/180);
      glVertex2f (xpos, ypos);
   }
   glEnd();

   // print the shape order
   if (color.ubvec[0] + color.ubvec[1] + color.ubvec[2] > 382) {
      const GLubyte tCol[] {0, 0, 0};
      glColor3ubv (tCol);
   }
   else {
      const GLubyte tCol[] {255, 255, 255};
      glColor3ubv (tCol);
   }
   const GLubyte* toPrint = reinterpret_cast<const GLubyte*> (
      to_string(order).c_str());
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (GLUT_BITMAP_HELVETICA_18, toPrint);

   // print the border if supposed to
   if (hasBorder) {
      glLineWidth (borderSize);
      glBegin (GL_LINE_LOOP);
      const GLubyte bCol[] { borderColor.ubvec[0],
         borderColor.ubvec[1], borderColor.ubvec[2]};
      glColor3ubv (bCol);
      for (GLfloat degree = 0; degree <= 360; degree += 0.5) {
         GLfloat xpos =
            center.xpos + dimension.xpos * cos(degree * PI/180);
         GLfloat ypos =
            center.ypos + dimension.ypos * sin(degree * PI/180);
         glVertex2f (xpos, ypos);
      }
      glEnd();
   }

}

void polygon::draw (const vertex& center, const rgbcolor& color,
      const rgbcolor& borderColor, GLfloat borderSize,
      bool hasBorder, int order) const {
   DEBUGF ('d', this << "(" << center << "," << color << borderColor
      << "," << borderSize << "," << hasBorder << "," << order << ")");

   // find center offset
   int xoff = 0, yoff = 0;
   for (vertex cur : vertices) {
      xoff += cur.xpos;
      yoff += cur.ypos;
   }
   xoff /= vertices.size();
   yoff /= vertices.size();

   // print the shape
   const GLubyte sCol[] {
      color.ubvec[0], color.ubvec[1], color.ubvec[2]};
   glBegin (GL_POLYGON);
   glColor3ubv (sCol);
   for (vertex cur : vertices)
      glVertex2f (cur.xpos + center.xpos - xoff,
      cur.ypos + center.ypos - yoff);
   glEnd();

   // print the shape order
   if (color.ubvec[0] + color.ubvec[1] + color.ubvec[2] > 382) {
      const GLubyte tCol[] {0, 0, 0};
      glColor3ubv (tCol);
   }
   else {
      const GLubyte tCol[] {255, 255, 255};
      glColor3ubv (tCol);
   }
   string order2 = to_string(order);
   const GLubyte* toPrint = reinterpret_cast<const GLubyte*> (
      order2.c_str());
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (GLUT_BITMAP_HELVETICA_18, toPrint);

   // print the border if supposed to
   if (hasBorder) {
      const GLubyte bCol[] { borderColor.ubvec[0],
         borderColor.ubvec[1], borderColor.ubvec[2]};
      glLineWidth (borderSize);
      glBegin (GL_LINE_LOOP);
      glColor3ubv (bCol);
      for (vertex cur : vertices)
         glVertex2f (cur.xpos + center.xpos - xoff,
         cur.ypos + center.ypos - yoff);
      glEnd();
   }

   
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

