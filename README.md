# Order Book TCP Server

## Build & Run

From the project root:

```bash
mkdir build
cd build
cmake ..
make
```

To run with thread sanitiser

```bash
mkdir build
cd build
cmake -DENABLE_TSAN=ON -DCMAKE_CXX_COMPILER=clang++ ..
make
```

To run the TCP Server:

```bash
cd build
./mini_trader
```

To run the tests:

```bash
cd build/tests
./test_basic
```

## Examples

### Example 1: Full Match (Buy ↔ Sell)

#### Client 1

```bash
nc 127.0.0.1 12345
ORDER buy 100.5 10 clientA
```

**Response from server:**

```
--- Trade List ---
--- End of List ---
```

#### Client 2

```bash
nc 127.0.0.1 12345
ORDER sell 100.5 10 clientB
```

**Response from server:**

```
--- Trade List ---
Trade 1:
BuyOrder: 1
SellOrder: 2
Price: $100.5
Quantity: 10
Timestamp: 1768054286042
--- End of List ---
```

### Example 2: Snapshot

#### Client

```bash
nc 127.0.0.1 12345
ORDER buy 100.5 10 clientA
ORDER buy 110 20 clientA
SNAPSHOT 2
```

**Response from server:**

```
ORDER buy 100.5 10 clientA
--- Trade List ---
--- End of List ---
ORDER buy 110 20 clientA
--- Trade List ---
--- End of List ---
SNAPSHOT 2
{"bids": [[110,20], [100.5,10]], "asks": [] }
```

### Example 3: Cancelling an order

#### Client

```bash
nc 127.0.0.1 12345
ORDER buy 100.5 10 clientA
SNAPSHOT 1
CANCEL 1
SNAPSHOT 1
```

**Response from server:**

```
ORDER buy 100.5 10 clientA
--- Trade List ---
--- End of List ---
SNAPSHOT 1
{"bids": [[100.5,10]], "asks": [] }
CANCEL 1
CANCELLED
SNAPSHOT 1
{"bids": [], "asks": [] }
```
