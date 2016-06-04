#ifndef CORE_H
#define CORE_H

#include <QtCore>
#include <QtSql>
#include "note.h"

class Core
{
public:
    Core();

    bool createDB();
    QSqlDatabase dbConnect();

    bool addNote(QString title, QString body, int marker);
    NoteArray searchNotes(QString str, int marker);
    Note getNote(int noteID);
    Note getNoteByTitle(QString title);
    NoteArray getAllNotesWithoutBody();
    bool updateNote(int noteID, QString title, QString body, int marker);
    bool deleteNote(int noteID);

    QString strLatToRus(QString str);
    QString strRusToLat(QString str);

};

#endif // CORE_H
