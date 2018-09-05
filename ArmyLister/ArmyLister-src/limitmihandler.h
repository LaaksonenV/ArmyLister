#ifndef LIMITMIHANDLER_H
#define LIMITMIHANDLER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QMap>

class ModelItem;

class LimitMIhandler : public QObject
{
    Q_OBJECT

public:
    LimitMIhandler(QObject *parent);
    ~LimitMIhandler();

    void setLimits(int limit, int by = 0, int of = -1);
    void setModifiable(bool b, int of = -1);

    void addSelections(int of, const QStringList &texts,
                       int limit, int by = 0, bool modif = false);
    void setAddSource(ModelItem *item);
    void addPickTarget(ModelItem *item);
    void setPickSource(ModelItem *item);
    void setFollowing(ModelItem *item);

    void inputLimit (ModelItem *item);

public slots:
    void on_copy(ModelItem *cp);
    void on_cloning(ModelItem *to, int type);
    void on_remove(ModelItem *itm);
    void on_selection(int at, const QString &text, ModelItem *by);
    void on_check(int chk, ModelItem *by);
    void on_modelChange(int m, ModelItem* = nullptr);
    void on_modifierChange(int m);

private:
    struct Limiter
    {
        int _current = 0;
        int _limit = 0;
        int _timer = 0;
        int _divider = 0;
        bool _modifiable = false;
        QStringList _selections = QStringList();

        Limiter()
            : _current(0)
            , _limit(0)
            , _timer(-1)
            , _divider(0)
            , _modifiable(false)
            , _selections(QStringList())
        {}

        Limiter(Limiter *l)
            : _current(0)
            , _limit(0)
            , _timer(l->_timer)
            , _divider(l->_divider)
            , _modifiable(l->_modifiable)
            , _selections(l->_selections)
        {}
    };
    int updateLimit(Limiter *limt, bool global);

    QList<ModelItem*> _targets;

    QMap<int, QList<Limiter*>* > _selectables;
    QList<Limiter*> _globalLimits;
    int _globallim;
    int _modifier;
    int _models;
    ModelItem *_followed;

    LimitMIhandler* _limitClone;
};

#endif // LIMITMIHANDLER_H
