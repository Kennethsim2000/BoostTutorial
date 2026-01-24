#pragma once

#include "types.hpp"
#include <string>
#include <vector>

// creating free functions here

Order CreateBuyOrder(double price, uint64_t qty,
                     const std::string &client = "TestClient");

Order CreateSellOrder(double price, uint64_t qty,
                      const std::string &client = "TestClient");

void VerifyTrade(const Trade &trade,
                 OrderId expected_buy,
                 OrderId expected_sell,
                 double expected_price,
                 uint64_t expected_qty);

int CountCSVLines(const std::string &filename);

Trade ParseCSVLine(const std::string &line);

std::vector<Trade> ReadAllTrades(const std::string &filename);
