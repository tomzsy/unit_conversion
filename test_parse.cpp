#include <string.h>
#include <iostream>
#include <cctype>
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
      void s1(){
        //search for brackets first
        for(int i=0;i<expr.size();i++){
        }
      };

};

string str_no_space(string str){
  for(string::iterator i=str.begin();i!=str.end();i++){
    while (i!=str.end()&& isspace(*i)){
      str.erase(i);
    }
  }
  return str;
}

int main(){
  string test_string = "5/(x-3) + sin(45*log(2n -2))";
  cout<< test_string; cout<<"\n";
  string new_string = str_no_space(test_string);
  
  Expr new_expr(new_string);
  cout <<new_expr.expr; cout<<"\n";

  if (new_expr.terms == NULL){
    cout<< "true";
  }
  return 0;


}
