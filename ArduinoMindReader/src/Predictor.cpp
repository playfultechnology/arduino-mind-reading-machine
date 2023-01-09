#include "Predictor.h"
#include "Arduino.h"

Predictor::Predictor(){};
Predictor::Predictor(uint8_t memoryLength, PredictorType predictorType, DataType dataType){
	m_memoryLength = memoryLength;  //history length to look at
	m_predictorType = predictorType;
	m_dataType = dataType;          // REGULAR_DATA_SERIES means to operate on the input value, FLIPPING_DATA_SERIES means to operate on the changes in input value
	m_predictionsHistory = {};	
	
	// Following is only used for REACTIVE predictor type - should probably be moved to subclass	
	int numStates = pow(2, 2*m_memoryLength-1);
	stateMachine = new uint8_t[numStates];
	memset(stateMachine, 0, numStates);
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
		// Calculate the player's "average" move over the number of previous turns specified
		// i.e. Have they chosen left or right more frequently
		int8_t aggregateMove = 0;
		int cnt=0;
		while(cnt<m_memoryLength && (numTurns - cnt) > 0) {
			aggregateMove += userMoves[numTurns-1-cnt];
			cnt++;
		}
		prediction = aggregateMove / cnt;
	}
	else if(m_predictorType == PredictorType::BIAS && m_dataType == FLIPPING_DATA_SERIES) {
		// Calculate the player's "average" decision to flip based on the number of previous turns specified
		int8_t aggregateMove = 0;
		int cnt=0;
		while(cnt<m_memoryLength && (numTurns - cnt)>0) {
			aggregateMove += userMovesFlipping[numTurns - cnt - 1];
			cnt++;
		}
		// On average, has been more likely to flip or not? Apply that to the previous input
		prediction = (aggregateMove / cnt) * userMoves[numTurns-1];	
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
		// Prediction is simply the first element in the extracted array
		uint8_t initial_prediction = pattern[0];
		uint8_t score = 0;
		
		for(int i = numTurns - m_memoryLength - 1; i >= max(0, numTurns-3*m_memoryLength); i--) {
			if(pattern[m_memoryLength-1] == userMoves[i]){
				score++;
			}
			// Take the last element off the end of the array
			uint8_t temp = pattern[m_memoryLength-1];
			// Shift all the elements back one
			for(int j=m_memoryLength-1; j>0; j--){
				pattern[j] = pattern[j-1];
			}
			// And then put the last element back on the beginning
			pattern[0] = temp;
		}
		// The maximum score is achieved when the pattern repeats itself twice
		score /= (2*m_memoryLength);
		// Adjust the strength of the prediction based on the calculated score
		prediction = initial_prediction * score;
	}
	else if(m_predictorType == PredictorType::PATTERN && m_dataType == FLIPPING_DATA_SERIES) {

		if(numTurns < m_memoryLength) {
			return 0;
		}
		
		// Retrieve the most recent entries from the array of data
		uint8_t pattern[m_memoryLength];
		for(int i=0; i<m_memoryLength; i++){
			pattern[i] = userMovesFlipping[numTurns - m_memoryLength - 1 + i];
		}			
		
		uint8_t initial_prediction = pattern[0];
		
		uint8_t score = 0;
		
		
		for(int i = numTurns - m_memoryLength - 1; i >= max(0, numTurns-3*m_memoryLength); i--) {
			if(pattern[m_memoryLength-1] == userMovesFlipping[i]){
				score++;
			}
			// Take the last element off the end of the array
			uint8_t temp = pattern[m_memoryLength-1];
			// Shift all the elements back one
			for(int j=m_memoryLength-1; j>0; j--){
				pattern[j] = pattern[j-1];
			}
			// And then put the last element back on the beginning
			pattern[0] = temp;
		}
		// The maximum score is achieved when the pattern repeats itself twice
		score /= (2*m_memoryLength);
		// Adjust the strength of the prediction based on the calculated score
		prediction = initial_prediction * score * userMoves[numTurns-1];	
	}
	else if(m_predictorType == PredictorType::REACTIVE && m_dataType == USER_REACTIVE) {
		// We need to create a unique index that defines the current game situation, which we do by a bit pattern
		// of n user moves and the corresponding n win/losses
		// Copy an array of items from startIndex to endIndex (i.e. slice())
		uint8_t* partOfMoves = malloc(sizeof(uint8_t) * m_memoryLength);;
		memcpy(partOfMoves, &userMoves[numTurns-1 - m_memoryLength], m_memoryLength);
		int8_t* partOfWins = malloc(sizeof(int8_t) * m_memoryLength);
		memcpy(partOfWins, &wins[numTurns-1 - m_memoryLength], m_memoryLength);
	  
		// Merge the two arrays (i.e. concat())
		int8_t* lastState = malloc(sizeof(int8_t) * 2 * m_memoryLength);
		memcpy(lastState, partOfWins, m_memoryLength);
		memcpy(&lastState[m_memoryLength], partOfMoves, m_memoryLength);
	  
		// Create a unique index to represent the current state
		int lastStateInd = 0;
		for(int i=0; i<m_memoryLength*2; i++){
			// Only set the bit for 1 (for -1 we just leave as 0)
			if(lastState[i]==1) {
				lastStateInd += pow(2, i);
			}
		}
		// At this point, lastStateInd has a value something like B010111, showing that the player played three turns of 1, 1, 1 - they lost the first, won the second, and lost the third.
		
		// What did the user do last time this state occurred?
		int8_t lastStateResult = userMoves[numTurns-1];
		

		
		// Now set confidence with which we think the user will exhibit the same behaviour again
		if (stateMachine[lastStateInd] == 0) { // No prior info of this state having occurred
			stateMachine[lastStateInd] = lastStateResult*0.3;
		}
		else if (stateMachine[lastStateInd] == lastStateResult*0.3) {  // We've seen this pattern once before so strengthen prediction
			stateMachine[lastStateInd] = lastStateResult*0.8;
		}
		else if (stateMachine[lastStateInd] == lastStateResult*0.8) { // We've seen this pattern twice before so strengthen prediction
			stateMachine[lastStateInd] = lastStateResult*1;
		}
		else if (stateMachine[lastStateInd] == lastStateResult*1) { // User has already repeated this pattern >2 times in the past - maximum confidence it will occur again
			stateMachine[lastStateInd] = lastStateResult*1;
		}
		else {  // Last time this pattern occurred, the user did something else, so delete assumptions
			stateMachine[lastStateInd] = 0;
		}
		
		// TODO Check the following
		uint8_t* currentPartOfMoves = malloc(sizeof(uint8_t) * m_memoryLength);;
		memcpy(currentPartOfMoves, &userMoves[numTurns - m_memoryLength + 1], m_memoryLength);
		int8_t* currentPartOfWins = malloc(sizeof(int8_t) * m_memoryLength);
		memcpy(currentPartOfWins, &wins[numTurns - m_memoryLength], m_memoryLength);
		// Merge the two arrays
		int8_t* currentState = malloc(sizeof(int8_t) * 2 * m_memoryLength);
		memcpy(currentState, currentPartOfWins, m_memoryLength);
		memcpy(&currentState[m_memoryLength], currentPartOfMoves, m_memoryLength);
		// Create a unique index to represent the current state
		int currentStateInd = 0;
		for(int i=0; i<m_memoryLength*2; i++){
			if(currentState[i]==1) {
				lastStateInd += pow(2, i);
			}
		}
		
		// Lookup the prediction based on the index of the current state
		prediction = stateMachine[currentStateInd];
	}
	
	// As above, but calculate based on flipping user moves rather than raw input
	else if (m_predictorType == PredictorType::REACTIVE && m_dataType == USER_REACTIVE_REG_DATA) {
		;
	}
/*
	if (isNaN(prediction)) {
		prediction = 0;
	}
*/
	m_predictionsHistory[numTurns] = prediction;
	return prediction;		
};
		