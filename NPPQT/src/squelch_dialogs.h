#ifndef SQUELCH_DIALOGS_H
#define SQUELCH_DIALOGS_H


#include "src/npp.h"
#include <src/nppdialog.h>
#include <QGridLayout>




class QualitySquelchDialog : public NPPDialog
{
    Q_OBJECT

public:
    explicit QualitySquelchDialog(void);


private slots:
    void update_quality_squelch_settings(void);
    void set_all_quality_none(void) {set_all_squelch_quality(SQUELCH_NONE);}
    void set_all_quality_cursed(void) {set_all_squelch_quality(SQUELCH_CURSED);}
    void set_all_quality_average(void) {set_all_squelch_quality(SQUELCH_AVERAGE);}
    void set_all_quality_good_strong(void) {set_all_squelch_quality(SQUELCH_GOOD_STRONG);}
    void set_all_quality_good_weak(void) {set_all_squelch_quality(SQUELCH_GOOD_WEAK);}
    void set_all_quality_all_but_artifact(void) {set_all_squelch_quality(SQUELCH_ALL);}

private:
    QPointer<QWidget> central;

    void make_quality_squelch_pushbuttons(QHBoxLayout *return_layout);
    void make_quality_squelch_radiobuttons(QGridLayout *return_layout);
    void set_all_squelch_quality(int new_quality);
};

#endif // SQUELCH_DIALOGS_H

