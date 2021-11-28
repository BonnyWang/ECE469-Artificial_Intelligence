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

    # for mNode in mNN.hiddenNodes:
    #     mNode.value = weight*inputValue for 

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
    nHiddenNodes = int(lines[0].split()[1]);
    nOutNodes = int(lines[0].split()[2]);

    for nLine in range(1, nHiddenNodes+1):
        tempHiddenNode = node();
        tempWeights = np.array(lines[nLine].split(" "));

        tempHiddenNode.edgeInWeights = tempWeights.astype(np.float);
        mNN.hiddenNodes.append(tempHiddenNode);
    
    for nLine in range(nHiddenNodes+1, len(lines)):
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