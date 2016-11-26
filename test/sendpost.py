import requests

url = 'http://localhost:23232/game' # Set destination URL here

post_params = {'teams': { 1 : [1,2,3], 2: [4,5,6] } }
r = requests.post(url, json = post_params)

