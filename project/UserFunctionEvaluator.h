#ifndef USER_FUNCTION_EVALUATOR_H
#define USER_FUNCTION_EVALUATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <complex>
#include <variant>
#include <cctype>


using std::string, std::vector, std::stack, std::unordered_set, std::unordered_map;

using std::cout, std::endl;

using complex = std::complex<float>;

using Variant_Type = std::variant<complex, string>;

complex sinFunction(const complex& z);
complex cosFunction(const complex& z);
complex tanFunction(const complex& z);
complex sinhFunction(const complex& z);
complex coshFunction(const complex& z);
complex tanhFunction(const complex& z);
complex expFunction(const complex& z);
complex logFunction(const complex& z);
complex absFunction(const complex& z);
complex sqrtFunction(const complex& z);



class UserFunctionEvaluator {
public:
	UserFunctionEvaluator(string strFunction);

	~UserFunctionEvaluator();

	complex evaluate(const complex& input);

	void changeFunction(string strFunction);

	void test(complex c);

	void printRPNfunc();

private:
	vector<Variant_Type> RPNfunction;
	unordered_set<char> validOperators = { '+', '-', '/', '*','^' };
	unordered_set<string> validFunctions = { "sin", "cos" , "tan", "sinh", "cosh", "tanh", "exp", "log", "abs", "sqrt" };

	// Define the unordered map to store function pointers
	std::unordered_map<std::string, complex(*)(const complex&)> ComplexFunctionMap = {
		{ "sin", sinFunction },
		{ "cos", cosFunction },
		{ "tan", tanFunction },
		{ "sinh", sinhFunction },
		{ "cosh", coshFunction },
		{ "tanh", tanhFunction },
		{ "exp", expFunction },
		{ "log", logFunction },
		{ "abs", absFunction },
		{ "sqrt", sqrtFunction}
	};



	bool isValidOperator(const char token);

	bool isValidFunction(const string& token);

	int getPrecedence(const char op);

	vector<Variant_Type> infixToRPN(string postFix);

};


#endif
