#ifndef OBJECT_DIALOG_H
#define OBJECT_DIALOG_H

#include "src/utilities.h"
#include <src/function_declarations.h>
#include <QVBoxLayout>
#include <QLabel>
#include <QVariant>
#include <QPushButton>
#include <QTabWidget>

/*
 * Holds information to add a command to the menu.
 */
class item_command
{
public:
    QChar action_char;
    byte object_command;
};

//Different types of object commands.
enum
{
    ITEM_EXAMINE = 0,
    ITEM_TAKEOFF,
    ITEM_WIELD,
    ITEM_SWAP,
    ITEM_USE,
    ITEM_REFILL,
    ITEM_FIRE,
    ITEM_FIRE_NEAR,
    ITEM_DROP,
    ITEM_PICKUP,
    ITEM_BROWSE,
    ITEM_STUDY,
    ITEM_CAST,
    ITEM_DESTROY,
    ITEM_INSCRIBE,
    ITEM_UNINSCRIBE,
    ITEM_ACTIVATE,
    ITEM_THROW,
    ITEM_MAX,
};



extern item_command item_command_info[ITEM_MAX];


/*
 *  Include all generic functions to be
 * used across all object dialog boxes.
 */
class ObjectDialog : public QDialog
{
    Q_OBJECT


public:
    QVBoxLayout *main_layout;
    void clear_grid_layout(QGridLayout *lay);
    void add_plain_label(QGridLayout *lay, QString label, int row, int col);
    void add_letter_label(QGridLayout *lay, QChar location, int label_num, int row, int col);
    void add_object_button(QGridLayout *lay, object_type *o_ptr, QChar location, s16b item_slot, int row, int col);
    void add_weight_label(QGridLayout *lay, object_type *o_ptr, int row, int col);


    // Functions to determine which buttons to add for each item.
    bool should_add_takeoff(object_type *o_ptr, s16b item_slot);
    bool should_add_wield(object_type *o_ptr, s16b item_slot);
    bool should_add_swap(object_type *o_ptr, s16b item_slot);
    bool should_add_use(object_type *o_ptr, s16b item_slot);
    bool should_add_refill(object_type *o_ptr, s16b item_slot);
    bool should_add_fire(object_type *o_ptr, s16b item_slot);
    bool should_add_fire_near(object_type *o_ptr, s16b item_slot);
    bool should_add_drop(object_type *o_ptr, s16b item_slot);
    bool should_add_pickup(object_type *o_ptr, s16b item_slot);
    bool should_add_browse(object_type *o_ptr, s16b item_slot);
    bool should_add_study(object_type *o_ptr, s16b item_slot);
    bool should_add_cast(object_type *o_ptr, s16b item_slot);
    bool should_add_destroy(object_type *o_ptr, s16b item_slot);
    bool should_add_uninscribe(object_type *o_ptr, s16b item_slot);
    bool should_add_activate(object_type *o_ptr, s16b item_slot);
    bool should_add_throw(object_type *o_ptr, s16b item_slot);

    // Functions to add the buttons
    void do_buttons(QGridLayout *lay, object_type *o_ptr, s16b item_slot, s16b row, s16b col);
    void add_examine(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_takeoff(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_wield(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_swap(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_use(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_refill(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_fire(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_fire_near(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_drop(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_pickup(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_browse(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_study(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_cast(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_destroy(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_inscribe(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_uninscribe(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_activate(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_throw(QGridLayout *lay, s16b item_slot, int row, int col);

    s16b idx_from_click(QString id);

    void reset_messages(message_type last_message, QLabel *message_one, QLabel *message_two, QLabel *message_three);

    void update_floor_list(QGridLayout *lay, bool label, bool buttons);
    void update_inven_list(QGridLayout *lay, bool label, bool buttons);
    void update_equip_list(QGridLayout *lay, bool label, bool buttons);
    void update_quiver_list(QGridLayout *lay, bool label, bool buttons);

    public slots:
    void object_click();
    void button_click();

    //  These should be replaced by real functions
    // in the child class.
    virtual void update_dialog() {};
    virtual void close_dialog() {};
};

#endif // OBJECT_DIALOG_H
