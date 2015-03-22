#ifndef COMMAND_LIST_H
#define COMMAND_LIST_H


#include <src/player_screen.h>

typedef struct command_desc command_desc;

struct command_desc
{
    QString command_title;
    QString command_key;
};

class CommandList : public QDialog
{
    Q_OBJECT

public:
    explicit CommandList(void);

private:
    void add_dir_keyboard(QVBoxLayout *return_layout, bool keyboard);
    void add_dir_commands(QGridLayout *return_layout);
    void add_keyboard_commands(QGridLayout *return_layout);


};

extern void do_cmd_command_list(void);
extern void commands_new_keyset(QKeyEvent* which_key, bool shift_key, bool alt_key, bool ctrl_key, bool meta_key);
extern void commands_angband_keyset(QKeyEvent* which_key, bool shift_key, bool alt_key, bool ctrl_key, bool meta_key);
extern void commands_roguelike_keyset(QKeyEvent* which_key, bool shift_key, bool alt_key, bool ctrl_key, bool meta_key);

#endif // COMMAND_LIST_H
