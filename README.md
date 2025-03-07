## About

- JFUSE (Json FULL Schema Extractor) is a tool for schema extraction. Given a valid JSON file, it infers basic types, tagged unions, enumeration, arrays as tuples, and objects as collections.

### Running
```sh
$ g++ -o main *.cpp
```
```sh
$ ./main 4 <json collection> <schema name> <extract min and max> <extract key>
```

### Example
```sh
$ ./main 4 data.json structure.txt true true
```

### More

- A JSON generator was implemented for testing. To run it, it's necessary to have Python installed. It will ask for the quantity of root objects, and the chunk size.

### Running

```sh
$ python3 jsonGeneratorWithChunk.py
```
