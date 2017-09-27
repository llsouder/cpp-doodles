#include <iostream>

struct C {
  C() {}
  C(const C&) { std::cout << "A copy was made.\n"; }
  std::string name;
};
 
C f() {
  //copy is returned
  return C();
}
 
int main() {
  std::cout << "Hello World!\n";
  //assignment makes a copy
  C obj = f();
  //output without lines below show compiler
  //has Return Value Optimization because
  //only "Hello World" is printed

  //assignment makes a copy
  C obj2 = obj;
  //copy was made is printed
  return 0;
}
