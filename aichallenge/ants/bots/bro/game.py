#!/usr/bin/env python

__author__="kenorb"
__date__ ="$Apr 12, 2011 9:05:28 PM$"

import sys
import random

from engine import run_game
# engine import
sys.path.insert(0, "../..")
from ants import Ants

# custom import
from bro import *
from visualizer import *

def run_rounds():
    # this split of options is not needed, but left for documentation
    turns = 10
    rounds = 1
    game_options = {
        "map": '../../maps/random4.txt',
        "attack": 'damage',
        "food": 'sections',
        "viewradius2": 96,
        "attackradius2": 5,
        "spawnradius2": 2,
        "loadtime": 3000,
        "turntime": 1000,
        "turns": rounds,
        "seed": 42,
        }
    engine_options = {
        "loadtime": 3000,
        "turntime": 1000,
        "map_file": '../../maps/random4.txt',
        "turns": turns,
        "output_dir": '../../viewer/bro_viewer',
        "output_json": False,
        "log_input": False,
        "log_output": False,
        "serial": None,
        "verbose": True,
        "visualizer": opts.visualizer,
        }
    bots_files = (
        "python ../../dist/sample_bots/python/HunterBot.py",
        "python ../../dist/sample_bots/python/keth.py",
        "python ../../bots/bro/bro.py",
        "python ../../dist/sample_bots/python/HunterBot.py"
        )
    random.seed(game_options['seed'])
    for round in range(game_options['turns']):
        map_file = open(game_options['map'], 'r')
        game_options["map"] = map_file.read()
        map_file.close()
        game = Ants(game_options)
        bots = [('.', arg) for arg in bots_files]
        if game.num_players != len(bots):
            print("Incorrect number of bots for map.  Need %s, got %s" %
                  (game.num_players, len(bots)))
            for arg in args:
                print("Bot Cmd: %s" % arg)
            break
        print('playgame round %s' % round)
        result = run_game(game, bots, engine_options, round)
        if engine_options['output_json']:
            print result

def GameStartupOptions(parser):
    parser.add_option("-v", "--visualizer", dest="visualizer",
                      action="store_true", default=False,
                      help="Run in visualizer (pygame required).")
    return parser

if __name__ == "__main__":
    argv = sys.argv[1:]
    parser = OptionParser()
    parser = BroStartupOptions(parser) # parse bot options
    parser = GameStartupOptions(parser) # parse game options
    (opts, args) = parser.parse_args(argv)

    sys.exit(run_rounds())
