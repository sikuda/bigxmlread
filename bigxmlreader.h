#ifndef BIGXMLREADER_H
#define BIGXMLREADER_H

#include <QIcon>
#include <QFile>
#include <QXmlStreamReader>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTextDecoder>

QT_BEGIN_NAMESPACE
class BigXmlReader;
class BigXmlItem;
QT_END_NAMESPACE

class BigXmlItem: public QTreeWidgetItem
{

public:
    enum XmlItemType { Empty = 0, Node = 1, Attribute = 2, Comment = 3 };

    BigXmlItem(QTreeWidgetItem* parent, BigXmlItem::XmlItemType Type);
    BigXmlItem(QTreeWidget* parent, BigXmlItem::XmlItemType Type);

    inline void setXmlType( BigXmlItem::XmlItemType Type){type = Type;};
    inline BigXmlItem::XmlItemType getXmlType(){return type; };

private:
    XmlItemType type;

};

class BigXmlReader: public QTreeWidget
{
    Q_OBJECT

public:
    BigXmlReader(QWidget* parent = 0);
    ~BigXmlReader();

    bool readBigXML(QXmlStreamReader& xml);
    bool readBigXMLtoLevel(QXmlStreamReader& xml, int levelDown);
    bool findDataBigXML( QXmlStreamReader& xml, QString strData, QTreeWidgetItem* item);
    void buildTreeBigXMLToMaxIndex();
    QString errorXMLString(QXmlStreamReader& xml) const;
    bool openFile( QString& fileName, QXmlStreamReader& xml, bool fOpenNew = true);
    QString getFilename(){ return currentFile.fileName(); };

    QString strFindString;

public Q_SLOTS:
    void expandBigXmlItem(QTreeWidgetItem * item );
    void enterBigXmlItem(QTreeWidgetItem * item, int column);

private:
    BigXmlItem* createChildItem(BigXmlItem* item, BigXmlItem::XmlItemType Type );
    bool isNextIndex(QHash<int, int> &currentIndex);
    bool isInsideIndex(QHash<int, int> &currentIndex);

    QIcon folderIcon;
    QIcon bookmarkIcon;
    int readLevel;

    QFile currentFile;
    QHash<int, int> maxIndex;

};

#endif
