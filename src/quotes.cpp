
#include <iostream>
#include <ctime>

#include <boost/bind.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/random.hpp>

#include "quotes.h"
#include "spirit.h"

using namespace std;
using namespace BOOST_SPIRIT_CLASSIC_NS;

namespace weberknecht {
   quotes::quotes( irc::client& c, Database& db )
      : c_( c ),
        quote_( db ),
        numResults_( -1 ),
        currResult_( -1 )
   {
      ADD_MSG_HANDLER( "PRIVMSG", quotes, quotes_handler, 0 )
   }

   quotes::~quotes()
   {
      quote_.free_result();
   }

   bool quotes::quotes_handler( const irc::message& m )
   {
      std::string action;
      std::string selector;
      rule< > quote_parser, char_p;

      quote_parser = ( str_p( "!quote" ) | str_p( "!addquote" ) )[assign_a(action)] 
                     >> !(+ch_p( ' ' ) >> !( (+(char_p) )[assign_a(selector)] ) ) >> *ch_p( ' ' );
      char_p = ~ch_p( 0x0A ) & ~ch_p( 0x0D ) & ~ch_p( 0x3A );

      if( !quote_.Connected() )
      {
         cerr << "No database connection!" << endl;
      }

      if( m.param( 0 ) == c_.nick ) return false;

      if( parse( m.param( 1 ).c_str(), quote_parser ).full )
      {
         if( action == "!quote" )
         {
            std::string sql_select_count = "SELECT COUNT(text) FROM quotes WHERE channel='" + m.param( 0 ) + "'";
            std::string sql_select       = "SELECT text FROM quotes WHERE channel='" + m.param( 0 ) + "' AND ";

            if( selector.length() != 0 )
            {
               std::list<std::string> select_list;
               std::string tmp;
               std::string quoteID = "0";
               rule<> selector_rule, quoted, word;
               selector_rule = (+digit_p)[assign_a( quoteID )] | 
                               (*( *ch_p( ' ' ) >> ( quoted[push_back_a( select_list, tmp )] | word[push_back_a( select_list )] ) >> *ch_p( ' ' ) ) );
               quoted = ( ch_p( '"' ) >> *( ( *ch_p(' ') >> word >> *ch_p( ' ' ) )[append_a( tmp )] ) >> ch_p( '"' ) ) |
                        ( ch_p( '\'' ) >> *( ( *ch_p(' ') >> word >> *ch_p( ' ' ) )[append_a( tmp )] ) >> ch_p( '\'' ) );

               word = +( char_p & ~ch_p( '"' ) & ~ch_p( '\'' )  );

               sql_select_count += " AND ";

               if( parse( selector.c_str(), selector_rule ).full )
               {
                  if( quoteID == "0" )
                  {
                     std::list<std::string>::iterator it;
                     for( it = select_list.begin(); it != --select_list.end(); ++it )
                     {
                        sql_select       += "text LIKE '%" + *it + "%' OR ";
                        sql_select_count += "text LIKE '%" + *it + "%' OR ";
                     }
                     
                     sql_select       += "text LIKE '%" + *it + "%';";
                     sql_select_count += "text LIKE '%" + *it + "%';";
                  }
                  else
                     sql_select += "id=" + quoteID + ";";
 
                  cout << sql_select << endl;
                  if( numResults_ != -1 )
                     quote_.free_result();

                  numResults_ = quote_.get_count( sql_select_count );
                  quote_.free_result();

                  cout << "Number of results: " << numResults_ << endl;
                  currResult_ = -1;

                  quote_.get_result( sql_select );
                  if( numResults_ || quoteID != "0" )
                  {
                     quote_.fetch_row();
                     currResult_ = 1;
                     std::string text = quote_.getstr();
                     if( numResults_ < 2 )
                     {
                        quote_.free_result();
                        numResults_ = -1;
                     }
                     else
                        text += " ( " + boost::lexical_cast<std::string>(numResults_ - currResult_++) + " left )";

                     c_ << irc::PRIVMSG( m.param( 0 ), text );
                  }
                  return true;   
               }
            }
            if( numResults_ == -1 )
            {
               if( numResults_ != -1 )
                  quote_.free_result();

               int max = quote_.get_count( sql_select_count );
               boost::mt19937 rng( time( NULL ) ) ;
               boost::uniform_int<> uni(1, max);
               boost::variate_generator<boost::mt19937&, boost::uniform_int<> >
                  die(rng, uni);

               sql_select += " id=" + boost::lexical_cast<std::string>( die() ) + ";";

               cout << sql_select << endl;
               quote_.free_result();

               c_ << irc::PRIVMSG( m.param( 0 ), quote_.get_string( sql_select ) );
               quote_.free_result();

               return true;
            }
            if( (currResult_ <= numResults_) && (numResults_ > 0 ) )
            {
               quote_.fetch_row();

               std::string text = quote_.getstr();
               text += " ( " + boost::lexical_cast<std::string>(numResults_ - currResult_++) + " left )";

               if( numResults_ < currResult_ )
               {
                  quote_.free_result();
                  numResults_ = -1;
               }

               c_ << irc::PRIVMSG( m.param( 0 ), text );

               return true;
            }
         }
         else
         {
            if( selector.length() != 0 )
            {
               if( numResults_ != -1 )
                  quote_.free_result();

               std::string text = m.param( 1 ).substr( 10, m.param( 1 ).length() );
               boost::replace_all( text, "'", "\"" );
               cout << text << endl;

               if( quote_.execute( "INSERT INTO quotes (text, channel) VALUES ('" +text+ "', '" +m.param(0)+ "' );" ) )
               {
                  quote_.free_result();
                  long id = quote_.get_count( "SELECT MAX(id) FROM quotes WHERE channel='" + m.param(0) + "';");
                  c_ << irc::PRIVMSG( m.param(0), 
                          "Added Quote with id: " + boost::lexical_cast<std::string>( id) + "." );
                  quote_.free_result();
               }
            }
            return true;
         }
      }

      return false;
   }
} // end weberknecht
