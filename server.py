from flask import Flask, request
import math
import os

app = Flask(__name__)
STORE_PATH = "server_store.bin"

# In bytes.
BLOCK_SIZE = 1040
BUCKET_SIZE = 4
HEIGHT = 5
store_file = None


def init() -> None:
    global store_file
    if not os.path.exists(STORE_PATH):
        store_file = open(STORE_PATH, "w+b")
    
    else:
        store_file = open(STORE_PATH, "r+b")
    

def get_bucket(l: int, index: int) -> bytes:
    bucket_num_bytes = BLOCK_SIZE * BUCKET_SIZE
    level_offset = ((2 ** l)-1) * bucket_num_bytes
    bucket_index_bytes = (bucket_num_bytes * index) + level_offset
    store_file.seek(bucket_index_bytes)
    bucket_data = store_file.read(bucket_num_bytes)
    store_file.seek(0, 0)
    return bucket_data


def write_bucket(l: int, index: int, data: str) -> None:
    bucket_num_bytes = BLOCK_SIZE * BUCKET_SIZE
    level_offset = ((2 ** l)-1) * bucket_num_bytes
    bucket_index_bytes = (bucket_num_bytes * index) + level_offset
    store_file.seek(bucket_index_bytes)
    store_file.write(data)
    store_file.seek(0, 0)
    return


def get_path_from_leaf(leaf_index: int):
    path = []
    index = leaf_index
    while True:
        path.insert(0, index)
        if (index == 0) and (len(path) == HEIGHT):
            break
        elif (index == 0) and (len(path) < HEIGHT):
            for i in range(len(path), HEIGHT):
                path.insert(0, 0)
            break
        index = index // 2
    return path


def print_tree():
    for i in range(0, HEIGHT):
        print(f"\nDepth {i}: ", end='')
        for j in range(0, 2**i):
            print(f"{get_bucket(i, j)} ", end='')


@app.route("/access/<int:leaf>", methods=['POST'])
def access(leaf: int):
    if request.method == 'POST':
        # Get the path from the leaf, tree is stored as array.
        path = get_path_from_leaf(leaf)
        stored_data = []
        for d, i in enumerate(path):
            stored_data.append(get_bucket(d, i))
        data = b"".join(stored_data)
        return data, 200, {'Content-Type': 'application/octet-stream'}


@app.route("/access_write/<int:leaf>", methods=['POST'])
def access_writeback(leaf: int):
    if request.method == 'POST':
        path = get_path_from_leaf(leaf)
        stored_data = request.get_data()
        bucket_bytes_size = BLOCK_SIZE * BUCKET_SIZE
        blocks = []
        block_index = 0
        for i in range(0, HEIGHT):
            blocks.append(stored_data[block_index : bucket_bytes_size * (i+1)])
            block_index = bucket_bytes_size * (i+1)
        
        for d, i in enumerate(path):
            write_bucket(d, i, blocks[d])
        
        store_file.flush()
        return "OK", 200

if __name__ == '__main__':
    init()
    app.run(host="0.0.0.0", port=5000, debug=False)
