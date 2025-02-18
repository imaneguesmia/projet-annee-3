from cpp_chess import *

from misc import AttributeWatcher

if __name__ == "__main__":
    # a = WatchedValue(1)

    # print(a)
    # class a:
    #     def __init__(self):
    #         self.lmao = 1
        
    #     @property
    #     def lmao(self):
    #         return self.lmao

    class a(AttributeWatcher):
        def __init__(self):
            super().__init__()

            self.text = "lmao"
            self.other = 3

            self.r = [1, 2, 3]

            self.watch("text", "r")
        
        @property
        def a(self):
            return 10
        
        def _setattr__(self, name, value):
            print(f"{name} set to {value}")
            super().__setattr__(name, value)
    
    i = a()
    i.text = "hello"
    i.r.append("333")
    print(i.text, i.r, i.all_changed())

    # print(getattr(i, "a"))

    # print(PType.Pawn)
    # print(Player.White)
    # print(other_player(Player.White))

    # p = Position("e1")

    # print(p.row)
    # print(p.column)

    # print(p.position_string)
    # print(p.position_square)

    # p.position_string = "f8"
    # print(p.position_square)

    # m = Move()
    # print(m)

    # g = Game("8/8/8/8/8/1p6/5P2/8 b - - 0 1")

    # g.print_board()

    # print()
    # for m in g.get_current_pseudo_legals():
    #     print(
    #         f"from {Position(m.source)} to {Position(m.target)}\n"
    #         f"moving a \"{repr(m.player)[7:]} {repr(m.p_type)[6:]}\"\n"
    #         f"promoting to \"{repr(m.promotion)[6:]}\"\n"
    #         f"captured?    {m.capture}\n"
    #         f"double push? {m.double_push}\n"
    #         f"en passant?  {m.en_passant}\n"
    #         f"castle?      {m.castle}\n"
    #     )