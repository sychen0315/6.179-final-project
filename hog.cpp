#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <math.h>
#include <random>

// Forward declarations.
class Game;
class PlayerStats;
class Player;
class Dice;
class Strategy;

int FOUR_SIDE = 4;
int SIX_SIDE = 6;

//////////////////////////////////////////////////////////////
// Begin type interfaces.
//////////////////////////////////////////////////////////////

////////////////////////
// "Dice.h"
//
// This class represents a fixed sided dice
class Dice
{
  int side;
  public:
    Dice(int side): side(side) {};
    int dice();
};

////////////////////////
// "PlayerStats.h"
//
// This class is responsible for maintaining each
//   player's stats for a given tournament and simulating
//   how these stats change as the player interacts with
//   other players.
class PlayerStats
{
    int score;

  public:
    PlayerStats() : score(0) {}
    PlayerStats(int score) : score(score) {}

    int roll_dice(int num_rolls, int side);
    friend bool playerStatComparator(PlayerStats p1, PlayerStats p2);
    friend std::ostream &operator<<(std::ostream &os, const PlayerStats &ps);
    PlayerStats updatescore(int score) const;
    int getscore() const
    {
      return this->score;
    }
};


///////////////////////
// "Player.h"
//
// This struct is responsible for storing all the details
//    that identify a player
// Player is a header-only object.
struct Player
{
    std::string name;
    PlayerStats stats;

    public:
    Player() : name(),stats(){}
    Player(std::string name) : name(name), stats(){}
    Player(std::string name, PlayerStats stats) : name(name), stats(stats){}
    friend std::istream &operator>>(std::istream &is, Player &p)
    {
        is >> p.name;
        return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const Player &p)
    {
        os << p.name;
        return os;
    }

    friend PlayerStats getstats(Player p)
    {
      return p.stats;
    }

    Player updatestats(int score) const
    {
      //p.stats.updatescore(score);
      PlayerStats s = this->stats.updatescore(score);
      return {this->name, s};
    }


    friend std::string getname(Player p)
    {
      return p.name;
    }
};




////////////////////////
// "Game.h"
//
// This class represents a single match in a tournament, and
//    provides several static methods for building these matches
class Game
{
    Player p1;
    Player p2;
    int curr; // 0: p1; 1: p2

  public:
    Game(Player p1, Player p2) : p1(p1), p2(p2), curr(0) {}
    Game(Player p1, Player p2, int curr) : p1(p1), p2(p2), curr(curr) {}
    friend std::ostream &operator<<(std::ostream &os, Game &g);
    Game execute(Strategy s) const;
    Player getplayer1() const
    {
      return this->p1;
    }
    Player getplayer2() const
    {
      return this->p2;
    }
    int getcurr() const
    {
      return this->curr;
    }
};

////////////////////////
// "Strategy.h"
//
// This class represents a strategy, how many dices to roll and what 
// kind of dice to roll

class Strategy
{
    int num_rolls;
    int side;

  public:
    Strategy(int num_rolls, int side) : num_rolls(num_rolls), side(side){};
    friend int get_num(Strategy s)
    {
      return s.num_rolls;
    }
    friend int get_side(Strategy s)
    {
      return s.side;
    }
    Strategy  updateside(int side) const
    {
      return {this->num_rolls, side};
    }
    Strategy  updatenum(int num_rolls) const
    {
      return {num_rolls, this->side};
    }
};



//////////////////////////////////////////////////////////////
// Begin type implementations.
//////////////////////////////////////////////////////////////

////////////////////////
// "Dice.cpp"

// Compute the dice value of one turn
int Dice::dice()
{
  if (this->side > 0) {
    int dice_value;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, this->side);
    double uniformOn01 = dis(gen);
    dice_value = ceil(uniformOn01);
    return dice_value;
  }
  
  std::cout << "A dice must have more than 1 side";
  return 0;
}



////////////////////////
// "PlayerStats.cpp"

// Compute the new stats this player would have after either a
// WIN/LOSS/TIE against a player with the passed in stats.
int roll_dice(int num_rolls, int side) 
{
    Dice dice(side);
    int total = 0;
    while(num_rolls) {
      int result = dice.dice();
      if (result == 1){
        std::cout << "The total value of dice: " <<1<<std::endl;
        return 1;
      }
      total = result + total;
      num_rolls = num_rolls - 1;
    }
    std::cout << "The total value of dice: " <<total<<std::endl;
    return total;
    //std::cout << std::to_string(d) <<"\n";
}


// Compare P1 and P2
bool playerStatComparator(PlayerStats p1, PlayerStats p2)
{
    if (p1.score < p2.score) {
        return true;
    }
    return false;
}


PlayerStats PlayerStats::updatescore(int score) const
{
  return {this->score+score};
}

// render these stats to the stream in the format:
// score
std::ostream &operator<<(std::ostream &os, const PlayerStats &ps)
{
    os << "Score: " << std::to_string(ps.score) << std::endl;
    return os;
}



////////////////////////
// "Game.cpp"


Game Game::execute(Strategy s) const
{
  int num = get_num(s);
  int sid = get_side(s);
  if (this->curr == 0){
    PlayerStats p1stats;
    p1stats = getstats(this->p1);
    //this->p1 = this->p1.updatestats(roll_dice(num, sid));
    //std::cout << "Player 1 current score: " << getstats(this->p1);
    return {this->p1.updatestats(roll_dice(num, sid)), this->p2, 1};
  }
  else
  {
    PlayerStats p2stats;
    p2stats = getstats(this->p2);
    //this->p2 = this->p2.updatestats(roll_dice(num, sid));
    //std::cout << "Player 2 current score: " <<getstats(this->p2);
    return {this->p1, this->p2.updatestats(roll_dice(num, sid)),0};
  }
}



////////////////////////
// "Main.cpp"
///////////////////////

// driver. Dont touch this.
int main()
{	
  int num;
  // randomly decide who goes first
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0, 1);
  int whofirst = (int) (dis(gen) + 0.5f);
  std::string name1;
  std::string name2;
  std::cout << "Enter name for Player 1: ";
  std::cin >> name1;
  PlayerStats p1stats(0);
  Player p1(name1, p1stats);
  std::cout << "Player 1 name is: ";
  std::cout << getname(p1) << std::endl;
  std::cout << "Enter name for Player 2: ";
  std::cin >> name2;
  PlayerStats p2stats(0);
  Player p2(name2, p2stats);
  std::cout << "Player 2 name is: ";
  std::cout << getname(p2) << std::endl;
  std::cout << "Player " << whofirst+1 << " goes first" << std::endl;
  Game g(p1,p2, whofirst);
  Strategy s(2, 6);
  int who;
  while (true)
  {
    if ((getstats(g.getplayer1()).getscore()+getstats(g.getplayer2()).getscore())%7==0){
      s.updateside(FOUR_SIDE);
    }
    else
    {
      s.updateside(SIX_SIDE);
    }
    who = g.getcurr();
    if (who == 0){
      std::cout << "Select the number of dice to roll for Player "<< getname(p1) << ": ";
      std::cin >> num;
      while (num>10 || num <= 0)
      {
        std::cout << "Please enter a valid number between 1 to 10: ";
        std::cin >> num;
      }
      s.updatenum(num);
    }
    else
    {
      std::cout << "Select the number of dice to roll for Player "<< getname(p2) << ": ";
      std::cin >> num;
      while (num>10 || num <= 0)
      {
        std::cout << "Please enter a valid number between 1 to 10: ";
        std::cin >> num;
      }
      s.updatenum(num);
    }
    g = g.execute(s);
    std::cout << "Player "<< getname(p1) << " current score: " << getstats(g.getplayer1()) << std::endl;
    std::cout << "Player "<< getname(p2) << " current score: " << getstats(g.getplayer2()) << std::endl;
    if (getstats(g.getplayer1()).getscore()>=100)
    {
      std::cout << "Player "<< getname(p1) << " won! Congratulation" << std::endl;
      break;
    }
    if (getstats(g.getplayer2()).getscore()>=100)
    {
      std::cout << "Player "<< getname(p2) << " won! Congratulation" << std::endl;
      break;
    }
  }
  //std::cout << "Player 1 current score: " << getstats(p1)<< std::endl;

 //  std::cout << "Enter the number of sides: ";
 //  std::cin >> side;
	// Dice dice(side);
 //  std::cout << "One roll dice value: ";
 //  std::cout << dice.dice()<<std::endl;
  return 0;
}
