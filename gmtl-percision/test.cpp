#include <iostream>
#include <gmtl/Output.h>
#include <cstdio>
#include <cfloat>
#include <cmath>

#include <iomanip>

int main()
{
  std::cout<<"testing"<<std::endl;
  gmtl::Vec3d vec;
  vec[0]=1.0/7.0;
  std::cout<<"testing:"<<std::setprecision(51)<<vec[0]<<std::endl;
  std::cout<<"testing:"<<(1.0/7.0)<<std::endl;

  double x = 1.0/7.0;
  printf("FLT_RADIX = %d\n", FLT_RADIX);
  printf("DBL_DIG = %d\n", DBL_DIG);
  printf("DBL_MANT_DIG = %d\n", DBL_MANT_DIG);
  printf("%.17g\n%.17g\n%.17g\n", nextafter(x, 0.0), x, nextafter(x, 1.0));
  std::cout<<"testing:"<<x<<std::endl;

}
