#ifndef CMDS_H
#define CMDS_H



#include "src/object_classes.h"
#include "src/player.h"
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QKeyEvent>
#include <QTabWidget>
#include <QLabel>
#include <QDialog>

enum
{
    COL_SPELL_TITLE = 0,
    COL_LEVEL,
    COL_MANA,
    COL_FAIL_PCT,
    COL_INFO,
    COL_HELP
};

// Add the headers


class QTabWidget;



class SpellSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpellSelectDialog(int *spell, QString prompt, int mode, bool *success, bool *cancelled);

protected:
    void keyPressEvent(QKeyEvent* which_key);

private slots:
    // Receives the number of the button pressed.
    void button_press(int num);
    void help_press(int num);



private:

    QTabWidget *spell_dialog;
    QWidget *spell_tab;
    QLabel *main_prompt;

    QButtonGroup *spell_select_group;
    QButtonGroup *spell_help_group;


    // Functions to build the actual tabs
    void build_spellbook_dialog(int mode);

    QString format_button_name(QChar char_index, object_type *o_ptr, byte which_tab, int slot);

    //Functions to track the list of possible items
    void count_spells(int mode);
    void clear_spells(void);

    QString get_spell_comment(int spell);

    // Variables for keeping track of which item is selected
    int selected_button;

    bool available_spells[BOOKS_PER_REALM_ANGBAND][SPELLS_PER_BOOK];
    bool available_books[BOOKS_PER_REALM_ANGBAND];

    bool num_spells;
    int num_available_spellbooks;
    int max_spellbooks;

    // Are we a priest studying a book, or do we get to choose ?
    bool choosing_book;

};

class ObjectDestroyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectDestroyDialog(s16b o_idx);

private slots:
    void do_object_settings();
private:
    s16b this_object_num;

};



// cmd_actions.cpp
extern bool do_cmd_test(int y, int x, int action, bool do_message);
extern void do_cmd_go_up(void);
extern void do_cmd_go_down(void);
extern void command_open(cmd_arg args);
extern void do_cmd_open(void);
extern void command_disarm(cmd_arg args);
extern void do_cmd_disarm(void);
extern void do_search(void);
extern void do_cmd_toggle_search(void);
extern void command_search(cmd_arg args);
extern void do_cmd_search(void);
extern void command_tunnel(cmd_arg args);
extern void do_cmd_tunnel(void);
extern void command_close(cmd_arg args);
extern void do_cmd_close(void);
extern void command_alter(cmd_arg args);
extern void do_cmd_alter(int dir);
extern void command_spike(cmd_arg args);
extern void do_cmd_spike(void);
extern void command_rest(cmd_arg args);
extern void do_cmd_rest(void);
extern void command_run(cmd_arg args);
extern void do_cmd_run(int dir);
extern void command_walk(cmd_arg args);
extern void do_cmd_walk(int dir, bool jumping);
extern void command_bash(cmd_arg args);
extern void do_cmd_bash(void);
extern void command_hold(cmd_arg args);
extern void do_cmd_hold();


// cmd3.cpp
extern void wield_in_quiver(object_type *o_ptr, int slot);
extern void wield_item(object_type *o_ptr, int item, int slot);
extern bool make_monster_trap(void);
extern void py_set_trap(int y, int x);
extern bool py_modify_trap(int y, int x);
extern void do_cmd_look(void);
extern void command_make_trap(cmd_arg args);
extern void do_cmd_make_trap(void);

// cmd4.cpp
extern void do_cmd_feeling(void);
extern void do_cmd_repeat(void);

//cmd_objects
extern cmd_arg obj_wield(object_type *o_ptr, cmd_arg args);
extern cmd_arg obj_uninscribe(object_type *o_ptr, cmd_arg args);
extern bool trap_related_object(object_type *o_ptr);
extern void command_uninscribe(cmd_arg args);
extern void do_cmd_uninscribe(void);
extern void command_inscribe(cmd_arg args);
extern void do_cmd_inscribe(void);
extern cmd_arg obj_examine(object_type *o_ptr, cmd_arg args);
extern void command_examine(cmd_arg args);
extern void do_cmd_examine(void);
extern void command_takeoff(cmd_arg args);
extern void do_cmd_takeoff(void);
extern void command_wield(cmd_arg args);
extern void do_cmd_wield(void);
extern void command_drop(cmd_arg args);
extern void do_cmd_drop(void);
extern void command_refuel(cmd_arg args);
extern void do_cmd_refuel(void);
extern void command_swap(cmd_arg args);
extern void do_cmd_swap_weapon(void);
extern void command_destroy(cmd_arg args);
extern void do_cmd_destroy(void);
extern void do_cmd_activate(void);
extern void do_cmd_use_item(void);

// cmd_pickup.cpp
extern bool put_object_in_inventory(object_type *o_ptr);
extern void do_cmd_pickup_from_pile(bool pickup, bool message);
extern void py_pickup_gold(void);
extern void command_pickup(cmd_arg args);
extern void py_pickup(bool pickup);
extern void do_cmd_pickup(void);
extern int move_player(int dir, int jumping);

// cmd_spell.cpp
extern int spell_chance(int spell);
extern bool spell_okay(int spell, bool known);
extern void command_browse(cmd_arg arg);
extern void do_cmd_browse(void);
extern bool player_can_use_book(const object_type *o_ptr, bool known);
extern void command_study(cmd_arg args);
extern void do_cmd_study(void);
extern void command_cast(cmd_arg args);
extern void do_cmd_cast(void);
extern s16b get_spell_from_list(s16b book, s16b spell);

#endif // CMDS_H
