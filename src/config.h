#include <QtCore/QVariant>

// CONFIGURE THE BELOW PATH!! (it can be relative)
#define     SAVE_LOCATION   "/data/notepadzz.json"
// IF TRUE THEN SAVE_LOCATION MUST BE A FOLDER!  [[NOT IMPLEMENTED CURRENTLY]]
#define     MULTI_FILE_SAVE false
#define     SETTINGS    ("Notepadzz", "Notepadzz")
//if true save to settings, if false save to save_location. MAKE SURE YOU CONFIGURE SAVE_LOCATION PATH!
#define     SAVE_TO_SETTINGS    false

#define     FONT        QFont("Arial", 12, /*QFont::Monospace|*/QFont::Normal)
#define     FONT_MONO   QFont("Courier New", 12, QFont::TypeWriter)
#define     FONT2       QFont("Arial", 12, /*QFont::Monospace|*/QFont::Normal)

#define     AUTOSAVE_SEC        10

#define     M_SPLITTER_WIDTH    3
#define     S_SPLITTER_WIDTH    3
// indentation of file preview tree in pixels
#define     TREE_INDENTATION    10
//switch to new tab when you open it
#define     SWITCH_TO_NEW       true
// show filename extension in tab
#define     SHOW_EXTENSION      true
// close button on tabs
#define     SHOW_CLOSE          false
// on startup, do you jump to last tab (true) or first (false)
#define     OPEN_LAST           true


#define     SHORTCUT_TABS       QKeySequence(Qt::CTRL + Qt::Key_T)
#define     SHORTCUT_OPEN       QKeySequence(Qt::CTRL + Qt::Key_O)
#define     SHORTCUT_NEW        QKeySequence(Qt::CTRL + Qt::Key_N)
#define     SHORTCUT_CLOSE      QKeySequence(Qt::CTRL + Qt::Key_W)
#define     SHORTCUT_SAVE       QKeySequence(Qt::CTRL + Qt::Key_S)
#define     SHORTCUT_SAVEAS     QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S)
#define     SHORTCUT_NEXT       QKeySequence(Qt::ALT + Qt::Key_PageUp)
#define     SHORTCUT_PREV       QKeySequence(Qt::ALT + Qt::Key_PageDown)
#define     SHORTCUT_SIDE       QKeySequence(Qt::CTRL + Qt::Key_Q)
#define     SHORTCUT_ZOOMIN     QKeySequence(Qt::CTRL + Qt::Key_Plus)
#define     SHORTCUT_ZOOMOUT    QKeySequence(Qt::CTRL + Qt::Key_Minus)
#define     SHORTCUT_TZOOMIN    QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Plus)
#define     SHORTCUT_TZOOMOUT   QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Minus)
#define     SHORTCUT_FONT       QKeySequence(Qt::CTRL + Qt::Key_G)
#define     SHORTCUT_HIGHLIGHT  QKeySequence(Qt::CTRL + Qt::Key_H)
#define     SHORTCUT_RENAME     QKeySequence(Qt::CTRL + Qt::Key_R)