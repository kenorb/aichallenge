
import pygame
from pygame.locals import *

import string
import pprint

LAND = -1
FOOD = -2
WATER = -3
CONFLICT = -4
UNSEEN = -5
PLAYER1 = 0
PLAYER2 = 1
PLAYER3 = 2
PLAYER4 = 3

PLAYER_CHARS = string.ascii_lowercase
MAP_RENDER = PLAYER_CHARS + '?!%*.'

AIM = {'n': (-1, 0),
       'e': (0, 1),
       's': (1, 0),
       'w': (0, -1)}

BSIZE = 10 # block size
MSIZE = 100 # menu size
MSTATUS = 40 # menu size

class Visualizer():

    #           -1        1           2            3            4        Start    Finish    Find path         Reset
    ant = pygame.color.Color('black')
    land = pygame.color.Color('lightgray')
    food = pygame.color.Color('white')
    water = pygame.color.Color('blue')
    red = pygame.color.Color('red')
    darkgray = pygame.color.Color('darkgray')
    colors = {
      PLAYER1: 'yellow',
      PLAYER2: 'blue',
      PLAYER3: 'darkgreen',
      PLAYER4: 'magenta',
      LAND: 'lightgray',
      FOOD: 'white',
      WATER: 'black',
      CONFLICT: 'red',
      UNSEEN: 'darkgray',
    }

    def __init__(self, game):

        self.state = game.get_state_changes() # Return a list of all transient objects on the map. Food, living ants, ants killed this turn
        self.state= game.get_state()
        # self.pstate = game.get_player_state()
        self.scores = game.get_scores()
        self.no_of_ants = game.get_stats()

        self.screen_init(game)

    def screen_init(self, game):
        pygame.init()
        SCREENX = game.width*BSIZE+MSIZE
        self.screenx = game.width*BSIZE+MSIZE+MSTATUS
        self.screeny = game.height*BSIZE+MSTATUS
        self.screen = pygame.display.set_mode((self.screenx, self.screeny), HWSURFACE)
        pygame.display.set_caption('Ants Visualiser')

        self.screen.fill((150,150,150))

        self.initMap(game.width, game.height)
        self.drawMap(game)
        self.editmode = 0

        self.drawMenu()

    def pprint(self, obj):
        pp = pprint.PrettyPrinter (indent=2)
        pp.pprint(obj)

    def initMap(self,w,h):
        self.mapdata = []
        self.mapw = w
        self.maph = h
        self.startpoint = [1,1]
        self.endpoint = [w-2,h-2]

        size = w*h;
        for i in range(size):
            self.mapdata.append(1)

        self.mapdata[(self.startpoint[1]*w)+self.startpoint[0]] = 5
        self.mapdata[(self.endpoint[1]*w)+self.endpoint[0]] = 6

        self.maprect = Rect(0,0,w*BSIZE,h*BSIZE)

    def drawMap(self, game):

        x = 0
        y = 0
        rect = [0,0,BSIZE,BSIZE]
        # print game.map
        fnt = pygame.font.Font(pygame.font.get_default_font(),8)
        for row, squares in enumerate(game.map):
          for col, square in enumerate(squares):
            rect[0] = col*BSIZE
            rect[1] = row*BSIZE
            value = game.map[row][col]
            color = self.colors[value]
            self.screen.fill(pygame.color.Color(color),rect)
            # ts=fnt.render('a', 1, (255,255,255))

    def updateMap(self, game):
        self.drawMenu();
        self.drawMap(game);
        pygame.display.flip()
        self.checkEvents();

    def drawMenu(self):

        text = ["Blocking (-1)", "Walkable (1)", "Walkable (2)", "Walkable (3)", "Walkable (4)", "Start point", "End point", "Find path", "Reset"]

        fnt = pygame.font.Font(pygame.font.get_default_font(),11)

        self.menurect = Rect(self.screenx-MSIZE-MSTATUS/2,5,MSIZE,32*len(self.colors))

        rect = Rect(self.screenx-MSIZE-MSTATUS/2,5,MSIZE,32)

        i = 0
        for c in text:
            self.screen.fill(pygame.color.Color('black'),rect)
            ts=fnt.render(text[i], 1, (255,255,255))
            trect = ts.get_rect()
            trect.center = rect.center
            self.screen.blit(ts,trect.topleft)
            rect.y+=32+1;
            i+=1

    def checkEvents(self):

      for event in pygame.event.get():
          if event.type == QUIT:
              exit()
          elif event.type == KEYDOWN:
              if event.key == K_ESCAPE:
                  return

          elif event.type == MOUSEBUTTONDOWN:
              mx = event.pos[0]
              my = event.pos[1]
              """
              if len(self.pathlines):
                  self.pathlines=[]
                  self.drawMap()
              if self.maprect.collidepoint(mx,my):
                  self.updateMap(mx/16,my/16,self.editmode)
              elif self.menurect.collidepoint(mx,my):
                  my-=self.menurect.y
                  em = my/32
                  if em == 7: #trace
                      self.findPath()
                      if len(self.pathlines):
                          pygame.draw.lines(self.screen, (255,255,255,255), 0, self.pathlines)
                  elif em == 8: #reset
                      self.initMap(34,30)
                      self.drawMap()
                  else:
                      self.editmode = em
                """

          elif event.type == MOUSEMOTION and event.buttons[0]:
              mx = event.pos[0]
              my = event.pos[1]
              """
              if self.maprect.collidepoint(mx,my):
                  if len(self.pathlines):
                      self.pathlines=[]
                      self.drawMap()
                  self.updateMap(mx/16,my/16,self.editmode)
              """
