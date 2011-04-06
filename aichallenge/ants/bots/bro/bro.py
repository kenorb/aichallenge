#!/usr/bin/env python

# temp
import sys
from runner import *
sys.path.insert(0, "./")

# imports
from random import shuffle
from ants import *
from functions import *

LAND = -1
FOOD = -2
WATER = -3
CONFLICT = -4
UNSEEN = -5
PLAYERS = ('1', '2', '3', '4')

class BroBot():
    def __init__(self):
      pass
    def do_turn(self, ants):
        """
        print 'dupa'
        self.pprint(ants)
        self.map = MyMap(ants.width, ants.height)
        """
        destinations = []
        for a_row, a_col in ants.my_ants():
            # print ants.setup()
            targets = ants.food() + [(row, col) for (row, col), owner in ants.enemy_ants()]
            # find closest food or enemy ant
            closest_target = None
            closest_distance = 999999

            for t_row, t_col in targets:
                dist = ants.distance(a_row, a_col, t_row, t_col)
                if dist < closest_distance:
                    closest_distance = dist
                    closest_target = (t_row, t_col)
            if closest_target == None:
                # no target found, mark ant as not moving so we don't run into it
                destinations.append((a_row, a_col))
                continue

            directions = ants.direction(a_row, a_col, closest_target[0], closest_target[1])
            shuffle(directions)
            for direction in directions:
                n_row, n_col = ants.destination(a_row, a_col, direction)
                if ants.unoccupied(n_row, n_col) and not (n_row, n_col) in destinations:
                    destinations.append((n_row, n_col))
                    ants.issue_order((a_row, a_col, direction))
                    break
            else:
                # mark ant as not moving so we don't run into it
                destinations.append((a_row, a_col))

    def pprint(self, obj):
      import pprint
      pp = pprint.PrettyPrinter (indent=2)
      pp.pprint(obj)


class MyMap():
  map = []
  def __init__(self, width, height, map):
    self.width = width
    self.height = height
    self.map = map
    self.coverage = 0.0
    self.symetric = 0

class MyArea():
  def __init__(self, x, y, size):
    self.x = x
    self.y = y
    self.size = size
    self.dangerous = False
    self.enemy = False
    self.food = False
    self.unknown = False

class MyAnt():
  actions = []

  def __init__(self, x, y):
    self.x = x
    self.y = y
    self.direction = (0, 0)
    self.target = (x, y)
    self.radius = (0, 0)

  def safe(self, safe):
    self.safe = safe

  def setAction(self, action, priority):
    new_action = MyAction(self, action, priority)
    self.actions = append(new_action)

class MyAction(MyAnt):
  ACTIONS = ('discover', 'attack', 'run', 'stop', 'group')

  def __init__(self, action, safe, priority):
    self.target = (0, 0)
    self.priority = priority
    self.safe = safe

class MyEnemy():
  
  def __init__(self):
    self.aggresive = False
    self.won = False
    self.freeze = False
    self.stupid = False
    self.afraidOf2 = False
    self.afraidOf2 = False
    self.changes = 1.0


## Main Execution Code ##
if __name__ == '__main__':
    argv = sys.argv[1:]
    if len(argv) == 1 and argv[0] == 'run':
      runner.run_game()

    try:
        import psyco
        psyco.full()
    except ImportError:
        pass
    try:
        Ants.run(BroBot())
    except KeyboardInterrupt:
        print('ctrl-c, leaving ...')

