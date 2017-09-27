#include <iostream>
#include <memory>
#include <boost/shared_ptr.hpp>
class Anime
{
  public:
    virtual void bigEyes() = 0;
};
class Base : public Anime
{
  public:
    virtual void speak() = 0;
    virtual ~Base(){}
};

template <class T>
class Wrapper: public Base
{
  private:
    T data; 
  public:
   Wrapper(T t):data(t){}
   void speak() { data.speak(); }
   void bigEyes() { data.bigEyes(); }
   ~Wrapper(){}
};
class Dog :public Anime
{
  private:
    std::string name;
  public:
    Dog(std::string dogname):name(dogname){}
    ~Dog(){}
   void speak()
   {
     std::cout<<"rough, roof, ruff"<<std::endl;
   }
   void bigEyes()
   {
     std::cout<<"I can make it on my own"<<std::endl;
   }
};

class Man : public Anime
{
  private:
    std::string name;

  public:
    Man(std::string name):name(name)
    {
    }
    void speak()
    {
      std::cout<<"Hello World!"<<std::endl;
    }
   void bigEyes()
   {
     std::cout<<"Is that a challenge"<<std::endl;
   }
    ~Man(){}
};

void conversation(Base* w)
{
  w->speak();
  w->bigEyes();
} 

int main( int argc, char ** argv ) 
{
  Man tim("Tim");
  Dog pete("Pete");
  Wrapper<Man> manWrapper(tim);
  Wrapper<Dog> dogWrapper(pete);

  conversation(&manWrapper);
  conversation(&dogWrapper);
}
