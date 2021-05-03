#ifndef __MAIN_H_
#define __MAIN_H_

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <ncurses.h>
#include <string>
#include <vector>

#include "player.h"
#include "ship.h"

#define ctrl(x) ((x)&0x1f)

const int vBorder = '|', hBorder = '-', corner = '+', sea = '~', miss = 'o',
          hit = 'x';

#endif
