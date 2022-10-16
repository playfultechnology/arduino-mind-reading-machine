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
		return historyMean;	
	}
	else if(m_predictorType == PredictorType::PATTERN && m_dataType == DataType::REGULAR_DATA_SERIES) {
/*
        var pattern, prediction, score, ind;

        if (data.length < this.memoryLength) {
          return 0;
        }

        function rotatePattern() {
					// removes the last element from an array and returns that element.
            var temp = pattern.pop();
						// adds new elements to the beginning of an array
            pattern.unshift(temp);
        }

        //extract history length
				// returns selected elements in an array, as a new array. The slice() method selects from a given start, up to a (not inclusive) given end
        pattern = data.slice(-this.memoryLength);
        prediction = pattern[0];  //the prediction is simply the element in the pattern (i.e. the first in this extracted array)


        score = 0;
        ind = data.length-this.memoryLength-1;  //start right before the pattern
        while (ind>=Math.max(0, data.length-3*this.memoryLength)) { //check maximum 2 appearances of the full pattern
            if (pattern[pattern.length-1]==data[ind]) {
              score++;
            }
            rotatePattern()
            ind--;
        }
        score /= (2*this.memoryLength);  //the maximum score is achieved when the pattern repeats itself twice
        return prediction * score;
				*/
	}
	else if(m_predictorType == PredictorType::PATTERN && m_dataType == FLIPPING_DATA_SERIES) {  //calculate the mean of the last memoryLength moves
		;//prediction = this.childPredictor(userMovesFlipping) * userMoves[userMoves.length-1] * -1; // flip or not the last user move
	}
  else if(m_predictorType == PredictorType::REACTIVE && m_dataType == USER_REACTIVE) {  //calculate the mean of the last memoryLength moves
		;//prediction = this.childPredictor(userMovesFlipping, wins) * userMoves[userMoves.length-1] * -1; // flip or not the last user move
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
		