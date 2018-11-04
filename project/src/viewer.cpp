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
   QPushButton* axes = new QPushButton("Axes");
   QPushButton* texture = new QPushButton("Texture");

   //  Connect valueChanged() signals to Lorenz slots
   connect(reset, SIGNAL(clicked(void)), slam_viz, SLOT(reset(void)));
   connect(display, SIGNAL(clicked(void)), slam_viz, SLOT(toggleDisplay(void)));
   connect(lighting, SIGNAL(clicked(void)), slam_viz, SLOT(toggleLight(void)));
   connect(axes, SIGNAL(clicked(void)), slam_viz, SLOT(toggleAxes(void)));
   connect(texture, SIGNAL(clicked(void)), slam_viz, SLOT(switchTexture(void)));
   //  Connect lorenz signals to display widgets
   // connect(slam_viz , SIGNAL(dimen(double))   , dim    , SLOT(setValue(double)));



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
