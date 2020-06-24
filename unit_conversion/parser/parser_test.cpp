//  Copyright 2020 Tom Zheng

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

int main() {
  namespace pt = boost::property_tree;
  pt::ptree test_tree;
  //  test_tree.push_back(pt::ptree::value_type("pi", pt::ptree("3.14159")));
  test_tree.put("a1", "a2");
  test_tree.put("a1.a2", "a3");
  //  test_tree.add_child("a1", test_tree);
  pt::xml_parser::write_xml(std::cout, test_tree);
  //  std::cout << "imhere";
}
