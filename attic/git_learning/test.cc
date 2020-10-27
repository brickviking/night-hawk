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
	virtual void print_hello(void);
};

A::A(void)
{
	cout << "Begin A" << endl; // BV, 26OCT20
}

void A::print_hello(void)
{
	cout << "Gidday A" << endl; // BV, 27OCT20
}

class B : public A
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
	A *a_ptr;
	B b;

	a_ptr = &b;
	a_ptr->print_hello(); // A bit of polymorphism. JN, 27OCT20

	/*
	 * Added code. JN, 27OCT20
	 */
	cout << "Yes, Aussie Internet is pretty piss poor." << endl;

	/*
	 * Added code. BV, 28OCT20
	 */
	cout << " Perhaps I should move to New Zealand instead so I can see brickviking" << endl;
	return 0;
}

