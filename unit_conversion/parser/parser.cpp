//  Copyright 2020 Tom Zheng

#include <string.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <vector>

class Expr;
typedef std::string::iterator string_it;
typedef std::string string;
typedef std::vector<Expr>::iterator term_it;


class Expr {
 public:
      static Expr Init(string input_str) {
        Expr expression = Expr();
        expression.InputExprPreprocess(&input_str);
        expression.start_ = 0;
        expression.end_ = input_str.end()-input_str.begin();
        expression.expr_ = input_str;
        expression.Set();
        return expression;
      }
      bool IsValid() {
        return this->is_valid_;
      }

      std::string ErrorMsg() {
        return this->err_msg_;
      }

      void DisplayContent() {
        if (this->terms_.empty()) {
          std::cout <<this->expr_ <<"\n";
        } else {
          for (term_it it = this->terms_.begin();
           it != this->terms_.end(); it++)
            it->DisplayContent();
            std::cout <<this->expr_ <<"\n";
        }  // else
      }  // a debuging method

      Expr(const Expr &other) {
        // data members
        is_valid_ = other.is_valid_;
        err_msg_ = other.err_msg_;
        start_ = other.start_;
        end_ = other.end_;
        expr_ = other.expr_;
        fn_ = other.fn_;
        terms_ = other.terms_;
      }

 private:
      Expr() {}  // use Init(str) to construct object

      Expr Init(string input_expr, int start, int end) {
        // sub expression created from base expression
        Expr expression = Expr();
        expression.start_ = start;
        expression.end_ = end;
        expression.expr_ = input_expr;
        expression.Set();
        return expression;
      }
      Expr Init(string input_expr, int start, int end, bool is_term) {
        // sub term created from base expression
        Expr expression = Expr();
        expression.start_ = start;
        expression.end_ = end;
        if (is_term)
        expression.expr_ = input_expr;
        else
        expression.fn_ = input_expr;
        return expression;
      }

      void InputExprPreprocess(string *input_str) {
        InsertBracket(input_str);
        string_it end_pos = std::remove(input_str->begin(),
         input_str->end(), ' ');
        input_str->erase(end_pos, input_str->end());
      }

      void InsertBracket(string *str) {
        // insert brackets for exponential ie abc^+xyz and abc^-xyz to abc^(...)
        string all_operators = "+-*/^";
        bool is_exp = false;
        bool inserted = false;
        bool finish = false;
        int b_lvl = 0;
        for (string_it it=  str->begin(); it != str->end(); it++) {
          if (is_exp && ((*it) == '+'|| (*it) == '-') && inserted == false) {
            // insert bracket there
            it = str->insert(it, '(')+2;
            inserted = true;
            b_lvl = 0;
          }
          if ((*it)== '('&& inserted)
          b_lvl++;
          if ((*it)== ')'&& inserted)
          b_lvl--;
          if (string::npos != all_operators.find(*it) && inserted
            && b_lvl == 0) {
            str->insert(it, ')');
            finish = true;
            break;
          }  // is operator
          if ((*it)== '^')
          is_exp = true;
          else
          is_exp = false;
        }
        if (inserted) {
          if (!finish) {
            str->push_back(')');
          }
          InsertBracket(str);
        }
      }

      void Set() {
        this->Search1();  // search for terms and brackets
        if (!is_valid_) return;
        this->Search2();  // search for two consecutive terms
        if (!is_valid_) return;
        this->Search4("^");  // search for ^ operators
        if (!is_valid_) return;
        this->Search3();   // search for dual operators like 2/-5
        if (!is_valid_) return;
        this->Search4("*/");
        if (!is_valid_) return;
        this->Search4("+-");
        if (!is_valid_) return;
      }

      void Search1() {
        // search for terms and brackets
        int cur_b_lvl = 0;  // current bracket level
        string_it start_loc;  // start location of brackets
        string_it end_loc;  // end location of brackets
        bool is_term = false;  // is currently at a "term"
        string_it term_start;  // start of term
        string_it term_end;
        for (string_it it = expr_.begin(); it != expr_.end(); it++) {
          switch (*it) {
          case ')':
          break;
          case '+':
          case '-':
          case '*':
          case '/':
          case '^':
            if (cur_b_lvl == 0) {
              if (is_term) {
                // there is a term beforehand
                term_end = it;
                terms_.push_back(this->Init(string(term_start, term_end),
                  term_start-expr_.begin(), term_end-expr_.begin(), true));
                is_term = false;
              }
              Expr new_expr = this->Init(string(1, *it), it-expr_.begin(),
                it-expr_.begin()+1, false);
              terms_.push_back(new_expr);
            }
            break;
          case '(':
            if (cur_b_lvl == 0 && is_term) {
                // there is a term beforehand
                term_end = it;
                terms_.push_back(this->Init(string(term_start, term_end),
                  term_start-expr_.begin(), term_end-expr_.begin(), true));
                is_term = false;
            }
            break;
          default:
            if (cur_b_lvl == 0 && is_term == false) {
            is_term = true;
            term_start = it;
            }
          }  // switch
          if ((*it) == '(') {  // start of a bracketed term
            if (cur_b_lvl == 0)
              start_loc = it +1;
            cur_b_lvl++;
          }
          if ((*it) == ')') {  // end of a bracketed term
            if (cur_b_lvl <1) {
              SetErr("unbalanced brackets");
              return;
            }
            if (cur_b_lvl ==1) {
              end_loc = it;
              if (end_loc < start_loc) {
                SetErr("empty brackets");
                return;
              }
              terms_.push_back(this->Init(string(start_loc, end_loc),
                start_loc-expr_.begin()-1, end_loc-expr_.begin()+1));
            }
            cur_b_lvl--;
          }  // end of a bracketed term

          if (it == expr_.end()-1) {  // at the end of the test_string
            if (cur_b_lvl != 0) {
              SetErr("unbalanced brackets");
              return;
            }
            if (is_term) {
              term_end = it+1;
              terms_.push_back(this->Init(string(term_start, term_end),
                term_start-expr_.begin(), term_end-expr_.begin(), true));
              is_term = false;
            }
          }  // at the end of the test_string
        }  // end of for loop
      }
      void Search2() {
        // search for term term, for example, sin(x), "sin" is a term,
        // and (x) is another term
        bool n_term = false;
        // expression validity check
        int n_term_consecutive = 0;
        std::string term_id = "()";
        for (term_it it = terms_.begin(); it!= terms_.end(); it++) {
          if (it->fn_.compare(term_id) == 0)
            n_term_consecutive++;
          else
            n_term_consecutive = 0;
          if (n_term_consecutive == 3) {
            SetErr("Invalid expression, consecutive terms without operator");
            return;  // Impossible
          }
        }  // End of validity check
        // group terms
        for (term_it it = terms_.begin(); it < terms_.end()-1; it++) {
          if (it->IsTerm() && (it+1)->IsTerm()) {
            it->AddToFn(*(it+1));
            if (!is_valid_)
            return;
            terms_.erase(it+1, it+2);
          }
          // this->search4("");
        }
      }
      void Search3() {
        // search for operators
        int n_op_consecutive = 0;  // cannot have 2 operators consecutive
        string all_operators = "+-*/";
        std::size_t op_id1;
        std::size_t op_id2;
        std::size_t op_id3;
        // search for dual operators like 8*-7
        for (term_it it = terms_.begin(); it< terms_.end()-1; it++) {
          op_id1 = all_operators.find(it->fn_);
          op_id2 = all_operators.find((it+1)->fn_);
          if (it< terms_.end()-2)
          op_id3 = all_operators.find((it+2)->fn_);

          if (it == terms_.begin() && op_id1!= string::npos) {
            if (all_operators.at(op_id1) !='+'
            && all_operators.at(op_id1) !='-' ) {
              // starting operator such as */^ are not allowed
              SetErr("Invalid starting operator");
              return;
            }
            if (op_id2 != string::npos) {
              // a term cannot start with two operators
              SetErr("Cannot start with two operators");
              return;
            }
            (it)->CastOperator(*(it+1));
            terms_.erase(it+1, it+2);
            it = it +1;
          } else if (op_id1 != string::npos &&
            op_id2 != string::npos && it< terms_.end()-2) {
            // this is an operator
            if (op_id3 != string::npos) {
              SetErr("Invalid expression, consecutive operators");
              return;
            }
            if (all_operators.at(op_id2) !='+'
              && all_operators.at(op_id2) !='-' ) {
                SetErr("Invalid expression, invalid consecutive operators");
                return;
            }
            (it+1)->CastOperator(*(it+2));
            terms_.erase(it+2, it+3);
            it = it +1;
          }  // else
        }
      }
      void Search4(string op) {
        // search for operators op
        std::size_t op_id;
        for (term_it it = terms_.begin()+1; it< terms_.end()-1; it++) {
          op_id = op.find(it->fn_);
          if (op_id != string::npos) {
            // this is an operator
            (it)->AddToFn(*(it-1), *(it+1));  // add to current element
            if (!is_valid_)
            return;
            it = terms_.erase(it-1, it);  // erase previous element
            terms_.erase(it+1, it+2);  // erase next element
          }
        }
      }

      void CastOperator(Expr arg) {
        expr_ = fn_+"("+arg.expr_+")";
        end_ = arg.end_;
        terms_.push_back(arg);
      }
      bool IsTerm() {
        return this->fn_.compare("()") == 0;
      }
      void AddToFn(Expr arg) {
        fn_ = expr_;
        expr_ = expr_+"("+arg.expr_+")";
        end_ = arg.end_;
        if (terms_.empty() == 0) {
          SetErr("Invalid expression, consecutive bracketed expressions");
            return;
        }
        terms_.push_back(arg);
      }
      void AddToFn(Expr arg1, Expr arg2) {
        terms_.push_back(arg1); terms_.push_back(arg2);
        expr_ = arg1.expr_ + fn_ + arg2.expr_;
        start_ = arg1.start_;
        end_ = arg2.end_;
      }
      void SetErr(string msg) {
        is_valid_ = false;
        err_msg_ = msg;
      }


      // data members
      bool is_valid_ = true;
      string err_msg_ = "";
      int start_ = 0;   // start and end of the string
      int end_ = 0;
      string expr_ = "";
      string fn_ ="()";  // default type is a term
      std::vector<Expr> terms_;  // list of terms in this term
};




int main(int argc, char const *argv[]) {
  std::string test_string;
  if (argc ==1)
    test_string = "-(5/(-x-3)) + sin(8+45*-log(2n -2)^-2)";
  else
    test_string = std::string(argv[1]);

  Expr new_expr = Expr::Init(test_string);

  new_expr.DisplayContent();
  return 0;
}
