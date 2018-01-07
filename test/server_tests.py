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

# TODO(gus): this is really dumb. this stuff can probably be passed by CMake.
server_path = ""
if (os.name == "nt"): server_path = r'..\build\Debug\capstats_server.exe'
elif (os.name == "posix"): server_path = r"../build/capstats_server" #todo: what should this be?
  
class AuthenticatedTests(unittest.TestCase):
  key = "6f601a72-e9b7-11e7-80c1-9a214cf093ae"
  
  def setUp(self):
    self.p = subprocess.Popen([server_path, "--key", AuthenticatedTests.key])
    time.sleep(.01)
    
  def tearDown(self):
    self.p.terminate()
    self.p.wait()
    self.p = None

  def test_add_get_update_game_authenticated(self):
    post_params = {
      'teams': {
        '1' : [1,2,3],
        '2' : [4,5,6]
      },
      'points': {
        '1' : 11,
        '2' : 9
      },
      'playerpoints': {
        '1' : 1,
        '5' : 5
      }
    }

    params = {
      'key' : AuthenticatedTests.key
    }

    r = requests.post(game_url, json = post_params, params = params)
    json = r.json()
    
    self.assertTrue(json['id'] >= 0)
    
    for key in post_params.keys():
      self.assertEqual(post_params[key], json[key])

    r = requests.get(game_url + "/" + str(json['id']))
    json = r.json()
    for key in post_params.keys():
      self.assertEqual(post_params[key], json[key])
    
    post_params['teams']['3'] = [7,8,9]
    post_params['points']['3'] = 6
    post_params['playerpoints']['3'] = 3
    
    r = requests.put(game_url + "/" + str(json['id']), json = post_params, params = params)

    r = requests.get(game_url + "/" + str(json['id']))
    json = r.json()
    for key in post_params.keys():
      self.assertEqual(post_params[key], json[key])

  def test_not_authenticated(self):
    post_params = {
      'teams': {
        '1' : [1,2,3],
        '2' : [4,5,6]
      },
      'points': {
        '1' : 11,
        '2' : 9
      }
    }

    params = {
      'key' : "this is not a valid key"
    }

    r = requests.post(game_url, json = post_params, params = params)
    self.assertEqual(r.status_code, 403)
  

# These tests are performed on a server with authentication disabled.
# TODO(gus): evaluate whether it makes sense to continue doing this. The only
# reason I'm doing it now is because authentication was not implemented at
# first. Thus, to make these tests work, we'd have to add a key (which is easy;
# just use the test config file) and we'd have to then add that key to every
# PUT or POST request. It wouldn't be that much work.
class UnauthenticatedTests(unittest.TestCase):  
    
  # comment these out when you want to debug the server. start the server (using 
  # visual studio for example) and then run the tests.
  # some tests will only work with this stuff uncommented OR if you restart the
  # server each time you run the tests.
  def setUp(self):
    self.p = subprocess.Popen([server_path, "--disableAPIKeys"])
    time.sleep(.01)
    
  def tearDown(self):
    self.p.terminate()
    self.p.wait()
    self.p = None

  def test_add_get_update_game(self):
    post_params = {
      'teams': {
        '1' : [1,2,3],
        '2' : [4,5,6]
      },
      'points': {
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
    json = r.json()
    for key in post_params.keys():
      self.assertEqual(post_params[key], json[key])
    
    post_params['teams']['3'] = [7,8,9]
    post_params['points']['3'] = 6
    
    r = requests.put(game_url + "/" + str(json['id']), json = post_params)

    r = requests.get(game_url + "/" + str(json['id']))
    json = r.json()
    for key in post_params.keys():
      self.assertEqual(post_params[key], json[key])
    
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
    json = r.json();
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
 
  def test_find_game(self):
    r = requests.get(game_url)
    json = r.json()
    self.assertEqual(len(json), 0)

    post_params = {
      'teams': {
        '1' : [1,2,3],
        '2' : [4,5,6]
      },
      'points': {
        '1' : 11,
        '2' : 9
      }
    }
    r = requests.post(game_url, json = post_params)

    post_params['teams']['3'] = [7,8]
    post_params['points']['3'] = [2]
    r = requests.post(game_url, json = post_params)

    r = requests.get(game_url)
    json = r.json()

    self.assertEqual(len(json), 2)

    
  def test_find_game_with_players(self):
    post_params = {
      'teams': {
        '1' : [1,2,3],
        '2' : [4,5,6]
      },
      'points': {
        '1' : 11,
        '2' : 9
      }
    }
    r = requests.post(game_url, json = post_params)

    post_params['teams']['3'] = [7,8]
    post_params['points']['3'] = [2]
    r = requests.post(game_url, json = post_params)

    r = requests.get(game_url, params={'players': 1})
    json = r.json()

    self.assertEqual(len(json), 2)

    r = requests.get(game_url, params={'players': 7})
    json = r.json()

    self.assertEqual(len(json), 1)

    r = requests.get(game_url, params={'players': 10})
    json = r.json()

    self.assertEqual(len(json), 0)

if __name__ == '__main__':
    unittest.main()
