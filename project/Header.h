#ifndef MY_HEADER_H
#define MY_HEADER_H

#include <string>
#include <vector>
#include <stack>
#include <unordered_set>
#include <complex>
#include <variant>
#include <cctype>

#endif 

using std::string, std::vector, std::stack, std::unordered_set;

using complex = std::complex<float>;

using Variant_Type = std::variant<complex, string>;


unordered_set<string> validFunctions = { "sin", "cos","tan", "max", "abs", "sinh", "cosh", "tanh"};
unordered_set<char> validOperators = { '+', '-', '/', '*','^' };



static bool isValidOperator(const char token)
{
	return validOperators.find(token) != validOperators.end();
}

static int getPrecedence(const char op)
{
	if (op == '+' || op == '-') return 1;

	else if (op == '*' || op == '/') return 2;

	else if (op == '^') return 3;

	else return -1;
}

//void printVec(const vector<Variant_Type>& input)
//{
//	for (const auto& c : input)
//	{
//		std::visit([](auto&& arg)
//			{
//				using T = std::decay_t<decltype(arg)>;
//				if constexpr (std::is_same_v<T, std::complex<float>>)
//				{
//					// Print complex numbers in a specific format
//					std::cout << arg.real() << " + " << arg.imag() << "i" << ",  ";
//				}
//				else if constexpr (std::is_same_v<T, string>)
//				{
//					// Print strings in a different format
//					std::cout << arg << ",  ";
//				}
//			}, c);
//	}
//	cout << '\n';
//}


vector<Variant_Type> infixToPostfix(string postFix)
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


		// if operator
		else if (isValidOperator(charToken))
		{

			while (!operatorStack.empty()
				&& operatorStack.top() != "("
				&& isValidOperator(operatorStack.top()[0])
				&& getPrecedence(charToken) <= getPrecedence(operatorStack.top()[0]))
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
			if (!operatorStack.empty() && operatorStack.top() == "(") operatorStack.pop();

			while (!operatorStack.empty() && operatorStack.top() != "(")
			{
				output.push_back(operatorStack.top());
				operatorStack.pop();
			}

			if (!operatorStack.empty()) operatorStack.pop();;
		}

		else
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