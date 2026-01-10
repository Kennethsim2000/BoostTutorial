# Order Book TCP Server

## Build & Run

From the project root:

```bash
mkdir build
cd build
cmake ..
make
```

## Example 1: Full Match (Buy ↔ Sell)

### Client 1

```bash
nc 127.0.0.1 12345
ORDER buy 100.5 10 clientA
```

### Client 2

```bash
nc 127.0.0.1 12345
ORDER sell 100.5 10 clientB
```
