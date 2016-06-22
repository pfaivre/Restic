#include "state.h"

State::State()
{

}

QString State::toString() {
    return "id: " + id + ", date: " + DateHeure.toString();
}
