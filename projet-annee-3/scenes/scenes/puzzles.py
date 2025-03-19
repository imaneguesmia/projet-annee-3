from dataclasses import dataclass

@dataclass
class PuzzleInfo:
    description: str
    initial_state: str

PUZZLES = [
    PuzzleInfo("This is the first puzzle.", "R3Q1NR/PP2PP1P/2N5/3bK3/3Bp1B1/2q5/p1p3pp/1k3r2 b - - 0 1")
]