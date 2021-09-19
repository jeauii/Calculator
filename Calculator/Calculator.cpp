// Calculator.cpp

#include <algorithm>
#include <memory>
#include <functional>
#include <cmath>
#include <string>
#include <iostream>

using namespace std;

double add(double a, double b) { return a + b; }
double sub(double a, double b) { return a - b; }
double mult(double a, double b) { return a * b; }
double div(double a, double b) { return a / b; }
double expt(double a, double n) { return pow(a, n); }
double neg(double a) { return -a; }

class Expression
{
public:
	virtual double value() const = 0;
	virtual double value(double var) const = 0;
};

class Binary : public Expression
{
	shared_ptr<Expression> left, right;
	function<double(double, double)> oper;
public:
	Binary(const Binary &e) : 
		oper(e.oper), left(e.left), right(e.right)
	{}
	Binary(double (*op)(double, double), Expression *l, Expression *r) :
		oper(op), left(l), right(r)
	{}
	double value() const override
	{
		return oper(left->value(), right->value());
	}
	double value(double var) const override
	{
		return oper(left->value(var), right->value(var));
	}
};

class Unary : public Expression
{
	function<double(double)> oper;
	shared_ptr<Expression> right;
public:
	Unary(const Unary &e) : 
		oper(e.oper), right(e.right)
	{}
	Unary(double (*op)(double), Expression *r) : 
		oper(op), right(r)
	{}
	double value() const override
	{
		return oper(right->value());
	}
	double value(double var) const override
	{
		return oper(right->value(var));
	}
};

class Constant : public Expression
{
	double val;
public: 
	Constant(double v) : val(v) {}
	double value() const override
	{
		return val;
	}
	double value(double var) const override
	{
		return val;
	}
};

class Variable : public Expression
{
public:
	Variable() {}
	double value() const override
	{
		return NULL;
	}
	double value(double var) const override
	{
		return var;
	}
};

class Parser
{
public:
	static Expression* parse(string expr)
	{
		while (findEnd(expr, 0) == expr.size() - 1)
			expr = expr.substr(1, expr.size() - 2);
		size_t pos = findOper(expr);
		if (pos == string::npos)
		{
			if (expr == "x")
				return new Variable();
			else
				return new Constant(stod(expr));
		}
		else if (pos == 0)
		{
			Expression *right = parse(expr.substr(1));
			switch (expr[0])
			{
			case '+':
				return right;
				break;
			case '-':
				return new Unary(neg, right);
				break;
			}
		}
		else
		{
			Expression *left = parse(expr.substr(0, pos)),
				*right = parse(expr.substr(pos + 1));
			switch (expr[pos])
			{
			case '+':
				return new Binary(add, left, right);
				break;
			case '-':
				return new Binary(sub, left, right);
				break;
			case '*':
				return new Binary(mult, left, right);
				break;
			case '/':
				return new Binary(div, left, right);
				break;
			case '^':
				return new Binary(expt, left, right);
				break;
			}
		}
		return nullptr;
	}
private:
	static size_t findEnd(string expr, size_t pos)
	{
		if (expr[pos] != '(')
			return string::npos;
		int count = 0;
		for (int i = pos; i < expr.size(); ++i)
		{
			if (expr[i] == '(') ++count;
			else if (expr[i] == ')') --count;
			if (count == 0) return i;
		}
		return string::npos;
	}
	static size_t findOper(string expr)
	{
		size_t pos = string::npos;
		for (int i = 0; i < expr.size(); ++i)
		{
			switch (expr[i])
			{
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			case '.': case 'x':
				i = expr.find_first_of("+-*/^()", i) - 1;
				break;
			case '(':
				i = findEnd(expr, i);
				break;
			case '^':
				if (pos == string::npos) pos = i;
				break;
			case '*': case '/':
				if (pos == string::npos || expr[pos] != '+' && expr[pos] != '-') pos = i;
				break;
			case '+': case '-':
				pos = i;
				break;
			}
		}
		return pos;
	}
};

double solve(Expression* expr, 
	double x0, double dx)
{
	double x = x0;
	for (int i = 0; i < 1000; ++i)
	{
		double y = expr->value(x);
		double dy = expr->value(x + dx) - y;
		x -= y / (dy / dx);
		if (abs(y) < DBL_MIN)
			return x;
	}
	return x;
}

int main()
{
	string str; cin >> str;
	if (str.front() != '=')
	{
		Expression *expr = Parser::parse(str);
		double val = expr->value();
		cout << "=" << val << endl;
		delete expr;
	}
	else
	{
		Expression *expr = 
			Parser::parse(str.substr(1));
		double x0; cin >> x0;
		double x = solve(expr, x0, 1e-12);
		cout << "x=" << x << endl;
		delete expr;
	}
	return 0;
}
