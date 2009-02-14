
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_increment_actor.hpp>

#include "message.h"

using namespace BOOST_SPIRIT_CLASSIC_NS;

namespace weberknecht {
   namespace irc {

      message::message()
         : prefix_(),
           command_(),
           params_()
      {
         params_.reserve( 15 );
      }
      
      message::message( const std::string& command,
                        size_t numParams )
         : prefix_(),
           command_( command ),
           params_()
      {
         params_.reserve( 15 );
      }

      const std::string& message::prefix() const
      {
         return prefix_;
      }
      
      std::string& message::prefix()
      {
         return prefix_;
      }

      const std::string& message::command() const
      {
         return command_;
      }
      
      std::string& message::command()
      {
         return command_;
      }

      size_t message::numParams() const
      {
         return params_.size();
      }

      const std::string& message::param( size_t i ) const
      {
         assert( i < 15 );

         return params_[i];
      }
      
      std::string& message::param( size_t i )
      {
         assert( i < 15 );

         return params_[i];
      }
      
      void message::addParam( const std::string& param )
      {
         assert( params_.size() < 15 );

         params_.push_back(param);
      }

      bool message::parseNew( const std::string& m )
      {
         /*boost::function<void (char const*, char const*)>
            prefix_cb = boost::bind( &message::set_prefix, this, _1, _2 ),
            command_cb = boost::bind( &message::set_command, this, _1, _2 ),
            param_cb = boost::bind( &message::add_param, this, _1, _2 );*/

         rule<> start, prefix, command, params, trailing, middle, space, nospcrlfcl;
         
         space = +ch_p( ' ' );
         //nospcrlfcl = range_p( 0x01, 0x09 ) | range_p( 0x0B, 0x0C ) | range_p( 0x0E, 0x1F ) | range_p( 0x21, 0x39 ) | range_p( 0x3B, 0xFF );
         nospcrlfcl = ~ch_p( 0x0A ) & ~ch_p( 0x0D ) & ~ch_p( 0x20 ) & ~ch_p( 0x3A );

         start = !( ch_p( ':' ) >> prefix >> space ) >> command >> params >> ch_p( '\r' );

         prefix = (+nospcrlfcl)[assign_a( prefix_ )];

         command = (+( alpha_p ) | ( digit_p >> digit_p >> digit_p ))[assign_a( command_ )];

         params = space >> !( (ch_p( ':' ) >> trailing ) | ( middle >> params ) );

         trailing = ( +( nospcrlfcl | ch_p( ' ' ) | ch_p( ':' ) ) )[push_back_a( params_ )];

         middle = ( nospcrlfcl >> *( nospcrlfcl | ch_p( ':' ) ) )[push_back_a( params_ )];

         return parse( m.c_str(), start ).full;
      }

      void message::reset()
      {
         prefix_.clear();
         command_.clear();
         params_.clear();
      }

      // Connection Registration
      const message PASS    ( const std::string& password ) 
      {
         // TODO: complete!
         return message();
      }

      const message NICK    ( const std::string& nick )
      {
         message m( "NICK" );
         m.addParam( nick );
         return m;
      }

      const message USER    ( const std::string& user,
                              const std::string& mode,
                              const std::string& realname )
      {
         message m( "USER" );
         m.addParam( user );
         m.addParam( mode );
         m.addParam( "*" );
         m.addParam( realname );
         return m;
      }

      const message OPER    ( const std::string& name, 
                              const std::string& password )
      {
         // TODO: complete!
         return message();
      }

      const message MODE    ( const std::string& nick,
                              const std::string& mode )
      {
         // TODO: complete!
         return message();
      }

      const message SERVICE ( const std::string& nick,
                              const std::string& distribution,
                              const std::string& type,
                              const std::string& info )
      {
         // TODO: complete!
         return message();
      }

      const message QUIT    ( const std::string& msg )
      {
         // TODO: complete!
         return message();
      }

      const message SQUIT   ( const std::string& server,
                              const std::string& comment )
      {
         // TODO: complete!
         return message();
      }


      //Channel Operations
      const message JOIN    ( const std::string& channel, 
                              const std::string& key )
      {
         message m( "JOIN" );
         m.addParam( channel );
         return m;
      }

      const message PART    ( const std::string& channel,
                              const std::string& msg )
      {
         // TODO: complete!
         return message();
      }

      const message MODE    ( const std::string& channel,
                              const std::string& mode, 
                              const std::string& modeParam )
      {
         // TODO: complete!
         return message();
      }

      const message TOPIC   ( const std::string& channel, 
                              const std::string& topic,
                              bool clear )
      {
         // TODO: complete!
         return message();
      }

      const message NAMES   ( const std::string& channel,
                              const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message LIST    ( const std::string& channel,
                              const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message INVITE  ( const std::string& nick,
                              const std::string& channel )
      {
         // TODO: complete!
         return message();
      }

      const message KICK    ( const std::string& channel,
                              const std::string& nick,
                              const std::string& comment )
      {
         // TODO: complete!
         return message();
      }


      // Send Messages
      const message PRIVMSG ( const std::string& target,
                              const std::string& text )
      {
         message m( "PRIVMSG" );
         m.addParam( target );
         m.addParam( text );
         return m;
      }

      const message NOTICE  ( const std::string& target,
                              const std::string& text )
      {
         // TODO: complete!
         return message();
      }

      
      // Server Queries and Commands
      const message MOTD    ( const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message LUSERS  ( const std::string& mask,
                              const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message VERSION ( const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message STATS   ( const std::string& query,
                              const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message LINK    ( const std::string& remote_server,
                              const std::string& server_mask )
      {
         // TODO: complete!
         return message();
      }

      const message TIME    ( const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message CONNECT ( const std::string& target_server,
                              const std::string& port,
                              const std::string& remote_server )
      {
         // TODO: complete!
         return message();
      }

      const message TRACE   ( const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message ADMIN   ( const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message INFO    ( const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message SERVLIST( const std::string& mask,
                              const std::string& type )
      {
         // TODO: complete!
         return message();
      }

      const message SQUERY  ( const std::string& servicename, 
                              const std::string& text )
      {
         // TODO: complete!
         return message();
      }


      // User based Queries
      const message WHO     ( const std::string& mask, 
                              bool ops )
      {
         // TODO: complete!
         return message();
      }

      const message WHOIS   ( const std::string& target,
                              const std::string& mask )
      {
         message m( "WHOIS" );
         m.addParam( target );
         m.addParam( mask );
         return m;
      }

      const message WHOWAS  ( const std::string& nick,
                              const std::string& count, 
                              const std::string& target )
      {
         // TODO: complete!
         return message();
      }


      // Miscellaneous Messages
      const message KILL    ( const std::string& nickname,
                              const std::string& comment )
      {
         // TODO: complete!
         return message();
      }

      const message PING    ( const std::string& server1, 
                              const std::string& server2 )
      {
         // TODO: complete!
         return message();
      }

      const message PONG( const std::string& server1,
                          const std::string& /*server2*/ )
      {
         message m( "PONG" );
         m.addParam( server1 );
         return m;
      }

      const message ERROR( const std::string& error )
      {
         // TODO: complete!
         return message();
      }


      // Optional Features
      const message AWAY( const std::string& text )
      {
         // TODO: complete!
         return message();
      }

      const message REHASH()
      {
         // TODO: complete!
         return message();
      }

      const message DIE()
      {
         // TODO: complete!
         return message();
      }

      const message RESTART()
      {
         // TODO: complete!
         return message();
      }

      const message SUMMON( const std::string& user,
                            const std::string& target,
                            const std::string& channel )
      {
         // TODO: complete!
         return message();
      }

      const message USERS( const std::string& target )
      {
         // TODO: complete!
         return message();
      }

      const message WALLOPS( const std::string& text )
      {
         // TODO: complete!
         return message();
      }

      const message USERHOST( const std::string& nick1,
                              const std::string& nick2,
                              const std::string& nick3,
                              const std::string& nick4,
                              const std::string& nick5 )
      {
         // TODO: complete!
         return message();
      }
      
      std::ostream& operator<< ( std::ostream& os, const message& m )
      {
         if ( m.prefix_.length() != 0 )
            os << m.prefix_ << " ";
         os << m.command_;
         for( size_t i = 0; i < m.numParams(); ++i )
         {
            os << " ";
            if( boost::algorithm::contains( m.params_[i], " " ) )
               os << ":";
            os << m.params_[i];
         }

         return os;
      }
   } // end irc
} // end weberknecht
