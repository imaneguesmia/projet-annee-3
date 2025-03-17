# Chess Engine Optimizations

This document describes some of the search optimizations implemented in the Beluga chess engine to improve its performance and efficiency.

## Search Optimizations

### 1. Late Move Reduction (LMR)

**Implementation**: Added in `negamax()` function in `Search.cpp`.

**Description**: LMR reduces the search depth for moves that are less likely to be good (typically moves ordered later in the move list). This is based on the observation that the best moves are usually found early due to move ordering heuristics.

**How it works**:
- The first few moves are searched at full depth
- Later quiet moves (non-captures, non-checks) are searched with a reduced depth
- If a reduced search indicates a promising move (fails high), a full-depth search is performed
- The reduction amount increases for later moves and decreases for deeper searches

**Parameters**:
- `LMR_DEPTH_THRESHOLD`: Minimum depth for applying LMR (set to 3)
- `LMR_MOVES_THRESHOLD`: Number of moves to search at full depth before applying LMR (set to 3)
- `LMR_BASE`: Base reduction amount (set to 1)
- `LMR_MOVES_DIVISOR`: Divisor for calculating additional reduction based on move number
- `LMR_MAX_REDUCTION`: Maximum additional reduction allowed

**Benefits**:
- Significantly reduces the search tree size
- Can reduce the effective branching factor to less than 2
- Maintains tactical strength by verifying promising positions with full-depth search

### 2. Futility Pruning

**Implementation**: Added in `negamax()` function in `Search.cpp`.

**Description**: Futility pruning skips searching quiet moves that are unlikely to improve alpha in nodes close to the leaf nodes.

**How it works**:
- Applied at shallow depths (typically 1-3 plies from the horizon)
- Calculates a "futility margin" based on depth
- If the static evaluation plus the margin is still below alpha, quiet moves are skipped
- Captures and checks are always searched to maintain tactical awareness

**Parameters**:
- `FUTILITY_DEPTH`: Maximum depth for applying futility pruning (set to 3)
- `FUTILITY_MARGIN_BASE`: Base margin for futility pruning (set to 100)
- `FUTILITY_MARGIN_DEPTH`: Additional margin per depth level (set to 150)

**Benefits**:
- Reduces search effort in positions that are unlikely to improve
- Maintains tactical strength by always searching captures and checks

### 3. Late Move Pruning (LMP)

**Implementation**: Added in `negamax()` function in `Search.cpp`.

**Description**: LMP completely skips searching quiet moves that appear late in the move ordering at shallow depths.

**How it works**:
- Applied at shallow depths (typically 1-3 plies from the horizon)
- After searching a certain number of moves (based on depth), remaining quiet moves are skipped
- Captures and checks are always searched

**Parameters**:
- `LMP_DEPTH`: Maximum depth for applying LMP (set to 3)
- `LMP_BASE`: Base number of moves to search before applying LMP (set to 3)

**Benefits**:
- Further reduces the search tree size
- Complements LMR by completely skipping unpromising moves

### 4. Null Move Pruning

**Implementation**: Added in `negamax()` function in `Search.cpp`.

**Description**: Null Move Pruning is based on the observation that in most chess positions, giving the opponent an extra move (a "null move") would still not be enough for them to gain an advantage if the current position is very strong.

**How it works**:
- If the static evaluation is already above beta (indicating a strong position)
- Make a "null move" (skip a turn)
- Search the resulting position with reduced depth (R = 3)
- If this reduced search still returns a score above beta, we can assume the original position is so strong that we can prune the entire subtree

**Parameters**:
- `NULL_MOVE_MIN_DEPTH`: Minimum depth for applying null move pruning (set to 3)
- `NULL_MOVE_REDUCTION`: Depth reduction for null move search (set to 3)

**Potential Issues**:
- Null move pruning can fail in zugzwang positions (where any move worsens the position)
- Zugzwang positions are common in pawn endgames
- Our implementation does not include specific zugzwang detection, which could lead to incorrect pruning in some positions
- This is a trade-off between speed and accuracy

**Benefits**:
- Dramatically reduces the search tree in positions with clear advantages
- Can provide significant speedup (up to 50% in some positions)

### 5. Reverse Futility Pruning (Static Null Move Pruning)

**Implementation**: Added in `negamax()` function in `Search.cpp`.

**Description**: Reverse Futility Pruning (also known as Static Null Move Pruning) is based on the idea that if the static evaluation of a position is significantly better than beta, it's likely that any move will maintain that advantage.

**How it works**:
- Applied at shallow to medium depths (typically up to 7 plies)
- Calculates a "reverse futility margin" based on depth
- If the static evaluation minus the margin is still above beta, we can prune the entire node
- No moves are searched at all in this case

**Parameters**:
- `RFP_DEPTH`: Maximum depth for applying reverse futility pruning (set to 7)
- `RFP_MARGIN`: Margin per depth for reverse futility pruning (set to 80)

**Benefits**:
- Very efficient pruning that can eliminate entire subtrees
- Particularly effective in positions with large material or positional advantages
- Complements Null Move Pruning but is less expensive (no need to make/unmake a move)

## Principal Variation (PV) Nodes

The search now distinguishes between PV (Principal Variation) nodes and non-PV nodes:

- PV nodes are searched more thoroughly with fewer pruning techniques
- Non-PV nodes use more aggressive pruning to reduce the search tree size

This approach ensures that the main line of play is searched thoroughly while side variations are pruned more aggressively.

## Conclusion

These optimizations significantly improve the performance of the chess engine by reducing the effective branching factor and search tree size. The engine maintains its tactical strength by applying pruning techniques selectively and verifying promising positions with full-depth searches.

The parameters can be fine-tuned through testing to find the optimal balance between speed and playing strength. 