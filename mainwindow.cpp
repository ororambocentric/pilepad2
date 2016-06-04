#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "editnotedialog.h"
#include "core.h"
#include "note.h"
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <Qt>
#include <QCloseEvent>
#include <QShortcut>
#include <QLabel>

const int EDITOR_HISTORY_SIZE_LIMIT = 10;
Core *core = new Core();


void MainWindow::keyPressEvent(QKeyEvent *e){

    // Используем только в разработке, чтобы определять коды клавиш
    //qDebug() << QString::number(e->key());

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->editorHistory = new EditorHistory(EDITOR_HISTORY_SIZE_LIMIT);

    /*
    QFile file(":/styles/styles/theme-light.css");
    file.open(QFile::ReadOnly);
    QString strCSS = QLatin1String(file.readAll());
    qApp->setStyleSheet(strCSS);
    */

    // Перевод языка клавиш

    keyTranslator = new KeyTranslator(ui->plainTextEditEditorNoteBody);

    ui->plainTextEditEditorNoteBody->installEventFilter(keyTranslator);
    ui->lineEditEditorNoteTitle->installEventFilter(keyTranslator);
    ui->lineEditNotesFilter->installEventFilter(keyTranslator);
    ui->lineEditSearch->installEventFilter(keyTranslator);


    core->createDB();

    this -> setTrayIconActions();
    this -> showTrayIcon();

    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(slotAbout()));
    connect(ui->actionExit, SIGNAL(triggered()), SLOT(slotExit()));

    listWidgetNotesMenu = new QMenu();
    listWidgetNotesMenu->addAction("delete");


    QShortcut* shortcut1 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), ui->plainTextEditEditorNoteBody);
    connect(shortcut1, SIGNAL(activated()), this, SLOT(on_pushButtonApplyChanges_clicked()));

    QShortcut* shortcut2 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);
    connect(shortcut2, SIGNAL(activated()), this, SLOT(on_pushButtonMode_clicked()));

    QShortcut* shortcut3 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus), ui->plainTextEditEditorNoteBody);
    connect(shortcut3, SIGNAL(activated()), this, SLOT(slotPasteHorizonLineToEditor()));

    QShortcut* shortcut4 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Equal), ui->plainTextEditEditorNoteBody);
    connect(shortcut4, SIGNAL(activated()), this, SLOT(slotPasteDoubleHorizonLineToEditor()));

    QShortcut* shortcut5 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_D), ui->plainTextEditEditorNoteBody);
    connect(shortcut5, SIGNAL(activated()), this, SLOT(slotPasteDateToEditor()));

    QShortcut* shortcut6 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_N), this);
    connect(shortcut6, SIGNAL(activated()), this, SLOT(on_pushButtonAddNote_clicked()));

    QShortcut* shortcut7 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Comma), ui->plainTextEditEditorNoteBody);
    connect(shortcut7, SIGNAL(activated()), this, SLOT(slotEditorHistoryGoBack()));

    QShortcut* shortcut8 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Period), ui->plainTextEditEditorNoteBody);
    connect(shortcut8, SIGNAL(activated()), this, SLOT(slotEditorHistoryGoForward()));

    connect(listWidgetNotesMenu->actions()[0], SIGNAL(triggered()), this, SLOT(on_listWidgetNotes_contextDeleteNote()));

    //this->setWidth(950);
    //this->setHeight(650);
    ui->splitterEditorContainer->setSizes(QList<int>() << 2 << 5);
    ui->widgetSearchContainer->hide();
    ui->pushButtonApplyChanges->hide();
    this->currentNote.changed = false;
    this->mode = "editor";

    // загружаем список заметок
    this->updateListWidgetNotes();
    if(ui->listWidgetNotes->count()>0){
        on_listWidgetNotes_itemClicked(ui->listWidgetNotes->item(0));
        ui->listWidgetNotes->item(0)->setSelected(true);
        on_lineEditSearch_textChanged("");
    } else {
        this->ifNotesNotExists();
    }




}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::showTrayIcon()
{
    // Создаём экземпляр класса и задаём его свойства...
    trayIcon = new QSystemTrayIcon(this);
    QIcon trayImage(":/images/images/pilepad-icon.png");
    trayIcon -> setIcon(trayImage);
    trayIcon -> setContextMenu(trayIconMenu);

    // Подключаем обработчик клика по иконке...
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    // Выводим значок...
    trayIcon -> show();
}

void MainWindow::trayActionExecute(QSystemTrayIcon::ActivationReason reason)
{

    if (reason==QSystemTrayIcon::Trigger){  //если нажато левой кнопкой продолжаем

            if (!this->isVisible()) {  //если окно было не видимо - отображаем его
                this->showNormal();
                //this->show();
            } else {
                this->hide();  //иначе скрываем
            }

        }
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            this -> trayActionExecute(reason);
            break;
        default:
            break;
    }
}

void MainWindow::setTrayIconActions()
{
    // Setting actions...
    minimizeAction = new QAction("Minimize", this);
    restoreAction = new QAction("Show", this);
    quitAction = new QAction("Exit", this);

    // Connecting actions to slots...
    connect (minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect (restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    connect (quitAction, SIGNAL(triggered()), this, SLOT(slotContextExit()));

    // Setting system tray's icon menu...
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);
}

void MainWindow::changeEvent(QEvent *event)
{
    //QMainWindow::changeEvent(event);
    if (event -> type() == QEvent::WindowStateChange)
    {

        if (isMinimized())
        {
            this -> hide();

        }

        //this->show();
        /*
        if(this->isMinimized()){
            this->showNormal();
        }*/

        this->raise();
        this->activateWindow();

    }
}

void MainWindow::closeEvent(QCloseEvent *event){
    hide();
    event->ignore();
}

void MainWindow::slotAbout(){
    QMessageBox::about(0, "About Pilepad", "Pilepad\n\nver. 2.0.0\n\n© 2014 Sergey [idzn] Tolokonnikov\nidzn@yandex.ru");
}

void MainWindow::slotExit(){


    if(this->currentNote.changed==true){

        int result = QMessageBox::question(this,"","Save changes in note \""+this->currentNote.title+"\"?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        if(result == QMessageBox::Yes){
            on_pushButtonApplyChanges_clicked();
            QApplication::exit();
        } else if(result == QMessageBox::Cancel){
            return;
        } else {
            QApplication::exit();
        }

    } else {
        QApplication::exit();
    }


}

void MainWindow::slotContextExit(){

    trayIconMenu->hide();
    this->showNormal();
    slotExit();

}


void MainWindow::ifNotesNotExists(){
    /*
    ui->lineEditEditorNoteTitle->setText("");
    ui->plainTextEditEditorNoteBody->setPlainText("");
    ui->comboBoxEditorMarker->setDisabled(true);
    ui->lineEditEditorNoteTitle->setDisabled(true);
    ui->plainTextEditEditorNoteBody->setDisabled(true);
    this->currentNote.changed = false;
    ui->pushButtonApplyChanges->hide();
    ui->widgetEditorToolsPanel->setDisabled(true);
    */
    ui->splitterEditorContainer->setDisabled(true);
}

void MainWindow::firstNoteAdded(){
    /*
    ui->lineEditEditorNoteTitle->setDisabled(false);
    ui->plainTextEditEditorNoteBody->setDisabled(false);
    ui->comboBoxEditorMarker->setDisabled(false);
    ui->widgetEditorToolsPanel->setDisabled(false);
    */
    ui->splitterEditorContainer->setDisabled(false);
}

QIcon MainWindow::getIconForMarker(int marker){

    switch(marker){

        case 1:
            return QIcon(":/images/images/icon-note.png");
        case 2:
            return QIcon(":/images/images/icon-sun.png");
        case 3:
            return QIcon(":/images/images/icon-key.png");
        case 4:
            return QIcon(":/images/images/icon-person.png");
        case 5:
            return QIcon(":/images/images/icon-money.png");
        default:
            return QIcon(":/images/images/icon-note.png");

    }

}

void MainWindow::updateListWidgetNotes()
{

    NoteArray allNotesWithoutBody = core->getAllNotesWithoutBody();

    ui->listWidgetNotes->clear();

    for(int i=0; i<allNotesWithoutBody.count; i++){

        ui->listWidgetNotes->addItem(allNotesWithoutBody.notes[i].title);
        ui->listWidgetNotes->item(i)->setWhatsThis(QString::number(allNotesWithoutBody.notes[i].id));
        ui->listWidgetNotes->item(i)->setIcon(this->getIconForMarker(allNotesWithoutBody.notes[i].marker));

    }

}

void MainWindow::on_pushButtonApplyChanges_clicked()
{



    if(this->currentNote.changed==false) return;
    if(core->updateNote(this->currentNote.id, ui->lineEditEditorNoteTitle->text(), ui->plainTextEditEditorNoteBody->document()->toPlainText(), ui->comboBoxEditorMarker->currentIndex()+1)){

        this->currentNote.title = ui->lineEditEditorNoteTitle->text();
        this->currentNote.body = ui->plainTextEditEditorNoteBody->document()->toPlainText();
        this->currentNote.marker = ui->comboBoxEditorMarker->currentIndex()+1;

        ui->pushButtonApplyChanges->setText("...");
        for(int i=0; i < ui->listWidgetNotes->count(); i++){
            if(ui->listWidgetNotes->item(i)->whatsThis().toInt()==this->currentNote.id){
                ui->listWidgetNotes->item(i)->setText(ui->lineEditEditorNoteTitle->text());
                ui->listWidgetNotes->item(i)->setIcon(this->getIconForMarker(currentNote.marker));


            }
        }
        ui->pushButtonApplyChanges->setText("");

        ui->pushButtonApplyChanges->hide();
        this->currentNote.changed = false;

        on_lineEditSearch_textChanged("");
    }
}

void MainWindow::on_listWidgetNotes_itemClicked(QListWidgetItem *item)
{
    if(this->currentNote.id==item->whatsThis().toInt()) return;

    //если переход по заметкам, а открытая не сохранена - спросить
    if(this->currentNote.changed==true){

        int result = QMessageBox::question(this,"","Save changes in note \""+this->currentNote.title+"\"?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        if(result == QMessageBox::Yes){
            on_pushButtonApplyChanges_clicked();
        } else if(result == QMessageBox::Cancel){
            return;
        }

    }

    Note note;
    note = core->getNote(item->whatsThis().toInt());
    ui->plainTextEditEditorNoteBody->document()->setPlainText(note.body);
    ui->lineEditEditorNoteTitle->setText(note.title);
    ui->comboBoxEditorMarker->setCurrentIndex(note.marker-1);
    this->currentNote.id = note.id;
    this->currentNote.title = item->text();
    this->currentNote.body = note.body;
    this->currentNote.marker = note.marker;
    this->currentNote.changed = false;
    ui->pushButtonApplyChanges->hide();

    this->editorHistory->add(note.id);
    this->editorHistory->debug();

}



void MainWindow::on_pushButtonMode_clicked()
{
    if(this->mode == "editor"){

        QIcon icon(":/images/images/icon-edit.png");
        ui->pushButtonMode->setText("Editor");
        ui->pushButtonMode->setIcon(icon);
        ui->splitterEditorContainer->hide();
        ui->widgetSearchContainer->show();
        ui->lineEditSearch->setFocus();
        this->mode = "search";

    } else if(this->mode == "search"){

        QIcon icon(":/images/images/icon-search.png");
        ui->pushButtonMode->setText("Search");
        ui->pushButtonMode->setIcon(icon);
        ui->widgetSearchContainer->hide();
        ui->splitterEditorContainer->show();
        this->mode = "editor";

    }

}


void MainWindow::on_lineEditSearch_textChanged(const QString &arg1)
{
    NoteArray found = core->searchNotes(ui->lineEditSearch->text(), ui->comboBoxSearchMarker->currentIndex());
    this->printFoundNotes(found);
}

void MainWindow::printFoundNotes(NoteArray data){

    QMyHighlighter *highlighterTitle;
    QMyHighlighter *highlighterBody;

    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout;
    QPushButton *buttonOpen;
    QPushButton *buttonEdit;
    QPushButton *buttonDelete;
    QWidget *buttonSet;
    QBoxLayout *buttonSetLayout;
    QFrame *line;

    QPushButton *marker;

    QPlainTextEdit *plainTextEditTitle;
    QPlainTextEdit *plainTextEditBody;

    QIcon buttonOpenImage(":/images/images/icon-target.png");
    QIcon buttonEditImage(":/images/images/icon-pen.png");
    QIcon buttonDeleteImage(":/images/images/icon-trash.png");

    QFont titleFont("Monospace", 11, QFont::Bold);
    QFont bodyFont("Monospace", 10, QFont::Normal);

    ui->labelFoundCount->setText("Found: "+QString::number(data.count));

    for(int i=0; i<data.count; i++){

        buttonOpen = new QPushButton();
        buttonOpen->setMaximumWidth(70);
        buttonOpen->setObjectName("openNote_"+QString::number(data.notes[i].id));
        buttonOpen->setIcon(buttonOpenImage);
        buttonOpen->setToolTip("Open note in editor");
        connect(buttonOpen,SIGNAL(clicked()),this,SLOT(inSearchOpenButtonClickSlot()));

        buttonEdit = new QPushButton();
        buttonEdit->setMaximumWidth(70);
        buttonEdit->setObjectName("editNote_"+QString::number(data.notes[i].id));
        buttonEdit->setIcon(buttonEditImage);
        buttonEdit->setToolTip("Edit note");
        connect(buttonEdit,SIGNAL(clicked()),this,SLOT(inSearchEditButtonClickSlot()));

        buttonDelete = new QPushButton();
        buttonDelete->setMaximumWidth(70);
        buttonDelete->setObjectName("deleteNote_"+QString::number(data.notes[i].id));
        buttonDelete->setIcon(buttonDeleteImage);
        buttonDelete->setToolTip("Delete note");
        connect(buttonDelete,SIGNAL(clicked()),this,SLOT(inSearchDeleteButtonClickSlot()));

        marker = new QPushButton();
        marker->setIcon(this->getIconForMarker(data.notes[i].marker));
        marker->setFlat(true);
        marker->setEnabled(false);

        buttonSet = new QWidget();
        buttonSetLayout = new QBoxLayout(QBoxLayout::LeftToRight);


        //buttonSetLayout->addStretch(0);
        buttonSetLayout->addWidget(marker);
        buttonSetLayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
        buttonSetLayout->addWidget(buttonOpen);
        buttonSetLayout->addWidget(buttonEdit);
        buttonSetLayout->addWidget(buttonDelete);
        buttonSet->setLayout(buttonSetLayout);

        plainTextEditTitle = new QPlainTextEdit();
        plainTextEditTitle->document()->setPlainText(data.notes[i].title);
        plainTextEditTitle->setFont(titleFont);
        plainTextEditTitle->setReadOnly(true);
        plainTextEditTitle->setFrameStyle(0);
        plainTextEditTitle->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        plainTextEditTitle->setFixedHeight(plainTextEditTitle->fontMetrics().lineSpacing()+10); //10 - magic number
        plainTextEditTitle->installEventFilter(this->keyTranslator);

        plainTextEditBody = new QPlainTextEdit();
        plainTextEditBody->document()->setPlainText(data.notes[i].body);
        plainTextEditBody->setFont(bodyFont);
        plainTextEditBody->setReadOnly(true);
        plainTextEditBody->setFrameStyle(0);
        plainTextEditBody->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        plainTextEditBody->installEventFilter(this->keyTranslator);


        QString pattern;
        QStringList words = ui->lineEditSearch->text().toLower().split(" ");

        for(int i=0; i<words.count(); i++){

            pattern += core->strRusToLat(words.at(i))+" ";
            pattern += core->strLatToRus(words.at(i))+" ";
            pattern += core->strRusToLat(words.at(i)).toUpper()+" ";
            pattern += core->strLatToRus(words.at(i)).toUpper()+" ";

        }

        highlighterTitle = new QMyHighlighter(this);
        highlighterTitle->setDocument(plainTextEditTitle->document());
        highlighterTitle->setPattern(pattern.trimmed());
        highlighterTitle->highlightBlock("");

        highlighterBody = new QMyHighlighter(this);
        highlighterBody->setDocument(plainTextEditBody->document());
        highlighterBody->setPattern(pattern.trimmed());
        highlighterBody->highlightBlock("");

        line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        layout->addWidget(buttonSet);
        layout->addWidget(plainTextEditTitle);
        layout->addWidget(plainTextEditBody);
        layout->addWidget(line);


    }

    widget->setLayout(layout);
    ui->scrollAreaFoundNotes->setWidget(widget);

}

void MainWindow::inSearchOpenButtonClickSlot(){

    QString objectName = sender()->objectName();
    objectName.replace("openNote_", "");
    int noteID = objectName.toInt();

    for (int i=0; i < ui->listWidgetNotes->count(); i++){

        if(ui->listWidgetNotes->item(i)->whatsThis().toInt()==noteID){

            if(this->currentNote.changed==true){

                int result = QMessageBox::question(this,"","Save changes in note \""+this->currentNote.title+"\"?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
                if(result == QMessageBox::Yes){
                    on_pushButtonApplyChanges_clicked();
                } else if(result == QMessageBox::Cancel){
                    return;
                }

            }

            if(ui->listWidgetNotes->count()>0){
                on_listWidgetNotes_itemClicked(ui->listWidgetNotes->item(i));
                ui->listWidgetNotes->item(i)->setSelected(true);
            }
            on_pushButtonMode_clicked();
            return;

        }
    }

}

void MainWindow::inSearchEditButtonClickSlot(){

    QString objectName = sender()->objectName();
    objectName.replace("editNote_", "");
    int noteID = objectName.toInt();

    EditNoteDialog *editNoteDialog = new EditNoteDialog;
    editNoteDialog->setWindowTitle("Edit note");

    Note note = core->getNote(noteID);
    editNoteDialog->setNoteTitleEdit(note.title);
    editNoteDialog->setNoteBodyEdit(note.body);
    editNoteDialog->setNoteMarker(note.marker);

    if(this->currentNote.id == noteID){
        on_pushButtonMode_clicked();
        return;
    }

    if (editNoteDialog->exec() == QDialog::Accepted){

        if(core->updateNote(noteID, editNoteDialog->getNoteTitleEdit(), editNoteDialog->getNoteBodyEdit(), editNoteDialog->getNoteMarker())){
           on_lineEditSearch_textChanged("");

           for(int i=0; i < ui->listWidgetNotes->count(); i++){
               if(ui->listWidgetNotes->item(i)->whatsThis().toInt()==noteID){
                   ui->listWidgetNotes->item(i)->setText(editNoteDialog->getNoteTitleEdit());
                   ui->listWidgetNotes->item(i)->setIcon(this->getIconForMarker(editNoteDialog->getNoteMarker()));
               }
           }

        }

    }



}

void MainWindow::inSearchDeleteButtonClickSlot(){

    QString objectName = sender()->objectName();
    objectName.replace("deleteNote_", "");
    int noteID = objectName.toInt();

    if(QMessageBox::question(this,"Delete note","Are you shure?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){
        core->deleteNote(noteID);
        on_lineEditSearch_textChanged("");
        this->updateListWidgetNotes();
        if(ui->listWidgetNotes->count()>0){
            on_listWidgetNotes_itemClicked(ui->listWidgetNotes->item(0));
            ui->listWidgetNotes->item(0)->setSelected(true);
        } else {
            this->ifNotesNotExists();
        }

        this->editorHistory->remove(noteID);
        this->editorHistory->debug();

    }



}

void MainWindow::on_plainTextEditEditorNoteBody_textChanged()
{
    if(
            ui->lineEditEditorNoteTitle->text() == this->currentNote.title
            &&
            ui->plainTextEditEditorNoteBody->document()->toPlainText() == this->currentNote.body
            &&
            ui->comboBoxEditorMarker->currentIndex()+1 == this->currentNote.marker

    ) {
        return;
    }

    if(ui->listWidgetNotes->count()==0) return;
    this->currentNote.changed = true;
    ui->pushButtonApplyChanges->show();
}

void MainWindow::on_lineEditEditorNoteTitle_textChanged(const QString &arg1)
{

    if(ui->listWidgetNotes->count()==0) return;
    on_plainTextEditEditorNoteBody_textChanged();
}

void MainWindow::on_comboBoxEditorMarker_currentIndexChanged(const QString &arg1)
{
    if(ui->listWidgetNotes->count()==0) return;
    on_plainTextEditEditorNoteBody_textChanged();
}

void MainWindow::on_pushButtonAddNote_clicked()
{

    EditNoteDialog *editNoteDialog = new EditNoteDialog;
    editNoteDialog->setWindowTitle("Add note");


    if (editNoteDialog->exec() == QDialog::Accepted){

        core->addNote(editNoteDialog->getNoteTitleEdit(), editNoteDialog->getNoteBodyEdit(), editNoteDialog->getNoteMarker());

        on_lineEditSearch_textChanged("");
        this->updateListWidgetNotes();

        if(ui->listWidgetNotes->count()==1){
            this->firstNoteAdded();
            on_listWidgetNotes_itemClicked(ui->listWidgetNotes->item(0));
        }

    }

}

void MainWindow::on_lineEditNotesFilter_textChanged(const QString &arg1)
{
    QString str = ui->lineEditNotesFilter->text().toLower();
    QRegExp rxp1("("+core->strRusToLat(str)+")");
    QRegExp rxp2("("+core->strLatToRus(str)+")");

    for(int i=0; i<ui->listWidgetNotes->count(); i++){

        if(ui->lineEditNotesFilter->text().isEmpty()) {
            ui->listWidgetNotes->item(i)->setHidden(false);
            continue;
        }

        if(
                rxp1.indexIn(ui->listWidgetNotes->item(i)->text().toLower())!=-1
                ||
                rxp2.indexIn(ui->listWidgetNotes->item(i)->text().toLower())!=-1

        ) {
            ui->listWidgetNotes->item(i)->setHidden(false);
        } else {
            ui->listWidgetNotes->item(i)->setHidden(true);
        }

        if(!ui->listWidgetNotes->item(i)->text().startsWith(ui->lineEditNotesFilter->text())){

        } else {

        }
    }
}





void MainWindow::on_listWidgetNotes_customContextMenuRequested(const QPoint &pos)
{

    QPoint globalPos = ui->listWidgetNotes->viewport()->mapToGlobal(pos);

    listWidgetNotesMenu->exec(globalPos);





}

void MainWindow::on_listWidgetNotes_contextDeleteNote(){


    QList<QListWidgetItem *> selected = ui->listWidgetNotes->selectedItems();

    int noteID = selected[0]->whatsThis().toInt();

    if(QMessageBox::question(this,"","Are you shure? Delete note \""+selected[0]->text()+"\"?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){
        core->deleteNote(noteID);
        on_lineEditSearch_textChanged("");
        this->updateListWidgetNotes();
        if(ui->listWidgetNotes->count()>0){
            on_listWidgetNotes_itemClicked(ui->listWidgetNotes->item(0));
            ui->listWidgetNotes->item(0)->setSelected(true);
        } else {
            this->ifNotesNotExists();
        }

        this->editorHistory->remove(noteID);
        this->editorHistory->debug();

    }



}

void MainWindow::on_listWidgetNotes_itemSelectionChanged()
{
    if(ui->listWidgetNotes->selectedItems().count()>0){
        on_listWidgetNotes_itemClicked(ui->listWidgetNotes->selectedItems()[0]);
    }

}

void MainWindow::slotPasteHorizonLineToEditor(){

    ui->plainTextEditEditorNoteBody->insertPlainText("------------------------------");

}

void MainWindow::slotPasteDoubleHorizonLineToEditor(){

    ui->plainTextEditEditorNoteBody->insertPlainText("==============================");

}

void MainWindow::slotPasteDateToEditor(){

    ui->plainTextEditEditorNoteBody->insertPlainText(QDate::currentDate().toString("dd.MM.yyyy"));

}


void MainWindow::on_comboBoxSearchMarker_currentIndexChanged(const QString &arg1)
{
    on_lineEditSearch_textChanged("");
}

void MainWindow::switchEditorToNote(int noteID){

    for (int i=0; i < ui->listWidgetNotes->count(); i++){

        if(ui->listWidgetNotes->item(i)->whatsThis().toInt()==noteID){

            if(this->currentNote.changed==true){

                int result = QMessageBox::question(this,"","Save changes in note \""+this->currentNote.title+"\"?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
                if(result == QMessageBox::Yes){
                    on_pushButtonApplyChanges_clicked();
                } else if(result == QMessageBox::Cancel){
                    return;
                }

            }

            if(ui->listWidgetNotes->count()>0){
                on_listWidgetNotes_itemClicked(ui->listWidgetNotes->item(i));
                ui->listWidgetNotes->item(i)->setSelected(true);
            }


        }
    }
}

void MainWindow::slotEditorHistoryGoBack(){

    if(this->editorHistory->getSize() == 1) return;

    if(this->currentNote.changed==true){

        int result = QMessageBox::question(this,"","Save changes in note \""+this->currentNote.title+"\"?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        if(result == QMessageBox::Yes){
            on_pushButtonApplyChanges_clicked();
        } else if(result == QMessageBox::Cancel){
            return;
        } else if(result == QMessageBox::No){
            this->currentNote.changed = false;
            ui->pushButtonApplyChanges->hide();
        }

    }

    this->editorHistory->back();

    int noteID = this->editorHistory->getCurrent();

    for (int i=0; i < ui->listWidgetNotes->count(); i++){

        if(ui->listWidgetNotes->item(i)->whatsThis().toInt()==noteID){

            if(ui->listWidgetNotes->count()>0){
                on_listWidgetNotes_itemClicked(ui->listWidgetNotes->item(i));
                ui->listWidgetNotes->item(i)->setSelected(true);
            }
        }
    }

    this->editorHistory->debug();
}

void MainWindow::slotEditorHistoryGoForward(){

    if(this->editorHistory->getSize() == 1) return;

    if(this->currentNote.changed==true){

        int result = QMessageBox::question(this,"","Save changes in note \""+this->currentNote.title+"\"?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        if(result == QMessageBox::Yes){
            on_pushButtonApplyChanges_clicked();
        } else if(result == QMessageBox::Cancel){
            return;
        } else if(result == QMessageBox::No){
            this->currentNote.changed = false;
            ui->pushButtonApplyChanges->hide();
        }

    }

    this->editorHistory->forward();

    int noteID = this->editorHistory->getCurrent();

    for (int i=0; i < ui->listWidgetNotes->count(); i++){

        if(ui->listWidgetNotes->item(i)->whatsThis().toInt()==noteID){

            if(ui->listWidgetNotes->count()>0){
                on_listWidgetNotes_itemClicked(ui->listWidgetNotes->item(i));
                ui->listWidgetNotes->item(i)->setSelected(true);
            }
        }
    }

    this->editorHistory->debug();
}

void MainWindow::on_pushButtonEditorPanelDeleteNote_clicked()
{

    int noteID = this->currentNote.id;

    if(QMessageBox::question(this,"","Are you shure? Delete note \""+this->currentNote.title+"\"?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){
        core->deleteNote(noteID);
        on_lineEditSearch_textChanged("");
        this->updateListWidgetNotes();
        if(ui->listWidgetNotes->count()>0){
            on_listWidgetNotes_itemClicked(ui->listWidgetNotes->item(0));
            ui->listWidgetNotes->item(0)->setSelected(true);
        } else {
            this->ifNotesNotExists();
        }

        this->editorHistory->remove(noteID);
        this->editorHistory->debug();

    }
}
