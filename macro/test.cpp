#include <iostream>
#include <memory>
#include <boost/shared_ptr.hpp>

#define SUPER_COOL(__AWESOME__) \
  virtual void hope4thebest(){ std::cout<<typeid(*this).name()<<" super cool macro is the best "<< typeid(__AWESOME__).name() <<std::endl;} \
  virtual Anime* clone(){ return new __AWESOME__();} \
 \

class Anime
{
  public:
    virtual void bigEyes(){} ;
    SUPER_COOL(Anime)
    
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
    Dog():name("spot"){}
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
   SUPER_COOL(Dog)
};

class Man : public Anime
{
  private:
    std::string name;

  public:
    Man():name("ken"){}
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
   SUPER_COOL(Dog)
};

void conversation(Base* w)
{
  w->speak();
  w->bigEyes();
} 

int main( int argc, char ** argv ) 
{
  Man tim("Tim");
  Dog* pete = new Dog ("Pete");
  Wrapper<Man> manWrapper(tim);
  Wrapper<Dog> dogWrapper(*pete);

  conversation(&manWrapper);
  conversation(&dogWrapper);

  std::cout<<"print hope on pete:"<<std::endl;
  pete->hope4thebest();
  Anime* bassness = pete;
  std::cout<<"hold pete by Anime and hope:"<<std::endl;
  bassness->hope4thebest();
  std::cout<<"clone on the pete:"<<std::endl;
  bassness = pete->clone();
  std::cout<<typeid(bassness).name()<<"  hope on the clone:"<<std::endl;
  std::cout<<"hope on the clone:"<<std::endl;
  bassness->hope4thebest();
}
