
#include <iostream>
#include <string>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/random.hpp>

#include "helper.h"
#include "../spirit.h"

namespace weberknecht {

   using namespace BOOST_SPIRIT_CLASSIC_NS;

   std::string select      ( const std::string& table, const std::string& column, const std::string& channel )
   {
      return "SELECT "+column+" FROM "+table+" WHERE channel='" + channel + "'";
   }
   std::string select_count( const std::string& table, const std::string& column, const std::string& channel )
   {
      return "SELECT COUNT("+column+") FROM "+table+" WHERE channel='" + channel + "'";
   }
   std::string construct( const std::string &table, const std::string& column, const std::string& text )
   {
      std::string res;
      std::list<std::string> select_list;
      std::string tmp;
      std::string quoteID;
      rule<> selector_rule, quoted, word;
      selector_rule = (+digit_p)[assign_a( quoteID )] | 
                      (*( *ch_p( ' ' ) >> ( quoted[push_back_a( select_list, tmp )] | word[push_back_a( select_list )] ) >> *ch_p( ' ' ) ) );
      quoted = ( ch_p( '"' ) >> *( ( *ch_p(' ') >> word >> *ch_p( ' ' ) )[append_a( tmp )] ) >> ch_p( '"' ) ) |
               ( ch_p( '\'' ) >> *( ( *ch_p(' ') >> word >> *ch_p( ' ' ) )[append_a( tmp )] ) >> ch_p( '\'' ) );

      word = +( char_p & ~ch_p( '"' ) & ~ch_p( '\'' )  );

      if( parse( text.c_str(), selector_rule ).full )
      {
         res = " AND ";
         if( quoteID.length() == 0 )
         {
            std::list<std::string>::iterator it;
            for( it = select_list.begin(); it != --select_list.end(); ++it )
            {
               res += column + " LIKE '%" + *it + "%' OR ";
            }
            
            res += column + " LIKE '%" + *it + "%';";
         }
         else
            res += "id=" + quoteID + ";";
      }

      return res;
   }
   std::string select_random( Query& q, const std::string& table, const std::string& column, const std::string& channel )
   {
      std::string sql = select( table, column, channel );
      int max = q.get_count( select_count( table, column, channel ) );

      boost::mt19937 rng( time( NULL ) ) ;
      boost::uniform_int<> uni(1, max);
      boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
         die(rng, uni);

      sql += " AND id=" + boost::lexical_cast<std::string>( die() ) + ";";

      std::cout << sql << std::endl;
      q.free_result();

      std::string res = q.get_string( sql );
      q.free_result();
      return res;
   }
   void escape( std::string& text )
   {
      boost::replace_all( text, "'", "\"" );
   }

} // end namespace
