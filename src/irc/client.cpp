
#include <iostream>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include "message.h"
#include "client.h"

namespace weberknecht {
   namespace irc {

      using boost::asio::ip::tcp;

      client::client( const std::string& host,
                      const std::string& port,
                      boost::asio::io_service& io)
         : host_( host ),
           port_( port ),
           io_( io ),
           socket_( io )
      {}

      
      int client::addMsgHandler( const std::string& command,
                               msgHandler_fun handler,
                               int priority )
      {
         // TODO: - insert sorted
         //       - return proper id
         msgHandler newHandler( 0,
                                handler,
                                priority );

         std::list<msgHandler>& l( msgHandler_[command] );

         l.push_back( newHandler );

         return 0;
      }

      bool client::delMsgHandler( const std::string& command,
                                 int id )
      {
         bool deleted( false );
         
         std::list<msgHandler>& l( msgHandler_[command] );
         std::list<msgHandler>::iterator it;

         for( it = l.begin(); it != l.end(); ++it )
         {
            if( it->id() == id )
            {
               l.erase( it );
               deleted = true;
            }
         }

         return deleted;
      }

      void client::connect()
      {
         tcp::resolver res( io_ );
         tcp::resolver::query q( host_, port_ );

         tcp::resolver::iterator endpoint_iterator = res.resolve( q );

         tcp::endpoint endpoint = *endpoint_iterator;
         
         socket_.async_connect( endpoint, 
                                boost::bind( &client::connect_handler, 
                                             this,
                                             boost::asio::placeholders::error, 
                                             ++endpoint_iterator ) );
      }

      void client::disconnect()
      {
         io_.post( boost::bind( &client::close, this ) );
      }

      void client::send( const std::string& msg )
      {
         bool sending = !out_.empty();
         out_.push_back( msg );

         if( !sending )
         {
            boost::asio::async_write( socket_,
                                      boost::asio::buffer( out_.front().c_str(), out_.front().length() ),
                                      boost::bind( &client::send_handler,
                                                   this,
                                                   boost::asio::placeholders::error ) );
         }
      }

      void client::close()
      {
         socket_.close();
      }

      void client::connect_handler( const boost::system::error_code& error,
                                    boost::asio::ip::tcp::resolver::iterator endpoint_iterator )
      {
         if( !error )
         {
            boost::asio::async_read_until( socket_,
                                           buf_, boost::regex("\r\n$"),
                                           boost::bind( &client::receive_handler, 
                                                        this,
                                                        boost::asio::placeholders::error, 
                                                        boost::asio::placeholders::bytes_transferred ) );

            std::list<msgHandler>& l ( msgHandler_["connected"] );
            std::list<msgHandler>::iterator it;
            message empty;
            for( it = l.begin(); it != l.end(); ++it )
            {
               if( (*it)( empty ) ) break;
            }
         }
         else if( endpoint_iterator != tcp::resolver::iterator() )
         {
            socket_.close();
            tcp::endpoint endpoint = *endpoint_iterator;
            socket_.async_connect( endpoint,
                                   boost::bind( &client::connect_handler,
                                                this,
                                                boost::asio::placeholders::error,
                                                ++endpoint_iterator ) );
         }
      }

      void client::receive_handler( const boost::system::error_code& error, 
                                    size_t bytes_transferred )
      {
         if( !error )
         {
            std::istream buf_stream( &buf_ );
            std::string next;
            while( std::getline( buf_stream, next ) )
            {
               message m;
               if( m.parseNew( next ) )
               {
                  {
                     std::list<msgHandler>& l( msgHandler_["all"] );
                     std::list<msgHandler>::iterator it;
                     
                     for( it = l.begin(); it != l.end(); ++it )
                     {
                        if( (*it)( m ) ) break;;
                     }
                  }
                  {
                     std::list<msgHandler>& l( msgHandler_[m.command()] );
                     std::list<msgHandler>::iterator it;
                     
                     for( it = l.begin(); it != l.end(); ++it )
                     {
                        if( (*it)( m ) ) break;;
                     }
                  }
               }
            }
            boost::asio::async_read_until( socket_,
                                           buf_, boost::regex( "\r\n$" ),
                                           boost::bind( &client::receive_handler,
                                                        this,
                                                        boost::asio::placeholders::error, 
                                                        boost::asio::placeholders::bytes_transferred ) );   
         }
         else
         {
            close();
         }
      }

      void client::send_handler( const boost::system::error_code& error )
      {
         if( !error )
         {
            out_.pop_front();
            if( !out_.empty() )
            {
               // wait for two seconds
               boost::asio::deadline_timer t( io_, boost::posix_time::seconds( 2 ) );
               t.wait();

               boost::asio::async_write( socket_,
                                         boost::asio::buffer( out_.front().c_str(), out_.front().length() ),
                                         boost::bind( &client::send_handler,
                                                      this,
                                                      boost::asio::placeholders::error ) );
            }
         }
         else
         {
            close();
         }
      }
      
      client & operator<<( client& c, const message& m )
      {
         std::string msg;
         if( m.prefix_.length() != 0 )
            msg += m.prefix_ + " ";
         msg += m.command_ + " " ;
         for( size_t i = 0; i < m.numParams_; ++i )
         {
            msg += " ";
            if( boost::algorithm::contains( m.params_[i], " " ) )
               msg += ":";
            msg += m.params_[i];
         }
         msg += "\r\n";

         c.send( msg );
         return c;
      }
      client & operator>>( client& c,       message& m )
      {
         return c;
      }


   } // end irc
} // end weberknecht
