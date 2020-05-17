#include <string.h>
#include <iostream>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <algorithm>
using namespace std;

class Expr {
   public:
      string::iterator start;   // start and end of the string
      string::iterator end;
      vector<Expr> terms ;  // list of terms in this term, NULL if this is the last level
      //constructors
      Expr(string &expr){
        start = expr.begin();
        end = expr.end();
        this->Set();
      };
      Expr(string::iterator start, string::iterator end): start(start), end(end){
        this->Set();
      };
      //methods
      void Set(){
        this->search1();
      };
      bool search1(){
        //search for brackets first
        int cur_b_lvl = 0; //current bracket level
        bool valid = false;
        string::iterator start_loc;
        string::iterator end_loc;
        //cout<<string(start,end)<<"\n";
        for(string::iterator it = start; it!=end; it++){
          if((*it)=='('){
            if ( cur_b_lvl==0){
              if (it!=start)
              terms.push_back(Expr(start,it));
              start_loc = it +1;
            }
            cur_b_lvl++;
          }
          if((*it)==')'){
            if(cur_b_lvl <1){
              throw std::invalid_argument( "unbalanced brackets" );
            }
            if(cur_b_lvl ==1){
              end_loc = it;
              if (end_loc<start_loc)
              throw std::invalid_argument( "empty brackets" );
              terms.push_back(Expr(start_loc,end_loc));
              if(it!=end-1){
                terms.push_back(Expr(it+1,end));
              }
              valid = true;
              break;
            }
            cur_b_lvl--;
          }
        }
        return valid;
      };

};



void disp_content(Expr expr){
  cout<<"This Content:" <<string(expr.start, expr.end)<<"\n";
  cout<<"Sub Contents"<<"\n";
  if (expr.terms.empty()){
    cout<<"none\n";
  }
  else{
    for (vector<Expr>::iterator it = expr.terms.begin(); it!=expr.terms.end(); it++)
      disp_content(*it);
  }
}

int main(){
  string test_string = "(5/(x-3)) + sin(45*log(2n -2))";

  std::string::iterator end_pos = std::remove(test_string.begin(), test_string.end(), ' ');
  test_string.erase(end_pos, test_string.end());

  cout<< test_string; cout<<"\n";

  Expr new_expr(test_string);
  disp_content(new_expr);
  return 0;


}
