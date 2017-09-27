#include <iostream>
#include <vector>

vector<int> copyVecFast(vec<int>& original)
{

  vector<int> newVec;
  newVec.swap(original); 
  return newVec;
}

int main( int argc, char ** argv ) {
  vector<int> orig;
  orig.push_back(10);
  orig.push_back(11);
  orig.push_back(13);
  orig.push_back(14);
  vector<int> newVec;
  newVec.swap(orig); 
  std::cout<<newVec<<std::endl;

}
