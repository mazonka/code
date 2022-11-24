
Theseus and the Minotaur


Below is the extract from wikipedia about the game:

 "Theseus and the Minotaur is a type of logic maze designed by Robert Abbott. 
 In this maze, the player acts as Theseus, who is attempting to escape the Labyrinth. 
 The minotaur is also there, hunting the player down, taking two steps for 
 every one the player takes. While he is faster than the player, he's also 
 quite stupid. His moves are always determined by checking to see if he can 
 get closer to the player by moving horizontally, then he checks to see if 
 he can get any closer by moving vertically."


Random notes on the program.

* GUI has been written using wxWidgets 2.8
* I wrote the program quickly in 3 days trying to focus on the 
  algorithms rather than GUI. So, sorry about the interface.
* You can enable solving menu items by removing the file "nohelp".
* Solving algorithm finds the shortest solution.
* 'Wall filling' option specifies how many walls are created.
* 'Isolation' option specifies if walls created tend to be grouped together or sparse.
* 'Border exit' option allows to select sometimes exit inside the maze.
* 'Use walls' option specifies whether to regenerate walls of the maze or
  to try change exit and initial states of Theseus and Minotaur.
* Generation algorithm creates a number of mazes solves them and
  leaves the one with the longest solution (the solution is the
  shortest path).


Enjoy the game,

- Oleg Mazonka
