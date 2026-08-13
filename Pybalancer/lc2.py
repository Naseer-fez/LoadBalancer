from flask import Flask


LB2=Flask(__name__)


@LB2.route("/",methods=["GET"])
def home():
    return "Hi,From the second  server"


@LB2.route("/health",methods=["GET"])
def health():
    import random
    from flask import jsonify
    CPU=random.uniform(70,100)
    CPUCORES=random.randint(4,8)
    TOTALRAM=random.randint(8,16)
    RAMUSAGE=random.uniform(40,80)
    
    DATA={
  "CPU": CPU,
  "CPUCORES": CPUCORES,
  "TOTALRAM": TOTALRAM,
  "RAMUSAGE": RAMUSAGE
}
    return jsonify(DATA)



if __name__=="__main__":
    LB2.run(port=5002,debug=True)