#include "SZBinary_input_service.hpp"

namespace umdgw {

  sz_binary_input_service_t::sz_binary_input_service_t() {

  }
  sz_binary_input_service_t::~sz_binary_input_service_t() {

  }
  void sz_binary_input_service_t::init(std::string name,
    std::string addr, std::string port,
    SERVICE_DATA_TYPE dataType, SERVICE_IO_TYPE ioType,
    int ioPoolSize,boost::shared_ptr<ctx_t> context)
  {
    name_ = name;
    addr_ = addr;
    port_ = port;
    data_type_ = dataType;
    io_type_ = ioType;
    stoped = true;
    if (ioType == SIN) {
      input_services_pool_.reset(new io_service_pool_t(ioPoolSize));
     
    }
    else {
      output_services_pool_.reset(new io_service_pool_t(ioPoolSize));

    }

    context_ = context;
    socket_ = context_->createSocket(shared_from_this());
    OutputBuffer_.reserve(MAX_BUFFER_SIZE);
    inputBuffer_.resize(MAX_BUFFER_SIZE, 0);
    decoder_ptr_.reset(new szBinary_decoder_t());
    encoder_ptr_.reset(new szBinary_encoder_t());

    memset(&logonMsg_, 0, sizeof(logonMsg_));
    memset(&logoutMsg_, 0, sizeof(logoutMsg_));
    memset(&heartBeat_, 0, sizeof(heartBeat_));

    receive_msg_count_ = 0;
    receive_byte_count_ = 0;
    serviceState_ = INIT;

    reconnectTimer_ = RECONNECT_TIMER;
    heartBeatTimer_ = HEARTBEAT_TIMER;
  }

  void sz_binary_input_service_t::run() {
    stoped = false;

    logonMsg_.header.MsgType = 1;
    logonMsg_.header.BodyLength = sizeof(logonMsg_) - sizeof(SZB_Head);
    logonMsg_.HeartBtInt = 3;
    memmove(reinterpret_cast<char*>(logonMsg_.DefaultApplVerID), "1.00", strlen("1.00"));
    memmove(reinterpret_cast<char*>(logonMsg_.Password), "hehe", strlen("hehe"));
    memmove(reinterpret_cast<char*>(logonMsg_.SenderCompID), "zhengfan", strlen("zhengfan"));
    memmove(reinterpret_cast<char*>(logonMsg_.TargetCompID), "zorkMdgw", strlen("zorkMdgw"));

    heartBeat_.header.MsgType = 3;
    heartBeat_.header.BodyLength = 0;

    doConnect();
    input_services_pool_->run();
  }
  void sz_binary_input_service_t::stop() {

  }

  bool sz_binary_input_service_t::processor(int processorType) {


    return true;
  }


  void sz_binary_input_service_t::doConnect() {
    socket_->connect(addr_,port_);
  }

  void sz_binary_input_service_t::handleConnect(int errocode, std::string detail) {
    if (!errocode) {
      std::cout << socket_->getRemoteIP() << std::endl;
      sendLogon();
      //start to read from the server
      doRead();
    }
    else {
      std::cout << errocode << detail << std::endl;
      doSetTimer(reconnectTimer_,3000);
    }
  }

  void sz_binary_input_service_t::doRead() {
    socket_->async_read(inputBuffer_.data(),inputBuffer_.size());
  }

  void sz_binary_input_service_t::handleRead(int errocode,std::string detail,int bytes_transferred) {
    if (errocode) {
      doCancelTimer(heartBeatTimer_);

      doSetTimer(reconnectTimer_, 3000);
      std::cout << errocode<<detail << std::endl;
      return;
    }
    lastReceiveTime_ = boost::chrono::high_resolution_clock::now();

    receive_byte_count_ += bytes_transferred;

    decoder_ptr_->decode(inputBuffer_.data(), 
      inputBuffer_.data() + bytes_transferred, 
      boost::bind(&sz_binary_input_service_t::handlemessage_t, 
        shared_from_this(), _1, _2));

    doRead();
  }

  void sz_binary_input_service_t::doWrite() {
    uint8_t* content = 0;
    int size = 0;
    encoder_ptr_->flush(&content, &size);
    socket_->async_write(content, size);

  }
  void sz_binary_input_service_t::handleWrite(int errocode, std::string detail, int bytes_transferred) {
    if (!errocode) {
      std::cout << errocode << detail << std::endl;
    }
    else {
      doCancelTimer(heartBeatTimer_);
      std::cout << detail << std::endl;
      doSetTimer(reconnectTimer_, 3000);
    }
  }

  void sz_binary_input_service_t::doSetTimer(TIMER_ID timerID, int milli) {

    socket_->setTimer(timerID,milli);

  }

  void sz_binary_input_service_t::doCancelTimer(TIMER_ID timerID)
  {
    socket_->cancelTimer(timerID);
  }

  void sz_binary_input_service_t::handleTimer(TIMER_ID timerID) {
    switch (timerID) {
    case RECONNECT_TIMER: {
      doCancelTimer(RECONNECT_TIMER);
      encoder_ptr_->reset();
      decoder_ptr_->reset();
      doConnect();
      break;
    }
    case HEARTBEAT_TIMER: {

      doSetTimer(heartBeatTimer_, 3000);
      boost::chrono::milliseconds duration =
        boost::chrono::duration_cast<boost::chrono::milliseconds>
        (boost::chrono::high_resolution_clock::now() - lastReceiveTime_)
        ;
      if (duration.count() > 3000) {
        sendHeartBeat();
      }
      break;
    }
    default:
      break;
    }
  }

  void sz_binary_input_service_t::sendLogon() {
    encoder_ptr_->encode(reinterpret_cast<uint8_t*>(&(logonMsg_)), 
      sizeof(logonMsg_), MSG_TYPE::LOGON);
    std::cout << "sendlogon" << std::endl;
    doWrite();
  }


  void sz_binary_input_service_t::sendLogout() {
    encoder_ptr_->encode(reinterpret_cast<uint8_t*>(&(logoutMsg_)),
      sizeof(logoutMsg_), MSG_TYPE::LOGOUT);
    std::cout << "sendlogout" << std::endl;
    doWrite();
  }

  void sz_binary_input_service_t::sendHeartBeat(){
    encoder_ptr_->encode(reinterpret_cast<uint8_t*>(&(heartBeat_)),
      sizeof(heartBeat_), MSG_TYPE::HEARTBEAT);
    std::cout << "sendheart" << std::endl;
    doWrite();
  }



  void sz_binary_input_service_t::handlemessage_t(uint8_t * content, int size){
    receive_msg_count_++;
    uint32_t msgType = get_uint32(content);
    
    switch (msgType) {
    case 1://logon msg
      std::cout << "logon success" << std::endl;
      doSetTimer(heartBeatTimer_, 3000);
      break;
    case 2://logout msg
      std::cout << "logout received" << std::endl;
      doCancelTimer(heartBeatTimer_);
      doSetTimer(reconnectTimer_, 3000);
      break;
    case 3://heartbeat
      std::cout << "heartbeat recevived" << std::endl;
      break;
    default:
      std::cout << "else::" <<receive_msg_count_<< std::endl;
      break;
    }


  }
}

