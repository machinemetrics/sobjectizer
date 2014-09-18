/*
 * Test of canceling delayed/period timers by timer_id_t::release()
 * method and timer_id_t destructor.
 */

#include <iostream>
#include <stdexcept>

#include <so_5/rt/h/rt.hpp>
#include <so_5/api/h/api.hpp>

struct msg_test : public so_5::rt::message_t
	{
		int & m_counter;

		msg_test( int & counter ) : m_counter( counter ) {}
		~msg_test() { ++m_counter; }
	};

void
ensure_counter_value( int counter, int expected_value )
	{
		if( counter != expected_value )
			{
				std::ostringstream ss;
				ss << "unexpected counter value: " << counter
						<< ", expected value: " << expected_value;

				throw std::runtime_error( ss.str() );
			}
	}

class a_test_t : public so_5::rt::agent_t
	{
	public :
		a_test_t(
			so_5::rt::so_environment_t & env,
			int & message_counter )
			:	so_5::rt::agent_t( env )
			,	m_message_counter( message_counter )
			{
			}

		void
		so_evt_start()
			{
				ensure_counter_value( m_message_counter, 0 );

				m_id1 = so_environment().schedule_timer(
						make_message_object(),
						so_direct_mbox(),
						6000,
						0 );
				m_id2 = so_environment().schedule_timer(
						make_message_object(),
						so_direct_mbox(),
						6000,
						1000 );
				m_id3 = so_environment().schedule_timer(
						make_message_object(),
						so_direct_mbox(),
						6000,
						0 );
				m_id4 = so_environment().schedule_timer(
						make_message_object(),
						so_direct_mbox(),
						6000,
						1000 );

				m_id1.release();
				m_id2.release();

				ensure_counter_value( m_message_counter, 2 );

				so_deregister_agent_coop_normally();
			}

	private :
		int & m_message_counter;

		so_5::timer_id_t m_id1;
		so_5::timer_id_t m_id2;
		so_5::timer_id_t m_id3;
		so_5::timer_id_t m_id4;

		std::unique_ptr< msg_test >
		make_message_object()
			{
				return std::unique_ptr< msg_test >(
						new msg_test( m_message_counter ) );
			}
	};

void
do_test( so_5::timer_thread_factory_t factory )
	{
		int counter = 0;

		so_5::api::run_so_environment(
			[&counter]( so_5::rt::so_environment_t & env ) {
				env.register_agent_as_coop(
						"test", new a_test_t( env, counter ) );
			},
			[&factory]( so_5::rt::so_environment_params_t & params ) {
				params.timer_thread( factory );
			} );

		ensure_counter_value( counter, 4 );
	}

void
check_factory(
	const std::string & factory_name,
	so_5::timer_thread_factory_t factory )
	{
		std::cout << "Checking " << factory_name << " factory..." << std::flush;
		do_test( factory );
		std::cout << "OK" << std::endl;
	}

int
main()
{
	try
	{
		check_factory( "timer_wheel_factory", so_5::timer_wheel_factory() );
		check_factory( "timer_wheel_factory(20,1s)",
				so_5::timer_wheel_factory( 20, std::chrono::seconds(1) ) );
		check_factory( "timer_list_factory", so_5::timer_list_factory() );

		return 0;
	}
	catch( const std::exception & x )
	{
		std::cerr << "Exception caught: " << x.what() << std::endl;
	}

	return 2;
}


