#include <iostream>       // std::cout
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <mutex>
#include <stdexcept> // Opcional: solo si usas excepciones estándar

std::mutex mtx;   // mutex for critical section

void pause_thread(int n, int s) 
{
  std::this_thread::sleep_for (std::chrono::seconds(n));

  try {
    mtx.lock();

    int ii = 7;

    std::cout << "pause of " << n << " seconds ended," << " yo soy " << s << std::endl;
    throw (std::logic_error("error"));
  
    mtx.unlock();
  }
  catch (std::logic_error&) {
    std::cout << "[exception caught]\n";
  }
}
 
//

int main() 
{
  std::cout << "Spawning and detaching 3 threads...\n";
  
  for (int i = 1; i < 100; i++){
    std::thread (pause_thread,2,i).detach();
  }

  std::cout << "Done spawning threads.\n";

  std::cout << "(the main thread will now pause for 5 seconds)\n";
  // give the detached threads time to finish (but not guaranteed!):
  pause_thread(66,1000);
  return 0;
}

//ps -eLf | egrep thread.exe
//ps -eLf | egrep thread filtro para ver todos los threads
//g++ -o th.exe threads.cpp