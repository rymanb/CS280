#include "infix2postfix.h"
#include <stack>
#include <vector>
#include <string>
#include <locale>

int Evaluate( std::string const& postfix )
{
    // evaluate postfix notation using the stack algorithm

    // Create an empty stack
    std::stack<int> operandStack;

	// Convert the string to a vector of characters split by spaces.
	std::vector<char> postfixVector;
	std::string token;
	for (size_t i = 0; i < postfix.size(); ++i) {
		postfixVector.push_back(postfix[i]);
	}

	// Scan the vector from left to right.
	for (size_t i = 0; i < postfixVector.size(); ++i) {
		// If the current character is an operand, convert it to an integer and push the value onto the stack.
		if (isdigit(postfixVector[i])) {
			operandStack.push(postfixVector[i] - '0');
		}
		// If the current character is an operator, Pop the top two values from the stack.
		else {
			int operand2 = operandStack.top();
			operandStack.pop();
			int operand1 = operandStack.top();
			operandStack.pop();
			// Perform the arithmetic operation.
			int result = 0;
			switch (postfixVector[i]) {
			case '*':
				result = operand1 * operand2;
				break;
			case '/':
				result = operand1 / operand2;
				break;
			case '+':
				result = operand1 + operand2;
				break;
			case '-':
				result = operand1 - operand2;
				break;
			}
			// Push the result back onto the stack.
			operandStack.push(result);
		}
	}

	// result
	return operandStack.top();
	
}

int calculatePrecedence(char character) {
	if (character == '+' || character == '-') {
		return 1;              //Precedence of + or - is 1
	}
	else if (character == '*' || character == '/') {
		return 2;            //Precedence of * or / is 2
	}
	else {
		return 0;
	}
}

std::string Infix2postfix( std::string const& input )
{
	std::string infix = input;
	std::stack<char> stk;
	std::string postfix = "";         //initially the postfix string is empty	
	std::string::iterator it;
	for (it = infix.begin(); it != infix.end(); it++) {
		// If the scanned character is an operand, add it to output string.	
		if (isalnum(char(*it)))
		{
			postfix += *it;

		}
		// if its opening parenthesis push	
		else if (*it == '(')
		{
			stk.push('(');
		}
		// if its a closing parens	
		else if (*it == ')') {

			char curr = stk.top(); //get the top of the stack	

			// go until you find the opening parens or the stack is empty	
			while (!stk.empty() && curr != '(') {
				postfix += stk.top();


				stk.pop();
				curr = stk.top();
			}
			stk.pop();          //remove the opening parenthesis	
		}
		else {

			char right = 0;
			if (stk.size() != 0)
			{
				right = calculatePrecedence(stk.top());
			}

			char left = calculatePrecedence(*it);

			// calculate the precedence of the operators	
			// if the precedence of the operator on the stack is greater than the precedence of the operator in the string	
			if (left > right)
			{
				stk.push(*it);
			}
			else
			{
				// if the precedence of the operator is less than the precedence of the operator on the top of the stack	
				while (left <= right)
				{

					postfix += stk.top();        // store until higher pres	

					stk.pop();

					if (stk.empty())
					{
						break;
					}

					right = calculatePrecedence(stk.top());
					left = calculatePrecedence(*it);
				}
				stk.push(*it); // push the current operator	
			}
		}
	}
	while (!stk.empty()) {
		postfix += stk.top();        //store and pop until stack is not empty.	

		stk.pop();
	}
	return postfix;


}