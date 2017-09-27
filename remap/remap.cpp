#include <boost/cstdint.hpp>
#include <iostream>

typedef boost::uint64_t mask_word;

boost::uint16_t remapBit(const boost::uint16_t& bitnum)
{
  return ((bitnum/(sizeof(mask_word)*8)+1)*64-1) - (bitnum%(sizeof(mask_word)*8)); 
}

int main(int argc, char *args[])
{
  for(int x=0;x<=100;x++)
    std::cout<<x<<" -> "<<remapBit(x)<<std::endl;
  return 0;
}
