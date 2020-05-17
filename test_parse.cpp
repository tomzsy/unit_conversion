#include <string.h>
#include <iostream>
using namespace std;

class Expr {
   public:
      string expr;   // expression of the term in string
      Expr* terms ;  // list of terms in this term, NULL if this is the last level
      Expr(string expr):expr(expr){
        this->Set();
      };
      void Set(){
        this->terms = NULL;
      };

};

int main(){
  string test_string = "5/(x-3) + sin(45*log(2n -2))";
  cout<< test_string; cout<<"\n";
  Expr new_expr(test_string);
  cout <<new_expr.expr; cout<<"\n";
  if (new_expr.terms == NULL){
    cout<< "true";
  }
  return 0;


}
