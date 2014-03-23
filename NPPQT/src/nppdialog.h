#ifndef NPPDIALOG_H
#define NPPDIALOG_H

#include <QDialog>

class QScrollArea;

class NPPDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NPPDialog(QWidget *parent = 0, int _padding = 30, qreal _max_ratio = 0.8);

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

#endif // NPPDIALOG_H
