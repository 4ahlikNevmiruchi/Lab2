#ifndef LINK_H
#define LINK_H

#include <QString>

class Link {
public:
    Link(const QString &type, const QString &url, const QString &context)
        : type(type), url(url), context(context) {}

    QString getType() const { return type; }
    QString getUrl() const { return url; }
    QString getContext() const { return context; }

private:
    QString type;
    QString url;
    QString context;
};

#endif // LINK_H



