#include "nppdialog.h"
#include <QScrollArea>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QIntValidator>
#include "npp.h"

void RestDialog::on_clicked()
{
    QObject *obj = QObject::sender();
    int value = obj->property("choice").toInt();
    if (value < 0) choice = value;
    else {
        value = turns_edit->text().toInt();
        if (value < 0 || value > 9999) {
            pop_up_message_box("Invalid turns value", QMessageBox::Critical);
            return;
        }
        choice = value;
    }
    this->accept();
}

void RestDialog::keyPressEvent(QKeyEvent *event)
{
    QString txt = event->text();

    if (txt.length() > 0) {
        QChar chr = txt.toLower().at(0);

        if (chr.isDigit()) {
            turns_edit->setFocus();
            turns_edit->setText(chr);
            QPushButton *btn = this->findChild<QPushButton *>("turns");
            btn->setDefault(true);
            return;
        }

        if (chr.isLetter()) {
            QList<QPushButton *> list = this->findChildren<QPushButton *>();
            txt = chr;
            txt += ") ";
            for (int i = 0; i < list.size(); i++) {
                if (list.at(i)->text().startsWith(txt)) {
                    list.at(i)->click();
                    return;
                }
            }
        }
    }

    QDialog::keyPressEvent(event);
}

RestDialog::RestDialog(int *_choice)
{
    choice = *_choice = 0;

    QVBoxLayout *lay1 = new QVBoxLayout;
    this->setLayout(lay1);
    //lay1->setContentsMargins(0, 0, 0, 0);

    QLabel *lb = new QLabel("Pick the rest type");
    lb->setStyleSheet("font-weight: bold;");
    lay1->addWidget(lb);

    struct {
        QString name;
        int value;
    } choices[] = {
      {"Complete", -2},
      {"Hit points and Spell points", -1},
      {"Hit points", -3},
      {"Spell points", -4},
      {"", 0}
    };

    for (int i = 0; !choices[i].name.isEmpty(); i++) {
        QString lb = number_to_letter(i);
        lb += ") ";
        lb += choices[i].name;
        QPushButton *btn = new QPushButton(lb);
        btn->setProperty("choice", choices[i].value);
        btn->setStyleSheet("text-align: left");
        connect(btn, SIGNAL(clicked()), this, SLOT(on_clicked()));

        lay1->addWidget(btn);
    }

    QHBoxLayout *lay2 = new QHBoxLayout;
    lay2->setContentsMargins(0, 0, 0, 0);
    lay1->addLayout(lay2); 

    turns_edit = new QLineEdit;
    turns_edit->setValidator(new QIntValidator(0, 9999));
    lay2->addWidget(turns_edit, 1);

    QPushButton *btn2 = new QPushButton("e) Turns");
    btn2->setObjectName("turns");
    lay2->addWidget(btn2);
    btn2->setStyleSheet("text-align: left");
    btn2->setProperty("choice", 1);
    connect(btn2, SIGNAL(clicked()), this, SLOT(on_clicked()));

    this->exec();

    *_choice = choice;
}

NPPDialog::NPPDialog(QWidget *parent, int _padding, qreal _max_ratio) :
    QDialog(parent)
{
    padding = _padding;
    max_ratio = _max_ratio;

    scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout;
    this->setLayout(layout);

    layout->addWidget(scrollArea);

    layout->setContentsMargins(0, 0, 0, 0);

    client = 0;
}

QSize NPPDialog::sizeHint() const
{
    if (client == 0) return QSize(-1, -1);

    QSize size = client->sizeHint();

    QDesktopWidget dsk;

    QRect geo = dsk.screenGeometry();

    // Check screen size
    int w = geo.width() * max_ratio;
    int h = geo.height() * max_ratio;

    if (size.width() > w) {
        size.setWidth(w);
    }

    if (size.height() > h) {
        size.setHeight(h);
    }

    size += QSize(padding, padding);

    return size;
}

void NPPDialog::clientSizeUpdated()
{
    if (client) {
        client->updateGeometry();
        client->setMinimumSize(client->sizeHint());
    }

    this->resize(this->sizeHint());
}

// Call this function AFTER setting the client layout
void NPPDialog::setClient(QWidget *_client)
{
    client = _client;

    scrollArea->setWidget(client);
}
