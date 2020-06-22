//  Copyright 2020 Tom Zheng

#include <string.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <vector>


class Expr {
 public:
      static Expr Init(std::string input_str) {
        Expr expression = Expr();
        expression.start = 0;
        expression.end = input_str.end()-input_str.begin();
        expression.expr = input_str;
        expression.Set();
        return expression;
      }
      bool IsValid() {
        return this->is_valid;
      }
      std::string ErrorMsg() {
        return this->err_msg;
      }

      void disp_content() {
        if (this->terms.empty()) {
          std::cout <<this->expr <<"\n";
        } else {
          for (std::vector<Expr>::iterator it = this->terms.begin();
           it != this->terms.end(); it++)
            it->disp_content();
            std::cout <<this->expr <<"\n";
        }  // else
      }

      Expr(const Expr &other) {
        // data members
        is_valid = other.is_valid;
        err_msg = other.err_msg;
        start = other.start;
        end = other.end;
        expr = other.expr;
        fn = other.fn;
        terms = other.terms;
      }

 private:
      Expr() {}  // use Init(str) to construct object

      Expr Init(std::string input_expr, int start, int end) {
        // sub expression created from base expression
        Expr expression = Expr();
        expression.start = start;
        expression.end = end;
        expression.expr = input_expr;
        expression.Set();
        return expression;
      }
      Expr Init(std::string input_expr, int start, int end, bool is_term) {
        // sub term created from base expression
        Expr expression = Expr();
        expression.start = start;
        expression.end = end;
        if (is_term)
        expression.expr = input_expr;
        else
        expression.fn = input_expr;
        return expression;
      }

      void Set() {
        this->search1();
        this->search2();
        this->search4("^");
        this->search3();
        this->search4("*/");
        this->search4("+-");
      }
      void search1() {
        // search for terms and brackets
        int cur_b_lvl = 0;  // current bracket level
        std::string::iterator start_loc;  // start location of brackets
        std::string::iterator end_loc;  // end location of brackets
        bool is_term = false;  // is currently at a "term"
        std::string::iterator term_start;  // start of term
        std::string::iterator term_end;
        for (std::string::iterator it = expr.begin(); it != expr.end(); it++) {
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
                terms.push_back(this->Init(std::string(term_start, term_end),
                  term_start-expr.begin(), term_end-expr.begin(), true));
                is_term = false;
              }
              Expr new_expr = this->Init(std::string(1, *it), it-expr.begin(),
                it-expr.begin()+1, false);
              terms.push_back(new_expr);
            }
            break;
          case '(':
            if (cur_b_lvl == 0 && is_term) {
                // there is a term beforehand
                term_end = it;
                terms.push_back(this->Init(std::string(term_start, term_end),
                  term_start-expr.begin(), term_end-expr.begin(), true));
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
              this->set_err("unbalanced brackets");
              return;
            }
            if (cur_b_lvl ==1) {
              end_loc = it;
              if (end_loc < start_loc) {
                this->set_err("empty brackets");
                return;
              }
              terms.push_back(this->Init(std::string(start_loc, end_loc),
                start_loc-expr.begin()-1, end_loc-expr.begin()+1));
            }
            cur_b_lvl--;
          }  // end of a bracketed term

          if (it == expr.end()-1) {  // at the end of the test_string
            if (cur_b_lvl != 0) {
              this->set_err("unbalanced brackets");
              return;
            }
            if (is_term) {
              term_end = it+1;
              terms.push_back(this->Init(std::string(term_start, term_end),
                term_start-expr.begin(), term_end-expr.begin(), true));
              is_term = false;
            }
          }  // at the end of the test_string
        }  // end of for loop
      }

      void search2() {
        // search for term term, for example, sin(x), "sin" is a term,
        // and (x) is another term
        bool n_term = false;
        // expression validity check
        int n_term_consecutive = 0;
        std::string term_id = "()";
        for (std::vector<Expr>::iterator it = terms.begin();
        it!= terms.end(); it++) {
          if (it->fn.compare(term_id) == 0)
            n_term_consecutive++;
          else
            n_term_consecutive = 0;
          if (n_term_consecutive == 3) {
            this->set_err
            ("Invalid expression, consecutive terms without operator");
            return;  // Impossible
          }
        }  // End of validity check
        // group terms
        for (std::vector<Expr>::iterator it = terms.begin();
        it < terms.end()-1; it++) {
          if (it->is_term() && (it+1)->is_term()) {
            it->add_to_fn(*(it+1));
            terms.erase(it+1, it+2);
          }
          // this->search4("");
        }
      }

      void search3() {
        // search for operators
        int n_op_consecutive = 0;  // cannot have 2 operators consecutive
        std::string all_operators = "+-*/";
        std::size_t op_id1;
        std::size_t op_id2;
        std::size_t op_id3;
        // search for dual operators like 8*-7
        for (std::vector<Expr>::iterator it = terms.begin();
          it< terms.end()-1; it++) {
          op_id1 = all_operators.find(it->fn);
          op_id2 = all_operators.find((it+1)->fn);
          if (it< terms.end()-2)
          op_id3 = all_operators.find((it+2)->fn);

          if (it == terms.begin() && op_id1!= std::string::npos) {
            if (all_operators.at(op_id1) !='+'
            && all_operators.at(op_id1) !='-' ) {
              // starting operator such as */^ are not allowed
              this->set_err("Invalid starting operator");
              return;
            }
            if (op_id2 != std::string::npos) {
              // a term cannot start with two operators
              this->set_err("Cannot start with two operators");
              return;
            }
            (it)->cast_op(*(it+1));
            terms.erase(it+1, it+2);
            it = it +1;
          } else if (op_id1 != std::string::npos &&
            op_id2 != std::string::npos && it< terms.end()-2) {
            // this is an operator
            if (op_id3 != std::string::npos) {
              this->set_err("Invalid expression, consecutive operators");
              return;
            }
            if (all_operators.at(op_id2) !='+'
              && all_operators.at(op_id2) !='-' ) {
                this->set_err(
                  "Invalid expression, invalid consecutive operators");
                return;
            }
            (it+1)->cast_op(*(it+2));
            terms.erase(it+2, it+3);
            it = it +1;
          }  // else
        }
      }

      void search4(std::string op) {
        // search for operators op
        std::size_t op_id;
        for (std::vector<Expr>::iterator it = terms.begin()+1;
          it< terms.end()-1; it++) {
          op_id = op.find(it->fn);
          if (op_id != std::string::npos) {
            // this is an operator
            (it)->add_to_fn(*(it-1), *(it+1));  // add to current element
            it = terms.erase(it-1, it);  // erase previous element
            terms.erase(it+1, it+2);  // erase next element
          }
        }
      }
      void add_to_fn(Expr arg) {
        fn = expr;
        expr = expr+"("+arg.expr+")";
        end = arg.end;
        if (terms.empty() == 0) {
          this->set_err(
            "Invalid expression, consecutive bracketed expressions");
            return;
        }
        terms.push_back(arg);
      }
      void cast_op(Expr arg) {
        expr = fn+"("+arg.expr+")";
        end = arg.end;
        terms.push_back(arg);
      }
      void add_to_fn(Expr arg1, Expr arg2) {
        terms.push_back(arg1); terms.push_back(arg2);
        expr = arg1.expr + fn + arg2.expr;
        start = arg1.start;
        end = arg2.end;
      }
      void set_err(std::string msg) {
        this->is_valid = false;
        this->err_msg = msg;
      }
      bool is_term() {
        return this->fn.compare("()") == 0;
      }

      // data members
      bool is_valid = true;
      std::string err_msg = "";
      int start = 0;   // start and end of the string
      int end = 0;
      std::string expr = "";
      std::string fn ="()";  // default type is a term
      std::vector<Expr> terms;  // list of terms in this term
};



std::string insert_bracket(std::string str) {
  // insert brackets for exponential ie abc^+xyz and abc^-xyz to abc^(...)
  std::string all_operators = "+-*/^";
  bool is_exp = false;
  bool inserted = false;
  bool finish = false;
  int b_lvl = 0;
  for (std::string::iterator it=  str.begin(); it != str.end(); it++) {
    if (is_exp && ((*it) == '+'|| (*it) == '-') && inserted == false) {
      // insert bracket there
      it = str.insert(it, '(')+2;
      inserted = true;
      b_lvl = 0;
    }
    if ((*it)== '('&& inserted)
    b_lvl++;
    if ((*it)== ')'&& inserted)
    b_lvl--;
    if (std::string::npos != all_operators.find(*it) && inserted
      && b_lvl == 0) {
      str.insert(it, ')');
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
      str.push_back(')');
    }
    str = insert_bracket(str);
  }
  return str;
}

int main(int argc, char const *argv[]) {
  std::string test_string;
  if (argc ==1)
    test_string = "-(5/(-x-3)) + sin(8+45*-log(2n -2)^-2)";
  else
    test_string = std::string(argv[1]);

  test_string = insert_bracket(test_string);
  std::cout << test_string << std::endl;

  std::string::iterator end_pos =
    std::remove(test_string.begin(), test_string.end(), ' ');
  test_string.erase(end_pos, test_string.end());

  std::cout << test_string << "\n";

  Expr new_expr = Expr::Init(test_string);

  new_expr.disp_content();
  return 0;
}
