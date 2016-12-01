import requests
import unittest

player_url = 'http://localhost:23232/player' # Set destination URL here
game_url =  'http://localhost:23232/game'

class Tests(unittest.TestCase):
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
    print(player_url + location)
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
    
  def test_get_player_no_id(self):
    r = requests.get(player_url)
    

 