#include <iostream>
#include <symengine/expression.h>
using SymEngine::Expression;
using namespace std;
int main(int argc, char const *argv[]) {

  cout<<"teating\n";
  Expression x("x");

  auto ex = pow(x+sqrt(Expression(2)), 10);

  cout<< expand(ex);
  return 0;
}
