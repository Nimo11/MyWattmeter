#include "LogObject.h"

/// Log a string
void LogObject::print(DebugLevels l, String s)
{
    if (Debug && !(l > level))
        Serial.print(s);
}
void LogObject::print(DebugLevels l, byte &b, int i)
{
    if (Debug && !(l > level))
        Serial.print(b, i);
}
void LogObject::println(DebugLevels l, String s)
{
    if (Debug && !(l > level))
        Serial.println(s);
}
void LogObject::println(DebugLevels l, double s)
{
    if (Debug && !(l > level))
        Serial.println(s);
}
void LogObject::println(DebugLevels l, byte &b, int i)
{
    if (Debug && !(l > level))
        Serial.println(b, i);
}
void LogObject::printf(DebugLevels l,const char *s, ...)
{
    char buff[256];
    //build result
    va_list argptr;
    va_start(argptr, s);
    vsnprintf(buff, sizeof(buff), s, argptr);
    va_end(argptr);
    buff[sizeof(buff) / sizeof(buff[0]) - 1] = '\0';
    //send it
    if (Debug && !(l > level))
        Serial.print(buff);
}