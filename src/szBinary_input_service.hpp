#ifndef __UMDGW_SZBINARY_INPUT_SERVICE_HPP_INCLUDED__
#define __UMDGW_SZBINARY_INPUT_SERVICE_HPP_INCLUDED__


#include<stdint.h>
#include<vector>
#include<string>
#include<boost/shared_ptr.hpp>
#include<boost/scoped_ptr.hpp>
#include<boost/enable_shared_from_this.hpp>
#include<boost/chrono.hpp>
#include "service.hpp"
#include "ctx.hpp"
#include"wire.hpp"
#include"szBinary_decoder.hpp"
#include"szBinary_encoder.hpp"
#include"szBinary_message_define.hpp"

namespace umdgw {

  class sz_binary_input_service_t
    :public service_t,
    public boost::enable_shared_from_this<sz_binary_input_service_t>
  {

  public:
    sz_binary_input_service_t();
    ~sz_binary_input_service_t();

    virtual void init(std::string name, std::string addr, std::string port,
      SERVICE_DATA_TYPE dataType,SERVICE_IO_TYPE ioType, int ioPoolSize,
      boost::shared_ptr<ctx_t> context);
    virtual void run();
    virtual void stop();

    virtual bool processor(int processorType);

    virtual void doConnect();
    virtual void handleConnect(int errocode, std::string detail);

    virtual void doRead();
    virtual void handleRead(int errocode,std::string detail,int bytes_transferred);

    virtual void doWrite();
    virtual void handleWrite(int errocode,std::string detail, int bytes_transferred);

    virtual void doSetTimer(TIMER_ID timerID, int milli);
    virtual void doCancelTimer(TIMER_ID timerID);
    virtual void handleTimer(TIMER_ID timerID);

  private:
    void sendLogon();
    void sendLogout();
    void sendHeartBeat();

    void handlemessage_t(uint8_t* content, int size);

    enum State {
      INIT=1,
      CONNECTING,
      CONNECTED,
      LOGGING,
      ACTIVE,
      DISCONNECTED
    }serviceState_;

    bool stoped;
    const static int MAX_BUFFER_SIZE = 8 * 1024;
    std::vector<uint8_t> OutputBuffer_;
    std::vector<uint8_t> inputBuffer_;
    
    uint32_t receive_msg_count_;
    uint32_t receive_byte_count_;
    boost::scoped_ptr<szBinary_decoder_t> decoder_ptr_;
    boost::scoped_ptr<szBinary_encoder_t> encoder_ptr_;

    SZB_Logon logonMsg_;
    SZB_Logout logoutMsg_;
    SZB_Heartbeat heartBeat_;

    TIMER_ID reconnectTimer_;
    TIMER_ID heartBeatTimer_;

    boost::chrono::high_resolution_clock::time_point lastReceiveTime_;
    //disable copy constructor
    sz_binary_input_service_t(const sz_binary_input_service_t&) = delete;
    const sz_binary_input_service_t& operator=(const sz_binary_input_service_t&) = delete;
  };
}


#endif // !__UMDGW_SZBINARY_INPUT_SERVICE_HPP_INCLUDED__

