import h5py
import torch
from torch.utils import data

class H5Dataset(data.Dataset):
    def __init__(self, file_path):
        super(H5Dataset, self).__init__()

        self.filename = file_path

    def __getitem__(self, index):
        with h5py.File(self.filename, 'r') as f:
            state = f['feature'][index, :, :, :]
            action = f['prob'][index]
            value = f['val'][index]

        return (torch.from_numpy(state).float(),
                torch.FloatTensor(action),
                torch.FloatTensor(value))

    def __len__(self):
        with h5py.File(self.filename, 'r') as f:
            return f['feature'].shape[0]
