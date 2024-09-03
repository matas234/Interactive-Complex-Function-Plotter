#include <iostream>
#include <vector>
#include <stack>
#include <unordered_set>
#include <queue>
#include <complex>
#include <variant>

using std::string, std::vector, std::stack, std::unordered_set, std::queue;

using complex = std::complex<float>;

using Variant_Type = std::variant<complex, string>;


unordered_set<char> validOperators = { '+', '/', '*', '-', '('};



static bool isValidOperator(const char token)
{
	return validOperators.find(token) == validOperators.end();
}




vector<Variant_Type> infixToPostfix(string original)
{
	vector<Variant_Type> output;

	stack<char> operators;

	for (int i = 0; i<original.size(); i++)
	{
		char token = original[i];

		if (token == ' ' || token == ',')
		{
			continue;
		}


		if (isValidOperator(token))
		{
			operators.push(token);
			continue;
		}

		/// doesnt support stuff like +i rn
		if (std::isdigit(token))
		{
			string currentNum = string(1, token);

			while (i + 1 < original.size() && (std::isdigit(original[i+1]) || original[i+1] == '.'))
			{
				currentNum += original[i++];
			}
			
			if (i + 1 < original.size() && original[i + 1] == 'i')
			{
				output.push_back( complex(0, std::stof(currentNum)) );
			}
			else
			{
				output.push_back( complex(std::stof(currentNum), 0) );
			}
			continue;
		}

		if (token == ')')
		{
			while (operators.top() != '(')
			{
				output.push_back(operators.top());
				operators.pop();
			}
			operators.pop();
		}
	}

	return output;
}


int main()
{
	string test = "(3 + 4) * 2";

	vector<Variant_Type> out = infixToPostfix(test);

	for (const auto& c : out)
	{
		std::visit([](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::complex<float>>)
				{
					// Print complex numbers in a specific format
					std::cout << "Complex: " << arg.real() << " + " << arg.imag() << "i" << std::endl;
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					// Print strings in a different format
					std::cout << "String: " << arg << std::endl;
				}
			}, c);
	}


	return 0;
}