#! /usr/bin/env python3

import argparse

import pandas


def parseArguments(args):
    parser = argparse.ArgumentParser(
        description="Creates convergence plots from gathered stats"
    )
    parser.add_argument(
        "-f",
        "--file",
        type=argparse.FileType("r"),
        default="test-statistics.csv",
        help="The CSV file containing the gathered stats.",
    )
    return parser.parse_args(args)


def main(argv):
    args = parseArguments(argv[1:])

    df = pandas.read_csv(args.file)
    toMeshes = df["mesh B"].unique()
    assert (
        len(toMeshes) == 1
    ), f"There are {len(toMeshes)} to-meshes but only 1 is allowed. Fix your dataset!"
    df.sort_values("mesh A", inplace=True)
    series = df.groupby("mapping")
    for grouped in series:
        name, group = grouped
        print(name)
        group.to_csv(f"{name}.csv")
    return 0


if __name__ == "__main__":
    import sys

    sys.exit(main(sys.argv))
