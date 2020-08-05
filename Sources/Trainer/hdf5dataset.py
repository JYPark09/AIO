import h5py
import torch
from torch.utils import data

import random
import numpy as np

class H5Dataset(data.Dataset):
    def __init__(self, file_path):
        super(H5Dataset, self).__init__()

        self.filename = file_path

    def __getitem__(self, index):
        with h5py.File(self.filename, 'r') as f:
            state = f['feature'][index, :, :, :]
            action = f['prob'][index]
            value = f['val'][index]

        if random.randint(0, 1) == 0:
            state = np.swapaxes(state, 1, 2)
            action = np.swapaxes(action.reshape(8, 8), 0, 1).reshape(-1)

        return (torch.from_numpy(state).float(),
                torch.FloatTensor(action),
                torch.FloatTensor(value))

    def __len__(self):
        with h5py.File(self.filename, 'r') as f:
            return f['feature'].shape[0]
