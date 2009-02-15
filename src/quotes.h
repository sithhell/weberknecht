
#ifndef QUOTES_H
#define QUOTES_H

#include "irc/client.h"
#include "sqlite/Query.h"

namespace weberknecht {

   class quotes {
      public:
         quotes( Database& db );
         ~quotes();

         std::string next( const std::string& channel, const std::string& text );
         std::string add( const std::string& channel, const std::string& text );

      private:
         Query quote_;

         std::vector<std::string> result_;
         size_t currResult_;
   };

} // end namespace

#endif
