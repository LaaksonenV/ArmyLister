#ifndef SPECIALLABEL_H
#define SPECIALLABEL_H

/*#include <QLabel>
#include <QList>

class Selection;
class ModelItem;
class QPushButton;
class SpecialLabel : public QLabel
{
    Q_OBJECT

public:
    SpecialLabel(const QString &text, ModelItem *parent,
                 int position);
    SpecialLabel(SpecialLabel *copy, ModelItem *parent);
    ~SpecialLabel();

    void reset();

    void addSelection(QList<std::pair<QString, int> > list);
    void removeSelection(const QString &text);
    void denySelection(bool denied);
    bool hasSelections() const;
    void select(const QString &s);

    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void mousePressEvent(QMouseEvent*e);
    void paintEvent(QPaintEvent *e);

public slots:
    void setText(const QString& s);
    void setPoint(int p);
    void on_selection(const QString &text);

private:
    ModelItem *_modelParent;
    QString _baseText;
    int _basePoints;
    int _position;

    Selection *_selection;
    bool _selectionDenied;

    bool _mouseIn;
};

class Selection : public QWidget
{
    Q_OBJECT

public:
    Selection(const QFont &f);
    Selection(Selection *copy);

    void addSelection(const QList<std::pair<QString,int> > &list);
    void removeSelection(const QString &text);
    bool hasSelections() const;

    void mousePressEvent(QMouseEvent *e);
    void select(const QString &s);

signals:
    void selected(const QString &text);

private:
    void createSelection(const QStringList &list);

private:    
    const unsigned short itemHeight = 24;
    QFont _f;

    QList <QPushButton *> _labels;
};

class MultiSelectionHandler : public QObject
{
    Q_OBJECT

public:
    MultiSelectionHandler(ModelItem *parent);
    MultiSelectionHandler(MultiSelectionHandler *copy, ModelItem *parent);

    void addText(const QList<std::pair<QString, int> > &list);
    void addAt(int at);

public slots:
    void selectionChanged(int at, const QString &text, ModelItem *copy);

private:
    QList<int> _ats;
    QStringList _texts;
};
*/
#endif // SPECIALLABEL_H
