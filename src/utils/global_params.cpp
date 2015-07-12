#include <iostream>
#include <string>

#include "global_params.h"

using std::cout;
using std::endl;

int GlobalParam::Verbose = 1;

void showi(int i)
{
	cout << i << endl;
}

void showd(double d)
{
	cout << d << endl;
}

void showf(float f)
{
	cout << f << endl;
}

void shows(std::string s)
{
	cout << s << endl;
}
