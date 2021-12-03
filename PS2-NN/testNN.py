import numpy as np
from trainNN import NNetwork, calcNNOutput, initNN, biasValue;

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


def testNN(mNN, testFile, resultFile):

    # [output classes][A,B,C,D]
    counts = [[0]*4 for i in range(mNN.nOutNodes)];    
    
    fd = open(testFile);
    lines = fd.readlines();

    for nLine in range(1, len(lines)):
        tempDatas = np.array(lines[nLine].split(" "));
        tempDatas = tempDatas.astype(np.float);

        # Minus one to account for the bias value
        mNN.inputs = tempDatas[:mNN.nInputNodes-1];
        tempOutputs = tempDatas[mNN.nInputNodes-1:];

        # Adde the bias value
        mNN.inputs = np.insert(mNN.inputs, 0, biasValue);

        # Back Propagation process
        calcNNOutput(mNN);

        # Round Up all the results and calculate all the A B C D value for each class
        for n in range(mNN.nOutNodes):
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

    for n in range(mNN.nOutNodes):
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
    macroAccuracy = sumAccuracy/mNN.nOutNodes;
    macroPrecission = sumPrecision/mNN.nOutNodes;
    macroRecall = sumRecall/mNN.nOutNodes;
    macroF1 = (2*macroPrecission*macroRecall)/(macroPrecission + macroRecall);
    outputLine = mformat(macroAccuracy) + " " + mformat(macroPrecission) + " " + mformat(macroRecall) + " " + mformat(macroF1) + "\n";
    fd.write(outputLine);

        

def preProcess():

    nnFile = input("Please enter the name of the Neural Network file:");
    testFile = input("Please enter the name of the test file:");
    resultFile = input("Please enter the name of the result file:");

    # nnFile = "mgradestrained.txt";
    # testFile = "grades.test.txt";
    # resultFile = "mtestresult.txt";

    return nnFile,testFile,resultFile;
    
if __name__ == "__main__":

    mNN = NNetwork();
    
    nnFile,testFile,resultFile = preProcess();
    initNN(nnFile,mNN);
    testNN(mNN,testFile,resultFile);
