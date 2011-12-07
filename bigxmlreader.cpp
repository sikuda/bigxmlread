#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QDialog>
#include <QPushButton>
#include "bigxmlreader.h"

//------------------------------- BigXmlItem ---------------------------

BigXmlItem::BigXmlItem(QTreeWidgetItem* parent, BigXmlItem::XmlItemType Type):QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
    type = Type;
}

BigXmlItem::BigXmlItem(QTreeWidget* parent, BigXmlItem::XmlItemType Type):QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
    type = Type;
}

//------------------------------- BigXmlReader ---------------------------

BigXmlReader::BigXmlReader(QWidget* parent):QTreeWidget(parent)
{
    readLevel = -1;

    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon), QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon), QIcon::Normal, QIcon::On);
    bookmarkIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));
    setAlternatingRowColors(true);

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(expandBigXmlItem(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(enterBigXmlItem(QTreeWidgetItem*, int)));
}

BigXmlReader::~BigXmlReader()
{
    //TO DO
}

bool BigXmlReader::openFile(QString& fileName, QXmlStreamReader& xml, bool fOpenNew )
{
    if(  fOpenNew ){
        clear();
        currentFile.close();
        currentFile.setFileName(fileName);
        if (!currentFile.open(QFile::ReadOnly | QFile::Text )) {
            QMessageBox::warning(this, tr("BigXmlReader"),
                                 tr("Cannot read file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(currentFile.errorString()));
            return false;
        }
    }
    currentFile.seek(0);
    xml.setDevice(&currentFile);
    return true;
}

bool BigXmlReader::readBigXML(QXmlStreamReader& xml)
{
    BigXmlItem* item = 0;
    int level = 0;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    clear();
    while( !xml.atEnd() ){

        QXmlStreamReader::TokenType tokentype = xml.readNext();
        switch (tokentype) {
        case QXmlStreamReader::StartElement:
        {   level ++;
            item = createChildItem(item, BigXmlItem::Node);
            item->setText(0, xml.name().toString());
            item->setIcon(0, folderIcon);
            foreach(QXmlStreamAttribute attr, xml.attributes())
            {
                BigXmlItem* childItem = new BigXmlItem(item, BigXmlItem::Attribute);
                childItem->setText(0, attr.name().toString());
                childItem->setText(1, attr.value().toString());
            }
        }
            break;
        case QXmlStreamReader::EndElement:
        {
            if(item) item = static_cast<BigXmlItem*>(item->parent());
            level --;
            break;
        }
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::Comment:
            if(item){
                QString name(xml.text().toString().simplified());
                item->setText(1, name);
                if( name.size() > 0) item->setIcon(0, bookmarkIcon);
                item->setXmlType(BigXmlItem::Comment);
            }
            break;
        default: break;
        }
    }
    resizeColumnToContents(0);
    resizeColumnToContents(1);
    QApplication::restoreOverrideCursor();
    return !xml.error();
}

bool BigXmlReader::readBigXMLtoLevel(QXmlStreamReader& xml, int levelDown)
{
    BigXmlItem* item = 0;
    int level = 0;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    clear();
    while( !xml.atEnd() ){

        QXmlStreamReader::TokenType tokentype = xml.readNext();
        switch (tokentype) {
        case QXmlStreamReader::StartElement:
            level ++;
            if( level <= levelDown){
                item = createChildItem(item, BigXmlItem::Node);
                item->setText(0, xml.name().toString());
                item->setIcon(0, folderIcon);
                if( level == levelDown){
                    new BigXmlItem(item, BigXmlItem::Empty);
                }else{
                    foreach(QXmlStreamAttribute attr, xml.attributes())
                    {
                        BigXmlItem* childItem = new BigXmlItem(item, BigXmlItem::Attribute);
                        childItem->setText(0, attr.name().toString());
                        childItem->setText(1, attr.value().toString());
                    }
                }
            }
            break;
        case QXmlStreamReader::EndElement:
            if( level <= levelDown){
                if(item) item = static_cast<BigXmlItem*>(item->parent());
            }
            level --;
            break;
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::Comment:
            if( level <= levelDown){
                if(item){
                    QString name(xml.text().toString().simplified());
                    if( name.size() > 0){
                        item->setText(1, name);
                        item->setIcon(0, bookmarkIcon);
                        item->setXmlType(BigXmlItem::Comment);
                        item->takeChildren().clear();
                    }
                }
            }
            break;
        default: break;
        }
    }
    resizeColumnToContents(0);
    resizeColumnToContents(1);
    QApplication::restoreOverrideCursor();
    return !xml.error();
}

bool BigXmlReader::findDataBigXML( QXmlStreamReader& xml, QString strData, QTreeWidgetItem* item = 0)
{
    int level = 0;
    maxIndex.clear();

    //calculate current item
    QHash<int, int> currentIndex;
    int i = 0;
    if( item != 0 ){
        QTreeWidgetItem* item1 = item;
        while( item1 != 0 ){
            if( item1->parent() == 0 ) currentIndex.insert(i,indexOfTopLevelItem(item1));
            else currentIndex.insert(i,item1->parent()->indexOfChild(item1));
            item1 = item1->parent();
            i++;
        }
        int k = 0;
        int i2 = i / 2;
        while( k < i2 ){
            int val = currentIndex.value(k);
            currentIndex.insert(k, currentIndex.value(i-k-1));
            currentIndex.insert(i-k-1, val);
            k++;
        }
    }
    QString name;

    while( !xml.atEnd() ){

        QXmlStreamReader::TokenType tokentype = xml.readNext();
        switch (tokentype) {
        case QXmlStreamReader::StartElement:{
            if(maxIndex.contains(level)) maxIndex.insert( level, maxIndex.value(level)+1);
            else maxIndex.insert( level, 0);
            level ++;
            foreach(QXmlStreamAttribute attr, xml.attributes()){
                if(maxIndex.contains(level)) maxIndex.insert( level, maxIndex.value(level)+1);
                else maxIndex.insert( level, 0);
                name = attr.name().toString().toLower();
                if(name.contains(strData.toLower())){
                    if(isNextIndex(currentIndex)) return true;
                }
                name = attr.value().toString().toLower();
                if(name.contains(strData.toLower())){
                    if(isNextIndex(currentIndex)) return true;
                }
            }
            name = xml.name().toString().toLower();
            if(name.contains(strData.toLower())){
                maxIndex.remove(level);
                if(isNextIndex(currentIndex)) return true;
            }
        }
            break;
        case QXmlStreamReader::EndElement:{
            maxIndex.remove(level);
            level --;
        }
            break;
        case QXmlStreamReader::Characters:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::Comment:{
            name = xml.text().toString().toLower();
            if(name.contains(strData.toLower())){
                maxIndex.remove(level);
                if(isNextIndex(currentIndex)) return true;
            }
        }
            break;
        default: break;
        }
    }
    return false;
}

bool BigXmlReader::isNextIndex(QHash<int, int> &currentIndex)
{
    if(currentIndex.empty()) return true;
    int i = 0;
    while( true ){
        if( maxIndex.contains(i) ){
            if( currentIndex.contains(i) ){
                int diff = maxIndex.value(i) - currentIndex.value(i);
                if( diff > 0) return true;
                else if( diff < 0 ) return false;
            }
            else return true;
        }
        else return false;
        i++;
    }
}

bool BigXmlReader::isInsideIndex(QHash<int, int> &currentIndex)
{
    if(currentIndex.empty()) return true;
    for( int i = 0; i < currentIndex.size(); i++)
    {
        if( maxIndex.contains(i) ){
            if( maxIndex.value(i) != currentIndex.value(i)) return false;
        }else return false;
    }

    return true;
}

void BigXmlReader::buildTreeBigXMLToMaxIndex()
{
    BigXmlItem *item = 0;
    int level = 0;
    QHash<int, int> currentIndex(maxIndex);

    while( currentIndex.contains(level) ){
        if(item){
            if( item->getXmlType() == BigXmlItem::Empty ) expandBigXmlItem( item );
            else expandItem( item );
            item = static_cast<BigXmlItem*>(item->child( currentIndex.value(level)));
        }
        else item = static_cast<BigXmlItem*>(topLevelItem(currentIndex.value(level)));
        setCurrentItem ( item );
        level++;
    }

}

QString BigXmlReader::errorXMLString(QXmlStreamReader& xml) const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}

BigXmlItem* BigXmlReader::createChildItem(BigXmlItem *item, BigXmlItem::XmlItemType Type )
{
    BigXmlItem *childItem = 0;
    if (item != 0) {
        childItem = new BigXmlItem(item, Type );
    } else {
        childItem = new BigXmlItem(this, Type );
    }
    return childItem;
}

void BigXmlReader::expandBigXmlItem(QTreeWidgetItem* itemBegin )
{
    BigXmlItem* item = static_cast<BigXmlItem*>(itemBegin);
    if(item){
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        if(item->childCount() == 1){
            if( static_cast<BigXmlItem*>(item->child(0))->getXmlType() == BigXmlItem::Empty ){

                item->takeChildren().clear();
                QXmlStreamReader xml;

                //calculate current item
                QHash<int, int> currentIndex;
                int i = 0;
                QTreeWidgetItem* item1 = item;
                while( item1 != 0 ){
                    if( item1->parent() == 0 ) currentIndex.insert(i,indexOfTopLevelItem(item1));
                    else currentIndex.insert(i,item1->parent()->indexOfChild(item1));
                    item1 = item1->parent();
                    i++;
                }
                int k = 0;
                int i2 = i / 2;
                while( k < i2 ){
                    int val = currentIndex.value(k);
                    currentIndex.insert(k, currentIndex.value(i-k-1));
                    currentIndex.insert(i-k-1, val);
                    k++;
                }
                int maxLevel = i;

                QString strFilename;
                openFile(strFilename, xml, false);
                maxIndex.clear();
                bool InsideIndex = false;
                int level = 0;
                while( !xml.atEnd() ){

                    QXmlStreamReader::TokenType tokentype = xml.readNext();
                    switch (tokentype) {
                    case QXmlStreamReader::StartElement:
                    {
                        if(maxIndex.contains(level)) maxIndex.insert( level, maxIndex.value(level)+1);
                        else maxIndex.insert( level, 0);
                        level ++;

                        InsideIndex = isInsideIndex(currentIndex);

                        foreach(QXmlStreamAttribute attr, xml.attributes())
                        {
                            if(maxIndex.contains(level)) maxIndex.insert( level, maxIndex.value(level)+1);
                            else maxIndex.insert( level, 0);
                            if( InsideIndex && (level == maxLevel) ){
                                BigXmlItem* childItem = new BigXmlItem(item, BigXmlItem::Attribute);
                                childItem->setText(0, attr.name().toString());
                                childItem->setText(1, attr.value().toString());
                            }
                        }

                        if( InsideIndex && (level == maxLevel + 1)){
                            item = createChildItem(item, BigXmlItem::Node);
                            item->setText(0, xml.name().toString());
                            item->setIcon(0, folderIcon);
                            new BigXmlItem(item, BigXmlItem::Empty);
                        }
                    }
                        break;
                    case QXmlStreamReader::EndElement:
                        if( InsideIndex && (level > maxLevel) &&(level <= maxLevel + 1)){
                            if(item) item = static_cast<BigXmlItem*>(item->parent());
                        }
                        maxIndex.remove(level);
                        level --;
                        break;
                    case QXmlStreamReader::Characters:
                    case QXmlStreamReader::DTD:
                    case QXmlStreamReader::Comment:
                        if( InsideIndex && (level > maxLevel) && (level <= maxLevel + 1)){
                            QString name(xml.text().toString().simplified());
                            if( (name.size() > 0) && item ){
                                item->setText(1, name);
                                item->takeChildren().clear();
                                item->setIcon(0, bookmarkIcon);
                                item->setXmlType(BigXmlItem::Comment);
                            }
                        }
                        break;
                    default: break;
                    }
                }
            }
        }
    }
    QTreeWidget::expandItem(itemBegin);
    setCurrentItem ( itemBegin );
    QApplication::restoreOverrideCursor();
}

void BigXmlReader::enterBigXmlItem(QTreeWidgetItem* itemBegin, int column)
{
    QPlainTextEdit* label = new QPlainTextEdit(this);
    label->setPlainText(itemBegin->text(1));
    label->setReadOnly(true);
    label->setMaximumHeight(200);

    QVBoxLayout *Layout = new QVBoxLayout;
    Layout->addWidget(label);

    QDialog textDialog(this);
    textDialog.setMinimumSize(320, 240);
    textDialog.setLayout(Layout);
    textDialog.setWindowTitle(tr("BigXmlReader"));
    //connect(okButton, SIGNAL(pressed()), &textDialog, SLOT(accept()));

    if( textDialog.exec() == QDialog::Accepted){
        // TODO;
    }
    textDialog.close();
}


