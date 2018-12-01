//
//  Viewer Widget
//

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QLayout>
#include "viewer.h"
#include "SlamViz.h"

//
//  Constructor for Viewer widget
//
Viewer::Viewer(QWidget* parent)
    :  QWidget(parent)
{
   //  Set window title
   setWindowTitle(tr("Airplane Viewer Test"));

   //  Create new Lorenz widget
   SlamViz* slam_viz = new SlamViz;

   slam_viz->resize(slam_viz->sizeHint());

   //  Display widget for angles and dimension
   //QLabel* display = new QLabel();
   //QLabel* lighting = new QLabel();
   //QLabel* axes = new QLabel();

   //  Pushbutton to reset view angle
   QPushButton* reset = new QPushButton("Reset View");
   QCheckBox* display = new QCheckBox("Orthogonal");
   QCheckBox* axes = new QCheckBox("Show Axes");
   QPushButton* texture = new QPushButton("Texture");
   QCheckBox* sky_button = new QCheckBox("Show Skybox");
   QCheckBox* inactive = new QCheckBox("Show Inactive Lmrks");
   QDoubleSpinBox* land_lower = new QDoubleSpinBox();
   QCheckBox* track_pose = new QCheckBox("Track Pose With Cam");
   QCheckBox* prev_poses = new QCheckBox("Show Prev Poses");

   QLabel* dim = new QLabel();

   land_lower->setDecimals(2);
   land_lower->setSingleStep(0.01);
   land_lower->setRange(0.01,1.0);
   land_lower->setValue(0.03);

   //  Connect valueChanged() signals to Lorenz slots
   connect(reset, SIGNAL(clicked(void)), slam_viz, SLOT(reset(void)));
   connect(display, SIGNAL(clicked(void)), slam_viz, SLOT(toggleDisplay(void)));
   connect(axes, SIGNAL(clicked(void)), slam_viz, SLOT(toggleAxes(void)));
   connect(texture, SIGNAL(clicked(void)), slam_viz, SLOT(switchTexture(void)));
   connect(sky_button, SIGNAL(clicked(void)), slam_viz, SLOT(toggleSky(void)));
   connect(inactive, SIGNAL(clicked(void)), slam_viz, SLOT(toggleInactive(void)));
   connect(land_lower, SIGNAL(valueChanged(double)), slam_viz, SLOT(setLmrkDispBound(double)));
   connect(track_pose, SIGNAL(clicked(void)), slam_viz, SLOT(togglePoseTrack(void)));
   connect(prev_poses, SIGNAL(clicked(void)), slam_viz, SLOT(togglePrevPoses(void)));
   //  Connect lorenz signals to display widgets
   connect(slam_viz, SIGNAL(dimen(QString)), dim, SLOT(setText(QString)));


   //  Connect combo box to setPAR in myself
   // connect(preset , SIGNAL(currentIndexChanged(const QString&)), this , SLOT(setPAR(const QString&)));
   //  Set layout of child widgets
   QGridLayout* layout = new QGridLayout;
   layout->setColumnStretch(0,100);
   layout->setColumnMinimumWidth(0,100);
   layout->setRowStretch(4,100);

   //  Lorenz widget
   layout->addWidget(slam_viz,0,0,5,1);

   //  Group Display parameters
   QGroupBox* dspbox = new QGroupBox("Display");
   QGridLayout* dsplay = new QGridLayout;
   dsplay->addWidget(reset,1,0);
   dsplay->addWidget(display,4,0);
   dsplay->addWidget(axes,6,0);
   dsplay->addWidget(texture,2,0);
   dsplay->addWidget(sky_button,7,0);
   dsplay->addWidget(new QLabel("Minimum Lmrk Qual"),3,1);
   dsplay->addWidget(land_lower,3,0);
   dsplay->addWidget(dim,5,0);
   dsplay->addWidget(inactive,8,0);
   dsplay->addWidget(track_pose,9,0);
   dsplay->addWidget(prev_poses,10,0);
   dspbox->setLayout(dsplay);
   layout->addWidget(dspbox,2,1);

   //  Overall layout
   setLayout(layout);
}

/*
//  Set SBR, dt & dim in viewer
//
void Viewer::setPAR(const QString& str)
{
   QStringList par = str.mid(2).split(',');
   if (par.size()<5) return;
   s->setValue(par[0].toDouble());
   b->setValue(par[1].toDouble());
   r->setValue(par[2].toDouble());
   dt->setValue(par[3].toDouble());
   dim->setValue(par[4].toDouble());
}
*/
