import numpy as np;
import math;

# Initalize variables
nInputNodes = 0;
nOutNodes = 0;
nHiddenNodes = 0;

initialFile = "";
trainFile = "";
outputFile = "";
learningRate = 0.1;
epoch = 0;

biasValue = -1;


class node:
    edgeInWeights = [];
    value = 0;
    error = 0;

class NNetwork:
    # inNodes = [];
    hiddenNodes = [];
    outNodes = [];
  
mNN = NNetwork();

def sigmoid(x):
    return 1/(1 + np.exp(-x));


# Calculate the error of each node(back propagate)
def calcErrors(outputs):
    # Calculate the error in the output layer
    for mNode in mNN.outNodes:
        print("hhi");



# Calculate the forward values of each node
def calcNNOutput(inputs):

    # Add the bias value at the front of the array
    inputs = np.insert(inputs, 0, biasValue);

    # Calculate the values of the Hidden Nodes
    for mNode in mNN.hiddenNodes:
        sum = 0;

        # Account for the bias node
        if(mNode == mNN.hiddenNodes[0]):
            mNode.value = biasValue;
            continue;
        
        for nEdge in range(0,nInputNodes):
            sum += inputs[nEdge]*mNode.edgeInWeights[nEdge];
        mNode.value =  sigmoid(sum);

    # Calculate teh values of each Output value
    for mNode in mNN.outNodes:
        sum = 0;
        for nEdge in range(0,nHiddenNodes):
            sum += mNN.hiddenNodes[nEdge].value*mNode.edgeInWeights[nEdge];
        
        mNode.value = sigmoid(sum);
    
    print("calcOutputs Finished");

    
# Get the file names and initilize the network
def preProcess():
    global nInputNodes,nHiddenNodes,nOutNodes;
    global initialFile, trainFile, outputFile;
    global learningRate, epoch;
    global mNN;

    # initialFile = input("Enter the name of the intial neural network:");
    initialFile = "sample.NNWDBC.init.txt";
    # trainFile = input("Enter the name of the training set:");
    trainFile = "wdbc.train.txt";
    outputFile = input("Enter the name of the output file:");
    # learningRate = input("Enter the learning rate:");
    # epoch = input("Enter the epoch:");

    # Open initial File and set values to the NNs;
    fd = open(initialFile);
    lines = fd.readlines();

    nInputNodes = int(lines[0].split(" ")[0])+1;
    # Plus one to add the bias node
    nHiddenNodes = int(lines[0].split()[1])+1;
    nOutNodes = int(lines[0].split()[2]);

    # Add the hidden bias node to the NN
    mHiddenBiasNode = node();
    mHiddenBiasNode.edgeInWeights = np.zeros(nInputNodes);  
    mNN.hiddenNodes.append(mHiddenBiasNode);

    for nLine in range(1, nHiddenNodes):
        tempHiddenNode = node();
        tempWeights = np.array(lines[nLine].split(" "));

        tempHiddenNode.edgeInWeights = tempWeights.astype(np.float);
        mNN.hiddenNodes.append(tempHiddenNode);
    
    for nLine in range(nHiddenNodes, len(lines)):
        tempOutNode = node();
        tempWeights = np.array(lines[nLine].split(" "));

        tempOutNode.edgeInWeights = tempWeights.astype(np.float);
        mNN.outNodes.append(tempOutNode);

    fd.close();

def trainNetWork():

    fd = open(trainFile);

    lines = fd.readlines();

    for e in range(epoch):
        for nLine in range(1, len(lines)):
            tempDatas = np.array(lines[nLine].split(" "));
            tempDatas = tempDatas.astype(np.float);

            # Minus one to account for the bias value
            tempInputs = tempDatas[:nInputNodes-1];
            tempOutput = tempDatas[nInputNodes-1:];

            # Forward propagate to calculate the output value
            calcNNOutput(tempInputs);
  

if __name__ == "__main__":
    preProcess();
    trainNetWork();