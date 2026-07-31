from flask import Flask


LB1=Flask(__name__)


@LB1.route("/",methods=["GET"])
def home():
    return "Hi,From the first server"



if __name__=="__main__":
    LB1.run(port=5001,debug=True)