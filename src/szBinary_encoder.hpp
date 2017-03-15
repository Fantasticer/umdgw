#ifndef __UMDGW_SZBINARY_ENCODER_HPP_INCLUDED__
#define __UMDGW_SZBINARY_ENCODER_HPP_INCLUDED__

#include"i_encoder.hpp"
#include"szBinary_message_define.hpp"
#include<vector>
#include"wire.hpp"
namespace umdgw {
  enum MSG_TYPE {
    LOGON=1,
    LOGOUT,
    HEARTBEAT,
    OTHERS
  };

  class szBinary_encoder_t :
    public i_encoder_t {
  public:
    szBinary_encoder_t() {
      wait_buffer_ptr_ = buffer_;
      write_buffer_ptr_ = buffer_+1;
    
    }
    virtual ~szBinary_encoder_t()
    {
      wait_buffer_ptr_->clear();
      write_buffer_ptr_->clear();
    }
    virtual void reset() {
      wait_buffer_ptr_->clear();
      write_buffer_ptr_->clear();
      wait_buffer_ptr_ = buffer_;
      write_buffer_ptr_ = buffer_+1;
    }

    virtual void encode(uint8_t*content, int size, int type) {
      switch (type) {
      case LOGON:
      {
        for (int i = 0; i < size; i++) {
          wait_buffer_ptr_->push_back(*(content + i));
        }
        SZB_Logon* logonMsg = reinterpret_cast<SZB_Logon*>(wait_buffer_ptr_->data());
        uint32_t msgType = logonMsg->header.MsgType;
        uint32_t bodyLen = logonMsg->header.BodyLength;
        uint32_t heartbeat = logonMsg->HeartBtInt;
        put_uint32(reinterpret_cast<uint8_t*>(&(logonMsg->header.MsgType)), msgType);
        put_uint32(reinterpret_cast<uint8_t*>(&(logonMsg->header.BodyLength)), bodyLen);
        put_uint32(reinterpret_cast<uint8_t*>(&(logonMsg->HeartBtInt)), heartbeat);
        uint32_t cks = 0;
        for (int i = 0; i < size; i++) {
          cks += static_cast<uint32_t>(*(wait_buffer_ptr_->data() + i));
        }
        cks %= 256;
        SZB_Tail tail;
        memset(&tail, 0, sizeof(tail));
        put_uint32(reinterpret_cast<uint8_t*>(&tail), cks);
        for (int i = 0, tailSize = sizeof(tail); i < tailSize; i++) {
          wait_buffer_ptr_->push_back(*(reinterpret_cast<uint8_t*>(&tail) + i));
        }
      }

        break;
      case LOGOUT:
      {
        for (int i = 0; i < size; i++) {
          wait_buffer_ptr_->push_back(*(content + i));
        }
        SZB_Logout* logoutMsg = reinterpret_cast<SZB_Logout*>(wait_buffer_ptr_->data());
        uint32_t msgType = logoutMsg->header.MsgType;
        uint32_t bodyLen = logoutMsg->header.BodyLength;
        uint32_t sessionStatus = logoutMsg->SessionStatus;
        put_uint32(reinterpret_cast<uint8_t*>(&(logoutMsg->header.MsgType)), msgType);
        put_uint32(reinterpret_cast<uint8_t*>(&(logoutMsg->header.BodyLength)), bodyLen);
        put_uint32(reinterpret_cast<uint8_t*>(&(logoutMsg->SessionStatus)), sessionStatus);
        uint32_t cks = 0;
        for (int i = 0; i < size; i++) {
          cks += static_cast<uint32_t>(*(wait_buffer_ptr_->data() + i));
        }
        cks %= 256;
        SZB_Tail tail;
        memset(&tail, 0, sizeof(tail));
        put_uint32(reinterpret_cast<uint8_t*>(&tail), cks);
        for (int i = 0, tailSize = sizeof(tail); i < tailSize; i++) {
          wait_buffer_ptr_->push_back(*(reinterpret_cast<uint8_t*>(&tail) + i));
        }
      }
        break;
      case HEARTBEAT:
      {
        for (int i = 0; i < size; i++) {
          wait_buffer_ptr_->push_back(*(content + i));
        }
        SZB_Heartbeat* heartBeat = reinterpret_cast<SZB_Heartbeat*>(wait_buffer_ptr_->data());
        uint32_t msgType = heartBeat->header.MsgType;
        uint32_t bodyLen = heartBeat->header.BodyLength;
        put_uint32(reinterpret_cast<uint8_t*>(&(heartBeat->header.MsgType)), msgType);
        put_uint32(reinterpret_cast<uint8_t*>(&(heartBeat->header.BodyLength)), bodyLen);
        uint32_t cks = 0;
        for (int i = 0; i < size; i++) {
          cks += static_cast<uint32_t>(*(wait_buffer_ptr_->data() + i));
        }
        cks %= 256;
        SZB_Tail tail;
        memset(&tail, 0, sizeof(tail));
        put_uint32(reinterpret_cast<uint8_t*>(&tail), cks);
        for (int i = 0, tailSize = sizeof(tail); i < tailSize; i++) {
          wait_buffer_ptr_->push_back(*(reinterpret_cast<uint8_t*>(&tail) + i));
        }

      }
        break;
      case OTHERS:
        for (int i = 0; i < size; i++) {
          wait_buffer_ptr_->push_back(*(content + i));
        }
        break;
      default:
        break;
      }
    }
    virtual void flush(uint8_t**content, int*size) {
      *size = wait_buffer_ptr_->size();
      *content = wait_buffer_ptr_->data();
      write_buffer_ptr_->clear();
      auto tmp = wait_buffer_ptr_;
      wait_buffer_ptr_ = write_buffer_ptr_;
      write_buffer_ptr_ = tmp;
    }

  private:
    //the send buffer
    std::vector<uint8_t> buffer_[2];

    std::vector<uint8_t>* write_buffer_ptr_;
    std::vector<uint8_t>* wait_buffer_ptr_;  

  };

}// namespace umdgw


#endif // !__UMDGW_SZBINARY_ENCODER_HPP_INCLUDED__
