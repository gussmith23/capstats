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

    r = requests.get(game_url + "/" + str(json['id']), headers={'Accepts':'application/json'})
    print(r.json())
    