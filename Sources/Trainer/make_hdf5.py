import sys
import glob

import h5py
import numpy as np

if len(sys.argv) != 2:
    print('usage: python3 {} dir'.format(sys.argv[0]))
    sys.exit()

dir_name = sys.argv[1]

self_data_list = glob.glob(dir_name+'/*')

hf = h5py.File(dir_name+'.hdf5', 'w')

hf.create_dataset('feature', shape=(0, 9, 8, 8), maxshape=(None, 9, 8, 8), dtype=np.int8, compression='lzf')
hf.create_dataset('prob', shape=(0, 64), maxshape=(None, 64), dtype=np.float32, compression='lzf')
hf.create_dataset('val', shape=(0, 1), maxshape=(None, 1), dtype=np.int8, compression='lzf')

for self_data in self_data_list:
    features = []
    probs = []
    values = []

    with open(self_data+'/feat', 'rt') as f:
        for line in f.readlines():
            features.append(np.fromstring(line.strip(), dtype=np.int8, sep=' ').reshape(9, 8, 8))

    with open(self_data+'/prob', 'rt') as f:
        for line in f.readlines():
            prob = np.fromstring(line.strip(), dtype=float, sep=' ')
            prob /= np.sum(prob)

            probs.append(prob)

    with open(self_data+'/val', 'rt') as f:
        for line in f.readlines():
            values.append(np.fromstring(line.strip(), dtype=np.int8, sep=' '))

    orig_length = hf['feature'].shape[0]
    length = len(features)

    hf['feature'].resize(orig_length+length, axis=0)
    hf['prob'].resize(orig_length+length, axis=0)
    hf['val'].resize(orig_length+length, axis=0)

    features = np.array(features)
    probs = np.array(probs)
    values = np.array(values)

    hf['feature'][orig_length:orig_length+length] = features
    hf['prob'][orig_length:orig_length+length] = probs
    hf['val'][orig_length:orig_length+length] = values

hf.close()
