#include "src/MindReader.h"

MindReader mindReader(50);

int dummyUserMoves[8] = {1, -1, -1, -1, 1, 1, 1, -1};

void setup() {

  Serial.begin(115200);
  Serial.println(__FILE__ __DATE__);
  
  mindReader.begin();

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
