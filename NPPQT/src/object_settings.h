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
class ObjectSettingsDialog: public QDialog
{
    Q_OBJECT
public:

    ObjectSettingsDialog(s16b o_idx);

private:
    object_kind *k_ptr;
    object_type *o_ptr;

    QVBoxLayout *object_type_ver;
    QButtonGroup *object_type_group;

    QVBoxLayout *object_kind_ver;
    QButtonGroup *object_kind_group;

    QButtonGroup *squelch_group;
    QRadioButton *squelch_never;
    QRadioButton *squelch_pickup_no;
    QRadioButton *squelch_pickup_yes;
    QRadioButton *squelch_always;

    QButtonGroup *quality_group;
    QRadioButton *quality_none;
    QRadioButton *quality_cursed;
    QRadioButton *quality_average;
    QRadioButton *quality_good_strong;
    QRadioButton *quality_good_weak;
    QRadioButton *quality_all_but_artifact;

    QButtonGroup   *ego_group;
    QRadioButton *ego_no;
    QRadioButton *ego_yes;

    void add_type_checkbox(byte which_ver);
    void add_kind_checkbox(byte which_ver);
    void add_object_verifications();
    void add_squelch_buttons(QVBoxLayout *squelch_buttons);
    void add_quality_buttons(QVBoxLayout *quality_buttons);
    void add_ego_buttons(QVBoxLayout *ego_buttons);

private slots:
    void update_object_type_settings(int id, bool checked);
    void update_object_kind_settings(int id, bool checked);
    void update_squelch_setting(int id);
    void update_quality_setting(int id);
    void update_ego_setting(int id);
};

// object_settings
extern void object_settings(s16b o_idx);
extern bool get_item_allow(int item, int verify_command);
extern void apply_object_kind_settings(object_type *o_ptr);


#endif // OBJECT_SETTINGS_H
