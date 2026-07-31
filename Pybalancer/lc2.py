from flask import Flask


LB2=Flask(__name__)


@LB2.route("/",methods=["GET"])
def home():
    return "Hi,From the second  server"



if __name__=="__main__":
    LB2.run(port=5002,debug=True)