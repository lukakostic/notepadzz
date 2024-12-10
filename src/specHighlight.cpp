#include "specHighlight.h"
#include <QTextStream>

/*
void MyHighlighter::highlightBlock(const QString &text)
{
    QRegularExpressionMatchIterator matchIterator = commentStartExpression.globalMatch(text);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
    }
}
*/

void MyHighlighter::setEnabled(bool enabled) {
    isEnabled = enabled;
		(QTextStream(stdout))<<"HIGHLIGHTER: "<<isEnabled<<"\n";
    //rehighlight();
}

MyHighlighter::MyHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    /*
    patterns.push_back(HighlightPattern(this,"^##.*").bg("#D3D3FF"));
    patterns.push_back(HighlightPattern(this,"\\*\\S.*?\\S\\*").bg("#FFC3FF"));
    patterns.push_back(HighlightPattern(this,"#\\S.*?\\S#").bg("#D3D3FF"));
    patterns.push_back(HighlightPattern(this,"!\\S.*?\\S!").bg("#FFC3C3"));
    patterns.push_back(HighlightPattern(this,"^!##.*").bg("#FFC3C3"));
    patterns.push_back(HighlightPattern(this,"{\\S.*?\\S}").col("#C2C2C2"));
    patterns.push_back(HighlightPattern(this,"^\\.{.*").col("#C2C2C2"));
    */
    patterns.push_back(HighlightPattern(this,"#\\S.*?\\S#").bg("#D3D3FF").specCol(1,1));
    patterns.push_back(HighlightPattern(this,"^##.*? - (.*)$",QRegularExpression::MultilineOption).bg("#D3D3FF").fullLine());
    patterns.push_back(HighlightPattern(this,"^##.*?(?= - |$)",QRegularExpression::MultilineOption).bg("#D3D3FF").bold().specCol(2,0).fullLine());
    patterns.push_back(HighlightPattern(this,"^\\*~~.*",QRegularExpression::MultilineOption).bg("#F3F3FE").bold().specCol(3,0).fullLine());
    patterns.push_back(HighlightPattern(this,"!\\S.*?\\S!").bg("#FFC3FF").specCol(1,1));
    patterns.push_back(HighlightPattern(this,"\\*\\S.*?\\S\\*").bg("#E3E3FE").bold().specCol(1,1));
    patterns.push_back(HighlightPattern(this,"^!##.*",QRegularExpression::MultilineOption).bg("#FFC3C3").specCol(3,0).bold().fullLine());
    patterns.push_back(HighlightPattern(this,"{\\S.*?\\S}").col("#C2C2C2").specCol(1,1,"#FFFFFF"));
    patterns.push_back(HighlightPattern(this,"^\\.{.*",QRegularExpression::MultilineOption).col("#C2C2C2").fullLine());
    
    
    isEnabled = true;
}

void MyHighlighter::highlightBlock(const QString &text)
{
    if (!isEnabled) return;

    //(QTextStream(stdout))<<"Text: "<<text.left(15) << "\n";
	
    int idx = -1; //of full line colored
    for(int p  = 0; p< patterns.size(); p++ )// if(!p.fullLineColor) 
        if(patterns[p].Match(text,this,idx)){
            idx=p;
        }

    //(QTextStream(stdout))<<selections->count()<<"\n";
	
}
