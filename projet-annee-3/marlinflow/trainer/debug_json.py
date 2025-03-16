import json

json_path = "/Users/lucaferrari/CLionProjects/projet-annee-3/projet-annee-3/marlinflow/trainer/nn/beluga_v1.json"  # Mettez ici votre chemin exact

with open(json_path, "r") as f:
    jnet = json.load(f)

def check_2D_shape(key, expected_dim0=None, expected_dim1=None):
    """Affiche la forme (dim0, dim1) de la clé key dans jnet,
       et compare éventuellement à un (expected_dim0, expected_dim1)."""
    data_2d = jnet[key]  # On suppose que c’est une liste de listes

    if not isinstance(data_2d, list):
        print(f"{key} n'est pas une liste!")
        return

    dim0 = len(data_2d)
    dim1 = len(data_2d[0]) if dim0>0 and isinstance(data_2d[0], list) else 0

    print(f"{key} shape in JSON => (dim0, dim1) = ({dim0}, {dim1})")

    # Affichons un mini-exemple
    print(f"{key}[0][0] =", data_2d[0][0])
    print(f"{key}[-1][-1] =", data_2d[-1][-1])

    if expected_dim0 is not None and expected_dim1 is not None:
        if (dim0, dim1) == (expected_dim0, expected_dim1):
            print(f"  => OK, correspond à ({expected_dim0},{expected_dim1}).")
        else:
            print(f"  => ATTENTION: on attend ({expected_dim0},{expected_dim1}), obtenu ({dim0},{dim1}).")

def check_1D_shape(key, expected_dim=None):
    """Affiche la longueur d'une liste 1D et compare éventuellement."""
    data_1d = jnet[key]
    if not isinstance(data_1d, list):
        print(f"{key} n'est pas une liste 1D!")
        return

    length = len(data_1d)
    print(f"{key} length in JSON => {length}")
    print(f"{key}[0] =", data_1d[0])
    print(f"{key}[-1] =", data_1d[-1])

    if expected_dim is not None:
        if length == expected_dim:
            print(f"  => OK, correspond à {expected_dim}.")
        else:
            print(f"  => ATTENTION: on attend {expected_dim}, obtenu {length}.")



check_2D_shape("ft.weight", 128, 768)
check_1D_shape("ft.bias", 128)
check_2D_shape("out.weight", 1, 256)
check_1D_shape("out.bias", 1)
