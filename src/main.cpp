
#include <iostream>
#include <boost/bind.hpp>

#include "irc/message.h"
#include "irc/client.h"
#include "weberknecht.h"

using namespace std;
using namespace weberknecht;

int main( int argc, char** argv )
{
   boost::asio::io_service io;
   bot knecht( argv[1], "6667", io );

   knecht.addNick( "weberknecht" );
   knecht.addNick( "testknecht" );

   knecht.addChannel( "#weberknecht" );

   knecht.connect();

   io.run();

   return 0;
}
