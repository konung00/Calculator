#include<iostream>
#include<conio.h>
#include<cmath>
#include<string>
#include<vector>

using namespace std;

class Token {
public:
    char kind;
    double value;
    string name;

    // default constructor
    Token(): kind{0} {}

    // Initiate kind with ch
    Token(char ch): kind{ch} {}

    // Initiate kind and value
    Token(char ch, double val): kind{ch}, value{val} {}

    // Initiate kind and name
    Token(char ch, string n): kind{ch}, name{n} {}
};

class Token_stream {
public:
    Token get();
    void putback(Token token);
    void ignore(char c);
private:
    bool full {false};
    Token buffer;
};

class Variable {
public:
    string name;
    double value;

    Variable(string s, double d) : name{s}, value{d} {}
};

class Symbol_table {
public:
    vector<Variable> var_table;
    double define_name(string name, double value);
    bool is_declared(string name);
    double get_value(string name);
    void set_value(string name, double value);
};


const char number = '8'; // pointer to number
const char quit = 'q'; // command to exit
const char print = ';'; // command to calc the result
const char prompt = '>';
const char result = '=';
const char name = 'a'; // lexeme - Name
const char let = 'L'; // lexeme - let
const string declkey = "let"; // keyword "let"
const char ops = 'o'; // list of operations
const char sqrt_s = 'S'; // lexeme - sqrt
const string sqrt_key = "sqrt"; // keyword "sqrt"
const char pow_s = 'P'; // lexeme - pow
const string pow_key = "pow"; // keyword "pow"


// <list of lexemes>
double primary();
double fact_term();
double term();
double expression();
double declaration();
double statement();
double fact(int num)
{
    if (num < 0)
        return 0;
    else if (num == 0)
        return 1;
    else
        return num * fact(num - 1);
}
// </list of lexemes>

double Symbol_table::get_value(string name)
{
    //Return value of named variable
    for (const Variable& var : Symbol_table::var_table)
    {
        if (var.name == name)
            return var.value;
    }
    throw runtime_error{"get: undefined variable"};

}

void Symbol_table::set_value(string name, double value)
{
    for (Variable& var : Symbol_table::var_table)
    {
        if (var.name == name)
        {
            var.value = value;
            return;
        }
    }
    throw runtime_error{"set: undefined variable"};
}

bool Symbol_table::is_declared(string s)
{
    // Whether exists variable in the vector of variables?
    for (const Variable& var : Symbol_table::var_table)
    {
        if (var.name == s)
            return true;
    }
    return false;
}

double Symbol_table::define_name(string var, double val)
{
    // Append a pair of {name : value} into the vector of variables
    if (is_declared(var))
        throw runtime_error{"variable: repeated declaration"};
    Symbol_table::var_table.push_back(Variable(var, val));
    return val;
}


void Token_stream::putback(Token token)
{
    if (full)
        throw runtime_error{"putback(): buffer is full"};
    buffer = token;
    full = true;
}


Token Token_stream::get()
{
    if (full)
    {
        full = false;
        return buffer;
    }
    char ch;
    cin >> ch;

    switch(ch)
    {
    case print:
    case quit:
    case result:
    case ops:
    case '(':
    case ')':
    case '+':
    case '{':
    case '}':
    case '-':
    case '*':
    case '/':
    case '!':
    case '%':
    case ',':
        return Token{ch};
    case '.': // float number must begin with the point

    // number literals
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        {
            cin.putback(ch); // return a number to incoming stream
            double val;
            cin >> val; // read a float number
            return Token{number, val};
        }
    default:
        if (isalpha(ch))
        {
            string str;
            str =+ ch;
            while (cin.get(ch) && (isalpha(ch) || isdigit(ch)))
                str += ch;
            cin.putback(ch);
            if (str == declkey)
                return Token{let};
            else if (str == sqrt_key)
                return Token{sqrt_s};
            else if (str == pow_key)
                return Token{pow_s};
            return Token{name, str};
        }

        throw runtime_error{"Incorrect token"};
    }
}

void Token_stream::ignore(char c)
// ch is kind of lexeme
{
    // first check the buffer
    if (full && c == buffer.kind)
    {
        full = false;
        return;
    }
    full = false;

    // check income data
    char ch = 0;
    while (cin >> ch)
        if (ch == c) return;
}

// <class instances>
Symbol_table symbol_table;
Token_stream token_stream;
// </class instances>


// <lexemes>
double primary()
{
    Token token = token_stream.get();
    switch(token.kind)
    {
    case '(':
        {
            double right = expression();
            token = token_stream.get();
            if (token.kind != ')')
                throw runtime_error{"It needs ')'"};
            return right;
        }
    case '{':
        {
            double right = expression();
            token = token_stream.get();
            if (token.kind != '}')
                throw runtime_error{"It needs '}'"};
            return right;
        }
    case number:
        return token.value;
    case '-':
        return - primary();
    case '+':
        return primary();
    case name:
        return symbol_table.get_value(token.name);
    case sqrt_s:
        {
            double expr = expression();
            if (expr < 0)
                throw runtime_error{"Negative value is detected. Please use only positive numbers within sqrt function"};
            else
                return sqrt(expr);
        }
    /*case pow_s:
        {
            double left = expression();
            token = token_stream.get();
            if (token.kind == ',')
            {
                double right = expression();
                return pow(left, right);
            }
            else
                throw runtime_error{"pow: incorrect argument"};
        }*/
    case pow_s:
        {
            token = token_stream.get();
            if (token.kind == '(' || token.kind == '{')
            {
                token = token_stream.get();
                if (token.kind == number)
                {
                    double left = token.value;
                    token = token_stream.get();
                    if (token.kind == ',')
                    {
                        token = token_stream.get();
                        if (token.kind == number)
                        {
                            int right = static_cast<int>(token.value);
                            token = token_stream.get();
                            if (token.kind == ')' || token.kind == '}')
                                return pow(left, right);
                            else
                                throw runtime_error{"pow: incorrect argument"};
                        }
                        else
                            throw runtime_error{"pow: incorrect argument"};
                    }
                    else
                        throw runtime_error{"pow: incorrect argument"};
                }
                else
                    throw runtime_error{"pow: incorrect argument"};
            }
            else
                throw runtime_error{"pow: incorrect argument"};
        }
    default:
        throw runtime_error{"It needs primary expression"};
    }
}

double fact_term()
{
    double term = primary();
    Token token = token_stream.get();
    if(token.kind == '!')
        return fact(term);
    else
        {
            token_stream.putback(token);
            return term;
        }
}

double term()
{
    double left = fact_term();
    Token token = token_stream.get();
    while (true)
    {
        switch(token.kind)
        {
        case '*':
            left *= fact_term();
            token = token_stream.get();
            break;
        case '/':
            {
                double right = fact_term();
                if (right == 0)
                    throw runtime_error{"'/': Zero devision"};
                left /= right;
                token = token_stream.get();
                break;
            }
            break;
        case '%':
            {
                int int_left = static_cast<int>(left);
                int int_right = static_cast<int>(primary());
                if (int_right == 0)
                    throw runtime_error{"'%': Zero devision"};
                left = int_left % int_right;
                token = token_stream.get();
                break;
            }
        default:
            token_stream.putback(token);
            return left;
        }
    }
}

double expression()
{
    double left = term();
    Token token = token_stream.get();
    while (true) {
        switch (token.kind) {
        case '+':
            left += term();
            token = token_stream.get();
            break;
        case '-':
            left -= term();
            token = token_stream.get();
            break;
        default:
            token_stream.putback(token);
            return left;
        }
    }
}

double statement()
{
    Token token = token_stream.get();
    switch(token.kind)
    {
    case let:
        return declaration();
    default:
        token_stream.putback(token);
        return expression();
    }
}

double declaration()
{
    Token token = token_stream.get();
    if (token.kind != name)
        throw runtime_error{"it's expected name of variable"};
    string var_name = token.name;

    Token token2 = token_stream.get();
    if (token2.kind != result)
        throw runtime_error{"symbol ';' is missed"};
    double expr = expression();
    symbol_table.define_name(var_name, expr);
    return expr;
}

// </lexemes>


// <simple functions>

void clean_up_mess()
{
    // Processor of exceptions
    token_stream.ignore(print);
}

void list_ops()
{
    cout << "List of possible math operations (in order of priority):\n\t- '!' for factorial;\n\t- '*' for multiplication;\n\t- '/' for division;\n\t- '%' for division with remain;\n\t- '+' for addition;\n\t- '-' for subtraction." << endl;
    cout << "List of provided functions:\n\t- 'sqrt(x)' return the square root of x;\n\t- 'pow(x, n)' return x in n degree;\n\t- 'pi' return the value of Pi;\n\t- 'e' return the value of Euler's number." << endl;
    cout << "You may declare variables using key word 'let' example: 'let <name> = <value> ;' " << endl;
    cout << "Also you can use brackets '()' of '{}' to separate expressions" << endl;
}

void calculate()
{
    double val = 0;
        while (cin)
        {
            try
            {
                cout << prompt;
                Token token = token_stream.get();
            if (token.kind == quit)
            {
                cout << " Good bay! " << endl;
                cout << "...Press any key..." << endl;
                _getch();
                return;
            }
            else if (token.kind == print)
            {
                cout << result << val << endl;
                continue;
            }
            else if (token.kind == ops)
                {
                    list_ops();
                    continue;
                }
            else
                token_stream.putback(token);
            val = statement();
            }
            catch (exception& e)
            {
                cerr << e.what() << endl;
                clean_up_mess();
            }
        }
}
/*
void calculate()
{
    while(cin)
    {
        try
        {
            cout << prompt;
            Token token = token_stream.get();
            while (token.kind == print)
                token = token_stream.get();
            if (token.kind == quit)
                return;
            token_stream.putback(token);
            cout << result << statement() << endl;
        }
        catch (exception& e) {
            cerr << e.what() << endl;
            clean_up_mess();
        }
    }
}
*/
// </simple functions>


int main()
{
    cout << " *** Welcome to calculator! ***" << endl;
    cout << " *** Enter expressions with float numbers (singed or unsigned) ***" << endl;
    cout << " *** If you need to know the list of provided operations enter 'o' ***" << endl;
    cout << " *** Enter 'q' for exit *** " << endl;

    try
    {
        symbol_table.define_name("pi", 3.1415926535);
        symbol_table.define_name("e", 2.7182818284);
        calculate();
    }
    catch (exception& e)
    {
        cerr << e.what() << endl;
        cout << "...Press any key..." << endl;
        _getch();
        return 1;
    }
    catch (...)
    {
        cerr << "Exception" << endl;
        cout << "...Press any key..." << endl;
        _getch();
        return 2;
    }
}
