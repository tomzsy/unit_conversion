#include <string.h>
#include <iostream>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <algorithm>

using namespace std;



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
        this->search2();
        this->search4("^");
        this->search3();
        this->search4("*/");
        this->search4("+-");
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

          if(it == expr.end()-1){
            //at the end of the test_string
            if(cur_b_lvl!=0)
            throw std::invalid_argument( "unbalanced brackets" );
            if(is_term){
              term_end = it+1;
              terms.push_back(Expr(string(term_start,term_end),term_start-expr.begin(), term_end-expr.begin(),true));
              is_term = false;
            }
          }

        }
      };//searching for terms, and create sub expr for brackets

      void search2(){
        //search for term term
        bool n_term =false;

        //expression validity check
        int n_term_consecutive = 0;
        string term_id = "()";
        for(vector<Expr>::iterator it = terms.begin(); it!= terms.end(); it++){
          if(it->fn.compare(term_id) ==0)
            n_term_consecutive ++;
          else
            n_term_consecutive = 0;
          if (n_term_consecutive==3)
          throw std::invalid_argument( "Invalid expression, consecutive terms without operator" );
        }
        //group terms
        for(vector<Expr>::iterator it = terms.begin(); it <terms.end()-1; it++){
          if(it->fn.compare("()") ==0 && (it+1)->fn.compare("()") ==0){
            it->add_to_fn(*(it+1));
            terms.erase(it+1,it+2);
          }this->search4("");
        }
      }

      void search3(){
        //search for operators
        int n_op_consecutive = 0;// cannot have 2 operators consecutive
        string all_operators = "+-*/";
        std::size_t op_id1;
        std::size_t op_id2;
        std::size_t op_id3;
        //search for dual operators like 8*-7
        for(vector<Expr>::iterator it = terms.begin(); it< terms.end()-2; it++){
          op_id1 = all_operators.find(it->fn);
          op_id2 = all_operators.find((it+1)->fn);
          op_id3 = all_operators.find((it+2)->fn);

          if (it == terms.begin() && op_id1!= string::npos){
            if(all_operators.at(op_id1) !='+' && all_operators.at(op_id1) !='-' )
            throw std::invalid_argument( "Invalid starting operator" );
            if(op_id2!=string::npos)
            throw std::invalid_argument( "Cannot start with two operators" );
            (it)->cast_op(*(it+1));
            terms.erase(it+1,it+2);
            it = it +1;
          }
          else if(op_id1!= string::npos && op_id2!=string::npos){
            //this is an operator
            if(op_id3 != string::npos)
            throw std::invalid_argument( "Invalid expression, consecutive operators" );
            if(all_operators.at(op_id2) !='+' && all_operators.at(op_id2) !='-' ){

            throw std::invalid_argument( "Invalid expression, invalid consecutive operators" );
            }
            (it+1)->cast_op(*(it+2));
            terms.erase(it+2,it+3);
            it = it +1;
          }
        }

      }

      void search4(string op){
        //search for operators op
        std::size_t op_id;
        for(vector<Expr>::iterator it = terms.begin()+1; it< terms.end()-1; it++){
          op_id = op.find(it->fn);
          if(op_id!= string::npos){
            //this is an operator
            (it)->add_to_fn(*(it-1),*(it+1));//add to current element
            it = terms.erase(it-1,it);//erase previous element
            terms.erase(it+1,it+2);//erase next element
          }
        }
      }
      void add_to_fn(Expr arg){
        fn = expr;
        expr = expr+"("+arg.expr+")";
        end = arg.end;
        if (terms.empty() ==0)
        throw std::invalid_argument( "Invalid expression, consecutive bracketed expressions" );
        terms.push_back(arg);
      }
      void cast_op(Expr arg){
        expr = fn+"("+arg.expr+")";
        end = arg.end;
        terms.push_back(arg);
      }
      void add_to_fn(Expr arg1, Expr arg2){
        terms.push_back(arg1); terms.push_back(arg2);
        expr = arg1.expr + fn + arg2.expr;
        start = arg1.start;
        end = arg2.end;
      }

};



void disp_content(Expr expr){
  //cout<<"This Content:" <<expr.expr <<"\n";
  //cout<<"This Operator" <<expr.fn <<"\n";
  //cout<<"Sub Contents"<<"\n";
  if (expr.terms.empty()){
    cout <<expr.expr <<"\n";
  }
  else{
    for (vector<Expr>::iterator it = expr.terms.begin(); it!=expr.terms.end(); it++)
      disp_content(*it);
      cout <<expr.expr <<"\n";
  }
}

string insert_bracket(string str){
  //insert brackets for exponential ie abc^+xyz and abc^-xyz to abc^(...)
  string all_operators = "+-*/^";
  bool is_exp = false;
  bool inserted = false;
  bool finish = false;
  int b_lvl = 0;
  for (string::iterator it=  str.begin(); it!=str.end();it++){
    if (is_exp && ((*it)== '+'|| (*it)=='-') && inserted ==false){
      //insert bracket there
      it = str.insert(it,'(')+2;
      inserted = true;
      b_lvl = 0;
    }
    if((*it)== '('&& inserted)
    b_lvl++;
    if((*it)== ')'&& inserted)
    b_lvl--;
    if(string::npos != all_operators.find(*it) && inserted && b_lvl == 0){
      str.insert(it,')');
      finish = true;
      break;
    }//is operator
    if((*it)== '^')
    is_exp = true;
    else
    is_exp = false;
  }
  if (inserted){
    if(!finish){
      str.push_back(')');
    }
    str =insert_bracket(str);
  }
  return str;
}

int main(int argc, char const *argv[]) {
  string test_string;
  if(argc ==1)
   test_string = "-(5/(-x-3)) + sin(8+45*-log(2n -2)^-2)";
  else
   test_string = string(argv[1]);

  test_string = insert_bracket(test_string);
  cout << test_string<<std::endl;

  std::string::iterator end_pos = std::remove(test_string.begin(), test_string.end(), ' ');
  test_string.erase(end_pos, test_string.end());

  cout<< test_string; cout<<"\n";

  Expr new_expr(test_string);
  disp_content(new_expr);
  return 0;


}
