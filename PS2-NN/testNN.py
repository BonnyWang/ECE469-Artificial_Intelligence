import numpy as np
from numpy.core.records import recarray;
from trainNN import NNetwork, node, sigmoid;


mNN = NNetwork();

# Initalize variables
nInputNodes = 0;
nOutNodes = 0;
nHiddenNodes = 0;

nnFile = "";
testFile = "";
resultFile = "";

biasValue = -1;

def roundUp(output):
    if output >= 0.5:
        return 1;
    elif output < 0.5:
        return 0;

def decideABCD(expected, predicted):
    if expected == 1 and predicted == 1:
        # A
        return 0;
    if expected == 0 and predicted == 1:
        # B
        return 1;
    if expected == 1 and predicted == 0:
        # C
        return 2;
    if expected == 0 and predicted == 0:
        # D
        return 3;

def mformat(number):
    return format(np.round(number,3),'.3f');

def calcNNOutput(inputs):

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
    
    return mNN;

def testNN():
    global mNN;

    # [output classes][A,B,C,D]
    counts = [[0]*4 for i in range(nOutNodes)];    
    
    fd = open(testFile);
    lines = fd.readlines();

    for nLine in range(1, len(lines)):
        tempDatas = np.array(lines[nLine].split(" "));
        tempDatas = tempDatas.astype(np.float);

        # Minus one to account for the bias value
        tempInputs = tempDatas[:nInputNodes-1];
        tempOutputs = tempDatas[nInputNodes-1:];

        # Adde the bias value
        tempInputs = np.insert(tempInputs, 0, biasValue);

        # Back Propagation process
        calcNNOutput(tempInputs);

        # Round Up all the results and calculate all the A B C D value for each class
        for n in range(nOutNodes):
            mNN.outNodes[n].value = roundUp(mNN.outNodes[n].value);
            case = decideABCD(tempOutputs[n],mNN.outNodes[n].value);
            counts[n][case] += 1;
    
    fd.close();
    
    # calculate the resultant values and output to the output file

    fd = open(resultFile,"w");

    globalA = 0;
    globalB = 0;
    globalC = 0;
    globalD = 0;

    sumAccuracy = 0;
    sumPrecision = 0;
    sumRecall = 0;
    sumF1 = 0;

    for n in range(nOutNodes):
        A = counts[n][0];
        B = counts[n][1];
        C = counts[n][2];
        D = counts[n][3];

        globalA += A;
        globalB += B;
        globalC += C;
        globalD += D;

        OverallAccuracy = (A + D)/( A + B + C + D);
        Precision = A/(A + B);
        Recall = A/(A + C);
        F1 = (2*Precision*Recall)/(Precision + Recall);

        sumAccuracy += OverallAccuracy;
        sumPrecision += Precision;
        sumRecall += Recall;
        sumF1 += F1;

        outputLine = str(A) + " " + str(B) +" "+ str(C) +" "+ str(D) + " ";

        outputLine += mformat(OverallAccuracy) + " " + mformat(Precision) + " " + mformat(Recall) + " " + mformat(F1) + "\n";
        fd.write(outputLine);
    
    # Calc for micro averaging
    microAccuracy = (globalA + globalD)/( globalA + globalB + globalC + globalD);
    microPrecission = globalA/(globalA + globalB);
    microRecall = globalA/(globalA + globalC);
    microF1 = (2*microPrecission*microRecall)/(microPrecission + microRecall);

    outputLine = mformat(microAccuracy) + " " + mformat(microPrecission) + " " + mformat(microRecall) + " " + mformat(microF1) + "\n";
    fd.write(outputLine);

    # Calc for macro averaging
    macroAccuracy = sumAccuracy/nOutNodes;
    macroPrecission = sumPrecision/nOutNodes;
    macroRecall = sumRecall/nOutNodes;
    macroF1 = (2*macroPrecission*macroRecall)/(macroPrecission + macroRecall);
    outputLine = mformat(macroAccuracy) + " " + mformat(macroPrecission) + " " + mformat(macroRecall) + " " + mformat(macroF1) + "\n";
    fd.write(outputLine);

        

def preProcess():
    global nnFile, testFile, resultFile;
    global nInputNodes,nHiddenNodes,nOutNodes;
    global mNN;

    nnFile = input("Please enter the name of the Neural Network file:");
    testFile = input("Please enter the name of the test file:");
    resultFile = input("Please enter the name of the result file:");

    # nnFile = "mgraderesult.txt";
    # testFile = "grades.test.txt";
    # resultFile = "mtestresult.txt";

    fd = open(nnFile);
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

    
if __name__ == "__main__":
    preProcess();
    testNN();
