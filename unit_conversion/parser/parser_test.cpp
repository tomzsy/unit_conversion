//  Copyright 2020 Tom Zheng

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

int main() {
  namespace pt = boost::property_tree;
  pt::ptree test_tree;
  //  test_tree.push_back(pt::ptree::value_type("pi", pt::ptree("3.14159")));


  pt::ptree test_tree2, test_tree3, test_tree4;
  test_tree2.put("()", "pp");
  test_tree3.put("()", "jj");
  test_tree4.push_back(test_tree2);
  test_tree4.push_back(test_tree3);
  test_tree.add_child("^", test_tree4);
  pt::xml_parser::write_xml(std::cout, test_tree);
  //  std::cout << "imhere";
}
