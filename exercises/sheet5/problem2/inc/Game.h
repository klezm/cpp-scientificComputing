#ifndef BATTLESHIP_GAME_H
#define BATTLESHIP_GAME_H

#include "Board.h"

/**
 * @brief Class making the actual game run.
 * @author Lars Groeber
 */
class Game
{
public:
    /**
     * @brief Constructor
     * @param b pointer to Board object
     */
    Game ( Board* b );

    /**
     * @brief Main game function
     * Is the only function being called from outside to start the game
     * also includes the actual game loop.
     */
    void run ();

    /**
     * @brief Function that randomizes the stone's positions.
     * Uses check_position()
     * @return bool if placement was successful
     */
    bool randomize ();

    /**
     * @brief Function which checks if user has won.
     * @return bool
     */
    bool has_user_won ();

private:
    /** Pointer to the board being used */
    Board* mBoard;
    /** Dimensions of the board (x,y) */
    const std::pair<unsigned int, unsigned int> dim;

    /** The symbpols to use for both players */
    const std::string p1_sym;
    const std::string p2_sym;

    /** vectors holding the stones positions */
    std::vector<std::pair<int,int>> p1_stones;
    std::vector<std::pair<int,int>> p2_stones;

    /**
     * @brief Strategie of Player 1
     */
    void player1_play ();

    /**
     * @brief Strategy of Player 2
     */
    void player2_play ();

    /**
     * @brief Check if the board needs to be updated (like remove stones)
     */
    void update_board ();

    /**
     * @brief Get a random position on the board
     * @return pair<int,int> a rondom pos
     */
    std::pair<int,int> get_random_pos () const;

    /**
     * @brief Function which shows debug information
     * (for now only the ships location)
     */
    void show_debug () const;

    /**
     * @brief check if a postion is valid for a new ship.
     *        That means: a) the pos is on the board
     *                    b) the pos is free (there is no ship there)
     *                    c) there is no ship within one tile
     * @param pos the pos to be checkesd (local coords)
     * @return bool if pos is valid
     */
    bool check_position ( std::pair<int, int> pos ) const;
};


#endif //BATTLESHIP_GAME_H
