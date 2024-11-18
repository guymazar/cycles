#include "api.h"
#include "utils.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>

namespace guymazar {  // Custom namespace for your bot

using namespace cycles; // Import necessary types from the `cycles` namespace

class SmartBotClient {
  Connection connection;
  std::string name;
  GameState state;
  Player my_player;
  int previousDirection = -1;

  bool is_valid_move(Direction direction) {
    // Check the new position after moving in the given direction
    auto new_pos = my_player.position + getDirectionVector(direction);
    if (!state.isInsideGrid(new_pos)) {
      return false; // Out of bounds
    }
    if (state.getGridCell(new_pos) != 0) {
      return false; // Occupied cell
    }
    return true;
  }

  Direction decideMove() {
    const auto directions = {
        Direction::north, Direction::east, Direction::south, Direction::west};
    Direction chosenDirection = static_cast<Direction>(previousDirection);

    // Try to keep moving in the same direction if valid
    if (previousDirection != -1 && is_valid_move(chosenDirection)) {
      return chosenDirection;
    }

    // Choose a valid move from the directions in priority order
    for (const auto& direction : directions) {
      if (is_valid_move(direction)) {
        chosenDirection = direction;
        break;
      }
    }

    return chosenDirection;
  }

  void receiveGameState() {
    state = connection.receiveGameState();
    for (const auto& player : state.players) {
      if (player.name == name) {
        my_player = player;
        break;
      }
    }
  }

  void sendMove() {
    auto move = decideMove();
    previousDirection = getDirectionValue(move);
    connection.sendMove(move);
  }

public:
  SmartBotClient(const std::string& botName) : name(botName) {
    connection.connect(name);
    if (!connection.isActive()) {
      spdlog::critical("{}: Connection failed", name);
      exit(1);
    }
  }

  void run() {
    while (connection.isActive()) {
      receiveGameState();
      sendMove();
    }
  }
};

} 

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <bot_name>" << std::endl;
    return 1;
  }

  std::string botName = argv[1];
  guymazar::SmartBotClient bot(botName); 
  bot.run();

  return 0;
}