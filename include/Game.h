//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#pragma once

class Game {
public:
    Game();

    int getScore() const;
    void setScore(int score);
    void addScore(int points);

    int getShipsRemaining() const;
    void setShipsRemaining(int ships);
    void loseShip();

    int getHighScore() const;
    void updateHighScore();

private:
    int m_score;
    int m_shipsRemaining;
    int m_highScore;
};

