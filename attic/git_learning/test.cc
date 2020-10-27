/*
	This is a test.
        It is a perfectly acceptable test. It has no issues and
        should compile perfectly fine in most modern compilers.
        Here's an example compile line for the GNU Compiler Project

	Compile: g++ -O2 test.cc

	Mod'ed to use c++ stdlib (I've never liked it though. JN, 26OCT20)
*/

#include <iostream> // JN, 26OCT20

using namespace std; // You never used to need this declaration. it's a bit like python now. JN, 26OCT20

class A
{
public:
	A(void);
	void print_hello(void);
};

A::A(void)
{
	cout << "Begin A" << endl; // BV, 26OCT20
}

void A::print_hello(void)
{
	cout << "Gidday A" << endl; // BV, 27OCT20
}

class B : A
{
public:
	B(void);
	void print_hello(void);
};

B::B(void)
{
	cout << "Begin B" << endl; // BV, 27OCT20
}

void B::print_hello(void)
{
	cout << "Gidday B" << endl; // BV, 27OCT20
}

int main(void)
{
	B x;

	x.print_hello();

	/*
	 * Added code. JN, 27OCT20
	 */
	for (int i = 0; i < 5; i++) {
		cout << i << ". Yes, Aussie Internet is pretty piss poor" << endl;
	}

	return 0;
}
