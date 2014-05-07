#ifndef NPPDIALOG_H
#define NPPDIALOG_H

#include <QDialog>

class QScrollArea;
class QLineEdit;

class NPPDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NPPDialog(QWidget *parent = 0, int _padding = 20, qreal _max_ratio = 0.85);

    int padding;
    qreal max_ratio;
    QScrollArea *scrollArea;
    QWidget *client;

    virtual QSize sizeHint() const;

    void setClient(QWidget *_client);
    void clientSizeUpdated();

signals:

public slots:

};

class RestDialog: public QDialog
{
    Q_OBJECT
public:
    int choice;
    QLineEdit *turns_edit;

    RestDialog(int *_choice);

    void keyPressEvent(QKeyEvent *event);
public slots:
    void on_clicked();
};

#endif // NPPDIALOG_H
