#include "core.h"
#include "note.h"
#include <QtSql>

Core::Core()
{
}

QSqlDatabase Core::dbConnect(){

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("pilepad.db");
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError();
    }

    return db;
}

bool Core::createDB(){
    QSqlDatabase db = this->dbConnect();
    QSqlQuery query;
    QString sql;

    sql = "CREATE TABLE IF NOT EXISTS sqlite_sequence(name,seq);";
    query.exec(sql);

    sql = "CREATE TABLE IF NOT EXISTS \"note\" ( \"id\" INTEGER PRIMARY KEY AUTOINCREMENT, \"title\" TEXT, \"body\" TEXT, \"ltitle\" TEXT, \"lbody\" TEXT, \"marker\" INTEGER DEFAULT (1));";
    query.exec(sql);

    return true;

}

Note Core::getNote(int noteID){

    Note data;

    QSqlDatabase db = this->dbConnect();

    QSqlQuery query;
    QString sql = "SELECT * FROM note WHERE id='%1';";
    QString str = sql.arg(noteID);
    if (query.exec(str)) {
        QSqlRecord rec = query.record();
        while(query.next()){
            data.id = query.value(rec.indexOf("id")).toInt();
            data.title = query.value(rec.indexOf("title")).toString();
            data.body = query.value(rec.indexOf("body")).toString();
            data.marker = query.value(rec.indexOf("marker")).toInt();
        };
        return data;
    } else {
        qDebug() << "Cannot exec query:" << db.lastError();
        return data;
    }

}

Note Core::getNoteByTitle(QString title){

    Note data;
    QSqlDatabase db = this->dbConnect();
    QSqlQuery query;
    QString sql = "SELECT id, body, marker FROM note WHERE title = :title";
    query.prepare(sql);
    query.bindValue(":title", title);

    if (query.exec()) {
            QSqlRecord rec = query.record();
            while(query.next()){
                data.id = query.value(rec.indexOf("id")).toInt();
                data.body = query.value(rec.indexOf("body")).toString();
                data.marker = query.value(rec.indexOf("marker")).toInt();
            };
            return data;
        } else {
            qDebug() << "Cannot exec query:" << db.lastError();
            return data;
        }

}

NoteArray Core::getAllNotesWithoutBody(){

    NoteArray data;
    data.count = 0;
    data.notes = new Note[0];

    QSqlDatabase db = this->dbConnect();
    QSqlQuery query;
    QString sql = "SELECT id, title, marker FROM note ORDER BY id DESC";
    query.exec(sql);

    // Считываем данные из базы

    QSqlRecord rec = query.record();

    int recCount = 0;
    while (query.next()) {
        recCount++;
    };

    if(recCount==0) return data;

    data.count = recCount;
    data.notes = new Note[recCount];

    query.first();
    data.notes[0].id = query.value(rec.indexOf("id")).toInt();
    data.notes[0].title = query.value(rec.indexOf("title")).toString();
    data.notes[0].marker = query.value(rec.indexOf("marker")).toInt();

    int index=1;
    while(query.next()){
        data.notes[index].id = query.value(rec.indexOf("id")).toInt();
        data.notes[index].title = query.value(rec.indexOf("title")).toString();
        data.notes[index].marker = query.value(rec.indexOf("marker")).toInt();
        index++;
    };

    return data;
}

bool Core::addNote(QString title, QString body, int marker = 1){

    QString ltitle = title;
    ltitle = ltitle.toLower();

    QString lbody = body;
    lbody = lbody.toLower();

    QSqlDatabase db = this->dbConnect();

    QSqlQuery query;
    QString sql = "INSERT INTO note (title, body, ltitle, lbody, marker) VALUES(:title, :body, :ltitle, :lbody, :marker);";
    query.prepare(sql);
    query.bindValue(":title", title);
    query.bindValue(":body", body);
    query.bindValue(":ltitle", ltitle);
    query.bindValue(":lbody", lbody);
    query.bindValue(":marker", marker);

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "Cannot exec query:" << db.lastError();
        return false;
    }

}

bool Core::updateNote(int noteID, QString title, QString body, int marker){

    QString ltitle = title;
    ltitle = ltitle.toLower();

    QString lbody = body;
    lbody = lbody.toLower();

    QSqlDatabase db = this->dbConnect();

    QSqlQuery query;
    QString sql = "UPDATE note SET title = :title, body = :body, ltitle = :ltitle, lbody = :lbody, marker = :marker WHERE id = :id;";

    query.prepare(sql);
    query.bindValue(":title", title);
    query.bindValue(":body", body);
    query.bindValue(":ltitle", ltitle);
    query.bindValue(":lbody", lbody);
    query.bindValue(":marker", marker);
    query.bindValue(":id", QString::number(noteID));

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "Cannot exec query:" << db.lastError();
        return false;
    }

}

bool Core::deleteNote(int noteID){

    QSqlDatabase db = this->dbConnect();

    QSqlQuery query;
    QString sql = "DELETE FROM note WHERE id='%1';";
    QString str = sql.arg(noteID);
    if (query.exec(str)) {
        return true;
    } else {
        qDebug() << "Cannot exec query:" << db.lastError();
        return false;
    }

}


NoteArray Core::searchNotes(QString str, int marker){

    NoteArray data;
    data.count = 0;
    data.notes = new Note[0];

    QSqlDatabase db = this->dbConnect();

    QSqlQuery query;

    QStringList words = str.split(" ");

    QString sql = " SELECT id, title, body, marker FROM note WHERE (1 = 1) ";

    for(int i=0; i<words.count(); i++){

        if(words.at(i)=="") continue;

        if(i>=0){
          sql += " AND ";
        }
        sql += " (ltitle LIKE :a"+QString::number(i)+" OR ltitle LIKE :b"+QString::number(i)+" OR ltitle LIKE :c"+QString::number(i)+" OR lbody LIKE :a"+QString::number(i)+" OR lbody LIKE :b"+QString::number(i)+" OR lbody LIKE :c"+QString::number(i)+") ";

    }
    if(marker>0){
        sql+=" AND marker = :marker";
    }
    sql+=" ORDER BY id DESC";

    query.prepare(sql);

    for(int i=0; i<words.count(); i++){

        query.bindValue(":a"+QString::number(i), "%"+words.at(i)+"%");
        query.bindValue(":b"+QString::number(i), "%"+this->strLatToRus(words.at(i))+"%");
        query.bindValue(":c"+QString::number(i), "%"+this->strRusToLat(words.at(i))+"%");

    }

    if(marker>0){
        query.bindValue(":marker", marker);
    }

    query.exec();

    // Считываем данные из базы

    QSqlRecord rec = query.record();

    int recCount = 0;
    while (query.next()) {
        recCount++;
    };

    if(recCount==0) return data;

    data.count = recCount;
    data.notes = new Note[recCount];

    query.first();
    data.notes[0].id = query.value(rec.indexOf("id")).toInt();
    data.notes[0].title = query.value(rec.indexOf("title")).toString();
    data.notes[0].body = query.value(rec.indexOf("body")).toString();
    data.notes[0].marker = query.value(rec.indexOf("marker")).toInt();

    int index=1;
    while(query.next()){
        data.notes[index].id = query.value(rec.indexOf("id")).toInt();
        data.notes[index].title = query.value(rec.indexOf("title")).toString();
        data.notes[index].body = query.value(rec.indexOf("body")).toString();
        data.notes[index].marker = query.value(rec.indexOf("marker")).toInt();
        index++;
    };

    return data;

}

QString Core::strLatToRus(QString str){

    QString str_search = "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./";
    QString str_replace = "ё1234567890-=йцукенгшщзхъ\\фывапролджэячсмитьбю.";

    str = str.toLower();

    for(int i=0; i<str.length(); i++){
        for(int j=0; j<str_search.length(); j++){
            if(str.at(i)==str_search.at(j)){
                str[i]=str_replace.at(j);
            }
        }
    }

    return str;

}

QString Core::strRusToLat(QString str){

    QString str_search = "ё1234567890-=йцукенгшщзхъ\\фывапролджэячсмитьбю.";
    QString str_replace = "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./";

    str = str.toLower();

    for(int i=0; i<str.length(); i++){
        for(int j=0; j<str_search.length(); j++){
            if(str.at(i)==str_search.at(j)){
                str[i]=str_replace.at(j);
            }
        }
    }

    return str;

}
