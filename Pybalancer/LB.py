from flask import Flask,redirect,url_for
import requests as REQ
##Load balancer 
from threading import Lock
lock = Lock

SERVERS=list([
    "http://127.0.0.1:5001",
    "http://127.0.0.1:5002"
])
MOVE=0
TOTALSERVER=len(SERVERS)
NEWSERVERS=SERVERS
NEWNUM=NEWSERVERS

ACTIVE=dict()

for nodes in SERVERS:
    ACTIVE[nodes]=0





def LOAD():
    ##Here we will send the load balancers
    # return SIMPLEROUNDROBIN()
    # return HEALTHROUNDROBIN()
    return USERDIST()
    ...




def USERDIST():
    PREV=10*10*10
    TOSEND=None
    with lock():
        global ACTIVE
        for ip,users in ACTIVE.items():
            if (users<PREV):
                    PREV=users
                    TOSEND=ip
    if TOSEND is None:
        return "NULL"
    return TOSEND

        
    
    ...
    
    

def __HEALTHCHECK():
    global MOVE,NEWSERVERS,SERVERS,NEWNUM,ACTIVE
    from time import sleep
    while True:
        NEWLIST=list()
        ##Check wheter the server is alive or not
        print("Health Check Has Started")
        lenofserver=0
        for nodes  in  SERVERS:
            RES=REQ.get(url=nodes)
            print(f"Sending the request to {nodes}")
            if RES.status_code==200:
                NEWLIST.append(nodes) ## I can return here but that will make the the load balcer meaning less
                lenofserver+=1
            else:
                del ACTIVE[nodes]
        ##Now we got the list now time to transer
        with lock():
            
            NEWSERVERS=NEWLIST
            NEWNUM=lenofserver
        sleep(10)

def HEALTHROUNDROBIN():

    global MOVE,NEWSERVERS,NEWNUM
    with lock():
        node= NEWSERVERS[MOVE%NEWNUM]
        MOVE=MOVE+1
    return node
        
def SIMPLEROUNDROBIN():
    """
    Now the advanatege of this is it very simple to make but cannot know wheter the server is 
    working or not
    
    """
    
    global MOVE
    global SERVERS
    node=SERVERS[MOVE%(TOTALSERVER)]
    MOVE=MOVE+1
    return node



from threading import Thread
Thread(target=__HEALTHCHECK,daemon=True).start()

app=Flask(__name__)
@app.route("/",methods=["GET"])
def home():
    return   redirect((LOAD()))

if __name__=="__main__":
    print(SIMPLEROUNDROBIN.__doc__)
    # app.run(port=5000,debug=True)