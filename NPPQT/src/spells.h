#ifndef SPELLS_H
#define SPELLS_H

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>

typedef struct monster_banish_choices monster_banish_choices;

struct monster_banish_choices
{
    QString mon_symbol;
    QString mon_race;
};

class BanishSelectDialog : public QDialog
{
    Q_OBJECT

public:
    BanishSelectDialog(void);
    bool return_value;

private slots:
    void update_banish_choice(int choice);

private:
    int chosen_type;
    QDialogButtonBox *button_boxes;
    QComboBox *banish_choice;
};

#endif // SPELLS_H
