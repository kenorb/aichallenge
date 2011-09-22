#!/usr/bin/env python

import os
import sys

class Debug():
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
        msg = self._add_ant(msg, ant)
      else:
        msg = self._add_log_prefix(msg, 'BroBot')

    self.log.append(msg)
    if self.debug and self.debug_stderr:
      sys.stderr.write(msg)
      sys.stderr.write('\n')

  def var(self, name, value, ant = None):
    msg = repr(value)
    self.msg(msg, ant)

  def obj(self, obj, prefix = '', obj2 = None, obj3 = None):
    if self.debug:
      self.log.append(prefix + repr(obj))
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
      of.write(('\n' . join(self.log)) + '\n')
      of.close()
      self.msg('Closing file.')

  def test(self):
    self.end()
    self.msg('Self-testing finished.\n')
    sys.exit(0);

  def _add_ant(self, msg, ant):
    return "ant:%d[%dx%d]: " % (ant.id, ant.x, ant.y) + msg

  def _add_log_prefix(self, msg, prefix):
    return "%s: " % (prefix) + msg

if __name__ == "__main__":
    print "Hello World"
