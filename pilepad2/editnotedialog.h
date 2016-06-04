#ifndef EDITNOTEDIALOG_H
#define EDITNOTEDIALOG_H

#include <QDialog>
#include "keytranslator.h"

namespace Ui {
class EditNoteDialog;
}

class EditNoteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditNoteDialog(QWidget *parent = 0);
    ~EditNoteDialog();

    QString getNoteTitleEdit();
    QString getNoteBodyEdit();
    int getNoteMarker();

    void setNoteTitleEdit(QString title);
    void setNoteBodyEdit(QString body);
    void setNoteMarker(int marker);

public slots:

    void slotPasteHorizonLineToEditor();
    void slotPasteRandomPassword();
    void slotPasteDoubleHorizonLineToEditor();
    void slotPasteDateToEditor();
    void slotSaveShortcut();

private:
    Ui::EditNoteDialog *ui;
    KeyTranslator*  keyTranslator;

};

#endif // EDITNOTEDIALOG_H
