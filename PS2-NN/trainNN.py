import numpy as np;
import math;

biasValue = -1;

class node:
    edgeInWeights = [];
    value = 0;
    error = 0;

class NNetwork:
    inputs = [];
    hiddenNodes = [];
    outNodes = [];

    nInputNodes = 0;
    nHiddenNodes = 0;
    nOutNodes = 0;
  

def sigmoid(x):
    return 1/(1 + np.exp(-x));

# Updates all the weights based on the errors
def updateWeights(mNN, learningRate):

    # Update the weight from the hidden nodes to the output nodes
    for mNode in mNN.outNodes:
        for w in range(len(mNode.edgeInWeights)):
            mNode.edgeInWeights[w] = mNode.edgeInWeights[w] + learningRate*mNN.hiddenNodes[w].value*mNode.error;

    # Update the weight from the input nodes to the hidden nodes
    for mNode in mNN.hiddenNodes:
        # Skip the bias node
        if mNode == mNN.hiddenNodes[0]:
            continue;
        for w in range(len(mNode.edgeInWeights)):
            mNode.edgeInWeights[w] = mNode.edgeInWeights[w] + learningRate*mNN.inputs[w]*mNode.error;
    
    

# Calculate the error of each node(back propagate)
def calcErrors(expected, mNN):
   
    # Calculate the error in the output layer
    for i in range(mNN.nOutNodes):
        mNode = mNN.outNodes[i];
        mNode.error = (expected[i] - mNode.value)*mNode.value*(1-mNode.value);  

    # Calculate the error in the hidden layer
    for i in range(mNN.nHiddenNodes):
        sum = 0;
        mNode = mNN.hiddenNodes[i];
        for j in range(mNN.nOutNodes):
            tempOutNode = mNN.outNodes[j]
            sum += tempOutNode.error*tempOutNode.edgeInWeights[i];
        
        mNode.error = sum*mNode.value*(1-mNode.value);
    

# Calculate the forward values of each node
def calcNNOutput(mNN):

    # Calculate the values of the Hidden Nodes
    for mNode in mNN.hiddenNodes:
        sum = 0;

        # Account for the bias node
        if(mNode == mNN.hiddenNodes[0]):
            mNode.value = biasValue;
            continue;
        
        for nEdge in range(0,mNN.nInputNodes):
            sum += mNN.inputs[nEdge]*mNode.edgeInWeights[nEdge];
        mNode.value =  sigmoid(sum);

    # Calculate teh values of each Output value
    for mNode in mNN.outNodes:
        sum = 0;
        for nEdge in range(0,mNN.nHiddenNodes):
            sum += mNN.hiddenNodes[nEdge].value*mNode.edgeInWeights[nEdge];
        
        mNode.value = sigmoid(sum);
    
def initNN(initialFile, mNN):
    # Open initial File and set values to the NNs;
    fd = open(initialFile);
    lines = fd.readlines();

    mNN.nInputNodes = int(lines[0].split(" ")[0])+1;
    # Plus one to add the bias node
    mNN.nHiddenNodes = int(lines[0].split()[1])+1;
    mNN.nOutNodes = int(lines[0].split()[2]);

    # Add the hidden bias node to the NN
    mHiddenBiasNode = node();
    mHiddenBiasNode.edgeInWeights = np.zeros(mNN.nInputNodes);  
    mNN.hiddenNodes.append(mHiddenBiasNode);

    for nLine in range(1, mNN.nHiddenNodes):
        tempHiddenNode = node();
        tempWeights = np.array(lines[nLine].split(" "));

        tempHiddenNode.edgeInWeights = tempWeights.astype(np.float);
        mNN.hiddenNodes.append(tempHiddenNode);
    
    for nLine in range(mNN.nHiddenNodes, len(lines)):
        tempOutNode = node();
        tempWeights = np.array(lines[nLine].split(" "));

        tempOutNode.edgeInWeights = tempWeights.astype(np.float);
        mNN.outNodes.append(tempOutNode);

    fd.close();
    
# Get the file names and initilize the network
def preProcess():
    # initialFile = input("Enter the name of the intial neural network:");
    initialFile = "sample.NNGrades.init.txt";
    # trainFile = input("Enter the name of the training set:");
    trainFile = "grades.train.txt";
    outputFile = input("Enter the name of the output file:");
    learningRate = float(input("Enter the learning rate:"));
    # learningRate = 0.05;
    epoch = int(input("Enter the epoch:"));
    # epoch = 100;

    return initialFile,trainFile,outputFile,learningRate, epoch;

    
def trainNetWork(trainFile, epoch, learningRate, mNN):

    fd = open(trainFile);

    lines = fd.readlines();

    for e in range(epoch):
        for nLine in range(1, len(lines)):
            tempDatas = np.array(lines[nLine].split(" "));
            tempDatas = tempDatas.astype(np.float);

            # Minus one to account for the bias value
            mNN.inputs = tempDatas[:mNN.nInputNodes-1];
            expectedOutput = tempDatas[mNN.nInputNodes-1:];

            # Adde the bias value
            mNN.inputs = np.insert(mNN.inputs, 0, biasValue);

            # Back Propagation process
            calcNNOutput(mNN);
            calcErrors(expectedOutput, mNN);
            updateWeights(mNN, learningRate);
    
    fd.close();

def outputNetwork(mNN):
    fd = open(outputFile, "w");

    firstLine = str(mNN.nInputNodes-1) + " " + str(mNN.nHiddenNodes-1) + " " + str(mNN.nOutNodes);
    fd.write(firstLine + "\n");

    for mNode in mNN.hiddenNodes:

        # Skip the bias node in the hidden nodes
        if mNode == mNN.hiddenNodes[0]:
            continue;
        
        weightsLine = "";
        
        for weight in mNode.edgeInWeights:
            weightsLine += format(np.round(weight,3),'.3f') + " ";

        weightsLine = weightsLine.strip();
        fd.write(weightsLine + "\n");

    for mNode in mNN.outNodes:

        weightsLine = "";
        
        for weight in mNode.edgeInWeights:
            weightsLine += format(np.round(weight,3),'.3f') + " ";

        weightsLine = weightsLine.strip();
        fd.write(weightsLine + "\n");


if __name__ == "__main__":
    
    mNN = NNetwork();
    
    initialFile,trainFile,outputFile,learningRate, epoch = preProcess();

    initNN(initialFile, mNN);
    trainNetWork(trainFile, epoch, learningRate, mNN);
    outputNetwork(mNN);