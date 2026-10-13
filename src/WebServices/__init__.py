## this version stopped working :( (MS)
## from httplib import HTTP
## from urlparse import urlparse

## def checkURL(url):
##     # checking for existence over HTTP
##     p = urlparse(url)
##     assert p.scheme=='http'
##     h = HTTP(p.netloc)
##     h.putrequest('HEAD', p.path)
##     h.endheaders()

##     if h.getreply()[0] == 200: return True
##     else: return False


import urllib.request, urllib.error, urllib.parse
def checkURL(url):
    try:
        request = urllib.request.Request(url, None, {})
        response = urllib.request.urlopen(request)
        return True
    except:
        return False
