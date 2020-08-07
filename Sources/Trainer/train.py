import torch
from torch import nn, optim
import torch.utils.data as torch_data
from torch.autograd import Variable

import datetime
import time
import argparse
import glob
import os
import numpy as np

from network import ResNet
from hdf5dataset import H5Dataset

def train(network: ResNet, optimizer: optim.Optimizer, data_loader: torch_data.DataLoader, device: torch.device) -> tuple:
    running_loss = 0
    running_pi_loss = 0
    running_value_loss = 0

    mse_loss = nn.MSELoss()

    batch_len = len(data_loader)

    for batch_idx, (state, pi, value) in enumerate(data_loader, 1):
        state = Variable(state).to(device)
        pi = Variable(pi).to(device)
        value = Variable(value).to(device)

        optimizer.zero_grad()

        pred_pi, pred_value = network(state)

        pi_loss = torch.sum(-pi * (1e-8 + pred_pi), dim=1).mean()
        v_loss = mse_loss(pred_value, value)

        loss = pi_loss + v_loss
        loss.backward()

        optimizer.step()

        running_loss += loss.item()
        running_pi_loss += pi_loss.item()
        running_value_loss += v_loss.item()

        if batch_idx % 10 == 9:
            print('[iteration {}/{}] loss: {:.4f}, pi: {:.4f}, v: {:.4f}'.format(
                batch_idx, batch_len, running_loss/batch_idx, running_pi_loss/batch_idx, running_value_loss/batch_idx/2))

    data_len = len(data_loader)
    running_loss /= data_len
    running_pi_loss /= data_len
    running_value_loss /= data_len

    return running_loss, running_pi_loss, running_value_loss/2

def main():
    parser = argparse.ArgumentParser(description='AIO trainer')
    parser.add_argument('--lr', type=float, default=1e-1)
    parser.add_argument('--batch_size', type=int, default=32)
    parser.add_argument('--num_workers', type=int, default=4)
    parser.add_argument('--epochs', type=int, default=5)
    parser.add_argument('--dataset_path', type=str, required=True)
    parser.add_argument('--model_file', type=str)
    args = parser.parse_args()

    torch.manual_seed(42)
    torch.cuda.manual_seed_all(42)
    np.random.seed(42)

    print('[Train dataset]')
    dataset_files = glob.glob(args.dataset_path+'/*.hdf5')
    for f in dataset_files:
        print('>> ' + f)

    hdfdatasets = [H5Dataset(f) for f in dataset_files]
    dataset = torch_data.ConcatDataset(hdfdatasets)
    data_loader = torch_data.DataLoader(dataset, batch_size=args.batch_size, num_workers=args.num_workers, shuffle=True, pin_memory=True)

    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

    network = ResNet(9, 128, 5, 8**2).to(device)
    optimizer = optim.SGD(network.parameters(), lr=args.lr, momentum=.9, weight_decay=1e-4, nesterov=True)

    init_epoch = 0
    if args.model_file:
        if os.path.exists(args.model_file):
            state = torch.load(args.model_file)
            init_epoch = state['epoch']
            network.load_state_dict(state['state_dict'])

            optimizer.load_state_dict(state['optimizer'])

            print('{} loaded'.format(args.model_file))
            del state

    for epoch in range(init_epoch+1, args.epochs+1):
        start_time = time.time()
        loss, pi_loss, v_loss = train(network, optimizer, data_loader, device)

        print('epoch {}/{} - loss: {:.4f}, pi: {:.4f}, v: {:.4f}'.format(epoch, args.epochs, loss, pi_loss, v_loss))
        end_time = time.time()
        print('{} elapsed'.format(end_time - start_time))

        now = datetime.datetime.now()
        pth_file_name = 'pth/{}{:02d}{:02d}_{:02d}_ckpt_{}.pth'.format(now.year, now.month, now.day, now.hour, epoch)
        state = {
            'state_dict': network.state_dict(),
            'epoch': epoch,
            'optimizer': optimizer.state_dict(),
            'loss': (loss, pi_loss, v_loss)
        }
        torch.save(state, pth_file_name)

if __name__ == '__main__':
    main()
