#ifndef MINDREADER_H_
#define MINDREADER_H_

#define NUM_PREDICTORS 18

#include "Arduino.h"
#include "Predictor.h"

class MindReader {			
  public:
		MindReader(int numberOfGameTurns): m_numberOfGameTurns(numberOfGameTurns), m_gameTurn(0) {}
		void begin();
		void resetBot();
		void updateUserMove(int8_t userMove);
		int8_t getPrediction();

	private:
	
		void updateBotPrediction();	
	
		int8_t *m_userMoves;   // History of user moves (-1 or 1)
		int8_t *m_userMovesFlipping;   // History of user move changes (if same as previous then 1 else -1)
		int8_t *m_botMoves;    // History of bot moves  (including the current prediction, so its size is always gameTurn+1)
		int8_t *m_wins;        // History of who won (if bot successfully predicts then 1 else -1)
		
		Predictor *m_predictors;
			
		uint8_t m_gameTurn = 0;    // current game turn - pointer to the head of these arrays.
		uint8_t m_numberOfGameTurns;
};

#endif