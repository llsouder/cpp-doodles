#include <iostream>



class A
{
  public:
    virtual void print()
    {
      std::cout<<"A: print()"<<std::endl;
    }
};
class B : public A
{
  private:
    void print()
    {
      std::cout<<"B: print()"<<std::endl;
    }
};
      
int main()
{
  std::cout<<"hello"<<std::endl;
  A* a = new A();
  a->print();
  B b;
  a=&b;
  //b.print();
  a->print();
  return 0;

}
