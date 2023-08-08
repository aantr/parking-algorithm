from flask import Flask
from flask import request
from flask_cors import CORS, cross_origin

import pybind11
import pathlib
import json
import os
import sys
from subprocess import Popen, PIPE

dir = str(pathlib.Path(__file__).parent.resolve())

saved_args = sys.argv
sys.argv = ['setup.py', 'build_ext', '-i']
exec(open(f'{dir}/parking_lib/setup.py').read());
sys.argv = saved_args


command = [f'{sys.executable}', f'{dir}/parking_lib/setup.py', 'build_ext', '-i']
output, err = '', ''
print(command)
# p = Popen(command, cwd=f'{dir}/parking_lib',
#     stdin=PIPE, stdout=PIPE, stderr=PIPE)
# output, err = p  .communicate(b'')
print(output)
print(err)
try:
    import parking_lib.parking_lib as parking_lib
except Exception as e:
    ...


app = Flask(__name__)
CORS(app, support_credentials=True)

@app.route('/api/calc', methods = ['GET'])
@cross_origin(supports_credentials=True)
def calc():
    try:
        w = request.args.get('w', default = 1, type = int)
        h = request.args.get('h', default = 1, type = int)
        data = request.args.get('data', default = '', type = str)
        key = request.args.get('key', default = '', type = str).split(',')
        if 'actions' not in key and 'clients' not in key:
            return json.dumps({"data": {}})
        n = len(data.split(';'))
        if not data:
            n = 0
        out = parking_lib.solve(w, h, [list(map(int, i.split(','))) for i in data.split(';')]).strip()
        out = out.split('\n')
        N = int(out[0])
        clients = []
        actions = []
        res = {}

        for i in range(N):
            row = list(map(int, out[i + 1].split()))
            clients.append({'id': row[0], 'row': row[1], 'column': row[2]})
        M = int(out[N + 1])
        for i in range(M):
            row = list(map(int, out[N + 2 + i].split()))
            actions.append({'id': row[0], 'row': row[1], 'column': row[2], 'time': row[3], 'type': row[4]})

        if 'clients' in key:
            res['clients'] = clients
        if 'actions' in key:
            res['actions'] = actions
        

        return json.dumps({"data": res})
    except Exception as e:
        return f'{command}\n{output}\n{err}\n{str(e)}'
