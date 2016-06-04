#include "editorhistory.h"

EditorHistory::EditorHistory(int sizeLimit)
{
    this->sizeLimit = sizeLimit;
}

void EditorHistory::setSizeLimit(int size){
    sizeLimit = size;
}

void EditorHistory::clear(){
    history.clear();
    this->currentPosition=0;
}

void EditorHistory::add(int id){

    if(history.contains(id)) return;

    if(history.size() == sizeLimit){
        history.pop_front();
    }
    history.append(id);
    this->currentPosition = history.size() - 1;

    if(!history.isEmpty())
        if(history.at(0) == history.at(history.size()-1) && this->currentPosition>0){
            history.removeFirst();
            this->currentPosition--;
        }

}

void EditorHistory::remove(int id){

    if(history.size()==1) return;

    history.removeAll(id);
    if(this->isEmpty()){
        this->currentPosition = 0;
    } else {
        this->currentPosition = history.size() - 1;
    }

}

bool EditorHistory::isEmpty(){
    return history.empty();
}

bool EditorHistory::exists(int id){
    return history.contains(id);
}

void EditorHistory::back(){

    if(this->currentPosition == 0){
        this->currentPosition = history.size() - 1;
    } else {
        this->currentPosition --;
    }
}

void EditorHistory::forward(){

    if(this->currentPosition == history.size() - 1){
        this->currentPosition = 0;
    } else {
        this->currentPosition ++;
    }
}

int EditorHistory::getCurrent(){
    return history.at(this->currentPosition);
}


void EditorHistory::normalize(){
    history = history.toSet().toList();
    this->currentPosition = history.size() - 1;
}

int EditorHistory::getSize(){
    return history.size();
}

void EditorHistory::debug(){
    /*
    QList<int>::iterator it = this->history.begin();
    while (it != this->history.end()) {
        qDebug() << "ID:" << *it;
        ++it;
    }
    qDebug() << "currentPosition: " << this->currentPosition;
    qDebug() << "current id: " << history.at(this->currentPosition);
    qDebug() << "size: " << history.size();
    */
}
