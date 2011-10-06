#ifndef MSG_H
#define MSG_H
#define MSG_LEN 10
typedef struct{
    int size;
    int msg[MSG_LEN];    //消息列表
}msgT;
#endif
