#ifndef __UMDGW_I_DECODER_HPP_INCLUDED__
#define __UMDGW_I_DECODER_HPP_INCLUDED__

#include<stdint.h>
#include<boost/noncopyable.hpp>
#include<functional>

namespace umdgw {
  class i_decoder_t:
    private boost::noncopyable
  {
  public:
    virtual ~i_decoder_t() {};
    virtual int decode(uint8_t* begin, uint8_t* end, std::function<void(uint8_t*,int)> handle) = 0;
    virtual void reset() = 0;
  };


}// namespace umdgw

#endif // !__UMDGW_I_DECODER_HPP_INCLUDED__
