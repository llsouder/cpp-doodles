#include <iostream>
#include <memory>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>

void test(boost::uint64_t& x)
{
  std::cout<<x<<std::endl;
}

int main( int argc, char ** argv ) {

boost::uint64_t x = 15;
boost::bind(test, _1)(x);
return 0;
}
