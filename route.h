#pragma once

#include <QString>
#include <QList>

#include "state.h"

class Route
{
public:
    Route();

    QString NomCentreInit;
    QString NomCentreDest;
    QString IdGroupeExars;

    QList<State> states;
};

