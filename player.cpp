#include "player.h"

bool PlayerManager::checkDestruction() {
  bool allDestroyed = true;
  for (auto ship : this->ships) {
    bool destroyed = true;
    for (auto p : ship->getPositions()) {
      if (this->fields.at(p) == 0) {
        destroyed = false;
        break;
      }
    }
    if (destroyed) {
      ship->destroyShip();
    } else {
      allDestroyed = false;
    }
  }
  return allDestroyed;
}
