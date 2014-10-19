#ifndef PLAYER_COMMAND_H
#define PLAYER_COMMAND_H

#include "src/npp.h"


#define ARG_CHOICE          0x01
#define ARG_NUMBER          0x02
#define ARG_ITEM            0x04
#define ARG_DIRECTION		0x08
#define XX_UNUSED           0x10
#define ARG_POINT   		0x20
#define ARG_SPECIAL   		0x40 // needs a special dialog
#define ARG_SLOT            0x80



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
    CMD_WALK,
    CMD_OPEN,
    CMD_CLOSE,
    CMD_SPIKE,
    CMD_DISARM,
    CMD_BASH,
    CMD_TUNNEL,
    CMD_ALTER,
    CMD_SEARCH,
    CMD_MAKE_TRAP,
    CMD_HOLD,
    CMD_TAKEOFF,
    CMD_WIELD,
    CMD_SWAP,
    CMD_ITEM_USE,
    CMD_REFUEL,
    CMD_FIRE,
    CMD_FIRE_NEAR,
    CMD_DROP,
    CMD_PICKUP,
    CMD_BROWSE,
    CMD_STUDY,
    CMD_CAST,
    CMD_DESTROY,
    CMD_EXAMINE,
    CMD_INSCRIBE,
    CMD_UNINSCRIBE,
    CMD_ACTIVATE,
    CMD_THROW,
    CMD_MAX
};


class command_type
{

public:
    byte cmd_needs;                         // which arguments does the function need?
    void (*command_function)(cmd_arg args);  //What function should be called for this command?
    bool repeat_allowed;                    // Is repeating allowed?
    u16b repeat_num;                       // Automatic number of repeats.

    bool repeated_command_completed(void);
    bool needs_direction(void);
    bool needs_item(void);
    bool needs_quantity(void);
    QString prompt(int command);

};

extern command_type command_info[CMD_MAX];

extern void process_command(int item, s16b command);

#endif // PLAYER_COMMAND_H
