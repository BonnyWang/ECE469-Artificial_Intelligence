import numpy as np;

# Initalize variables
nInputNodes = 0;
nOutNodes = 0;
nHiddenNodes = 0;

initialFile = "";
trainFile = "";
outputFile = "";

biasValue = -1;


class node:
    edgeInWeights = [];
    value = 0;

class NNetwork:
    inNodes = [];
    hiddenNodes = [];
    outNodes = [];
  
mNN = NNetwork();


def backPropagation():
    print("Start back");

def calcNNOutput(inputs):
    output = [];

    # Add the bias value at the front of the array
    inputs.insert(0,biasValue);

    # Calculate the values of the Hidden Nodes
    for mNode in mNN.hiddenNodes:
        sum = 0;
        for nEdge in range(0,nInputNodes):
            sum += inputs[nEdge]*mNode.edgeWeights[nEdge];
        mNode.value = sum;

    # Calculate teh values of each Output value
    for mNode in mNN.outNodes:
        sum = 0;
        for nEdge in range(0,nHiddenNodes):
            sum += mNN.hiddenNodes[nEdge].value*mNode.edgeWeights[nEdge];
        
        mNode.value = sum;
    



# Get the file names and initilize the network
def preProcess():
    global nInputNodes,nHiddenNodes,nOutNodes;
    global initialFile, trainFile, outputFile;
    global mNN;

    initialFile = input("Enter the name of the intial neural network:");
    trainFile = input("Enter the name of the training set:");
    outputFile = input("Enter the name of the output file:");

    # Open initial File and set values to the NNs;
    fd = open(initialFile);
    lines = fd.readlines();

    nInputNodes = int(lines[0].split(" ")[0]);
    # Plus one to add the bias node
    nHiddenNodes = int(lines[0].split()[1])+1;
    nOutNodes = int(lines[0].split()[2]);

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

    for nLine in range(1, len(lines)):
        tempDatas = np.array(lines[nLine].split(" "));
        tempDatas = tempDatas.astype(np.float);

        tempInput = tempDatas[:nInputNodes];

        resultOutput = calcNNOutput(tempInput);



        

if __name__ == "__main__":
    preProcess();
    trainNetWork();