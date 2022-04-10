# ECE469 - Artificial_Intelligence
 Course work for ECE469-Artificial Intelligence in Cooper Union
## Project 1: Othello AI
### Compile Instruction
I use g++ on Windows OS to compile my program.

The version of my g++ is :
g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0

For efficiency, I coded my program in a single cpp file. Sorry for the messiness of the code. 

Therefore, to compile and run just simply type:
```
g++ -o Othello.exe main.cpp
./Othello.exe
```

### General Strategy
This program used Min-Max Alpha-Beta search for the AI part.

By experimenting, I set the initial search depth to be 7 and continue searching deeper if within the time limit specified by the user.

If the search is not completed the moveChosen by the previous search will be used as the resultant search.

### Evaluation(Heuristics) Used
The score of the evaluation function contains three parts:
- The difference between the number of two symbols
   * This actually is not very important at the beginning of the game. More weight is added to this part towards the end of the game
- The mobility of the two symbols
   * More available moves and less available moves of the opponent are preferred at the early stage of the game
- A weighted value for each position of the board
   * A weighted matrix is used from a paper: https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.108.5111&rep=rep1&type=pdf
   * If the symbol is placed in the position, the corresponding weight will be added.

## Project 2: Neural Network
### Compile Instruction
I used Python 3.9.6 on the Windows system.

To run the train program:
```
$ python trianNN.py
```
To run the test program:
```
$ python testNN.py
```
*The testNN.py Program used some functions from the trainNN.py. Therefore, trainNN.py needs to be in the same folder when run the testNN.py.

### Explanations of My Dataset
This dataset is created by a survey I created in the past. I’ve also included the program to generate the datasets in the folder. For this survey, the user needs to answer 8 questions each has 4 options and the survey will give a resultant personality according to their answers to the questions. Each answer user selected will add points to the resultant personality and the personality with the highest points will be the result.

I used the program I submitted to simulate the tasks. The choice of each question will be pseudo-randomly generated and I randomly split the result into the training set and the testing set.

For the dataset, each input node corresponds to one question and the value is the answer they selected for the answer(0 for option A, 0.33 for option B, 0.66 for option C, 1 for option D). Each output node corresponds to one category of the personality(1 represents belonging to the category and 0 is not belonging to the category).

The actual rule behind this dataset is each answer user selected will add points to the corresponding personality. There are 32 different answers and each adds different points to the result. Therefore, I choose 32 hidden nodes in my neural network, which means the edge from the input to the hidden could be the actual answer user selected and the edge from the hidden nodes can mean the corresponding points each answer adds to the target personality. By experimenting with other numbers like 4, 8 and 16 I also found 32 hidden nodes produce the best result. 

Due to the complexity of the task, I found that the neural network only produces good results with large training data and large epoch numbers. Therefore, to prevent a long time of training, the initial weights I submitted are actually already trained values I get from my pre training. For my pre-training, I generated 8000 results and first used a learning rate of 0.2 to to train 100 epochs. Then I used the resultant weights as the initial weights and trained again with a learning rate of 0.1 and epochs of 100. The final trained weights after this pre-train process are my submitted initial weights.


The parameters I used to produce my trained network with initial weights is 0.05 for learning rate and 20 epoch(epoch is small for reasonable training time and I found large training sets improve the result better than more epochs). Based on these parameters, I found the network produces reasonable results.

### Notes on Files

createDatasets.py - A modified my past program to produce the my dataset

surveyInit.txt - the initial weights file
surveyTrain.txt - the train data I used to train the network
surveyTest.txt - the test data I used to train the network

surveyTrained.05.20.txt - the trained weights with my neural network program using 0.05 as the learning rate and 20 epochs
surveyResult.05.20.txt - the test result I get from my trained weights 


