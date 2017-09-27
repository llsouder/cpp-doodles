#include <iostream>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/chrono.hpp>

int main( int argc, char ** argv ) {

int mode=(int)strtoul(argv[1], 0, 0);
std::cout<<"you passed in "<<mode<<std::endl;
switch(mode) {
        case 0 :
        case 1 :
        {
            std::cout<<"0 or 1"<<std::endl;
            return 0;
        }
        break;

        case 2 :
        case 3 :
        default :
        {
            std::cout<<"2 or 3 or default"<<std::endl;
            return 0;
        }
        break;
    }
   std::cout<<"exiting the main?  this should never happen"<<std::endl;
   return 0;
}
