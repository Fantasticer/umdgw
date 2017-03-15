#ifndef __UMDGW_CONFIG_HPP_INCLUDED__
#define __UMDGW_CONFIG_HPP_INCLUDED__
#include<stdint.h>
namespace umdgw {
  //service data type
  enum SERVICE_DATA_TYPE {
    FILE = 1,
    STREAM,
    PGM
  };
  //input or output service
  enum SERVICE_IO_TYPE {
    SIN = 1,
    SOUT,
    SACCEPT
  };
  enum TIMER_ID {
    RECONNECT_TIMER = 1,
    HEARTBEAT_TIMER
  };
  struct config_ctx_t{
    uint16_t input_io_threads_count;
    uint16_t output_io_threads_count;
  };


} // !namespace umdgw

#endif // !__UMDGW_CONFIG_HPP_INCLUDED__
