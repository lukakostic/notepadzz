
#include <QEvent>
#include <QObject>
#include <QDropEvent>
#include <QDebug>
#include <QMimeData>

//Class just for the eventFilter method to enable drag&drop
class DropFilter : public QObject{
	Q_OBJECT

public:
    DropFilter(){}
    DropFilter(const DropFilter &other){}
    ~DropFilter(){}

	bool eventFilter(QObject *obj, QEvent *event);
};

Q_DECLARE_METATYPE(DropFilter);