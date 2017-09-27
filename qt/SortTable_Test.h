#include "ui_basic_gui.h"

class SortTableTest : public QDialog, public Ui::TableDialog 
{
  Q_OBJECT
  public:
    SortTableTest();

  public Q_SLOTS:
    void addNewData();

};

