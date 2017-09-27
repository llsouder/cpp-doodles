#include <iostream>

int main( int argc, char ** argv ) {
  int v = -5;
  std::cout<<v<<" "<<(v<=0?"true":"false")<<std::endl;
  v = 0;
  std::cout<<v<<" "<<(v<=0?"true":"false")<<std::endl;
  v = 1;
  std::cout<<v<<" "<<(v<=0?"true":"false")<<std::endl;

}
