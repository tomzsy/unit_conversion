#include <string.h>
#include <iostream>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <algorithm>
#define TYP_T 1 //term type
#define TYP_O 2 //operator type

using namespace std;

const string all_operators = "!,+-*/^";

class Expr {
   public:
      int start;   // start and end of the string
      int end;
      string expr;
      string fn = "()"; //default type is a term
      vector<Expr> terms ;  // list of terms in this term
      //constructors
      Expr(string input_expr):expr(input_expr){
        start = 0;
        end = expr.end()-expr.begin();
        this->Set();
      }; //root expression
      Expr(string input_expr, int start, int end): // bracket sub-expression
      expr(input_expr),start(start), end(end){
        this->Set();
      };
      Expr(string input_expr, int start, int end, bool is_term):
      start(start), end(end){
        if(is_term)
        expr = input_expr;
        else
        fn = input_expr;
      }; //No need to set, is only a single term or a operator

      //methods
      void Set(){
        this->search1();
      };
      void search1(){
        //search for terms and brackets
        int cur_b_lvl = 0; //current bracket level
        string::iterator start_loc;//start location of brackets
        string::iterator end_loc;//end location of brackets
        bool is_term =false;// is currently at a "term"
        string::iterator term_start;//start of term
        string::iterator term_end;
        for(string::iterator it = expr.begin(); it!=expr.end(); it++){
          switch (*it){
            case ')':
            break;
            case '!':
            case '+':
            case '-':
            case '*':
            case '/':
            case '^':
            if(cur_b_lvl == 0){
              if(is_term){
                //there is a term beforehand
                term_end = it;
                terms.push_back(Expr(string(term_start,term_end),term_start-expr.begin(), term_end-expr.begin(),true));
                is_term = false;
              }
              terms.push_back(Expr( string(1,*it),it-expr.begin(), it-expr.begin()+1,false));
            }
            break;
            case '(':
            if(cur_b_lvl == 0&& is_term){
                //there is a term beforehand
                term_end = it;
                terms.push_back(Expr(string(term_start,term_end),term_start-expr.begin(), term_end-expr.begin(),true));
                is_term = false;
            }
            break;
            default:
            if(cur_b_lvl == 0 && is_term ==false){
            is_term = true;
            term_start = it;
            }
          };
          if((*it)=='('){
            if ( cur_b_lvl==0)
              start_loc = it +1;
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
              terms.push_back(Expr(string(start_loc,end_loc),start_loc-expr.begin()-1, end_loc-expr.begin()+1));
            }
            cur_b_lvl--;
          }

        }
      };
/*
      void search2(){
        //search for term term
        string::iterator it = start;
        vector<Expr>::iterator it_vec = this->terms.begin();
        string::iterator it_term ;
        for(; it!= end;){
          if(it == it_vec->start-1){
            //we are at the bracket (

          }
        }
        all_operators.find()
      }
      */
};



void disp_content(Expr expr){
  cout<<"This Content:" <<expr.expr <<"\n";
  cout<<"This Operator" <<expr.fn <<"\n";
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
