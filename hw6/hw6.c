/*
 *  Lighting
 *
 *  Demonstrates basic lighting using a piper cub model.
 *
 *  Key bindings:
 *  l          Toggles lighting
 *  a/A        Decrease/increase ambient light
 *  d/D        Decrease/increase diffuse light
 *  s/S        Decrease/increase specular light
 *  e/E        Decrease/increase emitted light
 *  n/N        Decrease/increase shininess
 *  F1         Toggle smooth/flat shading
 *  F2         Toggle local viewer mode
 *  F3         Toggle light distance (1/5)
 *  F8         Change ball increment
 *  F9         Invert bottom normal
 *  m          Toggles light movement
 *  []         Lower/rise light
 *  p          Toggles ortogonal/perspective projection
 *  +/-        Change field of view of perspective
 *  x          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx229.h"

int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world

// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light

// texture values
int texture[1];
int ntex = 0;
int num_textures = 1;


/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

#include "CSCIx229.h"
#include <stdbool.h>


void pointOnCircle(double th, double r, double c_x, double c_y, double c_z)
{
  glVertex3d(c_x, c_y + (r*Cos(th)), c_z + (r*Sin(th)));
}

void pointOnCircle2(double th, double r, double c_x, double c_y, double c_z,
                   double *px, double *py, double *pz)
{
  *px = c_x;
  *py = c_y + r*Cos(th);
  *pz = c_z + r*Sin(th);
}

void getCowlNorms(double aft_x, double aft_y, 
                  double fwd_x, double fwd_y, double th)
{
  double norm_i = aft_y - fwd_y;
  double norm_j = aft_x - fwd_x;
  double l = sqrt(norm_i*norm_i + norm_j*norm_j);
  norm_j = l*Cos(th);
  double norm_k = l*Sin(th);

  glNormal3f(norm_i, norm_j, norm_k);
}

void crossProduct(double a_i, double a_j, double a_k,
  double b_i, double b_j, double b_k,
  double c_i, double c_j, double c_k,
  double *n_i, double *n_j, double *n_k)
{
  float a_vec_i = a_i - c_i;
  float a_vec_j = a_j - c_j;
  float a_vec_k = a_k - c_k;
  float b_vec_i = b_i - c_i;
  float b_vec_j = b_j - c_j;
  float b_vec_k = b_k - c_k;

  *n_i = a_vec_j*b_vec_k - a_vec_k*b_vec_j;
  *n_j = a_vec_k*b_vec_i - a_vec_i*b_vec_k;
  *n_k = a_vec_i*b_vec_j - a_vec_j*b_vec_i;

  // calculate norm
  float norm = sqrt(*n_i**n_i + *n_j**n_j + *n_k**n_k);

  // normalize
  *n_i /= norm;
  *n_j /= norm;
  *n_k /= norm;
}

// sets the normal vector as a unit vector parallel to the 
// cross product of the two vectors from c to a and c to b
void crossProductNorm(double a_i, double a_j, double a_k,
  double b_i, double b_j, double b_k,
  double c_i, double c_j, double c_k)
{
  double i,j,k;
  crossProduct(a_i,a_j,a_k,b_i,b_j,b_k,c_i,c_j,c_k,&i,&j,&k);

  glNormal3f(i,j,k);
}

/*
 * Draws a piper cub's fuselage including nose cowling and engine 
 */
static void drawFuselage()
{
  float white[] = {1,1,1,1};
  float black[] = {0,0,0,1};
  glMaterialf(GL_FRONT,GL_SHININESS,0.5);
  glMaterialfv(GL_FRONT,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT,GL_EMISSION,black);

  // enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_MODULATE:GL_REPLACE);
  glColor3f(1.0,1.0,1.0);
  if(ntex) glBindTexture(GL_TEXTURE_2D, texture[0]);
  else glColor3f(1.0,1.0,0.0);

  glBegin(GL_QUADS);
   // aft tail boom  right side
  double tail_top = 0.17;
  double fwd_tail_top = 0.21;
  double tail = -0.85;
  double tail_boom_front = -0.25;
  
  crossProductNorm(tail, 0.1, 0.0,
               tail_boom_front, fwd_tail_top, 0.0875,
               tail, tail_top, 0.0);
  glTexCoord2f(0,0.7); glVertex3d(tail, tail_top, 0.0);
  glTexCoord2f(0,0.4); glVertex3d(tail, 0.1, 0.0);
  glTexCoord2f(2,0); glVertex3d(tail_boom_front, 0.025, 0.0875);
  glTexCoord2f(2,1); glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);

  // aft tail boom left side
  crossProductNorm(tail_boom_front, fwd_tail_top, -0.0875,
               tail, 0.1, 0.0,
               tail, tail_top, 0.0);
  glTexCoord2f(0,0.4); glVertex3d(tail, 0.1, 0.0);
  glTexCoord2f(0,0.7); glVertex3d(tail, tail_top, 0.0);
  glTexCoord2f(2,1); glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);
  glTexCoord2f(2,0); glVertex3d(tail_boom_front, 0.025, -0.0875);

  // aft tail boom top
  crossProductNorm(tail_boom_front, fwd_tail_top, 0.0875,
               tail_boom_front, fwd_tail_top, -0.875,
               tail, tail_top, 0.0);
  glTexCoord2f(0,0.5); glVertex3d(tail, tail_top, 0.0);
  glTexCoord2f(0,0.5); glVertex3d(tail, tail_top, 0.0);
  glTexCoord2f(2,0.9); glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);
  glTexCoord2f(2,0.1); glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);

  // aft tail boom bottom
  crossProductNorm(tail_boom_front, 0.025, -0.0875,
               tail_boom_front, 0.025, 0.875,
               tail, 0.1, 0.0);
  glTexCoord2f(0,0.5); glVertex3d(tail, 0.1, 0.0);
  glTexCoord2f(0,0.5); glVertex3d(tail, 0.1, 0.0);
  glTexCoord2f(2,0.9); glVertex3d(tail_boom_front, 0.025, -0.0875);
  glTexCoord2f(2,0.1); glVertex3d(tail_boom_front, 0.025, 0.0875);

  double door_top = 0.25;
  double door_bottom = 0.0;
  double fwd_tail_front = -0.1;

  // fwd tail boom right side
  crossProductNorm(tail_boom_front, 0.025, 0.0875,
               fwd_tail_front, door_top, 0.10,
               tail_boom_front, fwd_tail_top, 0.0875);
  glTexCoord2f(0,1); glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);
  glTexCoord2f(0,0); glVertex3d(tail_boom_front, 0.025, 0.0875);
  glTexCoord2f(0.5,-0.1); glVertex3d(fwd_tail_front, door_bottom, 0.10);
  glTexCoord2f(0.5,1.15); glVertex3d(fwd_tail_front, door_top, 0.10);

  // fwd tail boom left side
  crossProductNorm(fwd_tail_front, door_top, -0.10,
               tail_boom_front, 0.025, -0.0875,
               tail_boom_front, fwd_tail_top, -0.0875);
  glTexCoord2f(0,0.1); glVertex3d(tail_boom_front, 0.025, -0.0875);
  glTexCoord2f(0,0.9); glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);
  glTexCoord2f(0.5,1.15); glVertex3d(fwd_tail_front, door_top, -0.10);
  glTexCoord2f(0.5,-0.1); glVertex3d(fwd_tail_front, door_bottom, -0.10);



  // fwd tail boom top
  crossProductNorm(fwd_tail_front, door_top, 0.10,
               tail_boom_front, fwd_tail_top, -0.875,
               tail_boom_front, fwd_tail_top, 0.875);
  glTexCoord2f(0,0.9); glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);
  glTexCoord2f(0.5,1); glVertex3d(fwd_tail_front, door_top, 0.10);
  glTexCoord2f(0.5,0); glVertex3d(fwd_tail_front, door_top, -0.10);
  glTexCoord2f(0,0.1); glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);

  // fwd tail boom bottom
  crossProductNorm(fwd_tail_front, door_bottom, -0.10,
               tail_boom_front, 0.025, 0.0875,
               tail_boom_front, 0.025, -0.0875);
  glTexCoord2f(0,0.9); glVertex3d(tail_boom_front, 0.025, -0.0875);
  glTexCoord2f(0,0.1); glVertex3d(tail_boom_front, 0.025, 0.0875);
  glTexCoord2f(0.5,0); glVertex3d(fwd_tail_front, door_bottom, 0.10);
  glTexCoord2f(0.5,1); glVertex3d(fwd_tail_front, door_bottom, -0.10);

  // right door
  crossProductNorm(fwd_tail_front, door_bottom, 0.10,
               0.25, door_top, 0.10,
               fwd_tail_front, door_top, 0.10);
  glTexCoord2f(0,1.1); glVertex3d(fwd_tail_front, door_top, 0.10);
  glTexCoord2f(0,-0.1); glVertex3d(fwd_tail_front, door_bottom, 0.10);
  glTexCoord2f(1,-0.1); glVertex3d(0.25, door_bottom, 0.10);
  glTexCoord2f(1,1.1); glVertex3d(0.25, door_top, 0.10);

  // left door
  crossProductNorm(0.25, door_top, -0.10,
               fwd_tail_front, door_bottom, -0.10,
               fwd_tail_front, door_top, -0.10);
  glTexCoord2f(0,-0.1); glVertex3d(fwd_tail_front, door_bottom, -0.10);
  glTexCoord2f(0,1.1); glVertex3d(fwd_tail_front, door_top, -0.10);
  glTexCoord2f(1,1.1); glVertex3d(0.25, door_top, -0.10);
  glTexCoord2f(1,-0.1); glVertex3d(0.25, door_bottom, -0.10);

  // belly
  crossProductNorm(0.25, door_bottom, -0.1,
               fwd_tail_front, door_bottom, 0.1,
               fwd_tail_front, door_bottom, -0.1);
  glTexCoord2f(0,1); glVertex3d(fwd_tail_front, door_bottom, 0.1);
  glTexCoord2f(0,0); glVertex3d(fwd_tail_front, door_bottom, -0.1);
  glTexCoord2f(1,0); glVertex3d(0.25, door_bottom, -0.1);
  glTexCoord2f(1,1); glVertex3d(0.25, door_bottom, 0.1);

  // leave roof open, it will be covered by the wing

  double cowling_top = 0.18;
  double cowling_bottom = 0.01;
  double cowling_side = 0.09;
  double firewall = 0.33;


  // fwd fuselage right
  crossProductNorm(0.25, door_bottom, 0.1,
               firewall, cowling_top, cowling_side,
               0.25, door_top, 0.1);
  glTexCoord2f(0,1.1); glVertex3d(0.25, door_top, 0.1);
  glTexCoord2f(0,-0.1); glVertex3d(0.25, door_bottom, 0.1);
  glTexCoord2f(0.25,-0.05); glVertex3d(firewall, cowling_bottom, cowling_side);
  glTexCoord2f(0.25,0.8); glVertex3d(firewall, cowling_top, cowling_side);

  // fwd fuselage left
  crossProductNorm(firewall, cowling_top, -1. * cowling_side,
               0.25, door_bottom, -0.1,
               0.25, door_top, -0.1);
  glTexCoord2f(0,-0.1); glVertex3d(0.25, door_bottom, -0.1);
  glTexCoord2f(0,1.1); glVertex3d(0.25, door_top, -0.1);
  glTexCoord2f(0.25,0.8); glVertex3d(firewall, cowling_top, -1. * cowling_side);
  glTexCoord2f(0.25,-0.05); glVertex3d(firewall, cowling_bottom, -1. * cowling_side);

  // fwd belly
  crossProductNorm(0.25, door_bottom, -0.1,
               firewall, cowling_bottom, cowling_side,
               0.25, door_bottom, 0.1);
  glTexCoord2f(0,0); glVertex3d(0.25, door_bottom, -0.1);
  glTexCoord2f(0.25,0.1); glVertex3d(firewall, cowling_bottom, -1. * cowling_side);
  glTexCoord2f(0.25,0.9); glVertex3d(firewall, cowling_bottom, cowling_side);
  glTexCoord2f(0,1); glVertex3d(0.25, door_bottom, 0.1);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  // windscreen
  glBegin(GL_QUADS);
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1.0);
  glColor3f(0.2,0.6,0.8);
  crossProductNorm(firewall, cowling_top, cowling_side,
               0.25, door_top, -0.1,
               0.25, door_top, 0.1);
  glVertex3d(0.25, door_top, 0.1);
  glVertex3d(0.25, door_top, -0.1);
  glVertex3d(firewall, cowling_top, -1. * cowling_side);
  glVertex3d(firewall, cowling_top, cowling_side);

  glEnd();
  
  

  // windows
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(-1.0f,-1.0f);

  glBegin(GL_QUADS);

  double offset = 0.01;
  glNormal3f(0.,0.,1.);
  glVertex3d(fwd_tail_front + 0.1, door_top - offset, 0.10);
  glVertex3d(0.25 - offset, door_top - offset, 0.10);
  glVertex3d(0.25 - offset, cowling_top - offset, 0.10);
  glVertex3d(fwd_tail_front + 0.1, cowling_top - offset, 0.10);

  glNormal3f(0.,0.,-1.);
  glVertex3d(fwd_tail_front + 0.1, door_top - offset, -0.10);
  glVertex3d(0.25 - offset/2, door_top - offset, -0.10);
  glVertex3d(0.25 - offset/2, cowling_top - offset, -0.10);
  glVertex3d(fwd_tail_front + 0.1, cowling_top - offset, -0.10);

  crossProductNorm(0.25+offset/2, cowling_top - offset, 0.10,
                   firewall-offset, cowling_top-offset, cowling_side,
                   0.25+offset/2, door_top-0.015, 0.10);
  glVertex3d(0.25+offset/2, door_top-0.015, 0.10);
  glVertex3d(firewall-offset, cowling_top-offset, cowling_side);
  glVertex3d(0.25+offset/2, cowling_top - offset, 0.10);
  glVertex3d(0.25+offset/2, cowling_top - offset, 0.10);

  crossProductNorm(firewall-offset, cowling_top-offset, cowling_side,
                   0.25+offset/2, cowling_top - offset, 0.10,
                   0.25+offset/2, door_top-0.015, 0.10);
  glVertex3d(0.25+offset/2, door_top-0.015, -0.10);
  glVertex3d(firewall-offset, cowling_top-offset, -cowling_side);
  glVertex3d(0.25+offset/2, cowling_top - offset, -0.10);
  glVertex3d(0.25+offset/2, cowling_top - offset, -0.10);

  glEnd();

  glDisable(GL_POLYGON_OFFSET_FILL);

  

  glMaterialf(GL_FRONT,GL_SHININESS,0.5);
  glEnable(GL_TEXTURE_2D);
  if(ntex) glBindTexture(GL_TEXTURE_2D, texture[0]);
  else glColor3f(1.0,1.0,0.0);

  // aft cowling
  double cowl_y_center = 0.5 * (cowling_top + cowling_bottom);
  glBegin(GL_QUAD_STRIP);
  double radius = 0.06;
  double fwd_cowl = 0.45;
  double horiz_increment = cowling_side * 0.5;
  double horiz_position = -1. * cowling_side;
  int fwd_cowl_angle = 35;

  for (double th = -45; th <= 45; th += 22.5)
  {
    double px, py, pz;
    pointOnCircle2(th,radius,fwd_cowl,cowl_y_center,0.0,&px,&py,&pz);
    getCowlNorms(firewall, cowling_top, px, py, th);
    glTexCoord2f(0,th/90.); 
    glVertex3d(firewall, cowling_top, horiz_position);

    glNormal3f(Sin(fwd_cowl_angle),Cos(th),Sin(th));
    glTexCoord2f(0.25,th/90.); 
    glVertex3d(px,py,pz);
    horiz_position += horiz_increment;
  }

  double vert_increment = 0.25*(cowling_top - cowling_bottom);
  double vert_position = cowling_top;

  for (double th = 45; th <= 135; th += 22.5)
  {
    double px, py, pz;
    pointOnCircle2(th,radius,fwd_cowl,cowl_y_center,0.0,&px,&py,&pz);
    getCowlNorms(firewall, vert_position, px, py, th);
    glTexCoord2f(0,th/90.); 
    glVertex3d(firewall, vert_position, cowling_side);

    glNormal3f(Sin(fwd_cowl_angle),Cos(th),Sin(th));
    glTexCoord2f(0.25,th/90.); 
    glVertex3d(px,py,pz);
    
    vert_position -= vert_increment;
  }
  horiz_position -= horiz_increment;
  for(double th = 135; th <= 225; th += 22.5)
  {
    double px, py, pz;
    pointOnCircle2(th,radius,fwd_cowl,cowl_y_center,0.0,&px,&py,&pz);
    getCowlNorms(firewall, cowling_bottom, px, py, th);
    glTexCoord2f(0,th/90.); 
    glVertex3d(firewall, cowling_bottom, horiz_position);

    glNormal3f(Sin(fwd_cowl_angle),Cos(th),Sin(th));
    glTexCoord2f(0.25,th/90.); 
    glVertex3d(px,py,pz);
    
    horiz_position -= horiz_increment;
  }
  vert_position += vert_increment;
  for(double th = 225; th <= 315; th += 22.5)
  {
    double px, py, pz;
    pointOnCircle2(th,radius,fwd_cowl,cowl_y_center,0.0,&px,&py,&pz);
    getCowlNorms(firewall, vert_position, px, py, th);
    glTexCoord2f(0,th/90.); 
    glVertex3d(firewall, vert_position, -1 * cowling_side);

    glNormal3f(Sin(fwd_cowl_angle),Cos(th),Sin(th));
    glTexCoord2f(0.25,th/90.); 
    glVertex3d(px,py,pz);
    
    vert_position += vert_increment;
  }

  glEnd();
  
  // fwd cowling
  double nose = 0.49;
  glBegin(GL_TRIANGLE_STRIP);
  
  for (double th = 0; th <= 360; th += 22.5)
  {
    double px, py, pz;
    pointOnCircle2(th,radius,fwd_cowl,cowl_y_center,0.0,&px,&py,&pz);
    glNormal3f(Sin(fwd_cowl_angle), Cos(th), Sin(th));
    glTexCoord2f(0.35*Cos(th)+0.5,0.35*Sin(th)+0.5); 
    glVertex3d(px,py,pz);

    glNormal3f(1,0,0);
    glTexCoord2f(0.5,0.5); 
    glVertex3d(nose, cowl_y_center, 0.0);
  }

  glEnd();
  glDisable(GL_TEXTURE_2D);

}

static void drawWing()
{

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_MODULATE:GL_REPLACE);
  if(ntex) glBindTexture(GL_TEXTURE_2D, texture[0]);
  else glColor3f(1.0,1.0,0.0);

  // define wing cross section
  int num_points = 6;
  double cross_sec_x[] = {0.25, 0.26, 0.25, 0.23, 0.20, -0.1};
  double cross_sec_y[] = {0.25, 0.26, 0.27, 0.28, 0.29, 0.25};

  double wingtip = 1.2;

  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0.5);
  glColor3f(1.0,1.0,0.0);

  double tex_scale = 3.0;
  for (int j = 0; j < 2; j++)
  {
    wingtip *= -1.0;

    glBegin(GL_POLYGON);
    glNormal3f(0,0,wingtip);
    for (int i = 0; i < num_points; i++)
    {
      glTexCoord2f(tex_scale*cross_sec_x[i], tex_scale*cross_sec_y[i]);
      glVertex3d(cross_sec_x[i], cross_sec_y[i], wingtip);
    }
    glEnd();
  }

  glBegin(GL_QUAD_STRIP);
  double tex_pos = 0.0;
  for (int i = 0; i < num_points; i++)
  {
    // find indices of next and previous points
    int last_i = (i-1)%num_points;
    int next_i = (i+1)%num_points;
    // get vector from current to last point
    double a_i, a_j;
    a_i = cross_sec_x[last_i] - cross_sec_x[i];
    a_j = cross_sec_y[last_i] - cross_sec_y[i];
    // normalize
    double na = sqrt(a_i*a_i + a_j*a_j);
    a_i /= na;
    a_j /= na;
    // get vector from current to next point
    double b_i, b_j;
    b_i = cross_sec_x[next_i] - cross_sec_x[i];
    b_j = cross_sec_y[next_i] - cross_sec_y[i];
    // normalize
    double nb = sqrt(b_i*b_i + b_j*b_j);
    b_i /= nb;
    b_j /= nb;
    // get angle between vectors in radians
    double th = atan2(a_j,a_i) - atan2(b_j,b_i);

    if (th < M_PI)
      th = (2*M_PI) - th;

    double n_i, n_j;
    n_i = cos(th/2)*a_i - sin(th/2)*a_j;
    n_j = sin(th/2)*a_i + cos(th/2)*a_j;
  
    if (i == 0)
      glNormal3f(0.0, -1.0, 0.0);
    else if (i > 0 && i < 4)
      glNormal3f(-n_i,-n_j,0.0);
    else if (i < num_points - 1)
      glNormal3f(n_i,n_j,0.0);
    else
      glNormal3f(a_j, a_i, 0.0);



    glTexCoord2f(3,tex_pos);
    glVertex3d(cross_sec_x[i], cross_sec_y[i], wingtip);
    glTexCoord2f(-3,tex_pos);
    glVertex3d(cross_sec_x[i], cross_sec_y[i], -1.0*wingtip);
    tex_pos += tex_scale*nb;
  }
  glEnd();
  glBegin(GL_QUADS);
  glNormal3f(0,-1,0);
  glTexCoord2f(-3,0);
  glVertex3d(cross_sec_x[num_points-1],cross_sec_y[num_points-1],-1.0*wingtip);
  glTexCoord2f(3,0);
  glVertex3d(cross_sec_x[num_points-1],cross_sec_y[num_points-1],wingtip);
  glTexCoord2f(3,1);
  glVertex3d(cross_sec_x[0], cross_sec_y[0], wingtip);
  glTexCoord2f(-3,1);
  glVertex3d(cross_sec_x[0], cross_sec_y[0], -1.0*wingtip);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

static void drawVStab()
{
  int num_points = 7;
  double norm_th[] = {270, 270, 270, 200, 140, 90, 50};
  double cross_sec_x[] = {-0.65, -0.85, -0.85, -0.95, -0.90, -0.85, -0.80};
  double cross_sec_y[] = {0.17, 0.17, 0.11, 0.14, 0.35, 0.38, 0.35};
  double offset = 0.005;
  double direction = -1.0;

  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0.5);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_MODULATE:GL_REPLACE);
  if(ntex) glBindTexture(GL_TEXTURE_2D, texture[0]);
  else glColor3f(1.0,1.0,0.0);
  
  // draw sides of v-stab
  double tex_scale = 4.0;
  for (int j = 0; j < 2; j++)
  {
    direction *= -1.0;
    glBegin(GL_POLYGON);
    glNormal3f(0,0,direction);
    for (int i = 0; i < num_points; i++)
    {
      glTexCoord2f(tex_scale*cross_sec_x[i],tex_scale*cross_sec_y[i]);
      glVertex3d(cross_sec_x[i], cross_sec_y[i], direction*offset);
    }
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);

  glBegin(GL_QUAD_STRIP);
  for (int i = 0; i < num_points; i++)
  {
    glNormal3f(Cos(norm_th[i]),Sin(norm_th[i]),0.0);
    glVertex3d(cross_sec_x[i], cross_sec_y[i], offset);
    glVertex3d(cross_sec_x[i], cross_sec_y[i], -1.0*offset);
  }
  glNormal3f(Cos(norm_th[num_points-1]),Sin(norm_th[num_points-1]),0.0);
  glVertex3d(cross_sec_x[0], cross_sec_y[0], offset);
  glVertex3d(cross_sec_x[0], cross_sec_y[0], -1.0*offset);
  glEnd();
}

static void drawHStab()
{
  double stab_height = 0.15;
  int num_points = 7;
  int norm_th[] = {0, 340, 315, 270, 225, 150, 135};
  double cross_sec_x[] = {-0.65, -0.66, -0.71, -0.88, -0.95, -0.95, -0.87};
  double cross_sec_z[] = {0.0, 0.07, 0.15, 0.30, 0.25, 0.08, 0.0};
  double offset = .005;
  double y_dir = -1.;
  double z_dir = -1.;

  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0.5);
  
  
  double tex_scale = 4.0;
  for (int k = 0; k < 2; k++)
  {
    z_dir *= -1.;
    for (int j = 0; j < 2; j++)
    {
      y_dir *= -1.;
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_MODULATE:GL_REPLACE);
      if(ntex) glBindTexture(GL_TEXTURE_2D, texture[0]);
      else glColor3f(1.0,1.0,0.0);
      glBegin(GL_POLYGON);
      glNormal3f(0.0, y_dir, 0.0);
      for (int i = 0; i < num_points; i++)
      {
        glTexCoord2f(tex_scale*cross_sec_z[i],tex_scale*cross_sec_x[i]);
        glVertex3d(cross_sec_x[i], 
                   stab_height + (offset*y_dir), 
                   cross_sec_z[i]*z_dir);
      }
      glEnd();
      glDisable(GL_TEXTURE_2D);
    }
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < num_points; i++)
    {
      glNormal3f(Sin(norm_th[i]*z_dir),0.0,
                 Cos(norm_th[i]*z_dir));
      glVertex3d(cross_sec_x[i],
                 stab_height + offset, 
                 cross_sec_z[i]*z_dir);
      glVertex3d(cross_sec_x[i],
                 stab_height - offset,
                 cross_sec_z[i]*z_dir);
    }
    glEnd();
  }
}

/*
 *  Draw (something approximating) a piper cub
 *    at (x,y,z)
 *    nose towards (dx,dy,dz)
 *    up towards (ux,uy,uz)
 */
void drawPiperCub(double x,double y,double z,
  double dx,double dy,double dz,
  double ux,double uy, double uz)
{
   //  Unit vector in direction of flght
  double D0 = sqrt(dx*dx+dy*dy+dz*dz);
  double X0 = dx/D0;
  double Y0 = dy/D0;
  double Z0 = dz/D0;
  //  Unit vector in "up" direction
  double D1 = sqrt(ux*ux+uy*uy+uz*uz);
  double X1 = ux/D1;
  double Y1 = uy/D1;
  double Z1 = uz/D1;
  //  Cross product gives the third vector
  double X2 = Y0*Z1-Y1*Z0;
  double Y2 = Z0*X1-Z1*X0;
  double Z2 = X0*Y1-X1*Y0;
  //  Rotation matrix
  double mat[16];
  mat[0] = X0;   mat[4] = X1;   mat[ 8] = X2;   mat[12] = 0;
  mat[1] = Y0;   mat[5] = Y1;   mat[ 9] = Y2;   mat[13] = 0;
  mat[2] = Z0;   mat[6] = Z1;   mat[10] = Z2;   mat[14] = 0;
  mat[3] =  0;   mat[7] =  0;   mat[11] =  0;   mat[15] = 1;

  // save current transforms
  glPushMatrix();

  // offset, scale and rotate
  glTranslated(x,y,z);
  glMultMatrixd(mat);

  drawFuselage();
  drawWing();
  drawVStab();
  drawHStab();

  glPopMatrix();
}



/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=2.0;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);
      ball(Position[0],Position[1],Position[2] , 0.1);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
      glDisable(GL_LIGHTING);

   //  Draw scene
  drawPiperCub(0,0,0,
               1,0,0,
               0,1,0);

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
     th,ph,dim,fov,mode?"Perspective":"Orthogonal",light?"On":"Off");
   if (light)
   {
      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   }

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Smooth color model
   else if (key == GLUT_KEY_F1)
      smooth = 1-smooth;
   //  Local Viewer
   else if (key == GLUT_KEY_F2)
      local = 1-local;
   else if (key == GLUT_KEY_F3)
      distance = (distance==1) ? 5 : 1;
   //  Toggle ball increment
   else if (key == GLUT_KEY_F8)
      inc = (inc==10)?3:10;
   //  Flip sign
   else if (key == GLUT_KEY_F9)
      one = -one;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(mode?fov:0,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P')
      mode = 1-mode;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   //  Move light
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   //  Ambient level
   else if (ch=='a' && ambient>0)
      ambient -= 5;
   else if (ch=='A' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='d' && diffuse>0)
      diffuse -= 5;
   else if (ch=='D' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='s' && specular>0)
      specular -= 5;
   else if (ch=='S' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
   else if (ch=='t' || ch=='T')
      ntex = 1-ntex;
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project(mode?fov:0,asp,dim);
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode?fov:0,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(400,400);
   glutCreateWindow("Andrew Kramer: hw6");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   // load textures
   texture[0] = LoadTexBMP("bricks.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
