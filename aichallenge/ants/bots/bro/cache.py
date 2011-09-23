#!/usr/bin/env python

class Cache():

  def __init__(self):
    enabled = True
    cache = []

  def setCache(self, key, data):
    if self.enabled:
      self.cache[key] = data

  def getCache(self, key, data, default):
    if self.enabled:
      return self.cache
