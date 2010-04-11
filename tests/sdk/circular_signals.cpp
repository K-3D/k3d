#include <k3dsdk/signal_system.h>
#include <k3dsdk/signal_slots.h>

#include <iostream>
#include <set>
#include <stdexcept>
#include <string>

void on_signal(std::string Name)
{
	std::cerr << Name << std::flush;
}

class break_cycles
{
public:
	typedef void result_type;

	template<typename IteratorT>
	result_type operator()(IteratorT First, IteratorT Last)
	{
		++emissions;

		if(emissions > 10)
		{
			throw std::runtime_error("detected possible infinite loop");
		}

		for(IteratorT first = First; first != Last; ++first)
		{
			*first;
		}

		--emissions;
	}

private:
	static unsigned long emissions;
};

unsigned long break_cycles::emissions = 0;

typedef sigc::signal0<void, break_cycles> signal_type;

void run_test(signal_type& Signal)
{
	static unsigned long test_number = 0;

	std::cerr << "running test " << ++test_number << " " << std::flush;
	Signal.emit();
	std::cerr << std::endl;
}

int main(int argc, char* argv[])
{
	try
	{
		// Test A <--> B
		signal_type A;
		signal_type B;

		A.connect(sigc::bind(sigc::ptr_fun(on_signal), "A"));
		A.connect(k3d::signal::make_loop_safe_slot(B));

		B.connect(sigc::bind(sigc::ptr_fun(on_signal), "B"));
		B.connect(k3d::signal::make_loop_safe_slot(A));

		run_test(A);
		run_test(B);

		// Test C <--> C
		signal_type C;
		C.connect(sigc::bind(sigc::ptr_fun(on_signal), "C"));
		C.connect(k3d::signal::make_loop_safe_slot(C));

		run_test(C);

		// Test D --> E <--> F
		signal_type D;
		signal_type E;
		signal_type F;

		D.connect(sigc::bind(sigc::ptr_fun(on_signal), "D"));
		D.connect(k3d::signal::make_loop_safe_slot(E));

		E.connect(sigc::bind(sigc::ptr_fun(on_signal), "E"));
		E.connect(k3d::signal::make_loop_safe_slot(F));

		F.connect(sigc::bind(sigc::ptr_fun(on_signal), "F"));
		F.connect(k3d::signal::make_loop_safe_slot(E));

		run_test(D);
		run_test(E);
		run_test(F);

		/** Test G --> H <--> I
			 \
			  \---J
		*/
		signal_type G;
		signal_type H;
		signal_type I;
		signal_type J;

		G.connect(sigc::bind(sigc::ptr_fun(on_signal), "G"));
		G.connect(k3d::signal::make_loop_safe_slot(H));
		G.connect(k3d::signal::make_loop_safe_slot(J));

		H.connect(sigc::bind(sigc::ptr_fun(on_signal), "H"));
		H.connect(k3d::signal::make_loop_safe_slot(I));

		I.connect(sigc::bind(sigc::ptr_fun(on_signal), "I"));
		I.connect(k3d::signal::make_loop_safe_slot(H));

		J.connect(sigc::bind(sigc::ptr_fun(on_signal), "J"));

		run_test(G);
		run_test(H);
		run_test(I);
		run_test(J);
	}
	catch(std::exception& e)
	{
		std::cerr << "caught exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

