# depzip v0.1

## Dependency vendorer

[![Build Status](https://github.com/karnkaul/depzip/actions/workflows/ci.yml/badge.svg)](https://github.com/karnkaul/depzip/actions/workflows/ci.yml)

## Usage

The project exports two targets:

- `depzip-lib` (static library)
- `depzip` (CLI tool)

All the core logic is in the library, the CLI tool being a thin wrapper that processes command line arguments and parses a JSON manifest.

### CLI Tool

The depzip manifest (`depzip.json` by default) is a simple JSON object.

- [Manifest schema](depzip_schema.json)
- [Manifest example](ext/depzip.json)

For example, to (re)vendor `depzip`'s own dependencies (assuming a `depzip` executable is in the project root):

```
cd ext
../depzip
```

Or, to run from the root directory:

```
./depzip --pwd=ext ext/depzip.json
```

Run `depzip --help` or `depzip --usage` for more info.

### Library

WIP

## Contributing

Pull/merge requests are welcome.

**[Original repository](https://github.com/karnkaul/depzip)**
