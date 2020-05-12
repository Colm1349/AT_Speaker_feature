#pragma once
#include <Arduino.h>

#define Zero_ASCII    48
#define Default_AT_AP 5
#define Default_AT_RB 2
#define Default_AT_PL 31
#define Default_AT_CF 868749756 // meh 868749756 - 915000000
#define Default_AT_SM 0 
#define NO_ANSWER 0
#define OK_ANSWER 1
#define NEED_TO_CHANGE_THE_PARAMETER 2
#define OVERWRITE_PARAMETRERS 5


class AT_Speaker {

public:
    AT_Speaker(Stream *streamObject);
    AT_Speaker();
    ~AT_Speaker();
    int setDefaultSettings();
    void setSerial(Stream *setSerial);
    void setDebugPort(Stream *setDebugport);
    void resetMyEntire_stuff();
    void testFunc();
    void print_txt();
    
    bool verification_AT_XX(char FirstSymbol, char SecondSymbol, uint32_t default_value);
    void sendAtRequest(char a1, char a2);
    void sendAtCommand(char a1, char a2, int x);
    void send_AT_AC();
    bool fill_InputMessage_Val_fromBuffer(int8_t * inputValue, size_t s);
    int  checkAnswer(uint32_t SoloParameter);
    bool isOK_Answer();    
    bool setDefault_AT_AP(); // delete this
    //erase data in inputMessage
    void eraseInputMessage()
    {
        for (unsigned int i = 0; i < sizeof(inputMessage) / sizeof(inputMessage[0]) ; i++)
        inputMessage[i] = 94;  // symbol ^
        return;
    }
    bool compareWith_CommandMode_message();

    //debug
    void printInputMessage_and_write();
    void printInputMessage_to_Debug();  
    



//private:
    void sendTriplePlus();

    //var
    Stream *_streamRef;
    Stream *debugPort;
    bool commandModeFlag = false;
    bool need_to_SAVE_changes_flag = false;
    uint8_t length_answer = 0;
    const int8_t CommandMode_message [18] = {13, 10, 67, 111, 109, 109, 97, 110, 100, 32, 109, 111, 100, 101, 46, 13, 10, 62};
    const int8_t OK_message [8] = {13, 10, 79, 75, 46, 13, 10, 62};
    const int8_t endOfAnswer [3] = {13, 10, 62};
    int8_t inputMessage[18] = {0};
};