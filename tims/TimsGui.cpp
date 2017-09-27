#include <iostream>
#include "TimsGui.h"


TimsGui::TimsGui() : QDialog(), Ui::TimsGui()
{
  setupUi(this);
}

void TimsGui::shutUp()
{
  std::cout<<"SHUT UP!!!!!!"<<std::endl;
}
