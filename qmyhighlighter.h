#ifndef QMYHIGHLIGHTER_H
#define QMYHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class QMyHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit QMyHighlighter(QObject *parent = 0);

    void highlightBlock(const QString &text);

    void setPattern(QString pattern);

private:
    QString pattern;

signals:

public slots:



};

#endif // QMYHIGHLIGHTER_H
