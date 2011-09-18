#!/usr/bin/env python

import os

class BroDebug():
  debug = False
  log = []
  
  def __init__(self, opts):
    if opts.debug:
      self.debug = opts.debug
      self.debug = True
      self.debug_file = opts.debug_file

  def msg(self, msg, ant = None):
    if self.debug:
      if ant:
        _add_ant(msg, ant)
      self.log.append(msg)

  def var(self, name, value, ant = None):
    if self.debug:
      if ant:
        _add_ant(msg, ant)
      self.log.append({name: repr(value)})

  def obj(self, obj, obj2 = None, obj3 = None):
    if self.debug:
      self.log.append(repr(obj))

  def end(self):
    if self.debug and self.debug_file:
      of = open(self.debug_file, 'w')
      self.log.append('')
      of.write('\n' . join(self.log))
      of.close()


  def _add_ant(self, msg, ant):
    if self.debug:
      return "ant:%d[%dx%d]: " % (ant.id, ant.x, ant.y) + msg

if __name__ == "__main__":
    print "Hello World"
