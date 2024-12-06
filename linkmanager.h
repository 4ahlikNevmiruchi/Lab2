#ifndef LINKMANAGER_H
#define LINKMANAGER_H

#include "link.h"
#include <QVector>

class LinkManager {
public:
    void addLink(const Link &link);
    QVector<Link> getAllLinks() const;
    void setLinks(const QVector<Link> &links);
    void removeLinkAt(int index);

private:
    QVector<Link> links;

};

#endif // LINKMANAGER_H
