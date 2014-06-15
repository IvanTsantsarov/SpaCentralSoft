#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include <QMainWindow>
#include <QString>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#include "structures.h"

#define ERROR_MESSAGE "Failed to load \"config.xml\""

class ConfigLoader
{
private:
    QDomDocument _configDocument;
    QMainWindow* _mainWindow;

    ProgramSettings _programSettings;
    QList<IndicatorProperties> _listIndicatorProperties;

public:
    ConfigLoader(QString fileName, QMainWindow* mainWindow);

private:
    QDomElement firstChildElement(const QString& tagName, QDomDocument& document) const;
    QDomElement firstChildElement(const QString& tagName, QDomElement& element) const;
    QDomElement nextSiblingElement(const QString& tagName, QDomElement& element) const;
    QString getAttributeValue(const QString& attributeName, QDomElement& element) const;

    void parseProgramSettings(QDomElement& element);
    void parseIndicatorProperties(QDomElement& element);

public:
    ProgramSettings getProgramSettings() const;
    QList<IndicatorProperties> getIndicatorProperties() const;

private:
    void logToConsole(const QDomElement &element) const;
    void logToConsole(const QDomAttr& attribute) const;
    void showFatalError(const QString errorMessage) const;
};

#endif // CONFIGLOADER_H
