#!/usr/bin/env python

import json
import os

class BroDebug():
  debug = False
  log = []
  log_json = []
  
  def __init__(self, ops):
    if ops['debug']:
      self.debug = ops['debug']
      self.debug = True
      self.debug_file = ops['debug_file']
      self.debug_file_json = os.path.join(ops["replay_dir"], ops["debug_file_json"]) if ops["replay_dir"] else ops["debug_file_json"]

  def msg(self, msg):
    if self.debug:
      self.log.append(msg)

  def var(self, name, value):
    if self.debug:
      self.log.append({name: repr(value)})

  def obj(self, obj):
    if self.debug:
      self.log.append(repr(obj))

  def json(self, name, value):
    if self.debug:
      self.log_json[:] = {name: value}

  def end(self):
    if self.debug and self.debug_file:
      of = open(self.debug_file, 'w')
      self.log.append('')
      of.write('\n' . join(self.log))
      of.close()
    if self.debug and self.debug_file_json:
      json_data = json.dumps(self.log_json)
      of = open(self.debug_file_json, 'w')
      of.write(json_data)
      of.close()
    
if __name__ == "__main__":
    print "Hello World"
