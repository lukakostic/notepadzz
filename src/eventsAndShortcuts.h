
/************** QtEvents  **************/

void toggleHighlighting(){
	//if(highlighting==false) return;
	if(highlighter == nullptr){
		(QTextStream(stdout))<<"HIGHLIGHTER NULL\n";
		return;
	}
	highlighting=!highlighting;
	highlighter->setEnabled(highlighting);
	highlighter->rehighlight();
}

void Qtext_changed(){
	anyChanges = true;
	if(current.isEmpty()) return;
    if(!files.contains(current)) return;
	auto qtxt = Qtext->toPlainText();
	files[current].text = qtxt;
	files[current].dirty = true;
	colorTabDirty(cur());
    //(QTextStream(stdout))<<"Changed\n";
	reRunHighlighter();
}
void QtreeLine_changed(const QString &text){
	folder = text;
	viewFolder();
}
void Qtabs_moved(int from, int to){
	if(!ignoreNextMove)
		QtabList->insertItem(to, QtabList->takeItem(from));
	ignoreNextMove = !ignoreNextMove;
}
void QtabList_moved(const QModelIndex p1,int from, int to,const QModelIndex p2,int row){
	ignoreNextMove = true;
	row = from<row?row-1:row;
	//if(!ignoreNextMove)
	Qtabs->moveTab(from,row);
}
void Qtree_selected(const QModelIndex &index){
	openFile(model->filePath(index));
}
void Qtabs_selected(int index){
	if(initialShow<2){ 
		// super fucking hacky. We need an event once all widgets get made, and this seems to be it..
		// hacky way. Since this event is rigged to both tabBar and tabList , and both call this on window init.. we skip first 2 events.
		if(OPEN_LAST){
			index = Qtabs->count() - 1;
		}else{
			index = 0;
		}
		initialShow++;
		if(initialShow==2){ //last, final.
			Qtabs->setVisible(tabsVisible);
			Qtext->setFocus();
		}
	}
	// (QTextStream(stdout))<<"open:"<<index << " / "<<Qtabs->count()<<"\n";
	if(index == -1){
		Qtext->setPlainText("");
		return;
	}
	
	if(cur()!=index)
    	Qtabs->setCurrentIndex(index);
	
	openTab(tabDataStr(index));
}
void closing(){
	autosave();
}


/************** Shortcuts  **************/
void tabsShift(int s){
	int i = cur()+s;
	if(i<0)i=0;
	int c = Qtabs->count();
	if(i>=c) i = c-1;
	Qtabs_selected(i);
}
void toggleSide(){

}
void toggleTabs(){
	// (QTextStream(stdout))<<"toggle:"<<tabsVisible<<"\n";
	tabsVisible = !tabsVisible;
    Qtabs->setVisible(tabsVisible);
}
void zoomFont(int z, bool textOnly = false){
	auto f = Qtext->font();
	int s = f.pointSize()+z;
	if(s<6)s=6;
	if(s>20)s=20;
	f.setPointSize(s);
	Qtext->setFont(f);
	
	if(textOnly) return;
	auto f2 = QtreeLine->font();
	int s2 = f2.pointSize()+z;
	if(s2<6)s2=6;
	if(s2>20)s2=20;
	f2.setPointSize(s2);
	Qtree->setFont(f2);
	QtabList->setFont(f2);
	QtreeLine->setFont(f2);
	Qtabs->setFont(f2);
}
void changeFont(){
	int size = Qtext->font().pointSize();
	if(font_mono) size+=1; //zoom-out a bit to adjust since mono is bigger than other fonts
	else size -= 1;

	font_mono=!font_mono;
	Qtext->setFont(font_mono?FONT_MONO:FONT);

	auto f = Qtext->font();
	f.setPointSize(size);
	Qtext->setFont(f);
}


void setEvents(){
	QObject::connect(Qtabs,
	  &QTabBar::tabMoved, Qtabs_moved);
	QObject::connect(QtabList->model(),
	  &QAbstractItemModel::rowsMoved, QtabList_moved);
	QObject::connect(appP,
	  &QApplication::aboutToQuit, closing);
	QObject::connect(Qtext,
	  &QTextEdit::textChanged, Qtext_changed);
	QObject::connect(QtreeLine,
	  &QLineEdit::textChanged, QtreeLine_changed);
	QObject::connect(Qtree,
	  &QTreeView::activated, Qtree_selected);
	QObject::connect(Qtabs,
	  &QTabBar::currentChanged, Qtabs_selected);
	QObject::connect(QtabList, &QListWidget::currentItemChanged,
      [](){Qtabs_selected(QtabList->currentIndex().row());});
	QObject::connect(QtabList, &QListWidget::itemClicked,
      [](){Qtabs_selected(QtabList->currentIndex().row());});
	QObject::connect(Qtabs,
	  &QTabBar::tabCloseRequested, closeTab);
}

void setShortcuts(){
	QObject::connect(new QShortcut(SHORTCUT_OPEN, win),
	  &QShortcut::activated, openFileDialog);
	QObject::connect(new QShortcut(SHORTCUT_NEW, win),
	  &QShortcut::activated, newFile);
	QObject::connect(new QShortcut(SHORTCUT_CLOSE, win),
	  &QShortcut::activated, [](){closeTab();});
	QObject::connect(new QShortcut(SHORTCUT_SAVE, win),
	  &QShortcut::activated, [](){save();});
	QObject::connect(new QShortcut(SHORTCUT_SAVEAS, win),
	  &QShortcut::activated, [](){save(-1,true);});
	QObject::connect(new QShortcut(SHORTCUT_NEXT, win),
	  &QShortcut::activated, std::bind(tabsShift,1));
	QObject::connect(new QShortcut(SHORTCUT_PREV, win),
	  &QShortcut::activated, std::bind(tabsShift,-1));
	QObject::connect(new QShortcut(SHORTCUT_TABS, win),
	  &QShortcut::activated, toggleTabs);
	QObject::connect(new QShortcut(SHORTCUT_SIDE, win),
	  &QShortcut::activated, toggleSide);
	QObject::connect(new QShortcut(SHORTCUT_FONT, win),
	  &QShortcut::activated, changeFont);
	QObject::connect(new QShortcut(SHORTCUT_HIGHLIGHT, win),
	  &QShortcut::activated, toggleHighlighting);
	QObject::connect(new QShortcut(SHORTCUT_ZOOMIN, win),
	  &QShortcut::activated, std::bind(zoomFont,1,false));
	QObject::connect(new QShortcut(SHORTCUT_ZOOMOUT, win),
	  &QShortcut::activated, std::bind(zoomFont,-1,false));
	QObject::connect(new QShortcut(SHORTCUT_TZOOMIN, win),
	  &QShortcut::activated, std::bind(zoomFont,1,true));
	QObject::connect(new QShortcut(SHORTCUT_TZOOMOUT, win),
	  &QShortcut::activated, std::bind(zoomFont,-1,true));
	QObject::connect(new QShortcut(SHORTCUT_RENAME, win),
	  &QShortcut::activated, [](){rename();});
}