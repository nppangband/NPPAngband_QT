#ifndef PLAYER_EQUIPMENT_H
#define PLAYER_EQUIPMENT_H

// Class to build an interactive dialog
// for all player info.

#include <src/npp.h>
#include "src/utilities.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QVariant>
#include <QPushButton>

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
    void add_letter_label(QGridLayout *lay, QString id, int label_num, int row, int col);
    void add_object_button(QGridLayout *lay, object_type *o_ptr, QString id, int row, int col);
    void add_weight_label(QGridLayout *lay, object_type *o_ptr, int row, int col);
    void add_help_label(QGridLayout *lay, QString id, int row, int col);
    void add_message_area();
    void reset_messages();

    // Add message area
    QWidget *message_area;
    QVBoxLayout *lay_message;
    QLabel  *message_one;
    QLabel  *message_two;
    QLabel  *message_three;
    message_type last_message;

public slots:
    void object_click();
    void help_click();
};

class InvenDialog : public ObjectDialog
{
    Q_OBJECT
private:
    QGridLayout *inven_list;
    QLabel *header_inven;
    void update_inven_header();
    void update_inven_list();

public:
    explicit InvenDialog(void);
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
    void update_equip_list();

public:
    explicit EquipDialog(void);
    void equipment_update();

signals:

};

#endif // PLAYER_EQUIPMENT_H
