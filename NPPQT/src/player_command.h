#ifndef PLAYER_COMMAND_H
#define PLAYER_COMMAND_H

#include "src/npp.h"


#define ARG_CHOICE          0x01
#define ARG_NUMBER          0x02
#define ARG_ITEM            0x04
#define ARG_DIRECTION		0x08
#define ARG_TARGET  		0x10
#define ARG_POINT   		0x20
#define ARG_SPECIAL   		0x40 // needs a special dialog



//Player "rest_modes"
enum
{
    REST_BOTH_SP_HP = 1,
    REST_COMPLETE,
    REST_HP,
    REST_SP,
    REST_TURNCOUNT

};

//Player "commands"
// Any new command needs to be added to command_info structure below
enum
{
    CMD_NONE = 0,
    CMD_RESTING,
    CMD_RUNNING,
    CMD_MAX
};


class command_type
{
public:
    byte cmd_needs;                         // which arguments does the function need?
    void (*command_function)(cmd_arg args);  //What function should be called for this command?
    bool repeat_allowed;                    // Is repeating allowed?
    u16b repeat_num;                       // Should repeating be done automatically?

    bool repeated_command_completed(void);
};

extern command_type command_info[CMD_MAX];



#endif // PLAYER_COMMAND_H
