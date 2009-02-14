
#include <iostream>

#include<boost/bind.hpp>

#include "irc/message.h"
#include "weberknecht.h"

namespace weberknecht {

   bot::bot( const std::string& host,
             const std::string& port,
             const std::string& db,
             boost::asio::io_service& io )
      : io_( io ),
        c_( host, port, io ),
        host_( host ),
        port_( port ),
        nick_(),
        real_( "/msg weberknecht info" ),
        user_( "knecht" ),
        channel_(),
        db_( db ),
        quotes_( c_, db_ )
   {
      // Register the handlers to the client
      ADD_MSG_HANDLER( "000"    , bot, connected         , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "001"    , bot, registerd         , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "437"    , bot, unavail_resource  , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "433"    , bot, nick_taken        , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "432"    , bot, nick_error        , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "436"    , bot, nick_collision    , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "331"    , bot, channel_topic     , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "332"    , bot, channel_topic     , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "473"    , bot, channel_inviteonly, std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "471"    , bot, channel_full      , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "403"    , bot, channel_notexists , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "474"    , bot, channel_banned    , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "475"    , bot, channel_badkey    , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "476"    , bot, channel_badmask   , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "405"    , bot, channel_toomany   , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "366"    , bot, channel_join      , std::numeric_limits<size_t>::min() )
      ADD_MSG_HANDLER( "ERROR"  , bot, error             , std::numeric_limits<size_t>::max() )
      ADD_MSG_HANDLER( "PING"   , bot, pong              , std::numeric_limits<size_t>::min() )
      ADD_MSG_HANDLER( "PRIVMSG", bot, privmsg           , std::numeric_limits<size_t>::max() )
   }

   bot::~bot()
   {
   }
   
   void bot::addNick( const std::string& nick )
   {
      nick_.push_back( nick );
   }

   void bot::setRealname( const std::string& real )
   {
      real_ = real;
   }

   void bot::setUsername( const std::string& user )
   {
      user_ = user;
   }
   
   void bot::addChannel( const std::string& channel )
   {
      channel_.push_back( channel );
   }

   bool bot::connect()
   {
      c_.connect();

      return db_.Connected();
   }

   bool bot::privmsg( const irc::message& m )
   {
      if( m.param( 0 ) == *current_nick )
      {
         std::cout << "Query from " << m.prefix() << ": " << m.param( 1 ) << std::endl;
      }
      std::list<std::string>::iterator it;
      for( it = channel_.begin(); it != channel_.end(); ++it )
      {
         if( m.param( 0 ) == *it )
         {
            std::cout << "Message on channel " << *it << " from " << m.prefix() << ": " << m.param( 1 ) << std::endl;
         }
      }
      return false;
   }

   bool bot::pong( const irc::message& m )
   {
      c_ << irc::PONG( m.param( 0 ), m.param( 1 ) );

      return true;
   }
   
   // Connection Handler
   bool bot::connected( const irc::message& m )
   {
      current_nick = nick_.begin();
      std::cerr << "Connected to " << host_ << std::endl;
      c_.nick = *current_nick;
      c_ << irc::NICK( *current_nick ) << irc::USER( user_, "8", real_ );
      return false;
   }

   bool bot::registerd( const irc::message& m )
   {
      std::cerr << "Connection registered by " << host_ << std::endl;
      std::list<std::string>::iterator it;
      for( it = channel_.begin(); it != channel_.end(); ++it )
      {
         c_ << irc::JOIN( *it );
      }
      return false;
   }

   bool bot::error( const irc::message& m )
   {
      std::cerr << "ERROR: " << m.param( 0 ) << std::endl;
      c_.disconnect();
      return false;
   }


   // Error handling
   bool bot::unavail_resource( const irc::message& m )
   {
      /*boost::asio::deadline_timer t( io_, boost::posix_time::seconds( 2 ) );
      t.wait();*/
      std::cerr << "Resource currently unavailabe: " << m.param( 1 ) << std::endl;
      return false;
   }


   bool bot::nick_taken( const irc::message& m )
   {
      if( current_nick == nick_.end() )
      {
         *current_nick += "_";
         c_.nick = *current_nick;
         c_ << irc::NICK( *current_nick );
      }
      else
      {
         ++current_nick;
         c_.nick = *current_nick;
         c_ << irc::NICK( *current_nick );
      }
      return false;
   }

   bool bot::nick_error( const irc::message& m )
   {
      if( current_nick == nick_.end() )
      {
         std::cerr << "ERROR: No valid nickname available" << std::endl;
         c_.disconnect();
      }
      else
      {
         ++current_nick;
         c_.nick = *current_nick;
         c_ << irc::NICK( *current_nick );
      }
      return false;
   }

   bool bot::nick_collision( const irc::message& m )
   {
      std::cerr << "ERROR: Nick collision" << std::endl;
      return false;
   }


   bool bot::channel_topic( const irc::message& m )
   {
      std::cerr << "Topic for channel " << m.param( 1 ) << " is: " << m.param( 2 ) <<  std::endl;
      return false;
   }

   bool bot::channel_inviteonly( const irc::message& m )
   {
      std::cerr << "Channel " << m.param( 0 ) << " is invite only" << std::endl;
      return false;
   }

   bool bot::channel_full( const irc::message& m )
   {
      std::cerr << "Channel " << m.param( 0 ) << " is full" << std::endl;
      return false;
   }

   bool bot::channel_notexists( const irc::message& m )
   {
      std::cerr << "Channel " << m.param( 0 ) << " does not exist" << std::endl;
      return false;
   }

   bool bot::channel_banned( const irc::message& m )
   {
      std::cerr << "Banned from channel " << m.param( 0 ) << std::endl;
      return false;
   }

   bool bot::channel_badkey( const irc::message& m )
   {
      std::cerr << "Wrong key for channel " << m.param( 0 ) << std::endl;
      return false;
   }

   bool bot::channel_badmask( const irc::message& m )
   {
      std::cerr << "Bad channel mask ( " << m.param( 0 ) << std::endl;
      return false;
   }

   bool bot::channel_toomany( const irc::message& m )
   {
      std::cerr << "Cannot join channel " << m.param( 0 ) << ": Too many channels" << std::endl;
      return false;
   }
   
   bool bot::channel_join( const irc::message& m )
   {
      std::cerr << "Joined channel: " << m.param( 1 ) << std::endl;
      return false;
   }


} // end weberknecht
