/*
	This is a test.

	Compile: g++ -O2 test.cc

	Mod'ed to use c++ stdlib (I've never liked it though. JN, 26OCT20)
*/
//#include <stdio>
//#include <stdlib>
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
	cout << "Init A" << endl; // JN, 26OCT20
//	printf("Init A\n");
}

void A::print_hello(void)
{
	cout << "Hello A" << endl; // JN, 26OCT20
//	printf("Hello A\n");
}

class B : A
{
public:
	B(void);
	void print_hello(void);
};

B::B(void)
{
	cout << "Init B" << endl; // JN, 26OCT20
//	printf("Init B\n");
}

void B::print_hello(void)
{
	cout << "Hello B" << endl; // JN, 26OCT20
//	printf("Hello B\n");
}

int main(void)
{
	B x;

	x.print_hello();

	return 0;
}
