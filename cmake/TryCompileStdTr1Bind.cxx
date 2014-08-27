#include <tr1/functional>
void f(void) { }
int
main ()
{
std::tr1::bind(f)();
  ;
  return 0;
}

