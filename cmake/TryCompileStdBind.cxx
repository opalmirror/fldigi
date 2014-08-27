#include <functional>
void f(void) { }
int
main ()
{
std::bind(f)();
  ;
  return 0;
}

