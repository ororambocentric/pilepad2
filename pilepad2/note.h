#ifndef NOTE_H
#define NOTE_H

#include <QtCore>

class Note
{
public:
    Note();
    int id;
    QString title;
    QString body;
    int marker;
};

struct NoteArray{
    Note *notes;
    int count;
};



#endif // NOTE_H
