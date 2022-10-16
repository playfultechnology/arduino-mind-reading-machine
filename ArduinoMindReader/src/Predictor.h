#ifndef PREDICTOR_H_
#define PREDICTOR_H_

#include "Arduino.h"

class Predictor {
	public:
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
};
#endif