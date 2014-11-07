#ifndef OBJECT_SETTINGS_H
#define OBJECT_SETTINGS_H

#include <QVBoxLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QDialog>
#include <QLabel>
#include <src/object_classes.h>
#include <src/object_dialog.h>


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
    void close_dialog();
    int object_index;

    object_type *o_ptr;
    object_kind *k_ptr;

    QLabel *header_main;

    QVBoxLayout *main_layout;
    QHBoxLayout *main_across;
    QVBoxLayout *squelch_vlay;

    QVBoxLayout *squelch_buttons;
    QButtonGroup *squelch_group;
    QRadioButton *squelch_never;
    QRadioButton *squelch_pickup_no;
    QRadioButton *squelch_pickup_yes;
    QRadioButton *squelch_always;

    QVBoxLayout *quality_buttons;
    QGroupBox   *quality_group;
    QRadioButton *quality_none;
    QRadioButton *quality_average;
    QRadioButton *quality_good_strong;
    QRadioButton *quality_good_weak;
    QRadioButton *quality_all_but_artifact;

    QVBoxLayout *ego_buttons;
    QGroupBox   *ego_group;
    QRadioButton *ego_no;
    QRadioButton *ego_yes;

    void add_squelch_buttons();

    //void add_quality_buttons();
    //void add_ego_buttons();

private slots:
    void update_squelch_setting(int id);
};

// object_settings
extern void object_settings(int i);

#endif // OBJECT_SETTINGS_H
