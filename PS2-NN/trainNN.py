# Initalize variables
nInputNodes = 0;
nOutNodes = 0;
nHiddenNodes = 0;

class node:
    edgeInWeights = [];
    edgeOutWeigths = [];


def preProcess():
    initialFile = input("Enter the name of the intial neural network:");
    trainFile = input("Enter the name of the training set:");
    outputFile = input("Enter the name of the output file:");

    fd = open(initialFile);
    lines = fd.readlines();

    nInputNodes = lines[0].split(" ")[0];
    nHiddenNodes = lines[0].split()[1];
    nOutNodes = lines[0].split()[2];

    print("hi")
    # for line in fd.readlines():
    #     # if line

if __name__ == "__main__":
    preProcess();