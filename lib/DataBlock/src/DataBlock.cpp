#include "DataBlock.h"

DataBlock::DataBlock(int n, WiFiClient &wc)
{
    blockSize = n;
    //std::vector<char> data(n);
    data=new char[n];
    WClient = wc;
}

void DataBlock::addString(String s)
{
    for (uint i = 0; i < s.length(); i++)
    {
        addChar(s[i]);
    }
}

void DataBlock::addArray(const char c[], int len)
{
    for (int i = 0; i < len; i++)
    {
        if (c[i] == '\0')
            break;
        addChar(c[i]);
    }
}

void DataBlock::addChar(char c)
{
    data[pos] = c;
    pos++;

    if (pos == blockSize)
    {
        WClient.write(data, blockSize);
        pos = 0;
    }
}

void DataBlock::sendBuffer()
{
    WClient.write(data, pos);
}
