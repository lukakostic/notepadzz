int main(int argc, char *argv[ ]){
	QApplication app(argc, argv);
	appP = &app;
	/************** Make ui elements **************/	

	auto df = new DropFilter(); //detect file drops

	//////////window {
	win = new QMainWindow();
	win->setWindowTitle("Notepadzz");
	win->resize(600,500);
	win->setAcceptDrops(true);
	win->installEventFilter(df);

	auto w = new QWidget();
	win->setCentralWidget(w);
	auto winLayout = new QHBoxLayout(w);
	winLayout->setSpacing(0);
	winLayout->setMargin(0);
	
	splitter = new QSplitter(w);
	splitter->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
	splitter->setHandleWidth(M_SPLITTER_WIDTH);
	//splitter->setCollapsible(0,false);
	winLayout->addWidget(splitter);
	//////////window }

	//////////side panel {
	auto sidePane = new QWidget();
	auto sideLayout = new QVBoxLayout(sidePane);
	{//to not pollute variables
		auto sideSPolicy = QSizePolicy();
		sideSPolicy.setHorizontalStretch(0);
		sideSPolicy.setVerticalPolicy(QSizePolicy::Fixed);
		sidePane->setSizePolicy(sideSPolicy);
		sidePane->setMinimumSize(10,100);
		sideLayout->setSpacing(0);
		sideLayout->setMargin(0);
	}
	splitter->addWidget(sidePane);


	sideSplitter = new QSplitter(sidePane);
	sideSplitter->setOrientation(Qt::Orientation::Vertical);
	sideSplitter->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
	sideSplitter->setHandleWidth(S_SPLITTER_WIDTH);
	//sideSplitter->setCollapsible(0,false);
	sideLayout->addWidget(sideSplitter);


	QtabList = new QListWidget();
	QtabList->setFont(FONT2);
	QtabList->setDragDropMode(QAbstractItemView::InternalMove);
	QtabList->setMinimumHeight(10);
	//QtabList->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
	//QtabList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	sideSplitter->addWidget(QtabList);


	auto topPane = new QWidget();
	auto topLayout = new QVBoxLayout(topPane);
	topLayout->setSpacing(0);
	topLayout->setMargin(0);
	sideSplitter->addWidget(topPane);


	QtreeLine = new QLineEdit();
	QtreeLine->setFont(FONT2);
	QtreeLine->setFixedHeight(15);
	QtreeLine->setContentsMargins(0,0,0,0);
	QtreeLine->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	topLayout->addWidget(QtreeLine);

	Qtree = new QTreeView();
	Qtree->setFont(FONT2);
	Qtree->setAnimated(false);
	Qtree->setIndentation(20);
	Qtree->setSortingEnabled(true);
	topLayout->addWidget(Qtree);

	model = new QFileSystemModel(Qtree);
	model->setReadOnly(true);
	model->setFilter(QDir::AllDirs | QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);
	Qtree->setModel(model);
	Qtree->hideColumn(1);
	Qtree->hideColumn(2);
	Qtree->hideColumn(3);
	Qtree->setIconSize(QSize(0,0));
	Qtree->header()->setFixedHeight(3);

	Qtree->setIndentation(TREE_INDENTATION);
	Qtree->setTextElideMode(Qt::TextElideMode::ElideNone);
	Qtree->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	Qtree->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//Qtree->setStyleSheet( "QTreeView::branch { width:0pt; border-image: url(none.png); }" );
	Qtree->setHeaderHidden(true);
	Qtree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	Qtree->header()->setStretchLastSection(false);

	//////////side panel }

	//////////main panel {
	auto mainPane = new QWidget();
	auto mainLayout = new QVBoxLayout(mainPane);
	{//to not pollute variables
		auto mainSPolicy = QSizePolicy();
		mainSPolicy.setHorizontalStretch(20);
		mainPane->setSizePolicy(mainSPolicy);
		mainPane->setMinimumSize(100,100);
		mainLayout->setSpacing(0);
		mainLayout->setMargin(0);
	}
	splitter->addWidget(mainPane);

	Qtabs = new QTabBar();
	Qtabs->setFont(FONT2);
	Qtabs->setMaximumHeight(24);
	Qtabs->setTabsClosable(SHOW_CLOSE);
	Qtabs->setDrawBase(false);
	Qtabs->setMovable(true);
	Qtabs->setAcceptDrops(true);
	Qtabs->setAutoHide(false);
	Qtabs->setVisible(false); //initially but it gets later set to tabsVisible value.
	//Qtabs->setStyleSheet(
		//"QTabBar::tab{font-size:12pt;padding:3pt;}"
		//"QTabBar {background-color: #000000; color:#ffffff;}"
		//"QTabBar::tab:!selected {height: 50px; width: 100px;background-color: #000000; color:#ffffff;font-size:12pt;}"
		//"QTabBar::tab:selected {background-color: #FFFFFF;}"
	//	"QTabBar::tab:!selected {background-color: #CCCCCC;border-color: black;}"
		//"QTabBar::tab:selected {background-color: #FFFFFF;}"
	//);
	mainLayout->addWidget(Qtabs);


 	Qtext = new QTextEdit();
	Qtext->setAcceptRichText(false);

	highlighter = new MyHighlighter(Qtext->document());
	highlighter->setEnabled(highlighting);


 	Qtext->setFont(FONT);
	mainLayout->addWidget(Qtext);
	//////////main panel }
	
	//splitter->setCollapsible(0,false);
	splitter->setCollapsible(1,false);
	

	/************** Events **************/
	setEvents();
	
	/************** Shortcuts **************/
	setShortcuts();

	/************** Load previous session  **************/

	QString json = "";
	if(SAVE_TO_SETTINGS){
		QSettings settings SETTINGS;
		if(settings.contains("settings"))
			json = settings.value("settings").toString();
	}else{
		QFile file(QString(SAVE_LOCATION));
		if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream in(&file);
			json = in.readAll();
			file.close();
		}else{
			// QMessageBox::information(0, "error", file.errorString());
		}
	}
	if(json.length()>3){ // >3 in case its empty json list "[]" (3 because what if space or newline or something..)
		int i = 0;
		auto doc = QJsonDocument::fromJson(json.toUtf8());
		auto j = doc.array();
		folder = j[i++].toString();
		viewFolder(true);
		
		auto sizeList = QList<int>();
		sizeList.append(j[i++].toInt());
		sizeList.append(j[i++].toInt());
		splitter->setSizes(sizeList);
		auto sizeList2 = QList<int>();
		sizeList2.append(j[i++].toInt());
		sizeList2.append(j[i++].toInt());
		sideSplitter->setSizes(sizeList2);

		tabsVisible = j[i++].toBool();
	// (QTextStream(stdout))<<"tabsVis:"<<tabsVisible<<"\n";
		Qtabs->setVisible(tabsVisible);
		
		auto f1 = FONT;
		f1.setPointSize(j[i++].toInt());
		Qtext->setFont(f1);
		auto f2 = FONT2;
		f2.setPointSize(j[i++].toInt());
		Qtree->setFont(f2);
		QtabList->setFont(f2);
		QtreeLine->setFont(f2);
		Qtabs->setFont(f2);

		int ww = j[i++].toInt();
		int wh = j[i++].toInt();
		win->resize(ww, wh);

		// load files:
		int o = j[i++].toInt();
		//qDebug() << o;
		for(int f = 0; f < o; f++){
    		QJsonObject t = j[i++].toObject();
			bool dirty = t["dirty"].toBool();
			QString path = t["path"].toString();
			QString text = t["text"].toString();
			//qDebug() << path;
			Tab tab = {dirty,path,text};
			files.insert(path, tab);
			loadTab(path,dirty);
		}
		// if(o!=0){
		// 	(QTextStream(stdout))<<"preShow"<<"\n";
	
		// 	// if(OPEN_LAST){
		// 	// 	Qtabs_selected(o-1);
		// 	// }else{
		// 	// 	Qtabs_selected(0);
		// 	// 	// if(Qtabs)
		// 	// 	// Qtabs_selected(0); //if any tabs..
		// 	// 	// if(f==0)openTab(path,true);
		// 	// }
		// }
	}


	/************** Autosave  **************/
	auto timer = new QTimer(win);
	QObject::connect(timer, &QTimer::timeout, autosave);
	timer->start(AUTOSAVE_SEC*1000);


	win->show();
	return app.exec();
}