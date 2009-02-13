
#ifndef WEBERKNECHT_H
#define WEBERKNECHT_H

#include <string>
#include <list>

#include <boost/asio.hpp>

#include "irc/client.h"
#include "irc/message.h"
#include "userdb.h"

namespace weberknecht {

   class bot {
      public:
         bot( const std::string& host, 
              const std::string& port,
              boost::asio::io_service& io );

         ~bot();

         void addNick( const std::string& nick );
         void setRealname( const std::string& real );
         void setUsername( const std::string& user );
         void addChannel( const std::string& channel );

         void connect();

      private:
         boost::asio::io_service& io_;
         irc::client c_;

         std::string host_;
         std::string port_;
         std::list<std::string> nick_;
         std::list<std::string>::iterator current_nick;
         std::string real_;
         std::string user_;
         std::list<std::string> channel_;

         userdb users_;

         bool privmsg( const irc::message& m );

         bool pong( const irc::message& m );

         // basic irc server message handler
         // all the basic handler have lowest priority
         // and always return false
         // This is done in order to allow modules to react on
         // these different message ( for example a logger )
         // If fatal errors occur the connection is closed.
         // The handler report back to cerr
         
         // Connection Handler
         bool connected         ( const irc::message& m );
         bool registerd         ( const irc::message& m );
         bool error             ( const irc::message& m );

         // Error handling
         bool unavail_resource  ( const irc::message& m );

         bool nick_taken        ( const irc::message& m );
         bool nick_error        ( const irc::message& m );
         bool nick_collision    ( const irc::message& m );

         bool channel_topic     ( const irc::message& m );
         bool channel_inviteonly( const irc::message& m );
         bool channel_full      ( const irc::message& m );
         bool channel_notexists ( const irc::message& m );
         bool channel_banned    ( const irc::message& m );
         bool channel_badkey    ( const irc::message& m );
         bool channel_badmask   ( const irc::message& m );
         bool channel_toomany   ( const irc::message& m );
         bool channel_join      ( const irc::message& m );
   };
} // end weberknecht

#endif
