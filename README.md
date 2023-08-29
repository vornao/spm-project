# Parallel Huffman Encoding Algorithm with Threads and FastFlow
## SPM Project - Parallel and Distributed Systems, paradigms and models, @Unipisa

![Huffman Tree](https://upload.wikimedia.org/wikipedia/commons/thumb/8/82/Huffman_tree_2.svg/800px-Huffman_tree_2.svg.png)

![GitHub License](https://img.shields.io/badge/license-MIT-blue.svg)

This repository contains an efficient parallel implementation of the Huffman encoding algorithm using threads and the FastFlow library. Huffman encoding is a widely used technique for lossless data compression, and parallelizing it can greatly improve its performance on modern multi-core processors. The FastFlow library is used to manage the parallelism and communication between threads, making it easier to harness the power of parallel computing.

## Table of Contents

- [Parallel Huffman Encoding Algorithm with Threads and FastFlow](#parallel-huffman-encoding-algorithm-with-threads-and-fastflow)
  - [SPM Project - Parallel and Distributed Systems, paradigms and models, @Unipisa](#spm-project---parallel-and-distributed-systems-paradigms-and-models-unipisa)
  - [Table of Contents](#table-of-contents)
  - [Background](#background)
  - [Installation](#installation)
  - [Usage](#usage)
  - [Features](#features)
  - [Performance](#performance)
  - [License](#license)

## Background

Huffman encoding is a variable-length prefix coding scheme used for lossless data compression. It assigns shorter codes to frequently occurring symbols and longer codes to less frequent symbols, resulting in a compact representation of data. This project aims to parallelize the Huffman encoding algorithm using threads and the FastFlow library, allowing for faster compression and decompression of data.

## Installation

1. **Clone the Repository:**

    ```bash
    git clone https://github.com/your-username/parallel-huffman-fastflow.git
    cd parallel-huffman-fastflow
    ```

2. **Install FastFlow:**

    Before using the project, you need to install the FastFlow library. Follow the installation instructions provided in the FastFlow repository: [FastFlow Installation](https://github.com/fastflow/fastflow)

3. **Build the Project:**

    ```bash
    ./bld.sh
    ```

## Usage

**Compressing Data:**

```bash
./build/spm_project <input_file> n_mappers n_reducers n_encoders <seq|map|ff>
```
Compressed file will be written to `files/output.bin`.

## Features

- Parallel implementation of Huffman encoding using threads and FastFlow.
- Efficient handling of multi-core processors for faster compression and decompression.
- Command-line tools for compressing and decompressing files.

## Performance

The parallel implementation of the Huffman encoding algorithm benefits from the multi-threaded approach and the optimizations provided by the FastFlow library. The speedup achieved depends on various factors such as the number of available cores, the size of the input data, and the distribution of symbol frequencies.


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---