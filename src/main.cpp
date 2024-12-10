#include <QtCore/QVariant>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QFileSystemModel>
#include <QTextStream>
#include <QMessageBox>
#include <QMap>
#include <QShortcut>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QTextCodec>
#include <QTimer>
#include <QColor>
#include <QHeaderView>
#include <QMimeData>

#include <QtWidgets/QApplication>
#include <QWindow>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "config.h"
#include "tab.h"
#include "DropFilter.h"
#include "specHighlight.h"

//Declarations
void Qtabs_selected(int index);


QApplication		*appP;
QMainWindow			*win;
QSplitter			*splitter;
QSplitter			*sideSplitter;
QTabBar				*Qtabs;
QTextEdit		*Qtext;
QList<QTextEdit::ExtraSelection> selections;
QLineEdit			*QtreeLine;
QListWidget			*QtabList;
QTreeView			*Qtree;
QFileSystemModel	*model;
MyHighlighter	 *highlighter;
bool font_mono = false; //toggle font monospace
bool highlighting = false; //special highlighting
bool ignoreNextMove = false; //So tabMove and tabListMove dont clash
bool tabsVisible = true;

int initialShow = 0;  //used by Qtabs_selected, because first one gets selected on start of app. and instead we want OPEN_LAST to control which one is first selected.

//	path,	contents
QMap<QString, Tab> files;
QString current;

QString folder;

bool anyChanges = false;  // so autosave knows when to save or skip.

inline int cur(){ return Qtabs->currentIndex(); }
inline QString tabDataStr(int i){
	return Qtabs->tabData(i).value<QString>();
}

#define tabByIndex(I) files.value(tabDataStr(I))
inline int indexByPath(QString path){
	for(int i = 0; i< Qtabs->count(); i++)
		if(tabDataStr(i) == path)
			return i;
	return -1;
}

void reRunHighlighter(){

	selections.clear();

	if(highlighting){
    
	auto qtxt = Qtext->toPlainText();
	//(QTextStream(stdout))<<"Clear\n";

	for(auto a : highlighter->patterns){
		if(!a.fullLineColor) continue;
		
		//if (match.hasMatch()) {
        QRegularExpressionMatchIterator matchIterator = a.expression.globalMatch(qtxt);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
			//for(const QRegularExpressionMatch &match : a.expression.globalMatch(Qtext->toPlainText())){


    		    //(QTextStream(stdout))<<"Captured '"<<match.captured()<<"'\n";

                QTextCursor matchCursor(Qtext->document());

                // Position the cursor at the start of the match
                matchCursor.setPosition(match.capturedStart());
                //matchCursor.setPosition(match.capturedStart(i) + match.capturedLength(i), QTextCursor::KeepAnchor);

                // Move the cursor to the start of the line
            //    matchCursor.movePosition(QTextCursor::StartOfLine/*StartOfBlock*/);
                
                // Select the entire line/block
            //    matchCursor.movePosition(QTextCursor::EndOfLine/*EndOfBlock*/, QTextCursor::KeepAnchor);
                //matchCursor.movePosition(match.capturedStart() + match.capturedLength(), QTextCursor::KeepAnchor);
                

                QTextEdit::ExtraSelection selection;
                selection.format = a.format;//.setBackground(QColor("#FF0005"));//format.background().color());
                selection.format.setProperty(QTextFormat::FullWidthSelection,true);
                selection.cursor = matchCursor;
                selections.append(selection);
			}
		//}
	}
}
	Qtext->setExtraSelections(selections);//selections[MyHighlighter::selectionIdx]);
	//Qtext->setExtraSelections(selections[selectionIdx]);
	//MyHighlighter::selectionIdx = (++MyHighlighter::selectionIdx)%2;
	//selections[MyHighlighter::selectionIdx].clear();
}

inline void colorTab(int i, const QColor &c){
	Qtabs->setTabTextColor(i,c);
	QtabList->item(i)->setForeground(c);
}
inline void colorTabDirty(int i){
	colorTab(i,QColorConstants::Red);
}
inline void colorTabFresh(int i){
	colorTab(i,QColorConstants::Black);
}




void openTab(QString path, bool reload = false){
	if(reload == 0 && QString::compare(current,path) == 0)return;
	current = "";
	auto tab = files.value(path);
	Qtext->setPlainText(tab.text);
	int i = cur();
	if(tab.dirty || tab.isNew()) colorTabDirty(i);
	else colorTabFresh(cur());

	for(int j = 0; j < QtabList->count(); j++)
		QtabList->item(j)->setBackground(i==j?QColorConstants::Cyan : QColorConstants::White);
	
	current = path;
	reRunHighlighter();
}
int loadTab(QString path, bool dirty){

	QString title = path;
	if(path.startsWith('/')){
		auto fi = QFileInfo(QFile(path));
		title = SHOW_EXTENSION?fi.fileName():fi.baseName();
		
	}else dirty = true;///////

	int i = Qtabs->addTab(title);
	QtabList->addItem(title);
	QVariant data(path);
	Qtabs->setTabData(i,data);
	QtabList->item(i)->setData(0x0100, data);
	//Qtabs->setTabButton(index, QTabBar::ButtonPosition::RightSide,)
	
	if(dirty) colorTabDirty(i);

	return i;
}
int addTab(QString path, QString contents, bool dirty){
	if(files.contains(path))return -1;

	files.insert(path,{dirty, path, contents});
	
	return loadTab(path,false);
}



void modify_tab_name(Tab tab, QString fileName){
	qDebug()<<tab.path <<" "<<fileName<<" "<<tab.text;
	int i = cur();
	Qtabs->removeTab(i);
	QtabList->takeItem(i);
	qDebug()<<tab.path <<" "<<i;
	qDebug()<<tab.path <<" "<<files.count();
	files.remove(tab.path);
	qDebug()<<tab.path <<" "<<files.count();
	Qtabs_selected(addTab(fileName,tab.text, false));
	qDebug()<<tab.path <<" "<<fileName<<" "<<tab.text;
}


int confirmSave(QString q = "Save?"){
	auto reply = QMessageBox::question(win, "Save", q, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
	if (reply == QMessageBox::Yes) {
		return 2;
	}else if(reply == QMessageBox::No){
		return 1;	
	}
	return 0;
}
bool saveTab(Tab tab,
 bool saveAs = false, bool closing = false
){
	bool
	dirty = tab.dirty,
	isNew = tab.isNew(),
	empty = tab.text.count()==0;

	if(!dirty && !saveAs) return true;
	
	saveAs = saveAs || (!closing && isNew); //if saving empty new
	
	QString fileName = tab.path;

	if(closing && !saveAs && ((isNew)?(!empty):(dirty))){
		int c = confirmSave();
		if(c==1) return true;
		if(c==0) return false;
	}

	//Chose location for saveAs or if saving new
	if(saveAs || (isNew && closing && !empty)){
		fileName = QFileDialog::getSaveFileName(win, "Save File", folder,"");
		if(fileName.isEmpty()) return false;
		/*//above dialog already handles it
		//Overwiting?
		if(tab.path.compare(fileName) != 0 && QFile(fileName).exists()){
			int c = confirmSave("Overwrite existing file?");
			if(c==1) return true;
			if(c==0) return false;
		}
		*/
	}

	//Write:
	QFile file(fileName);
	if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
		QTextStream stream(&file);
		stream << tab.text;
		file.close();
	}else{
		QMessageBox::warning(win, "Couldnt save",
			"File '" + tab.path + "' couldnt be saved.", QMessageBox::Ok);
		return false;
	}
	if(closing) return true;
	
	//After saving:
	if(tab.path.compare(fileName) != 0){
	 	///////[TODO] Need to mark file dirty if fileName already open in dif tab
	 	///////[TODO] But then ill have 2 tabs of same file :(((
	 	///////[TODO] Allow samefile tabs? 2 tabs of same that share changes?
		
		modify_tab_name(tab,fileName);
	}else{
		files[tab.path].dirty = false;

		colorTabFresh(cur()); ///////[TODO] what if cur changes?
	}

	return true;
}

bool save(int i = -1,
 bool saveAs = false, bool closing = false
){
	if(i==-1) i = cur();
	if(i==-1) return false;
	return saveTab(tabByIndex(i), saveAs, closing);
}


void autosave(){ //only used for crashes and closing, save ALL tab states
	if(anyChanges == false) return; 
	QJsonArray j;

	j.push_back(QJsonValue::fromVariant(folder));

	auto sizeList = splitter->sizes();
	j.push_back(QJsonValue::fromVariant(sizeList[0]));
	j.push_back(QJsonValue::fromVariant(sizeList[1]));
	auto sizeList2 = sideSplitter->sizes();
	j.push_back(QJsonValue::fromVariant(sizeList2[0]));
	j.push_back(QJsonValue::fromVariant(sizeList2[1]));

	j.push_back(QJsonValue(tabsVisible));


	j.push_back(QJsonValue(Qtext->font().pointSize()));
	j.push_back(QJsonValue(QtreeLine->font().pointSize()));

	j.push_back(QJsonValue(win->width()));
	j.push_back(QJsonValue(win->height()));
	
	j.push_back(QJsonValue(Qtabs->count()));
	for(int i = 0; i < Qtabs->count(); i++){
		auto tab = files.value(tabDataStr(i));
	
    	QJsonObject t;
		t.insert("dirty", QJsonValue::fromVariant(tab.dirty));
		t.insert("path", QJsonValue::fromVariant(tab.path));
		t.insert("text", QJsonValue::fromVariant(tab.text));
		j.push_back(t);	
	}
	

	QString json = QJsonDocument(j).toJson(QJsonDocument::Compact);
	//qDebug() << json;

	if(SAVE_TO_SETTINGS){
		QSettings settings SETTINGS;
		settings.setValue("settings",json);
	}else{
		QFile file( QString(SAVE_LOCATION) );
		if ( file.open(QIODevice::WriteOnly  | QIODevice::Text) )
		{
			QTextStream stream( &file );
			stream << json;
			file.close();
		}
	}

	anyChanges = false;
}

void rename(){
	
		(QTextStream(stdout))<<"RENAME\n";

	QString fileName = "";
	int idx = 1;

	while(idx!=-1){ //if name already taken check
		
		fileName = QFileDialog::getSaveFileName(win, "Enter file name, ignore folder", "/","");
		if(fileName.isEmpty()) return;

		auto fi = QFileInfo(QFile(fileName));
		fileName = fi.fileName();
		idx = indexByPath(fileName);
		
	}
	//Qtabs->removeTab(i);
	auto tab = tabByIndex(cur());
	tab.path = fileName; 
	modify_tab_name(tab,fileName);
	autosave();
}

void closeTab(int i = -1){
	if(i==-1) i = cur();
	if(i==-1) return;
	auto tab = tabByIndex(i);
	if(!saveTab(tab,false,true)) return;
	files.remove(tab.path);
	current = "";
	Qtabs->removeTab(i);
	QtabList->takeItem(i);
	
	if(i >= Qtabs->count()) i--;
	Qtabs_selected(i);
}




void openFile(QString path){
	if(QDir(path).exists())return; //no folders!


	QFile file(path);
	if(!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
		return;
	}

	QTextStream in(&file);

	QString contents = in.readAll();
	file.close();


	if(files.contains(path)){
		files[path].text = contents;
		if(current == path){
			openTab(path, true);
		}
	}else{
		int i = addTab(path, contents, false);
		Qtabs_selected(SWITCH_TO_NEW?i:cur());
	}
}
void newFile(){
	int n = 1;
	while(files.contains(QString::number(n))) n++;
	QString name = QString::number(n);
	int i = addTab(name,"",true);
	Qtabs_selected(SWITCH_TO_NEW?i:cur());
}


void viewFolder(bool setText = false){
	if(setText) return QtreeLine->setText(folder);
	
	QDir dir(folder);
	bool exists = dir.exists();

	QtreeLine->setStyleSheet(exists?
	"QLineEdit{background-color:white;}":
	"QLineEdit{background-color:red;}");

	Qtree->reset();
	if(!exists) return;


	model->setRootPath(folder);
	//if (parser.isSet(dontUseCustomDirectoryIconsOption))
	//    model.setOption(QFileSystemModel::DontUseCustomDirectoryIcons);
	//if (parser.isSet(dontWatchOption))
	//    model.setOption(QFileSystemModel::DontWatchForChanges);
    
	Qtree->setRootIndex(model->index(folder));
	//QModelIndex index = model->index(path);
	//Qtree->setRootIndex(index);
	//Qtree->expand(index);
	//Qtree->scrollTo(index);
	

}



void openFileDialog(){
	QString fileName = QFileDialog::getOpenFileName(win, "Open File",folder,"Any (*.*)");
	if(fileName.isEmpty() == false) openFile(fileName);
}


bool DropFilter::eventFilter(QObject *obj, QEvent *event){

        if (event->type() == QEvent::DragEnter){
            QDragEnterEvent *tDragEnterEvent = static_cast<QDragEnterEvent *>(event);
            tDragEnterEvent->acceptProposedAction();
        
            return true;
        }else if (event->type() == QEvent::DragMove){
            QDragMoveEvent *tDragMoveEvent = static_cast<QDragMoveEvent *>(event);
            tDragMoveEvent->acceptProposedAction();
        
            return true;
        }
        else if (event->type() == QEvent::Drop){

            QDropEvent *tDropEvent = static_cast<QDropEvent *>(event);
            tDropEvent->acceptProposedAction();
            

            auto urls = tDropEvent->mimeData()->urls();
            for(auto u : urls)
                openFile(u.path());
        
            return true;
        }

		// standard event processing
	   		return obj->eventFilter(obj, event);

    }



#include "eventsAndShortcuts.h"

#include "main.h"