# To change this template, choose Tools | Templates
# and open the template in the editor.

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
