#ifndef __UMDGW_SZBINARY_DECODER_HPP_INCLUDED__
#define __UMDGW_SZBINARY_DECODER_HPP_INCLUDED__

#include"i_decoder.hpp"
#include<vector>
#include"szBinary_message_define.hpp"
#include"wire.hpp"
namespace umdgw {
  class szBinary_decoder_t :
  public i_decoder_t
  {
  public:
    szBinary_decoder_t():
    state_(HEAD), 
    bodySize_(0)
    {
    }
    virtual ~szBinary_decoder_t() {

    }


    virtual int decode(uint8_t* begin, uint8_t* end, std::function<void(uint8_t*, int)> handle) {
      int consumed = 0;
      int ret = 0;
      while (begin < end) {
        ret = consume(begin, end, consumed,handle);
        if (ret) {
          return ret;
        }

        begin += consumed;
      }
      return ret;
    }

    virtual void reset() {
      decodeBuffer_.clear();
      state_ = HEAD;
      bodySize_ = 0;
    }
  private:
    int consume(uint8_t* begin, uint8_t* end, int& consumed, std::function<void(uint8_t*, int)> handle) {
      int ret = 0;
      int toRead = end - begin;
      switch (state_) {
      case HEAD: {
        int headLack = headSize_ - decodeBuffer_.size();
        if (toRead >= headLack) {
          int count = 0;
          while (count < headLack) {
            decodeBuffer_.push_back(*(begin + count));
            count++;
          }
          state_ = BODY;
          consumed = headLack;
          bodySize_ = get_uint32(decodeBuffer_.data() + 4);
        }
        else {
          int count = 0;
          while (count < toRead) {
            decodeBuffer_.push_back(*(begin + count));
            count++;
          }
          consumed = toRead;
        }
        break;
      }
      case BODY: {
        int bodyLack = bodySize_ - decodeBuffer_.size() + headSize_;
        if (toRead >= bodyLack) {
          int count = 0;
          while (count < bodyLack) {
            decodeBuffer_.push_back(*(begin + count));
            count++;
          }
          state_ = TAIL;
          consumed = bodyLack;
        }
        else {
          int count = 0;
          while (count < toRead) {
            decodeBuffer_.push_back(*(begin + count));
            count++;
          }
          consumed = toRead;
        }
        break;
      }
      case TAIL: {
        int tailLack = bodySize_ + headSize_ + tailSize_ - decodeBuffer_.size();
        if (toRead >= tailLack) {
          int count = 0;
          while (count < tailLack) {
            decodeBuffer_.push_back(*(begin + count));
            count++;

          }
          uint32_t checkSum = GenerateCheckSum(decodeBuffer_.data(), decodeBuffer_.size() - 4);
          uint32_t cks_wire = get_uint32(decodeBuffer_.data()+decodeBuffer_.size()-4);
          if (cks_wire != checkSum) {
            ret = -1;
            reset();
          }
          else {
            consumed = tailLack;
            handle(decodeBuffer_.data(), decodeBuffer_.size());
            reset();
          }
        }
        else {
          int count = 0;
          while (count < toRead) {
            decodeBuffer_.push_back(*(begin + count));
            count++;
          }
          consumed = toRead;
        }
        break;
      }
      }
      return ret;
    }

    uint32_t GenerateCheckSum(uint8_t* buf, std::size_t size) {
      uint32_t checkSum = 0;
      std::size_t count = 0;
      while (count<size) {
        checkSum += (static_cast<uint32_t>(*(buf + count)));
        count++;
      }

      checkSum %= 256;
      return checkSum;
    }

    enum STATE {
      HEAD = 1,
      BODY,
      TAIL
    }state_;

    std::vector<uint8_t> decodeBuffer_;
    const static int headSize_ = sizeof(SZB_Head);
    const static int tailSize_ = sizeof(SZB_Tail);
    int bodySize_;
  };

}// namespace umdgw

#endif // !__UMDGW_SZBINARY_DECODER_HPP_INCLUDED__

