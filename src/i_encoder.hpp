#ifndef __UMDGW_I_ENCODER_HPP_INCLUDED__
#define __UMDGW_I_ENCODER_HPP_INCLUDED__

#include<stdint.h>
#include<boost/noncopyable.hpp>
namespace umdgw {

  class i_encoder_t:
    private boost::noncopyable
  {
  public:
    virtual ~i_encoder_t() {};
    virtual void reset() = 0;
    virtual void encode(uint8_t* content, int size,int type) = 0;
    virtual void flush(uint8_t** content, int* size) = 0;
  };





}// namespace umdgw

#endif // !__UMDGW_I_ENCODER_HPP_INCLUDED__
