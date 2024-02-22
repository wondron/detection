@echo off

..\..\bin\trtexec.exe --onnx=fbfClassifier.onnx --saveEngine=fbfClassifier.engine --explicitBatch --workspace=4096 --verbose


:end
PAUSE