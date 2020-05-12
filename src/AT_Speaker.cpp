#include "AT_Speaker.hpp"

AT_Speaker::AT_Speaker() {

}

AT_Speaker::~AT_Speaker() {

}

void AT_Speaker::testFunc() {

}

void AT_Speaker::setSerial(Stream *streamObject) {
   _streamRef=streamObject;
//    _streamRef->println("Start Serial from AT_Speaker");
}

void AT_Speaker::setDebugPort(Stream *streamObject) {
    debugPort = streamObject;
    debugPort->println(F("DEBUG port from AT_Speaker WORKS"));
}

void AT_Speaker::print_txt() {
    _streamRef->println(F("Scream from the Grave"));
    delay(2000);
}

void AT_Speaker::sendTriplePlus() {
    debugPort->println(F("Tripleplus SEND!!!!"));
    _streamRef->print(F("+"));
    delay(10);
    _streamRef->print(F("+"));
    delay(10);
    _streamRef->print(F("+"));
    delay(500);
}

int AT_Speaker::setDefaultSettings() 
{
    //
    resetMyEntire_stuff();

    //enterCommandMode
    sendTriplePlus();

    // if fill == true -> go send command
    if(fill_InputMessage_Val_fromBuffer( &inputMessage[0], (sizeof(inputMessage)/sizeof(inputMessage[0])) ) == true && inputMessage[0] != 0)
    {
        //DEBUG
        //printInputMessage_and_write();

        // check MODE INPUT ADD FUNCTION COMPARE WITH COMMANDMODE_MESSAGE
        if ( compareWith_CommandMode_message() == true )
        {
            debugPort->println(F("We are in COMMAND_MODE!"));
            commandModeFlag = true;
            eraseInputMessage();
            debugPort->println("commandModeFlag == " + (String) commandModeFlag );

            // setup AT CF (Frequency)
            if (verification_AT_XX('c', 'f', Default_AT_CF) == false )
            {
                debugPort->println("AT_CF verification FAILED");
                return false;
            }
            else
            {
                debugPort->println(F("WOW! ok atcf"));
                debugPort->println();
                //erase data in inputMessage
                eraseInputMessage();
            }
            
            // Setup AT PL
            if (verification_AT_XX('p', 'l', Default_AT_PL) == false )
            {   debugPort->println("AT_PL verification FAILED");
                return false;
            }
            else
            {
                debugPort->println(F("WOW! ok atpl"));
                debugPort->println();
                //erase data in inputMessage
                eraseInputMessage();
            }

            //setupAT AP  AP ('a','p')
            if (verification_AT_XX('a', 'p', Default_AT_AP) == false )
            {
                debugPort->println(F("AT_AP verification FAILED =("));
                eraseInputMessage();
                return false;
            }
            else
            {
                debugPort->println(F("WOW! ok atap"));
                debugPort->println();
                //erase data in inputMessage
                eraseInputMessage();
            }

            // Setup AT RB
            if (verification_AT_XX('r', 'b', Default_AT_RB) == false )
            {
                debugPort->println(F("AT_RB verification FAILED"));
                return false;
            }
            else
            {
                debugPort->println(F("WOW! ok atrb"));
                debugPort->println();
                //erase data in inputMessage
                eraseInputMessage();
            }
            
            // if (verification_AT_XX('s', 'm', Default_AT_SM) == false )
            //{
            //     debugPort->println("AT_SM verification FAILED");
            //     return false;
            //}
            //else
            //{
            //    debugPort->println(F("WOW! ok atsm"));
            //    debugPort->println();
            //    //erase data in inputMessage
            //    eraseInputMessage();
            //}

            debugPort->println("AFTER AT commands -> commandModeFlag == " + (String) commandModeFlag + "  and need_to_SAVE_changes_flag == " + (String) need_to_SAVE_changes_flag);      



            if (need_to_SAVE_changes_flag == true)
            {
                send_AT_AC(); // this return "  OK. >" check it!
                // delay(1000);
                debugPort->println(F("Read AT AC answer"));

                if ( fill_InputMessage_Val_fromBuffer( &inputMessage[0], (sizeof(inputMessage)/sizeof(inputMessage[0]))) ) 
                {
                    if (isOK_Answer() == true)
                    {
                        debugPort->println(F("AT AC isOK, new parameters is write"));
                        //if (CF_frequence_changed)
                        //eeprom_write(New_CF_Frequency)

                        need_to_SAVE_changes_flag = false;
                    }
                    else
                    {
                        debugPort->println(F("AT AC  failed?"));
                        return false;
                    }
                }
                else
                {
                    debugPort->println(F("Lost connection with Mbee"));
                    return false;
                }

                //exit from CommandMode 
                sendTriplePlus();
                commandModeFlag = !commandModeFlag;
                debugPort->println("AFTER AT commands -> commandModeFlag == " + (String) commandModeFlag);
                return OVERWRITE_PARAMETRERS;     
            }            
            
            debugPort->println(F("personal stop SIGNAL!!!"));
            //exit
            sendTriplePlus();
            commandModeFlag = !commandModeFlag;
            debugPort->println("AFTER AT commands -> commandModeFlag == " + (String) commandModeFlag);
            return true;    
        }
        else // not inside command mode
        {
            debugPort->println(F("Wrong answer on \"+++\""));
            //exit
            sendTriplePlus();
            commandModeFlag = !commandModeFlag;
            return false;
        }
    }
    else // no answer after +++
    {
        debugPort->println(F("No Answer from Mbee"));
        return false;
    }
    
    delay(1000);
    return false;

}


bool AT_Speaker::verification_AT_XX(char FirstSymbol, char SecondSymbol, uint32_t default_value)
{
    debugPort->println("we want-> at" + (String) FirstSymbol + (String) SecondSymbol + " value==" + (String) default_value);

    // Send A_T_FS_SS 
    sendAtRequest(FirstSymbol, SecondSymbol);

    switch(checkAnswer(default_value)) // UPGRADE
    {
        case NO_ANSWER:
        {
            debugPort->println(F("NO answer from MBee =("));
            return false;
            //break;
        }
        case OK_ANSWER:
        {
            debugPort->println("AT" + (String) FirstSymbol + (String) SecondSymbol + " == " + (String) default_value + " CONFIRMED!");
            return true;
        }
        case NEED_TO_CHANGE_THE_PARAMETER:
        {
            debugPort->println(F("Change AT parameter"));
            eraseInputMessage();
            sendAtCommand( FirstSymbol, SecondSymbol, default_value);  // UPGRADE
            if ( fill_InputMessage_Val_fromBuffer( &inputMessage[0], (sizeof(inputMessage)/sizeof(inputMessage[0]))) ) 
            {
                if (isOK_Answer() == true)
                {
                    debugPort->println("AT " + (String) FirstSymbol + (String) SecondSymbol + " parameter changed");
                    need_to_SAVE_changes_flag = true;
                    return true;
                    break;
                }
                else
                {
                    debugPort->println(F("AT parameter  NOT changed, error"));
                    return false;
                }
                break;
            }
            else
            {
                debugPort->println(F("Lost connection with Mbee =/"));
                return false;
            }
        }
    }      
    return false;
}
    /////////////////////////////////////////////func/////////////////////////////////////////////////////////// 



void AT_Speaker::sendAtCommand(char a, char b, int parameter)
{
    debugPort->println( "We want to set at" + (String) a + (String) b + (String) " to " + (String) parameter );
    //debugPort->
    // UPGRADE THIS
    while(1);
    if (parameter < 0 || parameter > 9)
    {
        debugPort->println(F("Invalid parameter"));
        return;
    }





    _streamRef->print(F("a"));
    delay(10);
    _streamRef->print(F("t"));
    delay(10);
    _streamRef->print(a);
    delay(10);
    _streamRef->print(b);
    delay(10);
    _streamRef->print(parameter);
    delay(10);
    _streamRef->write(13);  // carriage return
    delay(20);
    debugPort->println(F("AT command sended"));
    // delay(1000);
    return;
}

void AT_Speaker::send_AT_AC()
{
    _streamRef->print(F("a"));
    delay(10);
    _streamRef->print(F("t"));
    delay(10);
    _streamRef->print(F("a"));
    delay(10);
    _streamRef->print(F("c"));
    delay(10);
    _streamRef->write(13);  // carriage return
    delay(10);
    debugPort->println(F("AT AC sended"));
    delay(100);  // ???
    return;
}

void AT_Speaker::sendAtRequest(char a, char b)
{
    debugPort->println();
    debugPort->println(F("sendAtRequest()"));
    _streamRef->print(F("a")); // ???
    delay(10);
    _streamRef->print(F("t")); // ???
    delay(10);
    _streamRef->write(a);
    delay(10);
    _streamRef->write(b);
    delay(10);
    _streamRef->write(13);
    delay(100);

    if ( _streamRef->available() > 0)
        debugPort->println(F("at_xx answer mb ok"));
    else
    {
        debugPort->println(F("at_xx no answer "));   
        delay(1000); 
    }
    return;
}

bool AT_Speaker::fill_InputMessage_Val_fromBuffer(int8_t * inputVal, size_t ArraySize)
{
    int temp = 0;
    unsigned int j = 0;
    bool flag = false;
    // length_answer = 0;
    while (_streamRef->available() > 0)
    {
        if (flag == false) flag = true;
        temp = _streamRef->read();
        // debugPort->println(" ArraySize== " + (String) ArraySize);
        if ( j < ArraySize )                        /*(  sizeof(inputMessage)/sizeof(inputMessage[0]) ) )*/ // < 18
        {
            inputVal[j] = temp;
            // delay(1);   // wtf??? this cure bug

            debugPort->print(F("inputVal["));
            debugPort->print(j);
            debugPort->print(F("] == "));
            debugPort->write(inputVal[j]);
            debugPort->println();
        }
        j++;
    }
    if (j != false)
    {
        length_answer = j;
    }
    if (flag == 0) 
    {
        debugPort->println(F("no input data from Mbee Oo"));
        // length_answer = 0;
        delay(5000);
        return false;
    }
    return true;
}

void AT_Speaker::printInputMessage_to_Debug()
{
    for( unsigned int i = 0; i < sizeof(inputMessage) / sizeof(inputMessage[0]) ; i++ )
    {
        debugPort->print(inputMessage[i]);
        debugPort->print(F("|"));
        debugPort->write(inputMessage[i]);
        debugPort->print(F(" -|"));
    }
    debugPort->println();
}

bool AT_Speaker::compareWith_CommandMode_message()
{
    int8_t goodCompare = 0;
    for ( unsigned int i = 0; i < sizeof(CommandMode_message) / sizeof(CommandMode_message[0]) ; i++ )
    {
        //debugPort->println("inputMessage[" + (String) i + "] = " + (String) inputMessage[i] );
        if (CommandMode_message[i] == inputMessage[i])
        {
            goodCompare++;

        }
        else
        {
            debugPort->print(F("goodCompare = " ));
            debugPort->println(goodCompare);
            debugPort->println(F(" Not match Input"));
            goodCompare = 0;
            return false;
        }
    }
    if ( goodCompare == sizeof(CommandMode_message) / sizeof(CommandMode_message[0]) )
        return true;
    debugPort->println(F("Compare error"));
    return false;
}

int AT_Speaker::checkAnswer(uint32_t SoloParam)
{
    // read answer
    if (fill_InputMessage_Val_fromBuffer( &inputMessage[0], sizeof(inputMessage) / sizeof(inputMessage[0]) ) == true)
    {
        /*
        GOTO 
        1) выцедить ответ от mbee записав длину ответа (кол-во символов) 
        2) как только идёт 13 в ответе чекаем что дальше идет 13 10 62 ("Enter" "\n" ">")
        3) как только принимаем 62(">") от Mbee это конец приёма
        4) на выходе надо иметь индекс начала и индекс конца ответа на команду АТ ХХ в массиве inputMessage
        5) конвертировать из массива цифр в int и сравнить с тем значением которое нам нужно на самом деле
        */

        length_answer = 0;
        bool good_answer = false;
        for( uint8_t i = 0; i < sizeof(inputMessage) / sizeof(inputMessage[0]) - 1; i++)
        {
            if(inputMessage[i] >= 10 && inputMessage[i] <=57) 
            {
                if (inputMessage[i] == 13 && inputMessage[i+1] == 10)
                {   
                    good_answer = true;
                    debugPort->print("good_answer ==");
                    debugPort->println(good_answer);
                    break;
                }
                length_answer++;
                debugPort->print("_inputMessage[" + (String) i + "] - ");
                debugPort->write(inputMessage[i]);
                debugPort->println();
                debugPort->print("length_answer ==");
                debugPort->println(length_answer);
            }
        }
        uint32_t source_val = 0;
        uint32_t temp = 0;
        // temp = 100000*10L ; // fix ~

        // check flag
        if (good_answer == true)
        {
            //compile our number
            for( uint8_t i = 0; i < length_answer ; i++ )
            {
                temp = (inputMessage[i] - Zero_ASCII);
                for (uint8_t f = (length_answer - i - 1); f > 0 ; f--)
                {
                    temp = temp * 10;
                    // debugPort->print("temp[");
                    // debugPort->print(f);
                    // debugPort->print("] =");
                    // debugPort->println(temp);
                }
                
                // debugPort->print("add good- ");
                // debugPort->println(temp);
                source_val = temp + source_val;  // TOP!!!
                debugPort->print("source_val[");
                debugPort->print(i);
                debugPort->print("]==");
                debugPort->println(source_val);
                
                // source_val = azaza * pow(10, (length_answer - i - 1) ) + source_val; // fix it!

            }

        }
        else
        {
            debugPort->print("good_answer ==");
            debugPort->println(good_answer);
        }
        
        // write whole number
        debugPort->print(F("Debug output: "));
        debugPort->println(source_val);



        debugPort->print(F("Mbee answer: "));
        for( uint8_t i = 0; i < length_answer ; i++ )
        {
            debugPort->print(inputMessage[i] - Zero_ASCII);
        }     
        debugPort->println();
        // print inputMessage to DebugPort
        // printInputMessage_to_Debug();

        // sendTriplePlus();
        // debugPort->println(F("My Stop!"));
        // while(1);


        // REWORK
        // check actual AT_AP ADD FUNCTIOn checkDefault(int Default_AT_AP);
        if ( source_val == SoloParam) // 48 + 2 = 50. -> 2 in ASCII table
        {
            debugPort->println();
            debugPort->println(F("At_XX_already_is_OK"));
            sendTriplePlus();
            while(1);
            return OK_ANSWER;
        }
        else
        {
            // ДОРАБОТАТЬ
            debugPort->println(F("Change_AT_XX"));
            return NEED_TO_CHANGE_THE_PARAMETER;
            //Change_needed_flag = true;
        }
    }
    else
    {
        debugPort->println(F("Lost connection with Mbee"));
        return NO_ANSWER;
    }          
    /////////////////////////////////////////////func///////////////////////////////////////////////////////////
}

bool AT_Speaker::isOK_Answer()
{
    uint8_t combo_ok = 0; 
    //read input message
    for ( unsigned int i = 0; i < (sizeof(OK_message)/sizeof(OK_message[0])); i++ )
    {
        if(inputMessage[i] == OK_message[i])
        {
            // debugPort->println(F("ok+"));
            combo_ok++;
            // debugPort->println(inputMessage[i]);
        }
        else
        {   
            debugPort->println(F("ok=0 =("));
        }
    }
    // analyze
    if( combo_ok == sizeof(OK_message)/sizeof(OK_message[0]) )
    {
        debugPort->println(F("OK_message accepted"));
        return true;
    }
    else
    {
        debugPort->print(F("combo_ok =="));
        debugPort->println(combo_ok);
        combo_ok = 0;
        debugPort->println(F("OK_message not accepted"));
        return false;
    }
}

void AT_Speaker::printInputMessage_and_write()
{
    // print inputMessage to DebugPort
    debugPort->println();
    for (unsigned int i = 0; i < sizeof(inputMessage)/sizeof(inputMessage[0]) ; i++)
    {
        debugPort->print(inputMessage[i]);
        debugPort->print(F(" | "));                    
        // if (inputMessage[i] == 13)  // carriage return
            // {

            // }
    }
    debugPort->println();
    return;
}

void AT_Speaker::resetMyEntire_stuff()
{
    debugPort->println(F("Reseter"));
    //reset Temp Array
    eraseInputMessage();

    //reset flags/parameters
    need_to_SAVE_changes_flag = false;
    length_answer = 0;

    return;
}