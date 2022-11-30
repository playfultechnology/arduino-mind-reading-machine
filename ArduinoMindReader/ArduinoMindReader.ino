// INCLUDES
// Include the mind reader library
#include "src/MindReader.h"

// GLOBALS
// Create mindreader for a game with the specified number of turns
MindReader mindReader(50);

// For testing only, create a sequence of dummy moves
int dummyUserMoves[8] = {1, -1, -1, -1, 1, 1, 1, -1};

void setup() {

  // For debug output
  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);
  
  // Initialise the mindreader
  mindReader.begin();

  // Run over a series of test moves
  for(int i=0; i<8; i++){
    Serial.print(F("Prediction:"));
    Serial.print(mindReader.getPrediction());
    Serial.print(F(", Actual:"));
    Serial.println(dummyUserMoves[i]);
    mindReader.updateUserMove(dummyUserMoves[i]);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
