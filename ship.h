#ifndef __SHIP_H_
#define __SHIP_H_

#include <array>
#include <string>
#include <vector>

enum ShipPlacement {
  notPlaced,
  placing,
  placed,
};

enum Rotation {
  right,
  down,
  left,
  up,
};

class Ship {
protected:
  std::vector<int> positions;
  std::string name;
  bool destroyed = false;
  enum ShipPlacement placement = notPlaced;
  enum Rotation rotation = right;

public:
  void setRotation(enum Rotation r) { rotation = r; }
  enum Rotation getRotation() { return rotation; }
  void setPlacement(enum ShipPlacement p) { placement = p; }
  enum ShipPlacement getPlacement() { return placement; }
  std::vector<int> getPositions() { return positions; }
  void setPositions(std::vector<int> p) { positions = p; }
  auto getName() { return name; }
  void destroyShip() { destroyed = true; }
  bool getDestroyed() { return destroyed; }
};

class Destroyer : public Ship {
public:
  Destroyer() {
    positions = std::vector<int>{0, 1};
    name = "Destroyer";
  };
};
class Submarine : public Ship {
public:
  Submarine() {
    positions = std::vector<int>{0, 1, 2};
    name = "Submarine";
  };
};
class Cruiser : public Ship {
public:
  Cruiser() {
    positions = std::vector<int>{0, 1, 2};
    name = "Cruiser";
  };
};
class Battleship : public Ship {
public:
  Battleship() {
    positions = std::vector<int>{0, 1, 2, 3};
    name = "Battleship";
  };
};
class Carrier : public Ship {
public:
  Carrier() {
    positions = std::vector<int>{0, 1, 2, 3, 4};
    name = "Carrier";
  };
};

#endif
