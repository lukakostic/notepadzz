#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QtWidgets/QTextEdit>
#include <QTextStream>
#include <vector>

struct HighlightPattern;

class MyHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    MyHighlighter(QTextDocument *parent);
    void setEnabled(bool enabled);
    void publicSetFormat(int start, int count, const QTextCharFormat &format) {
        setFormat(start, count, format);
    }
    std::vector<HighlightPattern> patterns;
protected:
    void highlightBlock(const QString &text) override;

private:
/*
    QRegularExpression commentStartExpression;
    QRegularExpression hashExpression;
    QTextCharFormat commentFormat;
    QTextCharFormat hashFormat;
    */
    bool isEnabled;
};

struct HighlightPattern
{
    QRegularExpression expression;
    QTextCharFormat format;
    bool specColor;
    bool fullLineColor;
    int specS,specE;
    QTextCharFormat specFormat;

    MyHighlighter *highlighter;
    HighlightPattern(MyHighlighter *h,const QString &exp, QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption){
        specColor = false;
        fullLineColor = false;
        highlighter = h;
        expression.setPattern(exp);
        expression.setPatternOptions(options);
        specFormat.setFontWeight(QFont::Thin);
    }
    HighlightPattern& bg(QColor col){
        format.setBackground(QBrush(col));
        specFormat.setBackground(QBrush(col));
        return *this;
    }
    HighlightPattern& fullLine(){
        fullLineColor = true;
        return *this;
    }
    HighlightPattern& specCol(int s = 0, int e = 0, QString str = ""){
        specColor = true;
        specS = s; specE = e;
        QColor originalColor = format.background().color().darker(120);
        if(str.length() != 0) originalColor = QColor(str);
        
        // Tint it darker by 20%
        specFormat.setForeground(QBrush(originalColor));
        return *this;
    }
    HighlightPattern& col(QColor col){
        format.setForeground(QBrush(col));
        specFormat.setForeground(QBrush(col));
        return *this;
    }
    HighlightPattern& bold(){
        format.setFontWeight(QFont::Bold);
        return *this;
    }
    bool Match(const QString &text, MyHighlighter*highlighter = nullptr, int idx = -1){
    //(QTextStream(stdout))<<"Match\n";
        QRegularExpressionMatchIterator matchIterator = expression.globalMatch(text);
        bool matched = false;
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            auto black = QColor("#000000");
            //(QTextStream(stdout))<<"RegMatch\n";
            matched = true;
            
            auto format2 = format;
            if(idx!=-1){
                //if(specFormat.foreground().color()==format.foreground().color()){
                    auto qcol = highlighter->patterns[idx].format.background().color();
                    auto forqcol = highlighter->patterns[idx].format.foreground().color();
                    auto mycol = format.background().color();
                    if(qcol!=mycol){

                        auto hsv = ((/*forqcol==black*/true)?mycol.darker(300):forqcol).toHsv();
                        int hsvSat = hsv.hsvSaturation() + 90;
                        hsv.setHsv(hsv.hsvHue(),hsvSat,hsv.lightness());
                        format2.setForeground(hsv);
                        format2.setBackground(QColor("#FFFFFF"));
                    }
                //}
                //bgColor = highlighter->patterns[idx].format.background().color();
            }
            highlighter->publicSetFormat(match.capturedStart(), match.capturedLength(), format2);
            if(specColor){
                // Apply exclamFormat specifically to the first exclamation mark
                if(specS!=0)    highlighter->publicSetFormat(match.capturedStart(), specS, specFormat);

                // Apply exclamFormat specifically to the last exclamation mark
                if(specE!=0) highlighter->publicSetFormat(match.capturedStart() + match.capturedLength() - specE, specE, specFormat);

            }
        }
        return matched && fullLineColor;
    }
};
