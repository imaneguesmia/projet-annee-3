import cpp_chess as cm

from dataclasses import dataclass
from enum import Enum, auto

MoveTuple = tuple[
    cm.Position,  # Source
    cm.Position,  # Target
    cm.PType      # Promotion type
]

class HighlightType(Enum):
    ARROW   = auto()  # Draws an arrow from one square to another
    SQUARE  = auto()  # Highlights a square
    CLEAR   = auto()  # Clears all previous highlights

@dataclass
class Highlight:
    type: HighlightType
    position: cm.Position | None = None
    target: cm.Position | None = None

@dataclass
class ValidMoveInfo:
    description: list[str]
    result: list[MoveTuple]
    highlights: list[list[Highlight]]

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
        "Find the move for the\nwhite player that results\nin the least material lost.",
        "3k3n/8/8/4q3/8/3N2Q1/1b6/4K3 w - - 0 1",
        {
            # d3 -> e5
            (43, 28): ValidMoveInfo(
                description=[
                    "Taking with the knight first\nresults in a trade...",
                    "",
                    "",
                    "\n\nWhere white ends with a\nmaterial advantage!"
                ],
                result=[
                    (cm.Position("b2"), cm.Position("e5"), cm.PType.NoneType),
                    (cm.Position("g3"), cm.Position("e5"), cm.PType.NoneType),
                ],
                highlights=[
                    [
                        Highlight(HighlightType.ARROW, cm.Position("d3"), cm.Position("e5"))
                    ],
                    [
                        Highlight(HighlightType.CLEAR),
                        Highlight(HighlightType.ARROW, cm.Position("b2"), cm.Position("e5"))
                    ],
                    [
                        Highlight(HighlightType.CLEAR),
                        Highlight(HighlightType.ARROW, cm.Position("g3"), cm.Position("e5"))
                    ],
                    [
                        Highlight(HighlightType.CLEAR),
                        Highlight(HighlightType.SQUARE, cm.Position("e5"))
                    ]
                ]
            )
        }
    ),
    PuzzleInfo(
        "Piece square table.",
        "Find a move that places\na blue piece in a more\n\"advantageous\" position.",
        "r1r3k1/5ppp/4pn2/3p4/3P4/p3P2N/5PPP/1R2R1K1 b - - 1 40",
        {
            # a3 -> a2
            (40, 48): ValidMoveInfo(
                description=[
                    "Moving the pawn places it in a\nbetter position, bringing it closer\nto promotion.",
                ],
                result=[],
                highlights=[
                    [
                        Highlight(HighlightType.ARROW, cm.Position("a3"), cm.Position("a2"))
                    ],
                    [
                        Highlight(HighlightType.ARROW, cm.Position("a2"), cm.Position("a1"))
                    ]
                ]
            ),
            # c8 -> c2
            (2, 50): ValidMoveInfo(
                description=[
                    "Moving the rook to c2 places it\nin a very advantageous position where it\ncan put pressure on the entirety of\nwhite's back rank."
                ],
                result=[],
                highlights=[
                    [
                        Highlight(HighlightType.ARROW, cm.Position("c8"), cm.Position("c2"))
                    ],
                    [
                        Highlight(HighlightType.CLEAR),
                        Highlight(HighlightType.ARROW, cm.Position("c2"), cm.Position("a2")),
                        Highlight(HighlightType.ARROW, cm.Position("c2"), cm.Position("h2"))
                    ]
                ]
            ),
            # f6 -> e4
            (21, 36): ValidMoveInfo(
                description=[
                    "Placing the knight in a central\nposition allows it to put a lot of\npressure on white's defense."
                ],
                result=[],
                highlights=[
                    [
                        Highlight(HighlightType.ARROW, cm.Position("f6"), cm.Position("e4"))
                    ],
                    [
                        Highlight(HighlightType.CLEAR),
                        Highlight(HighlightType.ARROW, cm.Position("e4"), cm.Position("c3")),
                        Highlight(HighlightType.ARROW, cm.Position("e4"), cm.Position("d2")),
                        Highlight(HighlightType.ARROW, cm.Position("e4"), cm.Position("f2")),
                        Highlight(HighlightType.ARROW, cm.Position("e4"), cm.Position("g3")),
                    ]
                ]
            )
        }
    ),
    PuzzleInfo(
        "Mobility.",
        "Find a move that allows\nwhite to safely control\nmore squares on the\nboard.",
        "r2qr1k1/pp3ppp/3p1b2/8/8/5N2/P1P2PPP/2RQ1RK1 w - - 2 24",
        {
            # d1 -> d5
            (59, 27): ValidMoveInfo(
                description=[
                    "Moving the queen to a more central\nlocation allows it to control a\nlot more squares."
                ],
                result=[],
                highlights=[
                    [
                        Highlight(HighlightType.ARROW, cm.Position("d1"), cm.Position("d5"))
                    ],
                    [
                        Highlight(HighlightType.CLEAR),
                        Highlight(HighlightType.ARROW, cm.Position("d5"), cm.Position("e4")),
                        Highlight(HighlightType.ARROW, cm.Position("d5"), cm.Position("h5")),
                        Highlight(HighlightType.ARROW, cm.Position("d5"), cm.Position("f7")),
                        Highlight(HighlightType.ARROW, cm.Position("d5"), cm.Position("d6")),
                        Highlight(HighlightType.ARROW, cm.Position("d5"), cm.Position("b7")),
                        Highlight(HighlightType.ARROW, cm.Position("d5"), cm.Position("a5")),
                        Highlight(HighlightType.ARROW, cm.Position("d5"), cm.Position("b3"))
                    ]
                ]
            ),
            # c1 -> b1
            (58, 57): ValidMoveInfo(
                description=[
                    "Moving the rook over allows it to\ncontrol the entire column,"
                ],
                result=[],
                highlights=[
                    [
                        Highlight(HighlightType.ARROW, cm.Position("c1"), cm.Position("b1"))
                    ],
                    [
                        Highlight(HighlightType.CLEAR),
                        Highlight(HighlightType.ARROW, cm.Position("b1"), cm.Position("b7"))
                    ]
                ]
            )
        }
    ),
    PuzzleInfo(
        "Pawn structure.",
        "Find a way for the blue\nplayer to create a\nweakness in white's\npawn structure.",
        "3r4/p1p3pp/1p2kp2/3p4/2P1P3/1P1P4/P3K1PP/5R2 b - - 0 1",
        {
            # d5 -> e4
            (27, 36): ValidMoveInfo(
                description=[
                    "This creates what's called an\n\"isolated pawn\". After this move,\nwhite's best move is to take back\nthe pawn with its own pawn.\nHowever...",
                    "\n\nWhite's pawn is now isolated, with\nno other pawns to protect it."
                ],
                result=[
                    (cm.Position("d3"), cm.Position("e4"), cm.PType.NoneType)
                ],
                highlights=[
                    [
                        Highlight(HighlightType.ARROW, cm.Position("d5"), cm.Position("e4"))
                    ],
                    [
                        Highlight(HighlightType.CLEAR),
                        Highlight(HighlightType.ARROW, cm.Position("d3"), cm.Position("e4"))
                    ],
                    [
                        Highlight(HighlightType.CLEAR),
                        Highlight(HighlightType.SQUARE, cm.Position("e4")),
                        Highlight(HighlightType.ARROW, cm.Position("d3"), cm.Position("d5")),
                        Highlight(HighlightType.ARROW, cm.Position("f3"), cm.Position("f5"))
                    ]
                ]
            )
        }
    ),
    PuzzleInfo(
        "King safety.",
        "Find a move that puts\nthe blue king in danger!",
        "r4rk1/1ppq1ppp/1pnp3n/8/4P3/3Q3P/PPPB1PPN/R4RK1 w - - 4 18",
        {
            # d2 -> h6
            (51, 23): ValidMoveInfo(
                description=[
                    "Moving the bishop threatens to\ndestroy the wall of pawns\nprotecting the king.",
                    "\n\nIf the bishop is taken, it opens\nup the king to attack."
                ],
                result=[
                    (cm.Position("g7"), cm.Position("h6"), cm.PType.NoneType)
                ],
                highlights=[
                    [
                        Highlight(HighlightType.ARROW, cm.Position("d2"), cm.Position("h6"))
                    ],
                    [
                        Highlight(HighlightType.CLEAR),
                        Highlight(HighlightType.ARROW, cm.Position("g7"), cm.Position("h6"))
                    ],
                    [
                        Highlight(HighlightType.ARROW, cm.Position("g3"), cm.Position("g8")),
                    ]
                ]
            )
        }
    ),
]