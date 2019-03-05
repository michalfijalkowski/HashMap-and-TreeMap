#include <cstddef>
#include <cstdlib>
#include <string>
#include <chrono>
#include <ctime>
#include <iostream>
 
#include "TreeMap.h"
#include "HashMap.h"
 
namespace
{
 
template <typename K, typename V>
using Map = aisdi::HashMap<K, V>;
template <typename K, typename V>
using Tree = aisdi::TreeMap<K, V>;
 
void perfomTest(std::size_t repeatCount, std::size_t tableSize)
{
  (void)repeatCount;
  (void)tableSize;
  std::chrono::time_point<std::chrono::system_clock> start, end;
  std::chrono::duration<double> elapsed_seconds;
 
  Map<int, std::string> map(tableSize);
  Tree<int, std::string> tree;
 
  std::cout << "time for operation:\n";
  //HashMap - add
  {
 
    start = std::chrono::system_clock::now();
    for (std::size_t i = 0; i < repeatCount; i++)
      map[i]= std::rand();
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "\t add in HashMap \t\t time: " << elapsed_seconds.count() << "s\n";
  }
  //TreeMap - add
  {
 
    start = std::chrono::system_clock::now();
    for (std::size_t i = 0; i < repeatCount; i++)
      tree[i]= std::rand();
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "\t add in TreeMap \t\t time: " << elapsed_seconds.count() << "s\n";
  }
 
  //HashMap - remove
  {
    start = std::chrono::system_clock::now();
    for (std::size_t i = 0; i < repeatCount; i++)
      map.remove(i);
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "\t erase elements in HashMap   \t time: " << elapsed_seconds.count() << "s\n";
  }
  //TreeMap - remove
  {
    start = std::chrono::system_clock::now();
    for (std::size_t i = 0; i < repeatCount; i++)
      tree.remove(i);
    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "\t erase elements in TreeMap    \t time: " << elapsed_seconds.count() << "s\n";
  }
}
 
} // namespace
 
int main(int argc, char** argv)
{
  const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 10000;
  const std::size_t tableSize   = argc > 2 ? std::atoll(argv[2]) : 10000;
  //for (std::size_t i = 0; i < repeatCount; ++i)
  perfomTest(repeatCount, tableSize);
  perfomTest(20000, 20000);
  perfomTest(30000, 30000);
  return 0;
}
