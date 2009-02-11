
#include <iostream>
#include <boost/bind.hpp>

#include <irc/message.h>
#include <irc/client.h>

using namespace std;
using namespace weberknecht;

class Test
{
   public:
      Test( const string& nick,
            irc::client& c )
         : nick_( nick ),
           c_( c )
      {
         c_.addMsgHandler( "connected",
                          boost::bind( &Test::connected_handler,
                                       this,
                                       _1 ),
                          10 );
         c_.addMsgHandler( "PING",
                          boost::bind( &Test::ping_handler,
                                       this,
                                       _1 ),
                          10 );
         c_.addMsgHandler( "004",
                          boost::bind( &Test::registered_handler,
                                       this,
                                       _1 ),
                          10 );
         c_.addMsgHandler( "all",
                          boost::bind( &Test::default_handler,
                                       this,
                                       _1 ),
                          10 );
      }

   private:
      bool connected_handler( const irc::message& m )
      {

      }

      bool ping_handler( const irc::message& m )
      {
      }

      bool registered_handler( const irc::message& m )
      {
      }

      bool default_handler( const irc::message& m )
      {
         cout << " << " << m << endl;
      }

      string nick_;
      irc::client& c_;
};

int main( int argc, char** argv )
{
   boost::asio::io_service io;

   irc::client c( argv[1], "6667", io);
   Test t( "testknecht", c );

   c.connect();

   io.run();

   return 0;
}
