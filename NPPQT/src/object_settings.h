#ifndef OBJECT_SETTINGS_H
#define OBJECT_SETTINGS_H

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QDialog>
#include <QLabel>
#include <src/object_classes.h>

class verify_data
{
public:
    int matching_command;
    QString box_label;
    QString box_tooltip;

};

extern verify_data verification_data[VERIFY_MAX];


/*
 *  Include all generic functions to be
 * used across all object dialog boxes.
 */
class ObjectSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectSettingsDialog(int o_idx);

private:
    int object_index;

    object_type *o_ptr;
    object_kind *k_ptr;

    QSpacerItem *vspacer;

    QVBoxLayout *main_layout;
    QHBoxLayout *main_across;

    QVBoxLayout *object_type_ver;
    QButtonGroup *object_type_group;

    QVBoxLayout *object_kind_ver;
    QButtonGroup *object_kind_group;

    QVBoxLayout  *squelch_vlay;

    QVBoxLayout  *squelch_buttons;
    QButtonGroup *squelch_group;
    QRadioButton *squelch_never;
    QRadioButton *squelch_pickup_no;
    QRadioButton *squelch_pickup_yes;
    QRadioButton *squelch_always;

    QVBoxLayout  *quality_buttons;
    QButtonGroup *quality_group;
    QRadioButton *quality_none;
    QRadioButton *quality_cursed;
    QRadioButton *quality_average;
    QRadioButton *quality_good_strong;
    QRadioButton *quality_good_weak;
    QRadioButton *quality_all_but_artifact;

    QVBoxLayout *ego_buttons;
    QButtonGroup   *ego_group;
    QRadioButton *ego_no;
    QRadioButton *ego_yes;

    QDialogButtonBox *buttons;

    void add_type_checkbox(byte which_ver);
    void add_kind_checkbox(byte which_ver);
    void add_object_verifications();
    void add_squelch_buttons();
    void add_quality_buttons();
    void add_ego_buttons();

private slots:
    void update_squelch_setting(int id);
    void update_quality_setting(int id);
    void update_ego_setting(int id);
    void update_object_type_settings(int id, bool checked);
    void update_object_kind_settings(int id, bool checked);
};

// object_settings
extern void object_settings(int i);
extern bool get_item_allow(int item, int verify_command);
extern void apply_object_kind_settings(object_type *o_ptr);


#endif // OBJECT_SETTINGS_H
