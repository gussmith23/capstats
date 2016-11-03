import requests

url = 'http://localhost:23232/player' # Set destination URL here

post_params = {'telegramId': '1', 'name': 'gus'}
r = requests.post(url, json = post_params)

get_params = {'telegramId': '1'}
r = requests.get(url, params=get_params)
print(r.json())
