#include <iostream>
#include <string>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp>
struct Dog
{
   std::string name;
   int number;
   void clear()
   {
     name="";
     number=0;
   }
};
Dog setdog = {"not set", -1};
void setTheDog(Dog d)
{
  setdog = d;
}
int main( int argc, char ** argv ) {
  freak();
  std::cout<<setdog.name<<" "<<setdog.number<<std::endl;
  Dog d;
  d.name="Fred";
  d.number=100;
  std::cout<<d.name<<" "<<d.number<<std::endl;
  setTheDog(d);
  d.clear();
  std::cout<<d.name<<" "<<d.number<<std::endl;
  std::cout<<setdog.name<<" "<<setdog.number<<std::endl;
}
namespace
{
  std::string freak()
  {
    static int counter = 0;
    std::string freak = "freaky";
    std::cout<<freak<<boost::lexical_cast<std::string>(counter)<<std::endl;
    return freak;
  }
}
  
