#ifndef WIZARD_MODE_H
#define WIZARD_MODE_H

#include <QtWidgets>
#include "src/npp.h"

class EditObjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditObjectDialog(void);
};


class EditCharacterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditCharacterDialog(void);
};

class WizardModeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WizardModeDialog(void);

private:
    QLabel *main_prompt;
    QLabel *player_section;
    QLabel *dungeon_section;
    QLabel *object_section;

    QDialogButtonBox *buttons;



private slots:

    //Wizard mode functions
    void wiz_cure_all(void);
    void wiz_know_all(void);
    void wiz_jump(void);
    void wiz_teleport_to_target(void);
    void wiz_phase_door(void);
    void wiz_teleport(void);
    void wiz_edit_character(void);
    void wiz_summon(void);
    void wiz_banish(void);
    void wiz_detect_all_monsters(void);
    void wiz_detection(void);
    void wiz_magic_mapping(void);
    void wiz_level_light(void);
    void wiz_redraw_dungeon(void);
    void wiz_mass_create_items(void);
    void wiz_create_good_item(void);
    void wiz_create_great_item(void);
    void wiz_edit_object(void);
    void wiz_mass_identify_items(void);
    void wiz_winners_kit(void);
};

#endif // WIZARD_MODE_H
