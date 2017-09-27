#include <iostream>
#include <string>

int main( int argc, char ** argv ) {
  std::string name = "/vsim/Terrain/KRDR/EARTH-KRDR.earth";
  std::string ext = name.substr(name.find_last_of(".") + 1);
  bool isOsgEarthFile = ext=="earth";
  std::cout<<"ext = "<<ext<<" isOsgEarthFile = "<<isOsgEarthFile<<std::endl;

}
