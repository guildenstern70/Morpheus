//
// Project Morpheus
// Copyright (C) 2026 Alessio Saltarin
//
// This software is licensed under MIT license.
// See LICENSE.
//

#include "Game.h"

#include <algorithm>

Game::Game()
    : m_score(0),
      m_shipsRemaining(3),
      m_highScore(0) {
}

int Game::getScore() const {
    return m_score;
}

void Game::setScore(int score) {
    m_score = score;
    updateHighScore();
}

void Game::addScore(int points) {
    m_score += points;
    updateHighScore();
}

int Game::getShipsRemaining() const {
    return m_shipsRemaining;
}

void Game::setShipsRemaining(int ships) {
    m_shipsRemaining = ships;
}

void Game::loseShip() {
    if (m_shipsRemaining > 0) {
        m_shipsRemaining--;
    }
}

int Game::getHighScore() const {
    return m_highScore;
}

void Game::updateHighScore() {
    m_highScore = std::max(m_highScore, m_score);
}

