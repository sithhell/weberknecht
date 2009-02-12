
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

namespace weberknecht {
   namespace irc {
      class client
      {
         public:

            typedef boost::function<bool ( const message& )> msgHandler_fun;

            client( const std::string& host,
                    const std::string& port,
                    boost::asio::io_service& io );

            /**
             * @return id
             */
            int addMsgHandler( const std::string& command,
                               msgHandler_fun handler,
                               int priority );

            bool delMsgHandler( const std::string& command, 
                                int id );

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

            std::string host_;
            std::string port_;

            boost::asio::io_service& io_;
            boost::asio::ip::tcp::socket socket_;

            std::deque<std::string> out_;

            boost::asio::streambuf buf_;

            class msgHandler {
               public:
                  msgHandler( int  id,
                              msgHandler_fun handler,
                              int priority)
                     : id_( id ),
                       handler_( handler ),
                       priority_( priority )
                  {}
                  
                  int id() 
                  { 
                     return id_;
                  }
                  
                  int priority()
                  {
                     return priority_;
                  }
                  
                  bool operator() ( const message& m )
                  {
                     return handler_( m );
                  }
                  
               private:
                  int id_;
                  boost::function<bool ( const message& m )> handler_;
                  size_t priority_;
            };

            // TODO: introduce some smart pointer magic here
            boost::unordered_map<std::string, std::list<msgHandler>, boost::hash<std::string> >
               msgHandler_;

            friend client & operator<<( client& c, const message& m );
            friend client & operator>>( client& c,       message& m );
      };
   } // end irc
} // end weberknecht

#endif
