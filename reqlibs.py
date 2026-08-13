import requests as REQ

URL="http://localhost:8000"



while True:
    try:
        resp=REQ.get(URL)
        print(resp.status_code)
        print(resp.text)
    except Exception as e:
        print(f"Execption:{e}")
    import time
    
    
