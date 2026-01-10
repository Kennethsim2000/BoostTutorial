# Order Book TCP Server

## Build & Run

From the project root:

```bash
mkdir build
cd build
cmake ..
make
```

## Examples

### Example 1: Full Match (Buy ↔ Sell)

#### Client 1 (Buy Order)

```bash
nc 127.0.0.1 12345
ORDER buy 100.5 10 clientA
```

**Response from server:**

```
--- Trade List ---
--- End of List ---
```

#### Client 2 (Sell Order)

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
