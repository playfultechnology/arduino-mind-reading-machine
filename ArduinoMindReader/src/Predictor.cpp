#include "Predictor.h"
#include "Arduino.h"

Predictor::Predictor(){};
Predictor::Predictor(uint8_t memoryLength, PredictorType predictorType, DataType dataType){
	m_memoryLength = memoryLength;  //history length to look at
	m_predictorType = predictorType;
  m_dataType = dataType;          // REGULAR_DATA_SERIES means to operate on the direct input, FLIPPING_DATA_SERIES means to operate on the flipping series
  m_predictionsHistory = {};	
};

uint8_t Predictor::getPastAccuracy(int8_t* userMoves, uint8_t numTurns) {
	uint8_t accuracy = 0;
  for(int i=0; i<numTurns; i++) {
		accuracy += abs(userMoves[i] - m_predictionsHistory[i]);
  }
	return accuracy;	
};
int8_t Predictor::makePrediction(int8_t* userMoves, int8_t* userMovesFlipping, int8_t* wins, uint8_t numTurns){
	int8_t prediction = 0;

	if(m_predictorType == PredictorType::BIAS && m_dataType == DataType::REGULAR_DATA_SERIES) {
		int cnt = 0;
    int8_t historyMean = 0;
    while( cnt<m_memoryLength && (numTurns - cnt)>0 ) {
			historyMean += userMoves[numTurns - cnt - 1];
			cnt +=1;
		}
		historyMean /= cnt;
		return historyMean;
	}
	else if(m_predictorType == PredictorType::BIAS && m_dataType == FLIPPING_DATA_SERIES) {
		int cnt = 0;
    int8_t historyMean = 0;
    while( cnt<m_memoryLength && (numTurns - cnt)>0 ) {
			historyMean += userMovesFlipping[numTurns - cnt - 1];
			cnt +=1;
		}
		historyMean /= cnt;
		return historyMean * userMoves[numTurns-1] * -1; // flip or not the last user move;	
	}
	else if(m_predictorType == PredictorType::PATTERN && m_dataType == DataType::REGULAR_DATA_SERIES) {
		
		if(numTurns < m_memoryLength) {
			return 0;
		}
		
		// Retrieve the most recent entries from the array of data
		uint8_t pattern[m_memoryLength];
		for(int i=0; i<m_memoryLength; i++){
			pattern[i] = userMoves[numTurns - m_memoryLength - 1 + i];
		}			
		
		uint8_t prediction = pattern[0];
		
		uint8_t score = 0;
		int i = numTurns - m_memoryLength - 1;
		
		while(i >= max(0, numTurns-3*m_memoryLength)) {
			if(pattern[m_memoryLength-1] == userMoves[i]){
				score++;
			}
			// Take the last element off the end of the array
			uint8_t temp = pattern[m_memoryLength-1];
			// Shift all the elements back one
			for(int i=m_memoryLength-1; i>0; i--){
				pattern[i] = pattern[i-1];
			}
			// And then put the last element back on the beginning
			pattern[0] = temp;
			
			i--;
		}
		// The maximum score is achieved when the pattern repeats itself twice
		score /= (2*m_memoryLength); 
		return prediction * score;
	}
	else if(m_predictorType == PredictorType::PATTERN && m_dataType == FLIPPING_DATA_SERIES) {  //calculate the mean of the last memoryLength moves

		if(numTurns < m_memoryLength) {
			return 0;
		}
		
		// Retrieve the most recent entries from the array of data
		uint8_t pattern[m_memoryLength];
		for(int i=0; i<m_memoryLength; i++){
			pattern[i] = userMovesFlipping[numTurns - m_memoryLength - 1 + i];
		}			
		
		uint8_t prediction = pattern[0];
		
		uint8_t score = 0;
		int i = numTurns - m_memoryLength - 1;
		
		while(i >= max(0, numTurns-3*m_memoryLength)) {
			if(pattern[m_memoryLength-1] == userMovesFlipping[i]){
				score++;
			}
			// Take the last element off the end of the array
			uint8_t temp = pattern[m_memoryLength-1];
			// Shift all the elements back one
			for(int i=m_memoryLength-1; i>0; i--){
				pattern[i] = pattern[i-1];
			}
			// And then put the last element back on the beginning
			pattern[0] = temp;
			
			i--;
		}
		// The maximum score is achieved when the pattern repeats itself twice
		score /= (2*m_memoryLength); 
		return prediction * score * userMoves[numTurns-1] * -1; // flip or not the last user move;	
	}
  else if(m_predictorType == PredictorType::REACTIVE && m_dataType == USER_REACTIVE) {  //calculate the mean of the last memoryLength moves
		;//prediction = this.childPredictor(userMovesFlipping, wins) * userMoves[userMoves.length-1] * -1; // flip or not the last user move
		/*
		constructor(memoryLength, dataType) {
        super(memoryLength, dataType);
        this.stateMachine = new Array(Math.pow(2, 2*memoryLength-1)).fill(0);
        this.indMap = [];
        for (var i=2*memoryLength; i>=0; i--) {
          this.indMap.push(Math.pow(2, i));
        }
    }
    childPredictor(moves, wins) {
        // figure out what was the last state
        var partOfMoves = moves.slice(moves.length - this.memoryLength    , moves.length - 1);
        var partOfWins  = wins.slice (wins.length  - this.memoryLength - 1 , wins.length - 1);
        var lastState = partOfWins.concat(partOfMoves);
        var lastStateInd = 0
        for (var i=0; i<lastState.length; i++) {
            if (lastState[i]==1) {
                lastStateInd += Math.pow(2,i);
            }
        }
        var lastStateResult = moves[moves.length-1];

        //update the state machine
        if (this.stateMachine[lastStateInd] == 0) { //no prior info
            this.stateMachine[lastStateInd] = lastStateResult*0.3;
        }
        else if (this.stateMachine[lastStateInd] == lastStateResult*0.3) {  //we've been here before so strengthen prediction
            this.stateMachine[lastStateInd] = lastStateResult*0.8;
        }
        else if (this.stateMachine[lastStateInd] == lastStateResult*0.8) { //we've been here before so strengthen prediction
            this.stateMachine[lastStateInd] = lastStateResult*1;
        }
        else if (this.stateMachine[lastStateInd] == lastStateResult*1) { //maximum confidence
            this.stateMachine[lastStateInd] = lastStateResult*1;
        }
        else {  //changed his mind - so go back to 0
            this.stateMachine[lastStateInd] = 0;
        }

        // what is the current state
        var currentPartOfMoves = moves.slice(moves.length - this.memoryLength + 1    , moves.length);
        var currentPartOfWins  = wins.slice (wins.length  - this.memoryLength , wins.length);
        var currentState = currentPartOfWins.concat(currentPartOfMoves);
        var currentStateInd = 0
        for (var i=0; i<currentState.length; i++) {
            if (currentState[i]==1) {
                currentStateInd += Math.pow(2,i);
            }
        }

        var predictionAndScore = this.stateMachine[currentStateInd]

      //  console.log('last state ', lastState, ', last ind ', lastStateInd, '    current state ', currentState, '  current ind ', currentStateInd, '    state machine: ', this.stateMachine);
        return predictionAndScore;
		*/
		
	}
	else if (m_predictorType == PredictorType::REACTIVE && m_dataType == USER_REACTIVE_REG_DATA) {  //calculate the mean of the last memoryLength moves
		;//prediction = this.childPredictor(userMoves, wins);
	}
/*
	if (isNaN(prediction)) {
		prediction = 0;
	}
*/
	m_predictionsHistory[numTurns] = prediction;
	return prediction;		
};
		