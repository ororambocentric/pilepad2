#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QListWidgetItem>
#include <note.h>
#include "qmyhighlighter.h"
#include "keytranslator.h"
#include "editorhistory.h"


struct CurrentNote{
    int id;
    QString title;
    QString body;
    int marker;
    bool changed;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void closeEvent(QCloseEvent *event);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void switchToSearchMode();
    void switchToEditorMode();
    void ifNotesNotExists();
    void firstNoteAdded();
    void updateListWidgetNotes();
    void printFoundNotes(NoteArray data);

    void switchEditorToNote(int noteID);

    QIcon getIconForMarker(int marker);

    QString mode;
    CurrentNote currentNote;

    EditorHistory *editorHistory;

public slots:

    void slotAbout();
    void slotExit();

    void inSearchOpenButtonClickSlot();
    void inSearchEditButtonClickSlot();
    void inSearchDeleteButtonClickSlot();

    void slotPasteHorizonLineToEditor();
    void slotPasteDoubleHorizonLineToEditor();
    void slotPasteDateToEditor();
    void slotPasteRandomPassword();

    void slotEditorHistoryGoBack();
    void slotEditorHistoryGoForward();

private slots:

    void changeEvent(QEvent*);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void trayActionExecute(QSystemTrayIcon::ActivationReason reason);
    void setTrayIconActions();
    void showTrayIcon();
    void slotContextExit();

    void on_pushButtonApplyChanges_clicked();
    void on_listWidgetNotes_itemClicked(QListWidgetItem *item);
    void on_pushButtonMode_clicked();
    void on_lineEditSearch_textChanged(const QString &arg1);
    void on_lineEditEditorNoteTitle_textChanged(const QString &arg1);
    void on_plainTextEditEditorNoteBody_textChanged();
    void on_pushButtonAddNote_clicked();
    void on_lineEditNotesFilter_textChanged(const QString &arg1);
    void on_listWidgetNotes_customContextMenuRequested(const QPoint &pos);
    void on_listWidgetNotes_contextDeleteNote();



    void on_listWidgetNotes_itemSelectionChanged();



    void on_comboBoxEditorMarker_currentIndexChanged(const QString &arg1);

    void on_comboBoxSearchMarker_currentIndexChanged(const QString &arg1);


    void on_pushButtonEditorPanelDeleteNote_clicked();

private:
    Ui::MainWindow *ui;

    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;

    QMenu *listWidgetNotesMenu;

    void keyPressEvent(QKeyEvent *e);
    KeyTranslator*  keyTranslator;

};

#endif // MAINWINDOW_H
