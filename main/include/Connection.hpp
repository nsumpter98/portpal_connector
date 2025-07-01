#pragma once
#include <cstdint>
#include <cstddef>

class Connection
{
public:
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool send(uint8_t *data, size_t len) = 0;
    virtual bool receive(uint8_t *data, size_t maxLen, size_t &receivedLen) = 0;

protected:
    Connection();
};