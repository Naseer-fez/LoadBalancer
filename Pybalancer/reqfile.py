##Automation script to send countious requests

import requests as req
import time 

URL="http://127.0.0.1:5000/"

def sendreq():
    while True:
        try:
            Resp=req.get(
                url=URL
            ) 
            print("The data is:")
            print(f"The Status :{Resp.status_code}")
            print(f"The message is : {Resp.text}")
            
        except Exception as e:
            print("Waitt") 
        time.sleep(2)
        
