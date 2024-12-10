struct Tab{
    QString path;
    QString text;
    bool dirty;
    
    Tab(){
        dirty = false;
        path = "";
        text = "";
    }
    Tab(bool d, QString p, QString t){
        dirty = d;
        path = p;
        text = t;
    }

    bool isNew(){
        return !this->path.startsWith('/');
    }
};