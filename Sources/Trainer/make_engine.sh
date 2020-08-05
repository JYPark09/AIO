#! /bin/sh

name=$1

python3 pth2onnx.py ${name}.pth
trtexec --maxBatch=100 --workspace=1024 --onnx=${name}.onnx --saveEngine=${name}.engine
rm ${name}.onnx

