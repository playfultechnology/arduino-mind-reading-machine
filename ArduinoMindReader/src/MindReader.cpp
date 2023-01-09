#include "MindReader.h"
#include "Predictor.h"
#include "Arduino.h"

#define ARRAY_SIZE(array) ((sizeof(array))/(sizeof(array[0])))
#define USERWIN -1
#define BOTWIN  1

// See https://web.media.mit.edu/~guysatat/MindReader/GuySatat_MindReader_FinalProject.pdf

//MindReader::MindReader(int numberOfGameTurns) { }

void MindReader::begin(){
	resetBot();
	updateBotPrediction();
}

void MindReader::resetBot() {
	m_gameTurn = 0;
	
	// Initialise all arrays
	m_userMoves = new int8_t[m_numberOfGameTurns]{};
	m_userMovesFlipping = new int8_t[m_numberOfGameTurns]{};
	m_botMoves = new int8_t[m_numberOfGameTurns]{};
	m_wins = new int8_t[m_numberOfGameTurns]{};

	m_predictors = new Predictor[NUM_PREDICTORS] {
		Predictor(2, Predictor::PredictorType::BIAS, Predictor::DataType::REGULAR_DATA_SERIES),
		Predictor(3, Predictor::PredictorType::BIAS, Predictor::DataType::REGULAR_DATA_SERIES),
		Predictor(5, Predictor::PredictorType::BIAS, Predictor::DataType::REGULAR_DATA_SERIES),
		Predictor(2, Predictor::PredictorType::BIAS, Predictor::DataType::FLIPPING_DATA_SERIES),
		Predictor(3, Predictor::PredictorType::BIAS, Predictor::DataType::FLIPPING_DATA_SERIES),
		Predictor(5, Predictor::PredictorType::BIAS, Predictor::DataType::FLIPPING_DATA_SERIES),
		Predictor(2, Predictor::PredictorType::PATTERN, Predictor::DataType::REGULAR_DATA_SERIES),
		Predictor(3, Predictor::PredictorType::PATTERN, Predictor::DataType::REGULAR_DATA_SERIES),
		Predictor(4, Predictor::PredictorType::PATTERN, Predictor::DataType::REGULAR_DATA_SERIES),
		Predictor(5, Predictor::PredictorType::PATTERN, Predictor::DataType::REGULAR_DATA_SERIES),
		Predictor(2, Predictor::PredictorType::PATTERN, Predictor::DataType::FLIPPING_DATA_SERIES),
		Predictor(3, Predictor::PredictorType::PATTERN, Predictor::DataType::FLIPPING_DATA_SERIES),
		Predictor(4, Predictor::PredictorType::PATTERN, Predictor::DataType::FLIPPING_DATA_SERIES),
		Predictor(5, Predictor::PredictorType::PATTERN, Predictor::DataType::FLIPPING_DATA_SERIES),
		Predictor(1, Predictor::PredictorType::REACTIVE, Predictor::DataType::USER_REACTIVE),
		Predictor(2, Predictor::PredictorType::REACTIVE, Predictor::DataType::USER_REACTIVE),
		Predictor(1, Predictor::PredictorType::REACTIVE, Predictor::DataType::USER_REACTIVE_REG_DATA),
		Predictor(2, Predictor::PredictorType::REACTIVE, Predictor::DataType::USER_REACTIVE_REG_DATA),
	};	
}

/**
 * Process information after every new move that the user makes
 */
void MindReader::updateUserMove(int8_t userMove){
	// Update user move array
	m_userMoves[m_gameTurn] = userMove;
	// Update user flipping array
	if(m_gameTurn > 0) { // Make sure the player has taken at least one turn
		if(m_userMoves[m_gameTurn] == m_userMoves[m_gameTurn-1]) { // If new move is the same as their last move
			m_userMovesFlipping[m_gameTurn] = 1;
		}
		else {
			m_userMovesFlipping[m_gameTurn] = -1;
		}
	}
	// Update the "who won" array
	if(userMove == getPrediction()) {
		m_wins[m_gameTurn] = BOTWIN;
	}
	else {
		m_wins[m_gameTurn] = USERWIN;
	}
	// Move to the next turn
	m_gameTurn += 1;
	// Update the bot prediction for the next round
	updateBotPrediction();
}

/**
 * Populate the best guess for the user's next move based on move history
 */
void MindReader::updateBotPrediction(){

	double botPredictionProb = 0;
	uint8_t botPrediction;
	
	// Calculate 𝜂 as √((log 𝑁)/(2𝑇−1))
	double eta = sqrt(log(NUM_PREDICTORS) / (2*m_numberOfGameTurns - 1) );
	
	uint8_t expertPastAccuracy;
	double expertWeight;
	int8_t currentPrediction;
	double denominator=0, numerator=0;
	// Loop over every expert
	for(int expertInd=0; expertInd<NUM_PREDICTORS; expertInd++) {
		expertPastAccuracy = m_predictors[expertInd].getPastAccuracy(m_userMoves, m_gameTurn);
		expertWeight = exp(-1 * eta * expertPastAccuracy);
		currentPrediction = m_predictors[expertInd].makePrediction(m_userMoves, m_userMovesFlipping, m_wins, m_gameTurn);
		numerator += currentPrediction * expertWeight;
		denominator += expertWeight;
	}
	// Make a prediction based on the weighted average opinion of all the experts 
	botPredictionProb = numerator / denominator;

	// Rademacher distribution (https://en.wikipedia.org/wiki/Rademacher_distribution)
	int8_t sample = random(-1, 1);
	if(botPredictionProb < sample) {
		botPrediction = -1;
	}
	else{
		botPrediction = 1;
	}
	// Place the prediction into the array
	m_botMoves[m_gameTurn] = botPrediction;
}

// Retrieve current bot prediction
int8_t MindReader::getPrediction(){
	return m_botMoves[m_gameTurn];
}