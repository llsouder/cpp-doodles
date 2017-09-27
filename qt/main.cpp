#include <QApplication>
#include "SortTable_Test.h"


int main(int argc, char *argv[]) {

  QApplication app(argc, argv);


  SortTableTest widget;

  widget.show();
  return app.exec();
}
