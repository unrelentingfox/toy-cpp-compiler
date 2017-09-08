/* crew.hpp - Copyright 2014 Andrew Schwartzmeyer
 * Header file for Crew class
 */

class Crew {
private:
  // properties
  int thirst;		// Fry's thirst level
  int work;		// Leela's unfulfilled work orders
  int horde;		// Bender's horde of treasure
  int hunger;		// Zoidberg's hunger level
  int beauty;		// Amy's prettiness
  int receipts;		// Hermes's backlog of receipts to process
public:
  Crew();
  bool stable;		// Crew's ability to continue co-existing
  // helper functions
  void print_menu();	// Print the game menu
  // implementation needed
  bool check();		// Check crew's stability
  int score();		// Calculate total score
  void drink();		// Fry drinks Slurm
  void deliver();	// Leela pilots deliveries
  void steal();		// Bender steals things
  void eat();		// Zoidberg eats everything
  void primp();		// Amy primps herself
  void account();	// Hermes does accounting
};