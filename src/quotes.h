
#ifndef QUOTES_H
#define QUOTES_H

#include "irc/client.h"
#include "sqlite/Query.h"

namespace weberknecht {

   class quotes {
      public:
         quotes( irc::client& c, Database& db );
         ~quotes();

      private:
         irc::client& c_;
         Query quote_;

         long numResults_;
         long currResult_;

         bool quotes_handler( const irc::message& m );
   };

} // end namespace

#endif
