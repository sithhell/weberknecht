
#ifndef WORDS_H
#define WORDS_H

#include <boost/bind.hpp>
#include <boost/spirit/include/classic_core.hpp>

#include "irc/client.h"
#include "sqlite/Query.h"
#include "quotes.h"

namespace weberknecht {
   using namespace BOOST_SPIRIT_CLASSIC_NS;

   class generate_action;

   class words {
      public:
         words( irc::client& c, Database& db );
         ~words();

      private:
         irc::client& c_;
         Query word_;
         quotes quote_;

         bool words_handler( const irc::message& m );

         void generateReply( const std::string& channel, const std::string& word, const std::string& text, std::string& reply );

         void generateStandard( const std::string& channel, const std::string& text, std::string& reply );
         void generateWebSearch( const std::string& channel, const std::string& word, const std::string& text, std::string& reply );
         void generateRssFeed( const std::string& channel, const std::string& word, const std::string& text, std::string& reply );

         void addStandard( const std::string& channel, const std::string& word, std::string& text );
         void addWebSearch( const std::string& channel, const std::string& word, std::string& text );
         void addRssFeed( const std::string& channel, const std::string& word, std::string& text );

         friend class generate_action;
   };

   
   class generate_action
   {
      public:
         
         explicit generate_action( words& w, const std::string& channel, const std::string& word, const std::string& text, std::string& reply )
            : w_( w ),
              channel_( channel ),
              word_( word ),
              text_( text ),
              reply_( reply )
         {}
         
         template<typename T2>
         void operator()(T2 const& /*val*/) const
         {
            w_.generateReply( channel_, word_, text_, reply_ );
         }
         
         template<typename IteratorT>
         void operator()(
               IteratorT const& /*first*/,
               IteratorT const& /*last*/) const
         {
            w_.generateReply( channel_, word_, text_, reply_ );
         }
         
      private:
            words& w_;
         const std::string& channel_;
         const std::string& word_;
         const std::string& text_;
               std::string& reply_;
   };

   generate_action generate_a( words& w, const std::string& channel, const std::string& word, const std::string& text, std::string& reply );

} // end namespace

#endif
