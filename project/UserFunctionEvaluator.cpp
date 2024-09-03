#include <iostream>
#include <vector>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <complex>
#include <variant>
#include <cctype>
#include <cmath>
#include <stdexcept>

#include "UserFunctionEvaluator.h"



using std::string, std::vector, std::stack, std::unordered_set, std::unordered_map, std::cout, std::endl;

using complex = std::complex<float>;
using Variant_Type = std::variant<complex, string>;


static complex sinFunction(const complex& z) { return std::sin(z); }
static complex cosFunction(const complex& z) { return std::cos(z); }
static complex tanFunction(const complex& z) { return std::tan(z); }
static complex sinhFunction(const complex& z) { return std::sinh(z); }
static complex coshFunction(const complex& z) { return std::cosh(z); }
static complex tanhFunction(const complex& z) { return std::tanh(z); }
static complex expFunction(const complex& z) { return std::exp(z); }
static complex logFunction(const complex& z) { return std::log(z); }
static complex absFunction(const complex& z) { return std::abs(z); }
static complex sqrtFunction(const complex& z) { return std::sqrt(z); }






UserFunctionEvaluator::UserFunctionEvaluator(string strFunction)
{
	this->RPNfunction = infixToRPN(strFunction);
}

UserFunctionEvaluator::UserFunctionEvaluator() {}

UserFunctionEvaluator::~UserFunctionEvaluator() {}



complex UserFunctionEvaluator::evaluate(const complex& input)
{
	stack<complex> stack;

	for (const auto& c : RPNfunction)
	{
		if (std::holds_alternative<string>(c))
		{
			string arg = std::get<string>(c);
			if (arg == "+")
			{
				complex x1 = stack.top();
				stack.pop();
				complex x2 = stack.top();
				stack.pop();
				stack.push(x1 + x2);
			}
			
			else if (arg == "*")
			{
				complex x1 = stack.top();
				stack.pop();
				complex x2 = stack.top();
				stack.pop();
				stack.push(x1 * x2);
			}
			
			else if (arg == "-")
			{
				complex x1 = stack.top();
				stack.pop();
				complex x2 = stack.top();
				stack.pop();
				stack.push(x2 - x1);
			}
			
			else if (arg == "/")
			{
				complex x1 = stack.top();
				stack.pop();
				complex x2 = stack.top();
				stack.pop();
				stack.push(x2 / x1);
			}

			else if (arg == "z")
			{
				stack.push(input);
			}

			else
			{
				//if (ComplexFunctionMap.find(arg) == ComplexFunctionMap.end())
				//{
				//	std::cerr << "Error: Function '" << arg << "' not found in function map.\n";
				//	return complex(0,0);
				//}


				complex x1 = stack.top();
				stack.pop();
				stack.push(ComplexFunctionMap[arg](x1));
			}
		}
		else
		{
			complex arg = std::get<complex>(c);
			stack.push(arg);
		}
	}
	return stack.top();
}



void UserFunctionEvaluator::changeFunction(string strFunction)
{
	this->RPNfunction = infixToRPN(strFunction);
}



// debugging
void UserFunctionEvaluator::test(complex c)
{
	complex(*testfunc)(const complex&) = ComplexFunctionMap["sin"];
	complex testComplex = testfunc(c);

	cout << testComplex.real() << endl;


}

void UserFunctionEvaluator::printRPNfunc()
{
	for (const auto& c : RPNfunction)
	{
		std::visit([](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::complex<float>>)
				{
					// Print complex numbers in a specific format
					std::cout << arg.real() << " + " << arg.imag() << "i" << ",  ";
				}
				else if constexpr (std::is_same_v<T, string>)
				{
					// Print strings in a different format
					std::cout << arg << ",  ";
				}
			}, c);
	}
	cout << '\n';
}




bool UserFunctionEvaluator::isValidOperator(const char token)
{
	return validOperators.find(token) != validOperators.end();
}


bool UserFunctionEvaluator::isValidFunction(const string& token)
{
	return validFunctions.find(token) != validFunctions.end();
}


int UserFunctionEvaluator::getPrecedence(const char op)
{
	if (op == '+' || op == '-') return 1;

	else if (op == '*' || op == '/') return 2;

	else if (op == '^') return 3;

	else return -1;
}




/// <summary>
/// Converts an infix string complex expression (user inputter) to RPN that can be used to evaluate it at various values.
/// </summary>
/// <param name="postFix"></param>
/// <returns>
/// A vector of Variant_Type where each token represents an entry in the RPN expression. The tokens are either complex numbers (float)
/// or strings which are used to denote operators, functions and the variable "z". 
/// -Note the user must use z as the function variable.
/// </returns>
vector<Variant_Type> UserFunctionEvaluator::infixToRPN(string postFix)
{
	vector<Variant_Type> output;
	stack<string> operatorStack;

	for (int i = 0; i < postFix.length(); i++)
	{
		char charToken = postFix[i];

		if (charToken == ' ') continue;

		/// doesn't support stuff like +i rn
		else if (std::isdigit(charToken))
		{
			string currentNum = string(1, charToken);

			while (i + 1 < postFix.size() && (std::isdigit(postFix[i + 1]) || postFix[i + 1] == '.'))
			{
				currentNum += postFix[++i];
			}

			if (i + 1 < postFix.length() && postFix[i + 1] == 'i')
			{
				i++;
				output.push_back(complex(0, std::stof(currentNum)));
			}
			else
			{
				output.push_back(complex(std::stof(currentNum), 0));
			}
		}

		else if (charToken == 'z') output.push_back("z");


		else if (charToken == 'i') output.push_back(complex(0, 1));


		// if operator
		else if (isValidOperator(charToken))
		{
			while (!operatorStack.empty()
				&& operatorStack.top() != "("
				&& isValidOperator(operatorStack.top()[0])
				&& (getPrecedence(charToken) < getPrecedence(operatorStack.top()[0])
					|| (getPrecedence(charToken) == getPrecedence(operatorStack.top()[0]) && charToken != '^')))
			{
				output.push_back(operatorStack.top());
				operatorStack.pop();
			}
			operatorStack.push(string(1, charToken));
		}


		else if (charToken == '(') operatorStack.push("(");


		else if (charToken == ',')
		{
			while (!operatorStack.empty() && operatorStack.top() != "(")
			{
				output.push_back(operatorStack.top());
				operatorStack.pop();
			}
		}


		else if (charToken == ')')
		{
			while (!operatorStack.empty() && operatorStack.top() != "(")
			{
				output.push_back(operatorStack.top());
				operatorStack.pop();
			}

			if (operatorStack.empty()) throw (std::runtime_error("MISMATCHED PARANTHESES"));

			if (operatorStack.top() != "(") throw (std::runtime_error("ERROR1"));

			operatorStack.pop();



			if (!operatorStack.empty() && isValidFunction(operatorStack.top()))
			{
				output.push_back(operatorStack.top());
				operatorStack.pop();
			}
		}

		else if (std::isalpha(charToken))
		{
			string functionToken = string(1, charToken);
			while (i + 1 < postFix.length() && std::isalpha(postFix[i + 1]))
			{
				functionToken += postFix[++i];
			}
			operatorStack.push(functionToken);
		}
	}

	while (!operatorStack.empty())
	{
		output.push_back(operatorStack.top());
		operatorStack.pop();
	}

	return output;

}

