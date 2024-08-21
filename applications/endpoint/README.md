# RESTful API for OpenFOAM classes

The `endpoint.C` source file showcases minimal efforts needs to expose
OpenFOAM class internals through a
[RESTful](https://www.redhat.com/en/topics/api/what-is-a-rest-api)
API to the network.

## Showcased features

Most of these features are only because of the flexibility of the reflection
system.

- Only the base RTS class headers are included. Just like a real
  OpenFOAM solver application
  - Dynamic loading of more child classes is certainly possible but not yet showcased.
- Both minimal and elaborate reflection support is shown within class code.
  - `uiModel` classes feature rich reflection support with descriptions and default values
    for class members
  - `origOpenFOAMModel` classes do not fully support the reflection system hence default
    values are mostly "type defaults".
- The reflection system can be configured through OpenFOAM dictionaries to either
  show RTS options or fetch skeleton dictionary for the concrete configured model.
- For now, only the reflection system can be configured through `POST` requests, and not
  the classes themselves although this is also simple to implement:
  1. Perform non-configured reflection to get a skeleton of the class's dictionary
  2. In UI/UX, allow users to mutate keyword values (no adding of keywords needed).
  3. POST the modifications and apply them to the skeleton dictionary
  4. Construct an object of the target type using the mutated skeleton

## RESTful API docs

```bash
wmake && ./endpoint
```
Then look at `http://0.0.0.0:18080/` for up-to-date

Generally speaking:

- `GET` requests to some URLs yield JSON representations of OpenFOAM class members hierarchy
- `POST` requests to config URLs allow to perform specific actions on OpenFOAM side of things.
  Eg. Set keyword/values in the dictionary passed to the reflection system
- `DELETE` requests to config URLS reset the state of some OpenFOAM entities.
  Eg. Reset the reflection system configuration dictionary

## RESTful interaction example

```bash
wmake && ./endpoint
```

Consulting `http://0.0.0.0:18080/classes/origOpenFOAMModel` from a web browser results in
the following JSON response, as it runs the reflection system from an unconfigured state
in documentation mode:
```json
{
  "ndc": {
    "default": "\"\"",
    "type": "OFstream"
  },
  "ht": {
    "default": "0()",
    "type": "HashTable<word, word, Hash<word> >"
  },
  "dir": {
    "default": "(0 0 0)",
    "type": "Vector<double>"
  },
  "m": {
    "default": "0",
    "type": "int"
  },
  "name": {
    "default": "\"\"",
    "type": "<<onDemand>> word"
  },
  "origOpenFOAMModelType": {
    "default": "\"<<RTSoption>> ( origChild1Model origChild2Model )\"",
    "type": "origOpenFOAMModelType"
  }
}
```

Now POST some configuration, for simplicity, using curl:
```bash
curl -X POST -H "Content-Type: application/json" \
    -d '{"origOpenFOAMModelType":"origChild1Model"}' \
    'http://0.0.0.0:18080/classes/config/origOpenFOAMModel'
```

Consulting the same URL again reveals more members, and that `origChild1Model`
was selected as the concrete type!

```json
{
  "ndc": {
    "default": "\"\"",
    "type": "OFstream"
  },
  "ht": {
    "default": "0()",
    "type": "HashTable<word, word, Hash<word> >"
  },
  "dir": {
    "default": "(0 0 0)",
    "type": "Vector<double>"
  },
  "m": {
    "default": "0",
    "type": "int"
  },
  "name": {
    "default": "\"\"",
    "type": "<<onDemand>> word"
  },
  "k": {
    "default": "0",
    "type": "double"
  },
  "subModel": {
    "ndc": {
      "default": "\"\"",
      "type": "OFstream"
    },
    "ht": {
      "default": "0()",
      "type": "HashTable<word, word, Hash<word> >"
    },
    "dir": {
      "default": "(0 0 0)",
      "type": "Vector<double>"
    },
    "m": {
      "default": "0",
      "type": "int"
    },
    "name": {
      "default": "\"\"",
      "type": "<<onDemand>> word"
    },
    "origOpenFOAMModelType": {
      "default": "\"<<RTSoption>> ( origChild1Model origChild2Model )\"",
      "type": "origOpenFOAMModelType"
    }
  },
  "origChild1ModelType": {
    "default": "origChild1Model",
    "type": "origOpenFOAMModelType"
  }
}
```

BUT the `subModel` is still not fully configured, so we can make another POST:
```bash
curl -X POST -H "Content-Type: application/json" \
    -d '{"origOpenFOAMModelType":"origChild1Model", "subModelType":"origChild2Model"}' \
    'http://0.0.0.0:18080/classes/config/origOpenFOAMModel'
```

```json
{
  "ndc": {
    "default": "\"\"",
    "type": "OFstream"
  },
  "ht": {
    "default": "0()",
    "type": "HashTable<word, word, Hash<word> >"
  },
  "dir": {
    "default": "(0 0 0)",
    "type": "Vector<double>"
  },
  "m": {
    "default": "0",
    "type": "int"
  },
  "name": {
    "default": "\"\"",
    "type": "<<onDemand>> word"
  },
  "k": {
    "default": "0",
    "type": "double"
  },
  "subModel": {
    "ndc": {
      "default": "\"\"",
      "type": "OFstream"
    },
    "ht": {
      "default": "0()",
      "type": "HashTable<word, word, Hash<word> >"
    },
    "dir": {
      "default": "(0 0 0)",
      "type": "Vector<double>"
    },
    "m": {
      "default": "0",
      "type": "int"
    },
    "name": {
      "default": "\"\"",
      "type": "<<onDemand>> word"
    },
    "labels": {
      "default": "0()",
      "type": "List<int>"
    },
    "origChild2ModelType": {
      "default": "origChild2Model",
      "type": "origOpenFOAMModelType"
    }
  },
  "origChild1ModelType": {
    "default": "origChild1Model",
    "type": "origOpenFOAMModelType"
  }
}
```

NEAT! Isn't it?
