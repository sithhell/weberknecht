
#include "userdb.h"

namespace weberknecht {
   userdb::userdb( irc::client& c )
      : c_( c )
   {}

   bool userdb::names( const irc::message& m )
   {
      return true;
   }

   bool userdb::join( const irc::message& m )
   {
      return true;
   }

   bool userdb::privmsg( const irc::message& m )
   {
      return true;
   }
} // end weberknecht
