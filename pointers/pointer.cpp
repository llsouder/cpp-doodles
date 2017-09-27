#include <iostream>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/chrono.hpp>
class Dog
{
  private:
    std::string name;
  public:
    Dog(std::string dogname):name(dogname){std::cout<<name<<" made... woof woof"<<std::endl;}
     void speak()
     {
       std::cout<<"I am a dog named "<<name<<std::endl;
     }
    ~Dog(){std::cout<<name<<" is gone... RIP"<<std::endl;}

};
class Man
{
  private:
    std::string name;
    std::auto_ptr<Dog> fido;
    boost::shared_ptr<Dog> spot;

  public:
    Man(std::string name, std::string dog1name, std::string dog2name):name(name), fido(new Dog(dog1name)), spot(new Dog(dog2name))
    {
      std::cout<<name<<" made!"<<std::endl;
      std::cout<<name<<"***********************"<<std::endl;
    }
    void sharesFidoWith(Man& man)
    {
      std::cout<<name<<" sharing fido"<<std::endl;
      man.fido = this->fido; 
    }
    void sharesSpotWith(Man& man)
    {
      std::cout<<name<<" sharing spot"<<std::endl;
      man.spot = this->spot; 
    }
    ~Man(){std::cout<<name<<" is gone!"<<std::endl;}
     boost::shared_ptr<Dog> getDog()
     {
       return spot;
     }
};
int main( int argc, char ** argv ) {
boost::chrono::system_clock::time_point tp1 = boost::chrono::system_clock::now();
  Man* fred = new Man("Fred", "fido1", "spot1");
  Man pete("Pete", "fido2", "spot2");
  boost::shared_ptr<Dog> spot = pete.getDog();
  boost::shared_ptr<Dog> spot2 = pete.getDog();
  spot.reset();
  spot2->speak();
  std::cout<<"giggles..."<<std::endl;
  fred->sharesFidoWith(pete);
  pete.sharesSpotWith(*fred);
  std::cout<<"delete fred..."<<std::endl;
  delete fred; 
 
std::cout<<tp1<<std::endl;
usleep(10000);//10000us = 10ms
boost::chrono::system_clock::time_point tp2 = boost::chrono::system_clock::now();
std::cout<<tp2<<std::endl;
typedef boost::chrono::microseconds ms;
ms ms4u = boost::chrono::duration_cast<ms>(tp2-tp1);
std::cout<<"duration in ms = "<<ms4u<<std::endl;
std::cout<<"end of program..."<<std::endl;
}
