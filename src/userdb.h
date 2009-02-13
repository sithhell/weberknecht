
#ifndef USER_H
#define USER_H

#include <boost/unordered_map.hpp>

#include "irc/client.h"
#include "irc/message.h"

namespace weberknecht {
   class userdb {
      public:
         userdb( irc::client& c );

      private:

         struct userEntry {
            struct channelStat {
               std::string channel_;
               int wordCount_;
            };
            std::string nick_;
            std::string user_;
            std::string host_;
            std::vector<channelStat> channels_;
         };

         irc::client& c_;

         boost::unordered_map<std::string, userEntry> users_;

         // handler for recognising new users,
         // or answering to a stats request
         bool names( const irc::message& m );
         bool join( const irc::message& m );
         bool privmsg( const irc::message& m );
   };
} // end weberknecht

#endif
