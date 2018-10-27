#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <boost/config.hpp>
#include <boost/regex.hpp>

int main(int argc, char** argv)
{
    const std::string pattern("\
(?(DEFINE)(?'NAMESPACE'\\w*::))(?#r)\
(?(DEFINE)(?'CONSTANT'(\"(?:[^\"\\\\]|\\\\.)*\")|(\\d+\\.?\\d*f?)))(?#r)\
(?(DEFINE)(?'VARIABLE'(?P>NAMESPACE)*([A-Za-z_]\\w*\\.)*[A-Za-z_]\\w*))(?#r)\
(?(DEFINE)(?'OPERAND'(\\+|-)*((?P>VARIABLE)|(?P>CONSTANT))))(?#r)\
(?(DEFINE)(?'EXPRESSION'\\s*(?P>OPERAND)\\s*(\\s*[\\*\\+-\\/]\\s*(?P>OPERAND))*))(?#r)\
(?(DEFINE)(?'ARGUMENTS'(?P>EXPRESSION)(,\\s*(?P>EXPRESSION))*))(?#r)\
(?(DEFINE)(?'FUNCTION_CALL'(?P>VARIABLE)\\(\\s*(?P>ARGUMENTS)?\\s*\\)))(?#r)\
(?P>FUNCTION_CALL)");
    std::cout << "pattern: " << pattern << std::endl;
    boost::regex simple_function(pattern);
    boost::regex skip_exception("\\s*[\\(|,]");
    
    std::ifstream file("flask");
    if (file.is_open()) {
        std::string context((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        boost::smatch results;
        boost::match_flag_type flags = boost::match_default;
        auto start = context.cbegin();
        int line_n = 0;
        bool found = true;
        while (start < context.cend() && found ) {
            try {
                found = boost::regex_search(start, context.cend(), results, simple_function, flags);
                if(found) {
                    std::cout << '#' << line_n++ << ' ';
                    std::cout << results[0] << std::endl;
                    start = (results[0].length() == 0) ? results[0].first + 1 : results[0].second;
                }
            }
            catch (...) {
                try {
                    if (boost::regex_search(start, context.cend(), results, skip_exception, flags)) {
                        start = (results[0].length() == 0) ? results[0].first + 1 : results[0].second;
                    }
                }
                catch (...) {
                    ++start;
                }
            }
        }
    }
    return 0;
}
