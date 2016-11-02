from urllib.parse import urlencode
from urllib.request import Request, urlopen

url = 'http://localhost:23232/resource' # Set destination URL here
post_fields = {'foo': 'bar',
               'Content-Length' : 1}     # Set POST fields here

request = Request(url, urlencode(post_fields).encode())
json = urlopen(request).read().decode()
print(json)
