import os
import urllib.parse
import urllib.request, urllib.error, urllib.parse

from binaries import findExecModule

def recursive_download(url=None, local_dir=None):
    urldir = False

    if url == "" or local_dir == "":
        import sys
        print("ERROR: input URL or Directory empty")
        sys.exit()

    wget = findExecModule('wget')
            
    if url.endswith('/'):
        urldir = True
    else:
        urldir = True 
        tmp_url = url + '/'
             
        try:
            resp = urllib.request.urlopen(tmp_url)
            resp.close()
        except urllib.error.URLError as e:
            urldir = False

    sep = ";"

    if urldir == True:
        cutdirs = urllib.parse.urlparse(url).path.count('/')
            
        if url.endswith("/"):
            cutdirs = cutdirs - 1
                
        wget = wget + " -np -nH -r --cut-dirs=" + str(cutdirs)
    else:
        wget = wget + " "

    # TBD, add sep
        
    cmd = "cd " + local_dir + sep + " " + wget + " " + url
    os.system(cmd)


