
#include <boost/bind.hpp>

#include <irc/message.h>
#include <irc/client.h>

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
            boost::asio::async_read( socket_,
                                     boost::asio::buffer(buf_),
                                     boost::bind( &client::receive_handler, 
                                                  this,
                                                  boost::asio::placeholders::error, 
                                                  boost::asio::placeholders::bytes_transferred ) );

            std::list<msgHandler> l = msgHandler_["connected"];
            std::list<msgHandler>::iterator it;
            for( it = l.begin(); it != l.end(); ++it )
            {
               (*it)( serverMsg_ );
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
            if( serverMsg_.parse( buf_[0] ) )
            {
               std::list<msgHandler> l;
               std::list<msgHandler>::iterator it;
               
               l = msgHandler_["all"];
               for( it = l.begin(); it != l.end(); ++it )
               {
                  (*it)( serverMsg_ );
               }
               
               l = msgHandler_[serverMsg_.command()];
               for( it = l.begin(); it != l.end(); ++it )
               {
                  (*it)( serverMsg_ );
               }
            }
            boost::asio::async_read( socket_,
                                     boost::asio::buffer(buf_),
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


   } // end irc
} // end weberknecht
