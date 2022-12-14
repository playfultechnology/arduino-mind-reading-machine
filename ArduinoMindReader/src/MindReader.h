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
	
		int8_t *m_userMoves;   //holds past user moves
		int8_t *m_userMovesFlipping;   //holds past user moves in the flipping case (if current=last then 1 else -1)
		int8_t *m_botMoves;    //holds past bot moves  (including the current prediction, so its size is always gameTurn+1)
		int8_t *m_wins;        // holds the history of who won
		
		Predictor *m_predictors;
			
		uint8_t m_gameTurn = 0;    // current game turn - pointer to the head of these arrays.
		uint8_t m_numberOfGameTurns;
};

#endif