/*
 * Cooperation registration test.
 *
 * A cooperation is registered. Then an attempt of registering another
 * cooperation with the same name is made. This attempt should fail.
 */

#include <iostream>
#include <map>
#include <exception>

#include <so_5/api/h/api.hpp>
#include <so_5/rt/h/rt.hpp>
#include <so_5/h/ret_code.hpp>

// A dummy agent to be placed into test cooperation.
class test_agent_t
	:
		public so_5::rt::agent_t
{
		typedef so_5::rt::agent_t base_type_t;

	public:
		test_agent_t(
			so_5::rt::so_environment_t & env )
			:
				base_type_t( env )
		{}

		virtual ~test_agent_t() {}
};

void
init( so_5::rt::so_environment_t & env )
{
	const std::string coop_name( "main_coop" );

	env.register_agent_as_coop( coop_name, new test_agent_t( env ) );

	bool exception_thrown = false;
	try
	{
		// Create a duplicate.
		env.register_agent_as_coop( coop_name, new test_agent_t( env ) );
	}
	catch( const so_5::exception_t & )
	{
		exception_thrown = true;
	}

	if( !exception_thrown )
		throw std::runtime_error( "duplicating coop should not be registered" );

	env.stop();
}

int
main( int, char ** )
{
	try
	{
		so_5::api::run_so_environment(
			&init );
	}
	catch( const std::exception & ex )
	{
		std::cerr << "error: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}



