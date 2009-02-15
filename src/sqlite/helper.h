
#ifndef SQLITE_HELPER_H
#define SQLITE_HELPER_H

#include <string>

#include "Query.h"

namespace weberknecht {

   std::string select      ( const std::string& table, const std::string& column, const std::string& channel );
   std::string select_count( const std::string& table, const std::string& column, const std::string& channel );
   std::string construct( const std::string &table, const std::string& column, const std::string& text );
   std::string select_random( Query& q, const std::string& table, const std::string& field, const std::string& channel );
   void escape( std::string& text );

} // end namespace

#endif
