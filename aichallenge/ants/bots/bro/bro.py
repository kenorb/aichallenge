#!/usr/bin/env python

# temp
# from runner import *
# sys.path.insert(0, "./")

# debug
from debug import *

# libraries
import sys
from random import shuffle
from optparse import OptionParser

# imports
from ants import *
from astar.astar import AStar

# my imports
from map import *
from log import *

turn_number = 0
bot_version = 'v0.1'

LAND = -1
FOOD = -2
WATER = -3
CONFLICT = -4
UNSEEN = -5
PLAYERS = ('1', '2', '3', '4')

class BroBot():
    def __init__(self):
        """Add our log filter so that botversion and turn number are output correctly"""
        log_filter  = LogFilter()
        getLogger().addFilter(log_filter)

        getLogger().debug("START!")

        self.debug = BroDebug(opts)
        self.debug.msg('START')
        self.debug.obj(opts)
        self.map = None
       
    def do_turn(self, ants):
        # self.debug.obj(ants.map_data)
        self.debug.obj(vars(ants))
        # print repr(ants.map_data)
        self.debug.var('width', ants.width)
        self.debug.var('height', ants.height)
        self.debug.msg('turn 0')

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

    def end_game(self, ants, err):
      self.debug.obj(err)
      self.debug.msg('END')
      self.debug.end()

class MyAnt():
  actions = []

  def __init__(self, x, y, id = 0):
    self.id = id
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

def BroStartupOptions(parser):
  parser.add_option("-r", "--replay-dir", dest="replay_dir", default='.',
                    help="Directory where replay files are stored.")
  parser.add_option("-d", "--debug", dest="debug",
                     action="store_true", default=False,
                     help="Run in debug mode.")
  parser.add_option("-D", "--debug-file", dest="debug_file", default='./debug.log',
                     help="Specify custom debug filename.")
#  parser.add_option("-o", "--debug-json", dest="debug_file_json", default='debug.json.log',
#                    help="Create debug json file in replay directory.")
  parser.add_option("-s", "--strategy", dest="strategy",
                    help="Choose strategy (five two digit numbers).")
  return parser

def BroPathfinding():
  pathmap = []
  def find_path(self, ant, map):
    pathmap = map_convert(ant, map)
    a = AStar(self.pathmap)
    for i in a.step():
        pass
    path = a.path
    start = a.source
    end = a.target
    if path:
        # print 'Route length:', len(path)
        self.debug.msg('Found route to: %s, length: %d' % (end, len(path)), ant)
    else:
        self.debug.msg('Failed to find the path to: %s' % (end), ant)

  def map_convert(self, ant, map):
    self.pathmap = map
        
## Main Execution Code ##
if __name__ == '__main__':
    argv = sys.argv[1:]
    # parse bot options
    parser = OptionParser()
    (opts, args) = BroStartupOptions(parser).parse_args(argv)

    # (opts, args) = ParseStartupOptions()
    # (opts, args) = parser.parse_args(argv)

    """
    ops = {
        "replay_dir": opts.replay_dir,
        "debug": opts.debug,
        "debug_file": opts.debug_file,
        "debug_file_json": opts.debug_file_json,
        "strategy": opts.strategy,
    }"""
    

    # if len(argv) == 1 and argv[0] == 'run':
      # runner.run_game()

    try:
        # import psyco
        # psyco.full()
        pass
    except ImportError:
        pass
    try:
        Ants.run(BroBot())
    except KeyboardInterrupt:
        print('ctrl-c, leaving ...')
        System.exit(-1);

