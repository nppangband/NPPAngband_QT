#ifndef PLAYER_EQUIPMENT_H
#define PLAYER_EQUIPMENT_H

// Class to build an interactive dialog
// for all player info.

#include <src/player_command.h> //includes npp.h
#include "src/utilities.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QVariant>
#include <QPushButton>
#include <QString>



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
    void add_object_button(QGridLayout *lay, object_type *o_ptr, QChar location, int label_num, int row, int col);
    void add_weight_label(QGridLayout *lay, object_type *o_ptr, int row, int col);


    void add_message_area();
    void reset_messages();


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

    // Add the buttons
    void do_buttons(QGridLayout *lay, object_type *o_ptr, s16b item_slot, s16b row, s16b col);
    void add_examine(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_takeoff(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_wield(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_swap(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_use(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_refill(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_fire(QGridLayout *lay, s16b item_slot, int row, int col);
    void add_fire_near(QGridLayout *lay, s16b item_slot, int row, int col);

    s16b idx_from_click(QString id);

    // Add message area
    QWidget *message_area;
    QVBoxLayout *lay_message;
    QLabel  *message_one;
    QLabel  *message_two;
    QLabel  *message_three;
    message_type last_message;

public slots:
    void object_click();
    void button_click();
};

class InvenDialog : public ObjectDialog
{
    Q_OBJECT
private:
    QGridLayout *inven_list;
    QLabel *header_inven;
    void update_inven_header();
    void update_inven_list(bool buttons);

public:
    explicit InvenDialog(bool buttons);
    void inventory_update();

signals:

};

class EquipDialog : public ObjectDialog
{
    Q_OBJECT

private:
    QGridLayout *equip_list;
    QLabel *header_equip;
    void update_equip_header();
    void update_equip_list(bool buttons);

public:
    explicit EquipDialog(bool buttons);
    void equipment_update();

signals:

};

#endif // PLAYER_EQUIPMENT_H
