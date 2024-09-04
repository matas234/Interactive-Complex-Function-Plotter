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




class UserFunctionEvaluator {
public:
	UserFunctionEvaluator(string strFunction);

	UserFunctionEvaluator();

	~UserFunctionEvaluator();

	complex evaluate(const complex& input);

	void changeFunction(string strFunction);

	void test(complex c);

	void printRPNfunc();

private:
	vector<Variant_Type> RPNfunction;

	static unordered_set<char> validOperators;
	
	static unordered_set<string> validFunctions;
	
	static unordered_map<string, complex(*)(const complex&)> ComplexFunctionMap;
	
	static unordered_map<string, complex(*)(const complex&, const complex&)> ComplexOperatorMap;


	bool isValidOperator(const char token);

	bool isValidFunction(const string& token);

	int getPrecedence(const char op);

	vector<Variant_Type> infixToRPN(string postFix);

	static complex sinFunction(const complex& z);
	static complex cosFunction(const complex& z);
	static complex tanFunction(const complex& z);
	static complex sinhFunction(const complex& z);
	static complex coshFunction(const complex& z);
	static complex tanhFunction(const complex& z);
	static complex expFunction(const complex& z);
	static complex logFunction(const complex& z);
	static complex absFunction(const complex& z);
	static complex sqrtFunction(const complex& z);

	static complex plusOperator(const complex& x, const complex& y);
	static complex minusOperator(const complex& x, const complex& y);
	static complex timesOperator(const complex& x, const complex& y);
	static complex divideOperator(const complex& x, const complex& y);
};




#endif
