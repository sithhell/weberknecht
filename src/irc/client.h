
#ifndef IRC_CLIENT_H
#define IRC_CLIENT_H

#include <string>
#include <list>
#include <deque>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>

#include "message.h"

#define ADD_MSG_HANDLER( command, Class, Handler, priority ) \
   c_.addMsgHandler( command,  \
                     boost::bind( &Class::Handler, \
                                  this, \
                                  _1 ), \
                                  priority );

namespace weberknecht {
   namespace irc {

      class client
      {
         public:
            typedef boost::function<bool ( const message& )> msgHandler_fun;


            client( const std::string& host,
                    const std::string& port,
                    boost::asio::io_service& io );

            ~client();

            /**
             * @return id
             */
            size_t addMsgHandler( const std::string& command,
                               msgHandler_fun handler,
                               int priority );

            bool delMsgHandler( const std::string& command, 
                                size_t id );

            void connect();

            void disconnect();

         private:
            void send( const std::string& msg );
            void close();

            void connect_handler( const boost::system::error_code& error,
                                  boost::asio::ip::tcp::resolver::iterator endpoint_iterator );
            void receive_handler( const boost::system::error_code& error, 
                                  size_t bytes_transferred );
            void send_handler( const boost::system::error_code& error );

            void handleMsg( const std::string& command, const message& m );

            std::string host_;
            std::string port_;

            boost::asio::io_service& io_;
            boost::asio::ip::tcp::socket socket_;

            std::deque<std::string> out_;

            boost::asio::streambuf buf_;

            class msgHandler {
               public:
                  msgHandler( msgHandler_fun handler,
                              int priority)
                     : handler_( handler ),
                       priority_( priority )
                  {}
                  
                  int priority() const
                  {
                     return priority_;
                  }
                  
                  bool operator() ( const message& m )
                  {
                     return handler_( m );
                  }

                  struct match_priority {
                     match_priority( msgHandler* lhs ) : lhs_( lhs ) {}
                     bool operator() ( msgHandler* rhs )
                     {
                        return lhs_->priority() < rhs->priority();
                     }
                     msgHandler* lhs_;
                  };
                  
               private:
                  boost::function<bool ( const message& m )> handler_;
                  size_t priority_;
            };

            // TODO: introduce some smart pointer magic here
            boost::unordered_map<std::string, std::list<msgHandler *>, boost::hash<std::string> >
               msgHandler_;

            friend client & operator<<( client& c, const message& m );
      };

   } // end irc
} // end weberknecht

#endif
