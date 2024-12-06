#include "linkmanager.h"

void LinkManager::addLink(const Link &link) {
    links.append(link);
}

void LinkManager::setLinks(const QVector<Link> &newLinks) {
    links = newLinks;
}

QVector<Link> LinkManager::getAllLinks() const {
    return links;
}

void LinkManager::removeLinkAt(int index) {
    if (index >= 0 && index < links.size()) {
        links.removeAt(index);
    }
}

