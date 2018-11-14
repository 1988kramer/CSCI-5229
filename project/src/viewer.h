#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QDoubleSpinBox>

class Viewer : public QWidget
{
Q_OBJECT                 //  Qt magic macro
private:
  //QDoubleSpinBox* dim;  //  Spinbox for dim
public:
  Viewer(QWidget* parent=0); //  Constructor
private slots:
  //void setPAR(const QString&); //  Slot to set lighting, axes, and display mode
};

#endif
