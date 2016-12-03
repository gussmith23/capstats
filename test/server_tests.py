import requests
import unittest
import subprocess
import threading
from multiprocessing import Process
import time
import os
import signal


player_url = 'http://localhost:23232/player'
game_url =  'http://localhost:23232/game'

server_path = ""
if (os.name == "nt"): server_path = r'..\build\Debug\capstats_server.exe'
elif (os.name == "posix"): server_path = "" #todo: what should this be?

def startServer():
  subprocess.call([server_path])
    
  
class Tests(unittest.TestCase):  
    
  def setUp(self):
    self.p = subprocess.Popen(server_path)
    
  def tearDown(self):
    os.kill(self.p.pid, signal.SIGTERM) 

  def test_add_get_game(self):
    post_params = {
      'teams': {
        '1' : [1,2,3],
        '2' : [4,5,6]
      },
      'scores': {
        '1' : 11,
        '2' : 9
      }
    }
    r = requests.post(game_url, json = post_params)
    json = r.json()
    
    self.assertTrue(json['id'] >= 0)
    
    for key in post_params.keys():
      self.assertEqual(post_params[key], json[key])

    r = requests.get(game_url + "/" + str(json['id']))
    
  def test_add_get_update_player(self):
    post_params = {
      'telegramId' : 1,
      'name' : 'gus'
    }
    
    r = requests.post(player_url, json = post_params)
    json = r.json()
    
    self.assertEqual(r.status_code, 201)
    location = r.headers['Location']
    id = int(r.headers['Location'].split('/')[1])
    self.assertTrue(id >= 0)
    
    for key in post_params.keys():
      self.assertEqual(post_params[key], json[key])

    r = requests.get(player_url + location)
    self.assertEqual(r.status_code, 200)
    json = r.json()
    
    for key in post_params.keys():
      self.assertEqual(post_params[key], json[key])
    
    post_params['name'] = 'henry'
    r = requests.put(player_url + location, json = post_params)
    r = requests.get(player_url + location)
    json = r.json()
    
    for key in post_params.keys():
      self.assertEqual(post_params[key], json[key])
    
  def test_get_unknown_player(self):
    r = requests.get(player_url + "/42069")
    self.assertEqual(r.status_code, 404)
    
  # tests uri /player WITHOUT giving an id. tests /player, /player?param=val,
  # etc.
  def test_get_player_no_id(self):    
    players =[ {
      'name' : 'gus',
      'telegramId' : 1
    },{
      'name' : 'henry',
      'telegramId' : 2
    },{
      'name' : 'gus',
      'telegramId' : 3
    }]
    
    for player in players: requests.post(player_url, json=player)
    
    r = requests.get(player_url)
    self.assertTrue(len(r.json()) == 3)

    r = requests.get(player_url, params={'name':'gus'})
    json = r.json()
    self.assertTrue(len(json) == 2)
    
    r = requests.get(player_url, params={'name':'henry'})
    json = r.json()
    self.assertTrue(len(json) == 1)
    
    r = requests.get(player_url, params={'name':'gus', 'telegramId' : 3})
    json = r.json()
    self.assertTrue(len(json) == 1)
    
    r = requests.get(player_url, params={'name':'gus', 'telegramId' : 2})
    json = r.json()
    self.assertTrue(len(json) == 0)

    

 