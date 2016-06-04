#ifndef EDITORHISTORY_H
#define EDITORHISTORY_H

#include <QDebug>
#include <QMap>
#include <QString>
#include <QList>

class EditorHistory
{

public:
    EditorHistory(int sizeLimit);
    void setSizeLimit(int size);
    void clear();
    void add(int id);
    void remove(int id);
    bool isEmpty();
    bool exists(int id);
    void normalize();
    int getSize();

    void back();
    void forward();
    int getCurrent();

    void debug();

private:
    int sizeLimit;
    int currentPosition;
    QList<int> history;
};

#endif // EDITORHISTORY_H
