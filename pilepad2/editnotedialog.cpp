#include "helper_functions.h"
#include "editnotedialog.h"
#include "ui_editnotedialog.h"
#include <QShortcut>

EditNoteDialog::EditNoteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditNoteDialog)
{
    ui->setupUi(this);

    keyTranslator = new KeyTranslator(ui->noteTitleEdit);
    ui->noteTitleEdit->installEventFilter(keyTranslator);
    ui->noteBodyEdit->installEventFilter(keyTranslator);

    QShortcut* shortcut1 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus), ui->noteBodyEdit);
    connect(shortcut1, SIGNAL(activated()), this, SLOT(slotPasteHorizonLineToEditor()));

    QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Equal), ui->noteBodyEdit);
    connect(shortcut2, SIGNAL(activated()), this, SLOT(slotPasteDoubleHorizonLineToEditor()));

    QShortcut* shortcut3 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_D), ui->noteBodyEdit);
    connect(shortcut3, SIGNAL(activated()), this, SLOT(slotPasteDateToEditor()));

    QShortcut* shortcut4 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), ui->noteBodyEdit);
    connect(shortcut4, SIGNAL(activated()), this, SLOT(slotSaveShortcut()));

    QShortcut* shortcut5 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_P), ui->noteBodyEdit);
    connect(shortcut5, SIGNAL(activated()), this, SLOT(slotPasteRandomPassword()));

    ui->noteTitleEdit->setFocus();
}

EditNoteDialog::~EditNoteDialog()
{
    delete ui;
}

QString EditNoteDialog::getNoteTitleEdit(){
    return ui->noteTitleEdit->text();
}
QString EditNoteDialog::getNoteBodyEdit(){
    return ui->noteBodyEdit->document()->toPlainText();
}
int EditNoteDialog::getNoteMarker(){
    return ui->comboBoxEditDialogMarker->currentIndex()+1;
}

void EditNoteDialog::setNoteTitleEdit(QString title){
    ui->noteTitleEdit->setText(title);
}
void EditNoteDialog::setNoteBodyEdit(QString body){
    ui->noteBodyEdit->setPlainText(body);
}
void EditNoteDialog::setNoteMarker(int marker){
    ui->comboBoxEditDialogMarker->setCurrentIndex(marker-1);
}
void EditNoteDialog::slotPasteHorizonLineToEditor(){

    ui->noteBodyEdit->insertPlainText("------------------------------");

}

void EditNoteDialog::slotPasteRandomPassword(){

    ui->noteBodyEdit->insertPlainText(generateRandomPassword());

}


void EditNoteDialog::slotPasteDoubleHorizonLineToEditor(){

    ui->noteBodyEdit->insertPlainText("==============================");

}

void EditNoteDialog::slotPasteDateToEditor(){

    ui->noteBodyEdit->insertPlainText(QDate::currentDate().toString("dd.MM.yyyy"));

}

void EditNoteDialog::slotSaveShortcut(){

    ui->buttonBox->accepted();

}

