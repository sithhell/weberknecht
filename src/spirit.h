
#ifndef SPIRIT_H
#define SPIRIT_H

#include <boost/spirit/include/classic_core.hpp>

namespace weberknecht {
   using namespace BOOST_SPIRIT_CLASSIC_NS;

   template< typename T >
   class append_action
   {
      public:
         
         explicit append_action(T& ref_)
            : ref(ref_){}
         
         template<typename T2>
         void operator()(T2 const& val) const
         {
            ref.append( val);
         }
         
         template<typename IteratorT>
         void operator()(
               IteratorT const& first,
               IteratorT const& last) const
         {
            ref.append(first,last);
         }
         
      private:
         T& ref;
   };

   template< typename T > append_action<T> append_a( T& ref)
   {
      return append_action<T>( ref );
   }

   const rule<> char_p = ~ch_p( 0x0A ) & ~ch_p( ' ' ) & ~ch_p( 0x0D );

} // end weberknecht

#endif
