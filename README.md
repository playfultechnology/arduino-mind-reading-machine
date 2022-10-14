# arduino-mind-reading-machine
An implementation of a "mind-reading machine", based on the work of Claude Shannon and D. W. Hagelbarger in the early 1950s.

The machine plays a game of "matching pennies" against a human opponent. To play, the player would have to say out loud either "right" or "left". The button in the centre of the machine would then be pressed, and either the right or left light would be illuminated. If the light matched the player's choice, the machine wins a point, otherwise the player wins a point. The player then moved the switch in the direction corresponding to the chocie she made.

The machine works by looking for certain types of patterns in the behaviour of the human player. When it finds these patterns, it remembers them and assumes that the player will follow the same patterns the next time the same situation arises. Essentially, it relies on the fact that humans are pretty predictable, and they're really bad at behaving "randomly".

There are plenty of software implementations of the machine, including:
 - http://success-equation.com/mind_reader.html
 - http://www.cs.williams.edu/~bailey/applets/MindReader/
 - https://web.media.mit.edu/~guysatat/MindReader/index.html

However, I wanted to create a physical hardware implementation, inspired by the original design of the machine:
![David Hagelbarger's "Outguessing Machine"](images/hagelbarger-outguessing-machine.jpeg)
![Claude Shannon's "Mind-Reading Machine"](images/claude-shannon_mind-reading-machine.jpg)

