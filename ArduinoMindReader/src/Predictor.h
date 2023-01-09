#ifndef PREDICTOR_H_
#define PREDICTOR_H_

#include "Arduino.h"

class Predictor {
	public:
		// BIAS predictor tracks user's bias for one action over the other over a specified duration of moves
		// R,R,R,R,L,R,R,R,L,L is bias of the REGULAR_DATA_SERIES (more R's than L's)
		// R,R,R,R,R,L,L,L,L,L is bias of the FLIPPING_DATA_SERIES (a bias _not_ to flip)
		// PATTERN predictor tracks occurrences of particular repeated patterns
		// R,R,R,L,R,L,R,R,R,L,R,L  is pattern bias  of REGULAR_DATA_SERIES(repeated R,R,R,L,R,L)
		// R,R,R,L,R,L,L,L,L,R,L,R is pattern bias of the FLIPPING_DATA_SERIES (repeated NoFlip, NoFlip, Flip, Flip, Flip)
		// REACTIVE predictor tracks user's response to winning and losing
		enum PredictorType { BIAS, PATTERN, REACTIVE };
		enum DataType {REGULAR_DATA_SERIES, FLIPPING_DATA_SERIES, USER_REACTIVE, USER_REACTIVE_REG_DATA };
		
	public:
		Predictor(uint8_t memoryLength, PredictorType predictorType, DataType dataType);
		Predictor();
								
		uint8_t getPastAccuracy(int8_t* userMoves);
		uint8_t getPastAccuracy(int8_t* userMoves, uint8_t numTurns);
		int8_t makePrediction(int8_t* userMoves, int8_t* userMovesFlipping, int8_t* wins, uint8_t numTurns);
		
	private:
		// Predictor parameters
		uint8_t m_memoryLength;  //history length to look at
		PredictorType m_predictorType;
		DataType m_dataType; // REGULAR_DATA_SERIES operates on direct input, FLIPPING_DATA_SERIES operates on flipping series
		int8_t* m_predictionsHistory; // The bot's prediction of the player's move in each turn 

		// This is only used by the REACTIVE predictor type - should probably be defined in a subclass
		uint8_t* stateMachine;
};
#endif