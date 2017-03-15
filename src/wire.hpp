#ifndef __UMDGW_WIRE_HPP_INCLUDED__
#define __UMDGW_WIRE_HPP_INCLUDED__

#include <stdint.h>

namespace umdgw
{

    //  Helper functions to convert different integer types to/from network
    //  byte order.

    inline void put_uint8 (unsigned char *buffer_, uint8_t value)
    {
        *buffer_ = value;
    }

    inline uint8_t get_uint8 (const unsigned char *buffer_)
    {
        return *buffer_;
    }

    inline void put_uint16 (unsigned char *buffer_, uint16_t value)
    {
        buffer_ [0] = (unsigned char) (((value) >> 8) & 0xff);
        buffer_ [1] = (unsigned char) (value & 0xff);
    }

    inline uint16_t get_uint16 (const unsigned char *buffer_)
    {
        return
            (((uint16_t) buffer_ [0]) << 8) |
            ((uint16_t) buffer_ [1]);
    }

    inline void put_uint32 (unsigned char *buffer_, uint32_t value)
    {
        buffer_ [0] = (unsigned char) (((value) >> 24) & 0xff);
        buffer_ [1] = (unsigned char) (((value) >> 16) & 0xff);
        buffer_ [2] = (unsigned char) (((value) >> 8) & 0xff);
        buffer_ [3] = (unsigned char) (value & 0xff);
    }

    inline uint32_t get_uint32 (const unsigned char *buffer_)
    {
        return
            (((uint32_t) buffer_ [0]) << 24) |
            (((uint32_t) buffer_ [1]) << 16) |
            (((uint32_t) buffer_ [2]) << 8) |
            ((uint32_t) buffer_ [3]);
    }

    inline void put_uint64 (unsigned char *buffer_, uint64_t value)
    {
        buffer_ [0] = (unsigned char) (((value) >> 56) & 0xff);
        buffer_ [1] = (unsigned char) (((value) >> 48) & 0xff);
        buffer_ [2] = (unsigned char) (((value) >> 40) & 0xff);
        buffer_ [3] = (unsigned char) (((value) >> 32) & 0xff);
        buffer_ [4] = (unsigned char) (((value) >> 24) & 0xff);
        buffer_ [5] = (unsigned char) (((value) >> 16) & 0xff);
        buffer_ [6] = (unsigned char) (((value) >> 8) & 0xff);
        buffer_ [7] = (unsigned char) (value & 0xff);
    }

    inline uint64_t get_uint64 (const unsigned char *buffer_)
    {
        return
            (((uint64_t) buffer_ [0]) << 56) |
            (((uint64_t) buffer_ [1]) << 48) |
            (((uint64_t) buffer_ [2]) << 40) |
            (((uint64_t) buffer_ [3]) << 32) |
            (((uint64_t) buffer_ [4]) << 24) |
            (((uint64_t) buffer_ [5]) << 16) |
            (((uint64_t) buffer_ [6]) << 8) |
            ((uint64_t) buffer_ [7]);
    }

}

#endif //!__UMDGW_WIRE_HPP_INCLUDED__
