#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

class Dog
{
  private:
    std::string name;
  public:
    Dog():name("Tim")
    {
      std::cout<<name<<" made... woof woof"<<std::endl;
    }

    //Dog():Dog("Tim"){}

    void speak()
    {
      std::cout<<"I am a dog named "<<name<<std::endl;
    }

    ~Dog(){std::cout<<name<<" is gone... RIP"<<std::endl;}

};
//class Man
//{
  //private:
    //std::string name;
    //std::auto_ptr<Dog> fido;
    //boost::shared_ptr<Dog> spot;
//
  //public:
    //Man(std::string name, std::string dog1name, std::string dog2name):name(name), fido(new Dog(dog1name)), spot(new Dog(dog2name))
    //{
      //std::cout<<name<<" made!"<<std::endl;
      //std::cout<<name<<"***********************"<<std::endl;
    //}
    //void sharesFidoWith(Man& man)
    //{
      //std::cout<<name<<" sharing fido"<<std::endl;
      //man.fido = this->fido; 
    //}
    //void sharesSpotWith(Man& man)
    //{
      //std::cout<<name<<" sharing spot"<<std::endl;
      //man.spot = this->spot; 
    //}
    //~Man(){std::cout<<name<<" is gone!"<<std::endl;}
     //boost::shared_ptr<Dog> getDog()
     //{
       //return spot;
     //}
//};
//
int y=0;

int f(int x = y)
{
  return x;
}

int main( int argc, char ** argv ) {
   //std::cout<<f()<<std::endl;
   //y=50000;
   //std::cout<<f()<<std::endl;
   //std::cout<<f(1000)<<std::endl;
//
   //std::cout<<"here1"<<std::endl;
   //std::map<std::string, Dog> map;
   //std::cout<<"here2"<<std::endl;
   //Dog d;
   //d.speak();
   //Dog d1 = d;
   //Dog& d = map["test"];
   //map["test"] = d;
   //std::cout<<"here3"<<std::endl;
   std::vector<int> vec;
   std::cout<<"vector is empty"<<std::endl;
   std::cout<<vec.empty()<<std::endl;
   vec.push_back(5);
   std::cout<<"vector is NOT empty"<<std::endl;
   std::cout<<vec.empty()<<std::endl;
   std::cout<<vec[0]<<std::endl;
   
}
