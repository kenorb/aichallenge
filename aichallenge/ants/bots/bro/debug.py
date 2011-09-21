#!/usr/bin/env python

import os
import sys

class BroDebug():
  debug = False
  log = []
  
  def __init__(self, opts):
    if opts.debug:
      self.debug = opts.debug
      self.debug_stderr = opts.debug_stderr
      self.debug_file = opts.debug_file

  def msg(self, msg, ant = None):
    if self.debug:
      if ant:
        _add_ant(msg, ant)
      self.log.append(msg)
    if self.debug and self.debug_stderr:
      sys.stderr.write(msg)
      sys.stderr.write('\n')

  def var(self, name, value, ant = None):
    if self.debug:
      if ant:
        _add_ant(msg, ant)
      self.log.append({name: repr(value)})
    if self.debug and self.debug_stderr:
      sys.stderr.write({name: repr(value)})
      sys.stderr.write('\n')

  def obj(self, obj, obj2 = None, obj3 = None):
    if self.debug:
      self.log.append(repr(obj))
    if self.debug and self.debug_stderr:
      sys.stderr.write(repr(obj))
      sys.stderr.write('\n')

  def end(self):
    self.msg(__name__ + '.end()')
    if self.debug and self.debug_file:
      self.msg('Creating file: ' + self.debug_file)
      of = open(self.debug_file, 'w+')
      self.log.append('')
      self.msg('Writing log into: ' + self.debug_file)
      of.write('\n' . join(self.log) + '\n')
      of.close()
      self.msg('Closing file.')

  def test(self):
    self.end()
    self.msg('Self-testing finished.\n')
    sys.exit(0);

  def _add_ant(self, msg, ant):
    if self.debug:
      return "ant:%d[%dx%d]: " % (ant.id, ant.x, ant.y) + msg

if __name__ == "__main__":
    print "Hello World"
