#include "storedialog.h"
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QSpacerItem>
#include <QGroupBox>
#include <QSpinBox>
#include "npp.h"
#include "store.h"

void launch_store(int store_idx)
{
    StoreDialog *dlg = new StoreDialog(store_idx);
    dlg->exec();
    delete dlg;
}

StoreDialog::StoreDialog(int _store, QWidget *parent): NPPDialog(parent)
{
    store_idx = _store;

    central = new QWidget;
    QVBoxLayout *lay1 = new QVBoxLayout;
    central->setLayout(lay1);
    lay1->setSpacing(10);
    this->setClient(central);  // IMPORTANT: it must be called AFTER setting the layout

    QWidget *area1 = new QWidget;
    lay1->addWidget(area1);
    QHBoxLayout *lay3 = new QHBoxLayout;
    area1->setLayout(lay3);
    lay3->setContentsMargins(0, 0, 0, 0);

    QPushButton *btn_buy = new QPushButton("Buy (F2)");
    lay3->addWidget(btn_buy);
    connect(btn_buy, SIGNAL(clicked()), this, SLOT(buy_click()));

    QPushButton *btn_sell = new QPushButton("Sell (F3)");
    lay3->addWidget(btn_sell);
    connect(btn_sell, SIGNAL(clicked()), this, SLOT(sell_click()));

    QPushButton *btn_toggle = new QPushButton("Toggle inven/equip (F4)");
    lay3->addWidget(btn_toggle);
    connect(btn_toggle, SIGNAL(clicked()), this, SLOT(toggle_inven()));

    mode_label = new QLabel("");
    lay3->addWidget(mode_label);
    mode_label->setStyleSheet("font-weight: bold;");

    set_mode(SMODE_DEFAULT);

    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay3->addItem(spacer);

    QWidget *area4 = new QWidget;
    QHBoxLayout *lay6 = new QHBoxLayout;
    area4->setLayout(lay6);
    lay6->setContentsMargins(0, 0, 0, 0);
    lay1->addWidget(area4);

    QPushButton *btn_info = new QPushButton("Information (F5)");
    lay6->addWidget(btn_info);
    connect(btn_info, SIGNAL(clicked()), this, SLOT(info_click()));

    QPushButton *btn_exam = new QPushButton("Examine (F6)");
    lay6->addWidget(btn_exam);
    connect(btn_exam, SIGNAL(clicked()), this, SLOT(exam_click()));

    spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay6->addItem(spacer);

    QWidget *area2 = new QWidget;
    QHBoxLayout *lay2 = new QHBoxLayout;
    area2->setLayout(lay2);
    lay2->setContentsMargins(0, 0, 0, 0);
    lay1->addWidget(area2);

    QGroupBox *box1 = new QGroupBox("Store items");
    lay2->addWidget(box1);
    QVBoxLayout *lay4 = new QVBoxLayout;
    box1->setLayout(lay4);

    store_area = new QWidget;
    lay4->addWidget(store_area);

    char_tabs = new QTabWidget;
    lay2->addWidget(char_tabs);

    inven_tab = new QWidget;
    equip_tab = new QWidget;

    char_tabs->addTab(inven_tab, tr("Inventory"));
    char_tabs->addTab(equip_tab, tr("Equipment"));
    char_tabs->setCurrentIndex(0);

    this->reset_store();
    this->reset_inventory();
    this->reset_equip();

    QWidget *area3 = new QWidget;
    QHBoxLayout *lay5 = new QHBoxLayout;
    area3->setLayout(lay5);
    lay5->setContentsMargins(0, 0, 0, 0);
    lay1->addWidget(area3);

    spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay5->addItem(spacer);

    QPushButton *btn_close = new QPushButton("Close");
    lay5->addWidget(btn_close);
    connect(btn_close, SIGNAL(clicked()), this, SLOT(reject()));

    this->clientSizeUpdated();
}

void StoreDialog::info_click()
{
    set_mode(SMODE_INFO);
}

void StoreDialog::exam_click()
{
    set_mode(SMODE_EXAMINE);
}

void StoreDialog::buy_click()
{
    set_mode(SMODE_BUY);
}

void StoreDialog::reset_store()
{
    QGridLayout *lay = dynamic_cast<QGridLayout *>(store_area->layout());
    if (lay == 0) {
        lay = new QGridLayout;
        store_area->setLayout(lay);
    }
    // Remove previous items
    QLayoutItem *item;
    while ((item = lay->takeAt(0)) != 0) {
        delete item;
    }
    lay->addWidget(new QLabel("Price"), 0, 1);
    store_type *st = &store[store_idx];
    int i;
    for (i = 0; i < st->stock_num; i++) {
        object_type *o_ptr = &st->stock[i];
        if (o_ptr->k_idx == 0) continue;
        QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        QPushButton *btn = new QPushButton(QString("%1) %2")
                                           .arg(number_to_letter(i))
                                           .arg(desc));
        QString id = QString("s%1").arg(i);
        btn->setProperty("item_id", QVariant(id));
        s32b price = price_item(o_ptr, false);
        QString style = "text-align: left;";
        if (price <= p_ptr->au) {
            style.append("font-weight: bold;");
        }
        btn->setStyleSheet(style);
        connect(btn, SIGNAL(clicked()), this, SLOT(item_click()));
        lay->addWidget(btn, i + 1, 0);

        QLabel *l = new QLabel(_num(price));
        lay->addWidget(l, i + 1, 1);
    }
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addItem(spacer, i + 1, 0);
}

void StoreDialog::set_mode(int _mode)
{
    mode = _mode;
    QString names[] = {
        QString(""), QString("Buying"), QString("Selling"),
        QString("Querying info"), QString("Examining")
    };
    mode_label->setText(names[mode]);
}

void StoreDialog::reset_inventory()
{
    QWidget *tab = inven_tab;
    QGridLayout *lay = dynamic_cast<QGridLayout *>(tab->layout());
    if (lay == 0) {
        lay = new QGridLayout;
        tab->setLayout(lay);
    }
    // Remove previous items
    QLayoutItem *item;
    while ((item = lay->takeAt(0)) != 0) {
        delete item;
    }
    lay->addWidget(new QLabel("Price"), 0, 1);
    int i;
    for (i = 0; i < INVEN_WIELD - 1; i++) {
        object_type *o_ptr = inventory + i;
        if (o_ptr->k_idx == 0) continue;
        QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        QPushButton *btn = new QPushButton(QString("%1) %2")
                                           .arg(number_to_letter(i))
                                           .arg(desc));
        QString id = QString("i%1").arg(i);
        btn->setProperty("item_id", QVariant(id));
        QString style = "text-align: left;";
        if (store_will_buy(store_idx, o_ptr)) {
            style.append("font-weight: bold;");
        }
        btn->setStyleSheet(style);
        connect(btn, SIGNAL(clicked()), this, SLOT(item_click()));
        lay->addWidget(btn, i + 1, 0);

        s32b price = price_item(o_ptr, true);
        QLabel *l = new QLabel(_num(price));
        lay->addWidget(l, i + 1, 1);
    }
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addItem(spacer, i + 1, 0);
}

void StoreDialog::reset_equip()
{
    QWidget *tab = equip_tab;
    QGridLayout *lay = dynamic_cast<QGridLayout *>(tab->layout());
    if (lay == 0) {
        lay = new QGridLayout;
        tab->setLayout(lay);
    }
    // Remove previous items
    QLayoutItem *item;
    while ((item = lay->takeAt(0)) != 0) {
        delete item;
    }
    int n = 0;
    int i;
    lay->addWidget(new QLabel("Price"), 0, 1);
    for (i = INVEN_WIELD; i < QUIVER_END; i++) {
        object_type *o_ptr = inventory + i;
        if (o_ptr->k_idx == 0) continue;
        QString use;
        if (i < QUIVER_START) {
            use = QString("%1: ").arg(mention_use(i));
        }
        QString desc = object_desc(o_ptr, ODESC_PREFIX | ODESC_FULL);
        QPushButton *btn = new QPushButton(QString("%1) %2%3")
                                           .arg(number_to_letter(n++))
                                           .arg(use)
                                           .arg(desc));
        QString id = QString("e%1").arg(i);
        btn->setProperty("item_id", QVariant(id));
        QString style = "text-align: left;";
        if (store_will_buy(store_idx, o_ptr)) {
            style.append("font-weight: bold;");
        }
        btn->setStyleSheet(style);
        connect(btn, SIGNAL(clicked()), this, SLOT(item_click()));
        lay->addWidget(btn, i + 1, 0);



        s32b price = price_item(o_ptr, true);
        QLabel *l = new QLabel(_num(price));
        lay->addWidget(l, i + 1, 1);
    }
    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addItem(spacer, i + 1, 0);
}

void StoreDialog::toggle_inven()
{
    char_tabs->setCurrentIndex((char_tabs->currentIndex() + 1) % char_tabs->count());
}

void StoreDialog::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_F4:
        this->toggle_inven();
        break;
    case Qt::Key_F2:
        this->buy_click();
        break;
    case Qt::Key_F3:
        this->sell_click();
        break;
    case Qt::Key_F5:
        this->info_click();
        break;
    case Qt::Key_F6:
        this->exam_click();
        break;
    default:
        if (event->text().length() > 0
                && event->text().at(0).isLetter()) {
            if (mode == SMODE_DEFAULT) return;
            QString letter = event->text().at(0);
            letter.append(") ");
            QWidget *container = char_tabs->currentWidget();
            if (mode == SMODE_BUY || mode == SMODE_EXAMINE) {
                container = store_area;
            }
            QList<QPushButton *> lst = container->findChildren<QPushButton *>();
            for (int i = 0; i < lst.size(); i++) {
                if (lst.at(i)->text().startsWith(letter)) {
                    lst.at(i)->click();
                    break;
                }
            }
            return;
        }
        NPPDialog::keyPressEvent(event);
    }
}

void StoreDialog::item_click()
{
    QObject *obj = QObject::sender();

    QString id = obj->property("item_id").toString();
    int aux_mode = mode;
    if (aux_mode == SMODE_DEFAULT) {
        if (id.startsWith("e") || id.startsWith("i")) aux_mode = SMODE_SELL;
        else aux_mode = SMODE_BUY;
    }

    set_mode(aux_mode);

    if (aux_mode == SMODE_SELL && id.startsWith("s")) return;
    if (aux_mode == SMODE_BUY && !id.startsWith("s")) return;

    object_type *o_ptr;
    int item = id.mid(1).toInt();  // Get item index
    if (id.startsWith("s")) {
        o_ptr = &(store[store_idx].stock[item]);
    }
    else {
        o_ptr = inventory + item;
    }

    int price = price_item(o_ptr, false);

    switch (aux_mode) {
    case SMODE_BUY:    
        if (price > p_ptr->au) {
            pop_up_message_box("It's too expensive", QMessageBox::Critical);
            return;
        }
        do_buy(o_ptr);
        break;
    case SMODE_SELL:
        if (!store_will_buy(store_idx, o_ptr)) {
            pop_up_message_box("I don't buy that kind of items", QMessageBox::Critical);
            return;
        }
        do_sell(o_ptr);
        break;
    case SMODE_INFO:
    case SMODE_EXAMINE:
        object_info_screen(o_ptr);
        break;
    }

    set_mode(SMODE_DEFAULT);
}

void StoreDialog::sell_click()
{
    set_mode(SMODE_SELL);
}

bool StoreDialog::do_buy(object_type *o_ptr)
{
    int amt = request_amt(o_ptr, true);

    if (amt == 0) return false;

    return true;
}

bool StoreDialog::do_sell(object_type *o_ptr)
{
    int amt = request_amt(o_ptr, false);

    if (amt == 0) return false;

    return true;
}

void QuantityDialog::do_accept()
{
    amt = amt_spin->value();
    accept();
}

QuantityDialog::QuantityDialog(object_type *op, bool buy)
{
    o_ptr = op;

    buying = buy;

    amt = 0;

    price = price_item(o_ptr, !buy);

    if (buying) {
        int money = p_ptr->au;
        max = money / MAX(price, 1);
        max = MIN(max, o_ptr->number);
    }
    else {
        max = o_ptr->number;
    }

    QVBoxLayout *lay1 = new QVBoxLayout;
    this->setLayout(lay1);

    QString verb = tr("sell");
    if (buying) verb = tr("buy");

    QString desc = object_desc(o_ptr, ODESC_FULL | ODESC_PREFIX);
    QString msg = tr("How many items do you want to %1 of %2?").arg(verb).arg(desc);
    question = new QLabel(msg);
    lay1->addWidget(question);

    amt_spin = new QSpinBox();
    lay1->addWidget(amt_spin);
    amt_spin->setMinimum(0);
    amt_spin->setMaximum(max);
    if (buying) amt_spin->setValue(1);
    else amt_spin->setValue(max);

    total_label = new QLabel("");
    lay1->addWidget(total_label);
    update_totals(amt_spin->value());

    connect(amt_spin, SIGNAL(valueChanged(int)), this, SLOT(update_totals(int)));

    QHBoxLayout *lay2 = new QHBoxLayout;
    lay2->setContentsMargins(0, 0, 0, 0);
    lay1->addLayout(lay2);

    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    lay2->addItem(spacer);

    QPushButton *btn1 = new QPushButton("Ok");
    lay2->addWidget(btn1);
    connect(btn1, SIGNAL(clicked()), this, SLOT(do_accept()));

    QPushButton *btn2 = new QPushButton("Cancel");
    lay2->addWidget(btn2);
    connect(btn2, SIGNAL(clicked()), this, SLOT(reject()));
}

void QuantityDialog::update_totals(int value)
{
    int n = value;
    int money = p_ptr->au;
    int sign = 1;
    if (buying) sign = -1;
    QString msg = tr("Max. items: %4 - Total price: %1 - Gold: %2 - After: %3")
            .arg(n * price).arg(money).arg(money + n * price * sign).arg(max);
    total_label->setText(msg);
}

int StoreDialog::request_amt(object_type *o_ptr, bool buying)
{
    int amt = o_ptr->number;
    int price = price_item(o_ptr, !buying);
    QString desc = object_desc(o_ptr, ODESC_FULL | ODESC_PREFIX);
    QString verb = tr("sell");
    if (buying) verb = tr("buy");

    if (amt == 1) {
        QString msg = tr("Do you want to %1 %2? Price: %3")
                .arg(verb).arg(desc).arg(price);
        if (!get_check(msg)) return 0;
        return 1;
    }

    QuantityDialog *dlg = new QuantityDialog(o_ptr, buying);
    dlg->exec();
    amt = dlg->amt;
    delete dlg;

    return amt;
}
