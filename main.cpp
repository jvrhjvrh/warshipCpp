#include "main.h"

enum GameState { positioning, play, victory };

struct Game {
  enum GameState gameState = positioning;
  std::array<std::shared_ptr<PlayerManager>, 2> players;
  int maxX, maxY;
  int currentPlayer = 0;
  int cursorPosition = 0;
} Game;

class Gameboard {
  int startx, starty;
  int height = 20, width = 20;

public:
  Gameboard(int, int);
  void showBoard(WINDOW *);
  void updateBoard(WINDOW *, struct Game);
  void showVictoryScreen(WINDOW *, struct Game);
};

Gameboard::Gameboard(int maxHeight, int maxWidth) {
  startx = (maxWidth - width) / 2;
  starty = (maxHeight - height) / 2;
}

void Gameboard::showVictoryScreen(WINDOW *win, struct Game game) {
  wclear(win);
  mvwprintw(win, 0, 0, "%s WON!!!",
            game.players[game.currentPlayer]->getName().c_str());
}

void Gameboard::showBoard(WINDOW *win) {
  mvwaddch(win, starty, startx, corner);
  mvwaddch(win, starty + height, startx, corner);
  mvwaddch(win, starty, startx + width, corner);
  mvwaddch(win, starty + height, startx + width, corner);

  for (int i = 0; i <= height; i += 2) {
    mvwhline(win, starty + i, startx + 1, hBorder, width - 1);
    mvwvline(win, starty + 1, startx + i, vBorder, height - 1);
    if (i < height) {
      mvwaddch(win, starty + height + 1, startx + 1 + i, 'A' + i / 2);
      mvwaddch(win, starty + 1 + i, startx + width + 1, '0' + i / 2);
    }
  }

  for (int i = 2; i < height; i += 2) {
    for (int j = 2; j < width; j += 2) {
      mvwaddch(win, starty + i, startx + j, corner);
    }
  }

  wrefresh(win);
}

void Gameboard::updateBoard(WINDOW *win, struct Game game) {
  switch (game.gameState) {
  case positioning:
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 10; j++) {
        mvwaddch(win, starty + 1 + 2 * i, startx + 1 + 2 * j, sea);
      }
    }
    {
      auto currentPlayer = game.players.at(game.currentPlayer);
      mvprintw(0, 0, "%s", currentPlayer->getName().c_str());
      auto ships = currentPlayer->getShips();
      int startColumn = startx + width + 15;
      int startLine = starty + 3;
      for (int i = 0; i < (int)ships.size(); i++) {
        std::string status;
        auto currentShip = ships[i];
        switch (currentShip->getPlacement()) {
        case placed:
          status = "Placed";
          for (int x = 0; x < currentShip->getPositions().size(); x++) {
            int position = currentShip->getPositions()[x];
            attron(A_BOLD);
            mvwaddch(win, starty + 1 + 2 * (position / 10),
                     startx + 1 + 2 * (position % 10), hit);
            attroff(A_BOLD);
          }
          break;
        case notPlaced:
          status = "Yet to place";
          break;
        case placing:
          status = "Current placing";
          for (int x = 0; x < currentShip->getPositions().size(); x++) {
            int position = currentShip->getPositions()[x];
            attron(A_REVERSE);
            mvwaddch(win, starty + 1 + 2 * (position / 10),
                     startx + 1 + 2 * (position % 10), hit);
            attroff(A_REVERSE);
          }
          break;
        }
        mvwprintw(win, startLine + 2 * i, startColumn + 15, "%s: %s",
                  ships[i]->getName().c_str(), status.c_str());
      }
    }
    break;
  case play: {
    auto currentPlayer = game.players.at(game.currentPlayer);
    auto otherPlayer = game.players.at(game.currentPlayer ? 0 : 1);
    mvprintw(0, 0, "%s", currentPlayer->getName().c_str());
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 10; j++) {
        int symbol;
        switch (otherPlayer->getFields().at(i * 10 + j)) {
        case 0:
          symbol = sea;
          break;
        case 1:
          symbol = miss;
          break;
        case 2:
          symbol = hit;
          break;
        }
        if (game.cursorPosition == i * 10 + j)
          attron(A_REVERSE);
        mvwaddch(win, starty + 1 + 2 * i, startx + 1 + 2 * j, symbol);
        attroff(A_REVERSE);
      }
    }
    int startColumn = startx + width + 15;
    int startLine = starty + 3;
    auto ships = otherPlayer->getShips();
    for (int i = 0; i < (int)ships.size(); i++) {
      auto currentShip = ships[i];
      std::string status =
          currentShip->getDestroyed() ? "Destroyed" : "Not Destroyed";
      mvwprintw(win, startLine + 2 * i, startColumn + 15, "%s: %s",
                ships[i]->getName().c_str(), status.c_str());
    }
  } break;
  case victory:
    break;
  }
  wrefresh(win);
}

void init() {
  initscr();

  raw();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);
}

void positionPieces(struct Game *game, int move) {
  std::shared_ptr<Ship> currentShip;
  auto ships = game->players.at(game->currentPlayer)->getShips();
  for (auto ship : ships) {
    if (ship->getPlacement() == placing) {
      currentShip = ship;
      break;
    }
    if (ship->getPlacement() == notPlaced) {
      ship->setPlacement(placing);
      currentShip = ship;
      break;
    }
  }

  auto positions = currentShip->getPositions();
  int size = positions.size();
  switch (move) {
  case KEY_UP:
    if (positions[0] - 10 < 0 || positions[size - 1] - 10 < 0)
      return;
    std::transform(positions.begin(), positions.end(), positions.begin(),
                   [](int c) -> int { return c - 10; });
    currentShip->setPositions(positions);
    break;
  case KEY_DOWN:
    if (positions[0] + 10 > 99 || positions[size - 1] + 10 > 99)
      return;
    std::transform(positions.begin(), positions.end(), positions.begin(),
                   [](int c) -> int { return c + 10; });
    currentShip->setPositions(positions);
    break;
  case KEY_RIGHT:
    switch (currentShip->getRotation()) {
    case right:
    case left:
      if (((positions[0] + 1) / 10) != ((positions[size - 1] + 1) / 10))
        return;
      break;
    case up:
    case down:
      if (((positions[0]) / 10) != ((positions[0] + 1) / 10))
        return;
      break;
    }
    std::transform(positions.begin(), positions.end(), positions.begin(),
                   [](int c) -> int { return ++c; });
    currentShip->setPositions(positions);
    break;
  case KEY_LEFT:
    switch (currentShip->getRotation()) {
    case right:
    case left:
      if (((positions[0] - 1) / 10) != ((positions[size - 1] - 1) / 10))
        return;
      break;
    case up:
    case down:
      if (((positions[0]) / 10) != ((positions[0] - 1) / 10))
        return;
      break;
    }
    std::transform(positions.begin(), positions.end(), positions.begin(),
                   [](int c) -> int { return --c; });
    currentShip->setPositions(positions);
    break;
  case 10:
    for (auto ship : ships) {
      if (ship->getPlacement() == notPlaced || ship->getPlacement() == placing)
        break;
      for (int i = 0; i < size; i++) {
        for (int j = 0; j < ship->getPositions().size(); j++) {
          if (positions[i] == ship->getPositions()[j])
            return;
        }
      }
    }
    currentShip->setPlacement(placed);
    {
      std::shared_ptr<Ship> nextShip;
      for (auto ship : ships) {
        if (ship->getPlacement() == notPlaced) {
          nextShip = ship;
          break;
        }
      }
      if (nextShip == nullptr) {
        if (game->currentPlayer == 0) {
          game->currentPlayer = 1;
          game->players.at(1)->getShips().at(0)->setPlacement(placing);
        } else {
          game->currentPlayer = 0;
          game->gameState = play;
        }
      } else {
        nextShip->setPlacement(placing);
      }
    }
    break;
  case 'r':
    switch (currentShip->getRotation()) {
    case right:
      if (positions[0] + (size - 1) * 10 >= 100) {
        return;
      }
      for (int i = 1; i < size; i++) {
        positions[i] = positions[0] + (10 * i);
      }
      currentShip->setPositions(positions);
      currentShip->setRotation(down);
      break;
    case down:
      if (positions[0] - size + 1 < 0 ||
          positions[0] / 10 != (positions[0] - size + 1) / 10) {
        return;
      }
      for (int i = 1; i < size; i++) {
        positions[i] = positions[0] - i;
      }
      currentShip->setPositions(positions);
      currentShip->setRotation(left);
      break;
    case left:
      if (positions[0] - (size - 1) * 10 < 0) {
        return;
      }
      for (int i = 1; i < size; i++) {
        positions[i] = positions[0] - (10 * i);
      }
      currentShip->setPositions(positions);
      currentShip->setRotation(up);
      break;
    case up:
      if (positions[0] + size - 1 > 99 ||
          positions[0] / 10 != (positions[0] + size - 1) / 10) {
        return;
      }
      for (int i = 1; i < size; i++) {
        positions[i] = positions[0] + i;
      }
      currentShip->setPositions(positions);
      currentShip->setRotation(right);
      break;
    }
    break;
  }
}

void playGame(struct Game *game, int move) {
  auto currentPlayer = game->players.at(game->currentPlayer);
  auto otherPlayer = game->players.at(game->currentPlayer ? 0 : 1);
  auto cursor = game->cursorPosition;
  switch (move) {
  case 'a':
  case 'A':
    cursor = (cursor / 10) * 10 + 0;
    break;
  case 'b':
  case 'B':
    cursor = (cursor / 10) * 10 + 1;
    break;
  case 'c':
  case 'C':
    cursor = (cursor / 10) * 10 + 2;
    break;
  case 'd':
  case 'D':
    cursor = (cursor / 10) * 10 + 3;
    break;
  case 'e':
  case 'E':
    cursor = (cursor / 10) * 10 + 4;
    break;
  case 'f':
  case 'F':
    cursor = (cursor / 10) * 10 + 5;
    break;
  case 'g':
  case 'G':
    cursor = (cursor / 10) * 10 + 6;
    break;
  case 'h':
  case 'H':
    cursor = (cursor / 10) * 10 + 7;
    break;
  case 'i':
  case 'I':
    cursor = (cursor / 10) * 10 + 8;
    break;
  case 'j':
  case 'J':
    cursor = (cursor / 10) * 10 + 9;
    break;
  case '0':
    cursor = 0 + (cursor % 10);
    break;
  case '1':
    cursor = 10 + (cursor % 10);
    break;
  case '2':
    cursor = 20 + (cursor % 10);
    break;
  case '3':
    cursor = 30 + (cursor % 10);
    break;
  case '4':
    cursor = 40 + (cursor % 10);
    break;
  case '5':
    cursor = 50 + (cursor % 10);
    break;
  case '6':
    cursor = 60 + (cursor % 10);
    break;
  case '7':
    cursor = 70 + (cursor % 10);
    break;
  case '8':
    cursor = 80 + (cursor % 10);
    break;
  case '9':
    cursor = 90 + (cursor % 10);
    break;
  case KEY_UP:
    if (cursor - 10 < 0)
      return;
    cursor -= 10;
    break;
  case KEY_DOWN:
    if (cursor + 10 > 99)
      return;
    cursor += 10;
    break;
  case KEY_RIGHT:
    if (cursor % 10 == 9)
      return;
    cursor++;
    break;
  case KEY_LEFT:
    if (cursor % 10 == 0)
      return;
    cursor--;
    break;
  case 10:
    auto pos = otherPlayer->getFields();
    mvprintw(1, 1, "%d", pos[cursor]);
    auto ships = otherPlayer->getShips();
    if (pos.at(cursor) != 0)
      return;
    bool found = false;
    for (auto ship : ships) {
      for (auto p : ship->getPositions()) {
        if (cursor == p) {
          found = true;
          break;
        }
      }
      if (found) {
        break;
      }
    }
    if (!found) {
      game->currentPlayer = game->currentPlayer ? 0 : 1;
    }
    pos.at(cursor) = found ? 2 : 1;
    otherPlayer->setFields(pos);
    if (otherPlayer->checkDestruction()) {
      game->gameState = victory;
    }
    break;
  }
  game->cursorPosition = cursor;
}

int main(int argc, char *argv[]) {
  struct Game game;
  int ch;
  init();
  getmaxyx(stdscr, game.maxY, game.maxX);

  game.players.at(0) =
      std::shared_ptr<PlayerManager>(new PlayerManager("Player 1"));
  game.players.at(1) =
      std::shared_ptr<PlayerManager>(new PlayerManager("Player 2"));
  Gameboard gameboard = Gameboard(game.maxY, game.maxX);

  gameboard.showBoard(stdscr);
  gameboard.updateBoard(stdscr, game);
  do {
    switch (game.gameState) {
    case positioning:
      positionPieces(&game, ch);
      break;
    case play:
      playGame(&game, ch);
      break;
    case victory:
      break;
    }
    if (game.gameState == victory) {
      gameboard.showVictoryScreen(stdscr, game);
    } else {
      gameboard.showBoard(stdscr);
      gameboard.updateBoard(stdscr, game);
    }
  } while ((ch = getch()) != ctrl('c'));

  return 0;
}
