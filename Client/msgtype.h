#ifndef MSGTYPE
#define MSGTYPE
#include<QFont>
typedef enum {Msg = 1, UsrEnter, UsrLeft, SndFile, RefuseFile}MsgType;//要发送消息的类型
typedef struct
{
    bool   Bold;
    bool   Underline;
    bool   Italic;
    QFont  Font;
    int    FontSize;
    QColor Color;
}FontSet;

typedef struct
{
    char    filedata[256];
}NET_PACK;

#endif // MSGTYPE

