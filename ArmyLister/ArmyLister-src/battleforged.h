#ifndef BATTLEFORGED_H
#define BATTLEFORGED_H

#include <QWidget>

template<typename T> class QList;

class Detachment;

class BattleForged : public QWidget
{
    Q_OBJECT
public:
    explicit BattleForged(QWidget *parent = nullptr);

    virtual QSize sizeHint() const;

    void setLists(const QString &detachments);
    void addDetachment(const QStringList &args);
    void clear();

signals:

public slots:
    void roleSelected(int role, int amount);
    void detachmentSelected(Detachment *det, bool check);
    void detachmentCloned(Detachment *det, QStringList args);
    
private:
    QList<Detachment*> qv_detachmentList;

    QList<int> qv_currentMin;
    QList<int> qv_currentMax;
};

#endif // BATTLEFORGED_H
