##Automation script to start all the servers

def startlb1():
    from lc1 import LB1
    LB1.run(port=5001, use_reloader=False)
def startlb2():
    from lc2 import LB2
    LB2.run(port=5002, use_reloader=False)
def startLB():
    import LB
    LB.app.run(port=5000, use_reloader=False)

##LEts automate the request file also
def startreq():
    from reqfile import sendreq
    sendreq()

if __name__ =="__main__":
    from multiprocessing import Process
    p1=Process(target=startlb1)
    p2=Process(target=startlb2)
    p3=Process(target=startLB)
    p4=Process(target=startreq)
    ##Start the process
    p1.start()
    p2.start()
    p3.start()
    p4.start()
    #Join the processes
    p1.join()
    p2.join()
    p3.join()
    p4.join()




