import numpy as np;

# Initalize variables
nInputNodes = 0;
nOutNodes = 0;
nHiddenNodes = 0;


class node:
    edgeInWeights = [];

class NNetwork:
    inNodes = [];
    hiddenNodes = [];
    outNodes = [];
  
mNN = NNetwork();

def preProcess():
    global nInputNodes,nHiddenNodes,nOutNodes;
    global mNN;

    initialFile = input("Enter the name of the intial neural network:");
    trainFile = input("Enter the name of the training set:");
    outputFile = input("Enter the name of the output file:");

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

    return;

        

if __name__ == "__main__":
    preProcess();