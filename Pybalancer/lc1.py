from flask import Flask


LB1=Flask(__name__)


@LB1.route("/",methods=["GET"])
def home():
    return {"hello":"Hi,From the first server"}


# Because we are testing in this i think it will be better to use random for the gralth
@LB1.route("/health",methods=["GET"])
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
    LB1.run(port=5001,debug=True)