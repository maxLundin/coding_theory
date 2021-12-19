import os.path
from os import listdir
import subprocess
import shutil


def encode(command, from_directory, to_directory):
    for file in listdir(from_directory):
        name = file.split('.')[0]
        tmp_command = command % (f'{from_directory}/{file}', f'{to_directory}/{name}.encode')
        subprocess.run(tmp_command.split(), stdout=subprocess.PIPE)
        start_size = os.path.getsize(f'{from_directory}/{file}')
        compressed_size = os.path.getsize(f'{to_directory}/{name}.encode')

        print(f'{file}: {start_size} {compressed_size}')


def decode(command, from_directory, to_directory):
    for file in listdir(from_directory):
        name = file.split('.')[0]
        tmp_command = command % (f'{from_directory}/{file}', f'{to_directory}/{name}.decode.jpg')
        subprocess.run(tmp_command.split(), stdout=subprocess.PIPE)


def diff(from_directory, to_directory):
    for file in listdir(from_directory):
        name = file.split('.')[0]
        tmp_command = f'diff {from_directory}/{file} {to_directory}/{name}.decode.jpg'
        result = subprocess.run(tmp_command.split(), stdout=subprocess.PIPE)
        if result.stdout:
            print(f'files {from_directory}/{file} and {to_directory}/{name}.decode.jpg are different')


def get_dir_size(dir):
    size = 0
    for file in listdir(dir):
        file_size = os.path.getsize(f'{dir}/{file}')
        size += file_size
    return size


def call_all(encode_command, decode_command, jpeg_dir, tmp_dir, res_dir):
    if not os.path.exists(tmp_dir):
        os.makedirs(tmp_dir)
    if not os.path.exists(res_dir):
        os.makedirs(res_dir)

    encode(encode_command, jpeg_dir, tmp_dir)
    decode(decode_command, tmp_dir, res_dir)
    diff(jpeg_dir, res_dir)
    jpeg_size = get_dir_size(jpeg_dir)
    encode_size = get_dir_size(tmp_dir)
    print(f'jpeg size = {jpeg_size}; encode size = {encode_size}; '
          f'compression = {(1. - encode_size * 1.0 / jpeg_size) * 100}%')


if __name__ == '__main__':
    jpeg30_dir = 'dataset/jpeg30'
    jpeg80_dir = 'dataset/jpeg80'
    tmp30_dir = 'dataset/tmp30'
    tmp80_dir = 'dataset/tmp80'
    res30_dir = 'dataset/res30'
    res80_dir = 'dataset/res80'
    encode_command = './max_coder e %s %s'
    decode_command = './max_coder d %s %s'

    print('processing jpeg30')
    call_all(encode_command, decode_command, jpeg30_dir, tmp30_dir, res30_dir)

    print('processing jpeg80')
    call_all(encode_command, decode_command, jpeg80_dir, tmp80_dir, res80_dir)

    shutil.rmtree(tmp30_dir)
    shutil.rmtree(tmp80_dir)

    shutil.rmtree(res30_dir)
    shutil.rmtree(res80_dir)