/*
	This is a test.
*/
#include <stdio>
#include <stdlib>

class A
{
public:
	A(void);
	void print_hello(void);
};

A::A(void)
{
	printf("Init A\n");
}

void A::print_hello(void)
{
	printf("Hello A\n");
}

class B : A
{
public:
	B(void);
	void print_hello(void);
};

B::B(void)
{
	printf("Init B\n");
}

void B::print_hello(void)
{
	printf("Hello B\n");
}

int main(void)
{
	B x;

	x.print_hello();

	return 0;
}
