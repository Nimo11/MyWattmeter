#ifndef LogObject_h
#define LogObject_h

#include <Arduino.h>

class LogObject
{
private:
public:
    enum DebugLevels
    {
        ErrorOnly,
        Normal,
        Verbose
    };

    bool Debug = true;
    DebugLevels level = Normal;

    void print(DebugLevels l, String s);
    void print(DebugLevels l, byte &b, int i);
    void println(DebugLevels l, String s);
    void println(DebugLevels l, double s);
    void println(DebugLevels l, byte &b, int i);
    void printf(DebugLevels l,const char *s, ...);
};
#endif