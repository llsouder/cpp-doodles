#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <boost/cstdint.hpp>

void printlist(std::vector<int> types)
{
  std::cout<<"*********start********** "<<std::endl;
for(int i=0; i<types.size();i++)
{
  std::cout<<i<<" : "<<types[i]<<std::endl;
}
  std::cout<<"**********end********* "<<std::endl;
}

std::vector<int> getRange(std::vector<int> types, int min, int max)
{
std::vector<int> newtypes;
for(int i=0; i<types.size();i++)
{
  if(types[i]>=min && types[i]<=max)  
  {
    newtypes.push_back(types[i]);
  }
}
return newtypes;
}

int main( int argc, char ** argv ) 
{
int min = 1;
int max = 3;

std::vector<int> types;
types.push_back(1);
types.push_back(2);
types.push_back(3);
types.push_back(5);
types.push_back(100);
types.push_back(101);
types.push_back(102);
types.push_back(103);
types.push_back(105);

printlist(types);

//find next unused value
std::vector<int> newtypes = getRange(types, min, max);
int newvalue=min;
//while(number is found and lessEq to max) get next number
while(std::find(newtypes.begin(), newtypes.end(), newvalue)!=newtypes.end() && newvalue<=max)
  newvalue++;

if(newvalue>max) 
  std::cout<<"you ran out of numbers"<<std::endl;
else
  std::cout<<"newvalue = " <<newvalue<<std::endl;

//find next largest value
newvalue = newtypes.back()+1;
std::cout<<"newvalue = " <<newvalue<<std::endl;
return 0;
}
