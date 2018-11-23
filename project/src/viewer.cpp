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

   //  Display widget for angles and dimension
   //QLabel* display = new QLabel();
   //QLabel* lighting = new QLabel();
   //QLabel* axes = new QLabel();

   //  Pushbutton to reset view angle
   QPushButton* reset = new QPushButton("Reset");
   QPushButton* display = new QPushButton("Display");
   QPushButton* lighting = new QPushButton("Lighting");
   QCheckBox* axes = new QCheckBox("Axes");
   QPushButton* texture = new QPushButton("Texture");
   QCheckBox* sky_button = new QCheckBox("Sky");
   QDoubleSpinBox* land_lower = new QDoubleSpinBox();
   //QDoubleSpinBox* land_upper = new QDoubleSpinBox("max lmrk quality");

   QLabel* dim = new QLabel();

   land_lower->setDecimals(2);
   land_lower->setSingleStep(0.01);
   land_lower->setRange(0.0,1.0);
   land_lower->setValue(0.0);

   //  Connect valueChanged() signals to Lorenz slots
   connect(reset, SIGNAL(clicked(void)), slam_viz, SLOT(reset(void)));
   connect(display, SIGNAL(clicked(void)), slam_viz, SLOT(toggleDisplay(void)));
   connect(lighting, SIGNAL(clicked(void)), slam_viz, SLOT(toggleLight(void)));
   connect(axes, SIGNAL(clicked(void)), slam_viz, SLOT(toggleAxes(void)));
   connect(texture, SIGNAL(clicked(void)), slam_viz, SLOT(switchTexture(void)));
   connect(sky_button, SIGNAL(clicked(void)), slam_viz, SLOT(toggleSky(void)));
   connect(land_lower, SIGNAL(valueChanged(double)), slam_viz, SLOT(setLmrkDispBound(double)));
   //  Connect lorenz signals to display widgets
   connect(slam_viz, SIGNAL(dimen(QString)), dim, SLOT(setText(QString)));


   //  Connect combo box to setPAR in myself
   //connect(preset , SIGNAL(currentIndexChanged(const QString&)), this , SLOT(setPAR(const QString&)));
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
   dsplay->addWidget(display,2,0);
   dsplay->addWidget(lighting,3,0);
   dsplay->addWidget(axes,4,0);
   dsplay->addWidget(texture,5,0);
   dsplay->addWidget(sky_button,6,0);
   dsplay->addWidget(new QLabel("min lmrk qual"),7,0);
   dsplay->addWidget(land_lower,7,1);
   dsplay->addWidget(dim,8,0);
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
