#ifndef PLAYER_EQUIPMENT_H
#define PLAYER_EQUIPMENT_H

// Class to build an interactive dialog
// for the player's inventory.


#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QVariant>
#include <QPushButton>


class InvenDialog : public QDialog
{
    Q_OBJECT
private:
    QVBoxLayout *main_layout;
    QLabel *header_inven;
    QGridLayout *inven_list;
    void update_header_label();
    void clear_grid_layout(QGridLayout *lay);


public:
    explicit InvenDialog(void);
    void inventory_update();

signals:


public slots:
    void inven_click();
    void help_click();

};

#endif // PLAYER_EQUIPMENT_H
