#include "ui_tims.h"

class TimsGui : public QDialog, public Ui::TimsGui
{
  Q_OBJECT
  public:
    TimsGui();
 
  public Q_SLOTS:
    void shutUp();

};
