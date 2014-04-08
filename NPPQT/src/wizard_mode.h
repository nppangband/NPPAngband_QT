#ifndef WIZARD_MODE_H
#define WIZARD_MODE_H

#include <QtWidgets>
#include "src/npp.h"

class WizardModeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WizardModeDialog(void);

private:
    QLabel *main_prompt;
    QDialogButtonBox *cancel_button;


    //Wizard mode functions
    void wiz_cure_all(void);
    void wiz_know_all(void);
    void wiz_jump(void);
    void wiz_summon(void);

};

#endif // WIZARD_MODE_H
