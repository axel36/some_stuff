from flask import Flask
app = Flask(__name__)


@app.route('/')
def hello():
    return "Hello, stranger, walk your road"

if __name__ == '__main__':
    app.run()