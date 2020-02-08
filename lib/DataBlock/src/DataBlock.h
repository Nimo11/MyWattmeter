#ifndef DataBlock_h
#define DataBlock_h

#include <ESP8266WebServer.h>

class DataBlock
{
private:
public:
    int blockSize;
    char* data;
    int pos = 0;

    WiFiClient WClient;

    DataBlock(int bc,WiFiClient &wc);

    void addString(String s);

    void addArray(const char c[], int len);

    void addChar(char c);

    void sendBuffer();
};
#endif
