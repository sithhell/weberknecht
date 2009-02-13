
#ifndef IRC_MESSAGE_H
#define IRC_MESSAGE_H

#include <string>
#include <vector>

namespace weberknecht {
   namespace irc {

      // forward declaration
      class client;

      class message
      {
         public:
            message();
            message( const std::string& command,
                     size_t params = 0 );
            
            const std::string&                   prefix()          const;
                  std::string&                   prefix();
            const std::string&                   command()         const;
                  std::string&                   command();
                  size_t                         numParams()       const;
            const std::string&                   param( size_t i ) const;
                  std::string&                   param( size_t i );
                  void                           addParam( const std::string& param );
            
            bool parseNew( const std::string& m );

            void reset();

         private:
            std::string prefix_;
            std::string command_;

            size_t numParams_;
            std::vector<std::string> params_;

            // private methods for parsing
            void set_prefix( char const* first, char const* last );
            void set_command( char const* first, char const* last );
            void add_param( char const* first, char const* last );

            friend std::ostream& operator<< ( std::ostream& os, const message& m );

            friend client& operator<< ( client& c, const message& m );
      };

      // Message creation functions
      //  -- all possible IRC commands are listed here
      // TODO: handle optional parameter

      // Connection Registration
      const message PASS    ( const std::string& password );
      const message NICK    ( const std::string& nick );
      const message USER    ( const std::string& user,
                              const std::string& mode,
                              const std::string& realname );
      const message OPER    ( const std::string& name, 
                              const std::string& password );
      const message MODE    ( const std::string& nick,
                              const std::string& mode );
      const message SERVICE ( const std::string& nick,
                              const std::string& distribution,
                              const std::string& type,
                              const std::string& info );
      const message QUIT    ( const std::string& message );
      const message SQUIT   ( const std::string& server,
                              const std::string& comment );

      //Channel Operations
      const message JOIN    ( const std::string& channel, 
                              const std::string& key = "" );
      const message PART    ( const std::string& channel,
                              const std::string& message );
      const message MODE    ( const std::string& channel,
                              const std::string& mode, 
                              const std::string& modeParam );
      const message TOPIC   ( const std::string& channel, 
                              const std::string& topic,
                              bool clear = false );
      const message NAMES   ( const std::string& channel,
                              const std::string& target );
      const message LIST    ( const std::string& channel,
                              const std::string& target );
      const message INVITE  ( const std::string& nick,
                              const std::string& channel );
      const message KICK    ( const std::string& channel,
                              const std::string& nick,
                              const std::string& comment );

      // Send Messages
      const message PRIVMSG ( const std::string& target,
                              const std::string& text );
      const message NOTICE  ( const std::string& target,
                              const std::string& text );
      
      // Server Queries and Commands
      const message MOTD    ( const std::string& target );
      const message LUSERS  ( const std::string& mask,
                              const std::string& target );
      const message VERSION ( const std::string& target );
      const message STATS   ( const std::string& query,
                              const std::string& target );
      const message LINK    ( const std::string& remote_server,
                              const std::string& server_mask );
      const message TIME    ( const std::string& target );
      const message CONNECT ( const std::string& target_server,
                              const std::string& port,
                              const std::string& remote_server );
      const message TRACE   ( const std::string& target );
      const message ADMIN   ( const std::string& target );
      const message INFO    ( const std::string& target );
      const message SERVLIST( const std::string& mask,
                              const std::string& type );
      const message SQUERY  ( const std::string& servicename, 
                              const std::string& text );

      // User based Queries
      const message WHO     ( const std::string& mask, 
                              bool ops = false );
      const message WHOIS   ( const std::string& target,
                              const std::string& mask = "" );
      const message WHOWAS  ( const std::string& nick,
                              const std::string& count, 
                              const std::string& target );

      // Miscellaneous Messages
      const message KILL    ( const std::string& nickname,
                              const std::string& comment );
      const message PING    ( const std::string& server1, 
                              const std::string& server2 );
      const message PONG    ( const std::string& server1,
                              const std::string& server2 = "" );
      const message ERROR   ( const std::string& error );

      // Optional Features
      const message AWAY    ( const std::string& text );
      const message REHASH  ();
      const message DIE     ();
      const message RESTART ();
      const message SUMMON  ( const std::string& user,
                              const std::string& target,
                              const std::string& channel );
      const message USERS   ( const std::string& target );
      const message WALLOPS ( const std::string& text );
      const message USERHOST( const std::string& nick1,
                              const std::string& nick2,
                              const std::string& nick3,
                              const std::string& nick4,
                              const std::string& nick5 );

   } // end irc
} // end weberknecht

#endif
