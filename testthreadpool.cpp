
#include <tgfframework/util/boost/threadpool.hpp>
#include <iostream>

  using namespace boost::threadpool;

  // Some example tasks
  void first_task(int x)
  {
    std::cout<<"task "<<x <<std::endl;
  }

  void second_task()
  {
    std::cout<<"task 2"<<std::endl;
  }

  void third_task()
  {
    std::cout<<"task 3"<<std::endl;
  }
  
  void execute_with_threadpool()
  {
    // Create a thread pool.
    pool tp(2);
    
    // Add some tasks to the pool.
    tp.schedule(&second_task);
    tp.schedule(&third_task);

    for(int x=1;x<20;x++)
    {
      tp.schedule(boost::bind(&first_task, x));
    }
    // Leave this function and wait until all tasks are finished.
  }
int main()
{
  execute_with_threadpool();
  return 0;
}
