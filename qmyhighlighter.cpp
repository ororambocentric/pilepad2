#include "qmyhighlighter.h"

QMyHighlighter::QMyHighlighter(QObject *parent) :
    QSyntaxHighlighter(parent)
{
}

void QMyHighlighter::highlightBlock(const QString &text){

    QTextCharFormat format;

    QStringList words = pattern.split(" ");
    for(int i=0; i<words.count(); i++){


        QRegExp rx(words.at(i));

        if(!rx.isValid() || rx.isEmpty() || rx.exactMatch("")){
            setFormat(0, text.length(), format);
            return;
        }

        format.setBackground(Qt::cyan);

        int index = rx.matchedLength();
        while(index >= 0){
            int length = rx.matchedLength();
            setFormat(index, length, format);
            index = rx.indexIn(text, index + length);
        }


    }



}

void QMyHighlighter::setPattern(QString pattern){
    this->pattern = pattern;
}
