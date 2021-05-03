#ifndef __PLAYER_H_
#define __PLAYER_H_
#include "ship.h"
#include <memory>
#include <string>

class PlayerManager {
  std::string name;
  std::array<int, 100> fields;
  std::array<std::shared_ptr<Ship>, 5> ships = {
      std::shared_ptr<Ship>(new Destroyer()),
      std::shared_ptr<Ship>(new Submarine()),
      std::shared_ptr<Ship>(new Cruiser()),
      std::shared_ptr<Ship>(new Battleship()),
      std::shared_ptr<Ship>(new Carrier())};

public:
  PlayerManager(std::string name) {
    fields.fill(0);
    this->name = name;
  };
  auto getName() { return name; };
  auto getFields() { return fields; };
  auto getShips() { return ships; };
  void setFields(std::array<int, 100> fields) { this->fields = fields; };
  bool checkDestruction();
};
#endif
