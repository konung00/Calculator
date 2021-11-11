#include<iostream>
#include<conio.h>
#include<cmath>

using namespace std;

class Token {
public:
    char kind;
    double value;
};

class Token_stream {
public:
    Token get();
    void putback(Token token);
private:
    bool full {false};
    Token buffer;
};


const char number = '8'; // pointer to number
const char quit = 'q'; // command to exit
const char print = '='; // command to calc the result
const char prompt = '->';
const char result = '= '; // pointer to the result of expression

double primary();
double fact_term();
double term();
double expression();
double fact(int num)
{
    if (num < 0)
        return 0;
    else if (num == 0)
        return 1;
    else
        return num * fact(num - 1);
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
        throw runtime_error{"Incorrect token"};
    }
}

Token_stream token_stream;

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
        token_stream.putback(token);
        return term;
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
                double right = primary();
                if (right == 0)
                    throw runtime_error{"'%': Zero devision"};
                left = fmod(left, right);
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

void calculate()
{
    double val = 0;
        while (cin)
        {
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
            else
                token_stream.putback(token);
            val = expression();
        }
}

int main()
{
    cout << " *** Welcome to calculator - program! ***" << endl;
    cout << "Enter expressions with float numbers (singed or unsigned)" << endl;
    cout << "List of possible math operations (in order of priority):\n\t- '!' for factorial;\n\t- '*' for multiplication;\n\t- '/' for division;\n\t- '+' for addition;\n\t- '-' for subtraction" << endl;
    cout << "Also you can use brackets '()' of '{}' to separate expressions" << endl;
    cout << " *** Enter 'q' for exit *** " << endl;
    cout << prompt;

    try
    {
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
