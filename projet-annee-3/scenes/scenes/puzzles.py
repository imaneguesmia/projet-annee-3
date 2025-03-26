import cpp_chess as cm

from dataclasses import dataclass

MoveTuple = tuple[int, int] | tuple[cm.Square, cm.Square]

@dataclass
class ValidMoveInfo:
    description: list[str]
    result: list[MoveTuple]

@dataclass
class PuzzleInfo:
    """Valid moves format:

    ([move]): {
        "description": "...",
        "result": [
            ([move 1]),
            ([move 2]),
            ...
        ]
    }
    """

    subtitle: str
    description: str
    initial_state: str
    valid_moves: dict[MoveTuple, ValidMoveInfo]

PUZZLES = [
    PuzzleInfo(
        "Material heuristic.",
        "desc",
        "3k4/8/8/4q3/8/2bN2Q1/8/4K3 w - - 0 1",
        {
            
        }
    ),
    PuzzleInfo(
        "Piece square table.",
        "desc",
        "r1r3k1/5ppp/4pn2/3p4/3P4/p3P2N/5PPP/1R2R1K1 b - - 1 40",
        {
            ("a3", "a2"): {
                "description": "Pawn",
                "result": []
            },
            ("c8", "c2"): {
                "description": "Rook",
                "result": []
            },
            ("f6", "e4"): {
                "description": "Knight",
                "result": []
            }
        }
    ),
    PuzzleInfo(
        "Mobility.",
        "desc",
        "r2qr1k1/pp3ppp/3p1b2/8/8/5N2/P1P2PPP/2RQ1RK1 w - - 2 24",
        {
            ("d1", "d5"): "Queen",
            ("c1", "b1"): "Rook"
        }
    ),
    PuzzleInfo(
        "Pawn structure.",
        "Find a way for the blue\nplayer to create a\nweakness in white's\npawn structure.",
        "3r4/p1p3pp/1p2kp2/3p4/2P1P3/1P1P4/P3K1PP/5R2 b - - 0 1",
        {
            # ("d5", "e4"): {
            (27, 36): ValidMoveInfo(
                description=[
                    "This creates what's called an\n\"isolated pawn\". After this move,\nwhite's best move is to take back\nthe pawn with its own pawn.\nHowever...",
                    "White's pawn is now isolated, with\nno other pawns to protect it!"
                ],
                result=[
                    (cm.Square.d3, cm.Square.e4)
                ]
            )
            # {
            #     "description": "Isolated pawn for opponent",
            #     "result": [(cm.Square.d3, cm.Square.e4)]
            # }
        }
    ),
    PuzzleInfo(
        "King safety.",
        "desc",
        "r4rk1/1ppq1ppp/1pnp3n/8/4P3/3Q3P/PPPB1PPN/R4RK1 w - - 4 18",
        {
            (cm.Square.d4, cm.Square.h6): {
                "description": "Bishop breaks king protection thing"
            }
        }
    ),
]