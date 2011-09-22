#!/usr/bin/env python

from random import shuffle

# my imports
from ants import *
from debug import *

class AI:

  def __init__(self, parent):
    self.parent = parent
    self.visited = [] #keep track of visited row/cols
    
  def hunt_ants(self,ants,destinations,a_row,a_col):
      self.parent.debug.msg("Start Finding Ant")
      closest_enemy_ant = ants.closest_enemy_ant(a_row,a_col)
      self.parent.debug.msg("Done Finding Ant")
      moved =False
      if closest_enemy_ant!=None:
          self.parent.debug.msg("chasing ant:start")
          directions = ants.direction(a_row,a_col,closest_enemy_ant[0],closest_enemy_ant[1])
          self.parent.debug.msg("chasing ant:directions:%s" % "".join(directions))
          shuffle(directions)
          for direction in directions:
              self.parent.debug.msg("chasing ant:direction:%s" % direction)
              (n_row,n_col) = ants.destination(a_row,a_col,direction)
              if (not (n_row,n_col) in destinations and
                  ants.unoccupied(n_row,n_col)):
                  moved=True
                  ants.issue_order((a_row,a_col,direction))
                  self.parent.debug.msg("issue_order:%s,%d,%d,%s" % ("chasing ant",a_row,a_col,direction))
                  destinations.append((n_row,n_col))
                  break
      return moved

  def hunt_food(self,ants,destinations,a_row,a_col):
      self.parent.debug.msg("Start Finding Food")
      closest_food = ants.closest_food(a_row,a_col)
      self.parent.debug.msg("Done Finding Food")
      moved = False
      if closest_food!=None:
          self.parent.debug.msg("chasing food:start")
          directions = ants.direction(a_row,a_col,closest_food[0],closest_food[1])
          self.parent.debug.msg("chasing food:directions:%s" % "".join(directions))
          shuffle(directions)
          for direction in directions:
              self.parent.debug.msg("chasing food:direction:%s;" % direction)
              (n_row,n_col) = ants.destination(a_row,a_col,direction)
              if (not (n_row,n_col) in destinations and
                  ants.unoccupied(n_row,n_col)):
                  moved=True
                  ants.issue_order((a_row,a_col,direction))
                  self.parent.debug.msg("issue_order:%s,%d,%d,%s" % ("chasing food",a_row,a_col,direction))
                  destinations.append((n_row,n_col))
                  break
      return moved

  def random_move(self,ants,destinations,a_row,a_col,do_visited=False):
      #if we didn't move as there was no food try a random move
      moved=False
      directions = AIM.keys()
      self.parent.debug.msg("random move:directions:%s" % "".join(directions))
      shuffle(directions)
      self.parent.debug.msg("random move:shuffled directions:%s" % "".join(directions))
      for direction in directions:
          self.parent.debug.msg("random move:direction:%s" % direction)
          (n_row, n_col) = ants.destination(a_row, a_col, direction)
          if (not (n_row, n_col) in destinations and
              ants.unoccupied(n_row, n_col)):
              if (n_row,n_col) not in self.visited or (do_visited and (n_row,n_col) in self.visited):
                  moved=True
                  ants.issue_order((a_row, a_col, direction))
                  self.parent.debug.msg("issue_order:%s,%d,%d,%s" % ("random move",a_row,a_col,direction))
                  destinations.append((n_row, n_col))
                  break
              else:
                  continue
      return moved
