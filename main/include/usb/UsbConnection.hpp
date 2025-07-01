#include "Connection.hpp"

class UsbConnection : public Connection
{
public:
    UsbConnection();
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool send(const uint8_t *data, size_t len) = 0;
    virtual bool receive(uint8_t *data, size_t maxLen, size_t &receivedLen) = 0;
};