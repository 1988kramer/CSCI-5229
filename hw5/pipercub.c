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
	glBegin(GL_QUADS);

	// aft tail boom  right side
  double tail_top = 0.17;
  double fwd_tail_top = 0.21;
  double tail = -0.85;
  double tail_boom_front = -0.25;

  glColor3f(1,0,0);
  crossProductNorm(tail, 0.1, 0.0,
               tail_boom_front, fwd_tail_top, 0.0875,
               tail, tail_top, 0.0);
  glVertex3d(tail, tail_top, 0.0);
  glVertex3d(tail, 0.1, 0.0);
  glVertex3d(tail_boom_front, 0.025, 0.0875);
  glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);

  // aft tail boom left side
  glColor3f(0,1,0);
  crossProductNorm(tail_boom_front, fwd_tail_top, -0.0875,
               tail, 0.1, 0.0,
               tail, tail_top, 0.0);
  glVertex3d(tail, 0.1, 0.0);
  glVertex3d(tail, tail_top, 0.0);
  glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);
  glVertex3d(tail_boom_front, 0.025, -0.0875);

  // aft tail boom top
  glColor3f(0,0,1);
  crossProductNorm(tail_boom_front, fwd_tail_top, 0.0875,
               tail_boom_front, fwd_tail_top, -0.875,
               tail, tail_top, 0.0);
  glVertex3d(tail, tail_top, 0.0);
  glVertex3d(tail, tail_top, 0.0);
  glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);
  glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);

  // aft tail boom bottom
  glColor3d(0.5,0.5,0);
  crossProductNorm(tail_boom_front, 0.025, -0.0875,
               tail_boom_front, 0.025, 0.875,
               tail, 0.1, 0.0);
  glVertex3d(tail, 0.1, 0.0);
  glVertex3d(tail, 0.1, 0.0);
  glVertex3d(tail_boom_front, 0.025, -0.0875);
  glVertex3d(tail_boom_front, 0.025, 0.0875);

  double door_top = 0.25;
  double door_bottom = 0.0;
  double fwd_tail_front = -0.1;

  // fwd tail boom right side
  glColor3f(0.5,1,0);
  crossProductNorm(tail_boom_front, 0.025, 0.0875,
               fwd_tail_front, door_top, 0.10,
               tail_boom_front, fwd_tail_top, 0.0875);
  glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);
  glVertex3d(tail_boom_front, 0.025, 0.0875);
  glVertex3d(fwd_tail_front, door_bottom, 0.10);
  glVertex3d(fwd_tail_front, door_top, 0.10);

  // fwd tail boom left side
  glColor3f(0,0.5,1);
  crossProductNorm(fwd_tail_front, door_top, -0.10,
               tail_boom_front, 0.025, -0.0875,
               tail_boom_front, fwd_tail_top, -0.0875);
  glVertex3d(tail_boom_front, 0.025, -0.0875);
  glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);
  glVertex3d(fwd_tail_front, door_top, -0.10);
  glVertex3d(fwd_tail_front, door_bottom, -0.10);

  // fwd tail boom top
  glColor3f(0,1,1);
  crossProductNorm(fwd_tail_front, door_top, 0.10,
               tail_boom_front, fwd_tail_top, -0.875,
               tail_boom_front, fwd_tail_top, 0.875);
  glVertex3d(tail_boom_front, fwd_tail_top, 0.0875);
  glVertex3d(fwd_tail_front, door_top, 0.10);
  glVertex3d(fwd_tail_front, door_top, -0.10);
  glVertex3d(tail_boom_front, fwd_tail_top, -0.0875);

  // fwd tail boom bottom
  glColor3f(0,0.5,0.5);
  crossProductNorm(fwd_tail_front, door_bottom, -0.10,
               tail_boom_front, 0.025, 0.0875,
               tail_boom_front, 0.025, -0.0875);
  glVertex3d(tail_boom_front, 0.025, -0.0875);
  glVertex3d(tail_boom_front, 0.025, 0.0875);
  glVertex3d(fwd_tail_front, door_bottom, 0.10);
  glVertex3d(fwd_tail_front, door_bottom, -0.10);

  // right door
  glColor3f(0.5,0.5,0.5);
  crossProductNorm(fwd_tail_front, door_bottom, 0.10,
               0.25, door_top, 0.10,
               fwd_tail_front, door_top, 0.10);
  glVertex3d(fwd_tail_front, door_top, 0.10);
  glVertex3d(fwd_tail_front, door_bottom, 0.10);
  glVertex3d(0.25, door_bottom, 0.10);
  glVertex3d(0.25, door_top, 0.10);

  // left door
  glColor3f(0.5,0,0.5);
  crossProductNorm(0.25, door_top, -0.10,
               fwd_tail_front, door_bottom, -0.10,
               fwd_tail_front, door_top, -0.10);
  glVertex3d(fwd_tail_front, door_bottom, -0.10);
  glVertex3d(fwd_tail_front, door_top, -0.10);
  glVertex3d(0.25, door_top, -0.10);
  glVertex3d(0.25, door_bottom, -0.10);

  // belly
  glColor3f(0.5,0.25,0.25);
  crossProductNorm(0.25, door_bottom, -0.1,
               fwd_tail_front, door_bottom, 0.1,
               fwd_tail_front, door_bottom, -0.1);
  glVertex3d(fwd_tail_front, door_bottom, 0.1);
  glVertex3d(fwd_tail_front, door_bottom, -0.1);
  glVertex3d(0.25, door_bottom, -0.1);
  glVertex3d(0.25, door_bottom, 0.1);

  // leave roof open, it will be covered by the wing

  double cowling_top = 0.18;
  double cowling_bottom = 0.01;
  double cowling_side = 0.09;
  double firewall = 0.33;

  // windscreen
  glColor3f(0.1,0.5,0.1);
  crossProductNorm(firewall, cowling_top, cowling_side,
               0.25, door_top, -0.1,
               0.25, door_top, 0.1);
  glVertex3d(0.25, door_top, 0.1);
  glVertex3d(0.25, door_top, -0.1);
  glVertex3d(firewall, cowling_top, -1. * cowling_side);
  glVertex3d(firewall, cowling_top, cowling_side);

  // fwd fuselage right
  glColor3f(0.25,0.25,0.1);
  crossProductNorm(0.25, door_bottom, 0.1,
               firewall, cowling_top, cowling_side,
               0.25, door_top, 0.1);
  glVertex3d(0.25, door_top, 0.1);
  glVertex3d(0.25, door_bottom, 0.1);
  glVertex3d(firewall, cowling_bottom, cowling_side);
  glVertex3d(firewall, cowling_top, cowling_side);

  // fwd fuselage left
  glColor3f(0.5, 0.1, 0.1);
  crossProductNorm(firewall, cowling_top, -1. * cowling_side,
               0.25, door_bottom, -0.1,
               0.25, door_top, -0.1);
  glVertex3d(0.25, door_bottom, -0.1);
  glVertex3d(0.25, door_top, -0.1);
  glVertex3d(firewall, cowling_top, -1. * cowling_side);
  glVertex3d(firewall, cowling_bottom, -1. * cowling_side);

  // fwd belly
  glColor3f(0.2, 0.1, 0.2);
  crossProductNorm(0.25, door_bottom, -0.1,
               firewall, cowling_bottom, cowling_side,
               0.25, door_bottom, 0.1);
  glVertex3d(0.25, door_bottom, -0.1);
  glVertex3d(firewall, cowling_bottom, -1. * cowling_side);
  glVertex3d(firewall, cowling_bottom, cowling_side);
  glVertex3d(0.25, door_bottom, 0.1);

  glEnd();

  // aft cowling
  double cowl_y_center = 0.5 * (cowling_top + cowling_bottom);
  glColor3f(0.1,0.6,0.2);
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
    glVertex3d(firewall, cowling_top, horiz_position);

    glNormal3f(Sin(fwd_cowl_angle),Cos(th),Sin(th));
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
    glVertex3d(firewall, vert_position, cowling_side);

    glNormal3f(Sin(fwd_cowl_angle),Cos(th),Sin(th));
    glVertex3d(px,py,pz);
    
    vert_position -= vert_increment;
  }
  horiz_position -= horiz_increment;
  for(double th = 135; th <= 225; th += 22.5)
  {
    double px, py, pz;
    pointOnCircle2(th,radius,fwd_cowl,cowl_y_center,0.0,&px,&py,&pz);
    getCowlNorms(firewall, cowling_bottom, px, py, th);
    glVertex3d(firewall, cowling_bottom, horiz_position);

    glNormal3f(Sin(fwd_cowl_angle),Cos(th),Sin(th));
    glVertex3d(px,py,pz);
    
    horiz_position -= horiz_increment;
  }
  vert_position += vert_increment;
  for(double th = 225; th <= 315; th += 22.5)
  {
    double px, py, pz;
    pointOnCircle2(th,radius,fwd_cowl,cowl_y_center,0.0,&px,&py,&pz);
    getCowlNorms(firewall, vert_position, px, py, th);
    glVertex3d(firewall, vert_position, -1 * cowling_side);

    glNormal3f(Sin(fwd_cowl_angle),Cos(th),Sin(th));
    glVertex3d(px,py,pz);
    
    vert_position += vert_increment;
  }

  glEnd();

  // fwd cowling
  double nose = 0.49;
  glColor3f(0.7,0.1,0.3);
  glBegin(GL_TRIANGLE_STRIP);
  
  for (double th = 0; th <= 360; th += 22.5)
  {
    double px, py, pz;
    pointOnCircle2(th,radius,fwd_cowl,cowl_y_center,0.0,&px,&py,&pz);
    glNormal3f(Sin(fwd_cowl_angle), Cos(th), Sin(th));
    glVertex3d(px,py,pz);

    glNormal3f(1,0,0);
    glVertex3d(nose, cowl_y_center, 0.0);
  }

  glEnd();

}

static void drawWing()
{
  // define wing cross section
  int num_points = 6;
  double cross_sec_x[] = {0.25, 0.26, 0.25, 0.23, 0.20, -0.1};
  double cross_sec_y[] = {0.25, 0.26, 0.27, 0.28, 0.29, 0.25};

  double wingtip = 1.2;

  glColor3f(0.3,0.1,0.4);
  for (int j = 0; j < 2; j++)
  {
    wingtip *= -1.0;

    glBegin(GL_POLYGON);
    glNormal3f(0,0,wingtip);
    for (int i = 0; i < num_points; i++)
    {
      glVertex3d(cross_sec_x[i], cross_sec_y[i], wingtip);
    }
    glEnd();
  }

  glColor3f(0.8,0.1,0.1);
  glBegin(GL_QUAD_STRIP);
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

    glVertex3d(cross_sec_x[i], cross_sec_y[i], wingtip);
    glVertex3d(cross_sec_x[i], cross_sec_y[i], -1.0*wingtip);
  }
  glEnd();
  glBegin(GL_QUADS);
  glNormal3f(0,-1,0);
  glVertex3d(cross_sec_x[num_points-1],cross_sec_y[num_points-1],-1.0*wingtip);
  glVertex3d(cross_sec_x[num_points-1],cross_sec_y[num_points-1],wingtip);
  glVertex3d(cross_sec_x[0], cross_sec_y[0], wingtip);
  glVertex3d(cross_sec_x[0], cross_sec_y[0], -1.0*wingtip);
  glEnd();
}

static void drawVStab()
{
  int num_points = 7;
  double norm_th[] = {270, 270, 270, 200, 140, 90, 50};
  double cross_sec_x[] = {-0.65, -0.85, -0.85, -0.95, -0.90, -0.85, -0.80};
  double cross_sec_y[] = {0.17, 0.17, 0.11, 0.14, 0.35, 0.38, 0.35};
  double offset = 0.005;
  double direction = -1.0;

  glColor3f(0.1,0.8,0.3);
  
  // draw sides of v-stab
  for (int j = 0; j < 2; j++)
  {
    direction *= -1.0;
    glBegin(GL_POLYGON);
    glNormal3f(0,0,direction);
    for (int i = 0; i < num_points; i++)
    {
      glVertex3d(cross_sec_x[i], cross_sec_y[i], direction*offset);
    }
    glEnd();
  }

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
  double cross_sec_x[] = {-0.65, -0.66, -0.71, -0.88, -0.95, -0.95, -0.87};
  double cross_sec_z[] = {0.0, 0.07, 0.15, 0.30, 0.25, 0.08, 0.0};

  glColor3f(0.7,0.3,0.5);
  
  for (int j = 0; j < 2; j++)
  {
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_points; i++)
    {
      cross_sec_z[i] *= -1.0;
      glVertex3d(cross_sec_x[i], stab_height, cross_sec_z[i]);
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
