
#include <boost/spirit/include/phoenix1.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/random.hpp>

#include "words.h"
#include "spirit.h"
#include "sqlite/helper.h"


namespace weberknecht {
   using namespace std;
   using namespace phoenix;
   using namespace BOOST_SPIRIT_CLASSIC_NS;

   words::words( irc::client& c, Database& db )
      : c_( c ),
        word_( db ),
        quote_( db )
   {
      ADD_MSG_HANDLER( "PRIVMSG", words, words_handler, 1 );
   }

   words::~words()
   {
   }

   bool words::words_handler( const irc::message& m )
   {
      std::string command;
      std::string argument;
      std::string text;
      rule<> command_parser, command_p, argument_p;
      command_parser= command_p >> !ch_p( ' ' ) >> ( *( *ch_p( ' ' ) >>  +char_p ) )[assign_a( text )] >> *ch_p( ' ' );
      command_p = ch_p( '!' ) >> (+char_p)[assign_a( command )] >> !argument_p;
      argument_p = ch_p( '<' ) >> (+char_p)[assign_a( argument )] >> ch_p( '>' );

      if( m.param( 0 ) == c_.nick )
         return false;

      if( !parse( m.param( 1 ).c_str(), command_parser ).full )
         return false;

      cout << "Command: " << command << endl;
      cout << "Text: " << text << endl;

      // Test for predefined commands
      if( command == "addword" )
      {
         size_t sub = text.find_first_of( " " );
         std::string newWord = text.substr( 0, sub );
         std::string newText = text.substr( sub+1, text.length() );

         if( newText.length() == 0 )
         {
            c_ << irc::PRIVMSG( m.param( 0 ), "please provide some arguments" );
            return false;
         }
         
         if( newWord == "WEBSEARCH" || newWord == "RSSFEED" )
            return false;

         if( argument == "standard" || argument.length() == 0 )
            addStandard( m.param( 0 ), newWord, newText );
         if( argument == "web" )
            addWebSearch( m.param( 0 ), newWord, newText );
         if( argument == "rss" )
            addRssFeed( m.param( 0 ), newWord, newText );

         return true;
      }
      if( command == "addquote" )
      {
         c_ << irc::PRIVMSG( m.param( 0 ), quote_.add( m.param( 0 ), m.param( 1 ) ) );

         return true;
      }

      irc::message ret = irc::PRIVMSG( m.param( 0 ), "" );

      generateReply( m.param( 0), command, text, ret.param( 1 ) );

      if( ret.param( 1 ).length() == 0 ) return false;

      c_ << ret;

      return true;
   }

   void words::generateReply( const std::string& channel, const std::string& word, const std::string& text, std::string& reply )
   {
      if( reply.length() > 512 ) return;

      if( word == "quote" )
      {
         reply += quote_.next( channel, text );
         return;
      }
      
      if( word == "words" )
      {
         std::string search;
         if( text.length() == 0 )
         {
            search = select_random( word_, "words", "text", channel );
            if( search == "WEBSEARCH" ) 
               reply += "Type !" + search + " <search>";
            else if( word == "RSSFEED" )
               reply += "Type !" + search  + " to get the latest news.";
            else reply += search;
         }
         else
         {
            size_t sub = text.find_first_of( " " );
            std::string sql_search = text.substr( 0, sub-1 );


            std::string sql = select( "words", "word", channel );
            sql += construct( "words", "word",  sql_search );
            cout << sql << endl;
            reply += "Words found:";
            word_.get_result( sql );
            while( word_.fetch_row() )
            {
               reply += " !" + std::string( word_.getstr() );
            }
            word_.free_result();
         }
         return;
      }

      std::string sql = select( "words", "text", channel ) + " AND word='"+word+"';";
      std::string tmp = word_.get_string( sql );
      word_.free_result();

      cout << sql << endl;
      cout << tmp << endl;

      if( tmp.length() == 0 )
      {
         if( reply.length() != 0 )
            reply += "!" + word;
         return;
      }

      cout << text << endl;

      if( text == "show" )
      {
         reply += tmp;
         return;
      }

      if( tmp == "WEBSEARCH" )
      {
         generateWebSearch( channel, word, tmp, reply );
         return;
      }
      if( tmp == "RSSFEED" )
      {
         generateRssFeed( channel, word, tmp, reply );
         return;
      }

      generateStandard( channel, tmp, reply );
   }

   void words::generateStandard( const std::string& channel, const std::string& text, std::string& reply )
   {
      std::string tmpWord, tmpText;
      rule<> standard_p, another_p, word_p;
      standard_p = +( ( word_p[append_a( reply )] | another_p ) >> (+ch_p( ' ' ))[append_a( reply )] );
      another_p = ch_p( '<') >> ch_p( '!' ) >> ( word_p[assign_a( tmpWord )] >> !ch_p( ' ' )
                  >> *( ( word_p >> *ch_p( ' ' ) )[append_a( tmpText )] )  )[generate_a( *this, channel, tmpWord, tmpText, reply )] >> ch_p( '>' );
      word_p = +( ~ch_p( '<' ) & ~ch_p( '>' ) & char_p );

      parse( text.c_str(), standard_p );
   }
   void words::generateWebSearch( const std::string& channel, const std::string& word, const std::string& text, std::string& reply )
   {
   }
   void words::generateRssFeed( const std::string& channel, const std::string& word, const std::string& text, std::string& reply )
   {
   }

   void words::addStandard( const std::string& channel, const std::string& _word, std::string& _text )
   {
      std::string text = _text;
      std::string word = _word;

      
      escape( word );
      escape( text );
      cout << text << endl;
      
      if( word_.execute( "INSERT INTO words (word, text, channel) VALUES ('"+word+"', '" +text+ "', '" + channel + "' );" ) )
      {
         word_.free_result();
         c_ << irc::PRIVMSG( channel, "Added word !" + _word + "." );
         return;
      }

      c_ << irc::PRIVMSG( channel,  "Error inserting to database" );
   }
   void words::addWebSearch( const std::string& channel, const std::string& word, std::string& text )
   {
   }
   void words::addRssFeed( const std::string& channel, const std::string& word, std::string& text )
   {
   }
   
   generate_action generate_a( words& w, const std::string& channel, const std::string& word, const std::string& text, std::string& reply )
   {
      return generate_action( w, channel, word, text, reply );
   }

} // end namspace
