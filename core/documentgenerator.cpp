#include <QDebug>
#include "documentgenerator.h"

using namespace MeBook;

DocumentGenerator::DocumentGenerator(const QString &filename, QObject *parent) : QNetworkAccessManager(parent),
    bookFilename(filename)
{


}

DocumentGenerator::~DocumentGenerator()
{

}
