
#include <iostream>
#include <ctime>

#include <boost/lexical_cast.hpp>

#include "quotes.h"
#include "sqlite/helper.h"

namespace weberknecht {
   using namespace std;

   quotes::quotes( Database& db )
      : quote_( db ),
        result_( ),
        currResult_( -1 )
   {
   }

   quotes::~quotes()
   {
   }

   std::string quotes::next( const std::string& channel, const std::string& text )
   {
      std::string ret;

      if( !quote_.Connected() )
         return ret;

      if( text.length() == 0 )
      {
         if( result_.size() == 0 )
         {
            return select_random( quote_, "quotes", "text", channel );
         }
         std::string ret = result_[currResult_];
         ++currResult_;
         if( currResult_ == result_.size() )
         {
            currResult_ = 0;
            result_.clear();
         }
         else
            ret += " (" + boost::lexical_cast<std::string>(result_.size() - currResult_) + " left)";

         return ret;
      }
      result_.clear();
      currResult_ = 1;
      std::string sql = select( "quotes", "text", channel );
      sql += construct( "quotes", "text", text );

      cout << sql << endl;
      
      quote_.get_result( sql );
      while( quote_.fetch_row() )
      {
         result_.push_back( quote_.getstr() );
      }
      quote_.free_result();

      if( result_.size() > 0 )
      {
         ret = result_[0];
         if( result_.size() > 1 )
            ret += " (" + boost::lexical_cast<std::string>(result_.size()-1) + " left)";
      }

      return ret;
   }

   std::string quotes::add( const std::string& channel, const std::string& _text )
   {
      std::string text = _text.substr( 10, _text.length() );

      if( text.length() == 0 )
      {
         return "Nothing to quote";
      }
      
      escape( text );
      cout << text << endl;
      
      if( quote_.execute( "INSERT INTO quotes (text, channel) VALUES ('" +text+ "', '" + channel + "' );" ) )
      {
         quote_.free_result();
         long id = quote_.get_count( "SELECT MAX(id) FROM quotes WHERE channel='" + channel + "';");
         quote_.free_result();
         return "Added Quote with id " + boost::lexical_cast<std::string>(id) + ".";
      }

      return "Error inserting to database";
   }
} // end weberknecht
