#include <iostream>
#include <stdlib.h>
#include <stdint.h>a
#include <map>

template<class T>
class EntityID
{
  private:
  T m_id;
  std::map<T, uint16_t> map;

  public:
  EntityID(T id):m_id(id)
  {
    // Hi!
  };

  const int16_t id16()
  {
    return m_id;
  }

  const T id()
  {
    return m_id;
  }
};
template<>
  EntityID<std::string>::EntityID(std::string id):m_id(id)
  {
    std::cout<<"mapping!!!"<<std::endl;
  };
template<>
  const int16_t EntityID<std::string>::id16()
  {
    return 50;
  };


int main()
{
  EntityID<int16_t> blah(5);
  EntityID<uint32_t> blah2(67512);
  EntityID<std::string> blah3("Hi There");
  std::cout << blah.id16() << std::endl;;
  std::cout << blah2.id16() << std::endl;;
  std::cout << blah3.id16() << std::endl;;
  return 1;
}
