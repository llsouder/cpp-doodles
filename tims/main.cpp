#include "TimsGui.h"
int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  TimsGui ui;
  //ui.setupUi(widget);

  ui.show();
  return app.exec();
}
