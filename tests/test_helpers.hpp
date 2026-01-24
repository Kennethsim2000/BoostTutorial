#pragma once

#include "types.hpp"
#include <string>
#include <vector>

// creating free functions here

Order createBuyOrder(double price, uint64_t qty,
                     const std::string &client = "TestClient");

Order createSellOrder(double price, uint64_t qty,
                      const std::string &client = "TestClient");

void verifyTrade(const Trade &trade,
                 OrderId expected_buy,
                 OrderId expected_sell,
                 double expected_price,
                 uint64_t expected_qty);

int countCSVLines(const std::string &filename);

Trade parseCSVLine(const std::string &line);

std::vector<Trade> readAllTrades(const std::string &filename);
