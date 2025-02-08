<h1 align="center">ostis-example-app</h1>

[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## Overview

`ostis-example-app` is a practical demonstration of an OSTIS-based system, built using the [OSTIS Technology](https://github.com/ostis-ai). This example provides a solid foundation for developers looking to create custom intelligent systems leveraging OSTIS. You can use this as a starting point, adapting and extending its components to fit your specific requirements.

This example application showcases the core components of an OSTIS system:

*   Knowledge Base: Information represented using SC-code (files with `.scs` and `.gwf` extensions);
*   Problem Solver: Agents written in C++ that implement the system's problem-solving logic;
*   Interface: A web-based user interface for interacting with the system.

## Getting Started

Choose your preferred method for running the application: Docker or Native installation.

### Prerequisites

Before you begin, ensure you have the following tools installed and configured based on your chosen installation method (Docker or Native):

#### Common Prerequisites (Required for Both Docker and Native Installation)

*   **Git:**  Required for cloning the repository.
    *   Installation:  Refer to the official Git documentation for installation instructions specific to your operating system:  [https://git-scm.com/book/en/v2/Getting-Started-Installing-Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)

#### Docker Prerequisites

*   **Docker:**  Containerization platform.
    *   Installation:  [https://www.docker.com/get-started/](https://www.docker.com/get-started/)
*   **Docker Compose:**  Tool for defining and running multi-container Docker applications.
    *   Installation:  [https://docs.docker.com/compose/install/](https://docs.docker.com/compose/install/)  (Usually included with Docker Desktop.  If not, follow the linked instructions.)

#### Native Installation Prerequisites

*   **pipx:** Python package installer for installing and running Python applications in isolated environments.
    *   Installation: [https://pipx.pypa.io/stable/installation/](https://pipx.pypa.io/stable/installation/)
*   **C++ Compiler:**  A compiler such as GCC or Clang is necessary for building the C++ problem solver.
    *   Installation (Ubuntu/Debian - GCC):  `sudo apt-get update && sudo apt-get install build-essential`
    *   Installation (macOS - Clang):  Usually included with Xcode Command Line Tools. Install with `xcode-select --install`
*   **CMake:**  Cross-platform build system generator.
    *   Installation: [https://cmake.org/download/](https://cmake.org/download/) or via your system's package manager (e.g., `sudo apt-get install cmake` on Ubuntu). Ensure CMake is added to your system's PATH.
*   **Conan:**  C/C++ package manager.
    *   Installation: See the Native Installation steps for detailed instructions.
*   **Node.js and npm (Node Package Manager):** Required for building the sc-web interface.
    *   Installation: Download and install from [https://nodejs.org/](https://nodejs.org/). npm is typically included with Node.js.

## Docker Setup

This is the recommended method for most users as it simplifies the setup process.

1.  **Clone the repository:**

    ```
    git clone https://github.com/ostis-apps/ostis-example-app.git
    cd ostis-example-app
    git checkout 0.10.0
    git submodule update --init --recursive
    ```

2.  **Build Docker images:**

    ```
    docker compose build
    ```

3.  **Build the knowledge base:**

    ```
    docker compose run --rm machine build
    ```

4.  **Start the ostis-system:**

    ```
    docker compose up
    ```

    The system will be accessible through your web browser at `localhost:8000`.

5.  **Stop the system:**

    ```
    docker compose down
    ```

    If you make changes to the knowledge base, rebuild it using step 3.

## Native Installation

Follow these steps to set up the application natively on your system.

1.  **Clone the repository:**

    ```
    git clone https://github.com/ostis-apps/ostis-example-app.git
    cd ostis-example-app
    git checkout 0.10.0
    git submodule update --init --recursive
    ```

2.  **Install `pipx`:**

    Follow the instructions here: [https://pipx.pypa.io/stable/installation/](https://pipx.pypa.io/stable/installation/).

3.  **Install Conan:**

    Conan is a decentralized package manager for C/C++. It's used in this project to manage C++ dependencies.

    ```
    pipx install conan
    pipx ensurepath
    ```

4.  **Restart your shell:**

    ```
    exec $SHELL
    ```

5.  **Install C++ problem solver dependencies**

    ```
    conan remote add ostis-ai https://conan.ostis.net/artifactory/api/conan/ostis-ai-sc-machine
    conan profile detect
    conan install . --build=missing
    ```

6.  **Install sc-machine binaries:**

    *   **Linux:**

        ```
        curl -LO https://github.com/ostis-ai/sc-machine/releases/download/0.10.0/sc-machine-0.10.0-Linux.tar.gz
        mkdir sc-machine && tar -xvzf sc-machine-0.10.0-Linux.tar.gz -C sc-machine --strip-components 1
        rm -rf sc-machine-0.10.0-Linux.tar.gz && rm -rf sc-machine/include
        ```

    *   **macOS:**

        ```
        curl -LO https://github.com/ostis-ai/sc-machine/releases/download/0.10.0/sc-machine-0.10.0-Darwin.tar.gz
        mkdir sc-machine && tar -xvzf sc-machine-0.10.0-Darwin.tar.gz -C sc-machine --strip-components 1
        rm -rf sc-machine-0.10.0-Darwin.tar.gz && rm -rf sc-machine/include
        ```

7.  **Install sc-web:**

    *   **Ubuntu:**

        ```
        cd interface/sc-web
        ./scripts/install_deps_ubuntu.sh
        npm install  # Ensure npm dependencies are installed
        npm run build
        cd ../..
        ```

    *   **macOS:**

        ```
        cd interface/sc-web
        ./scripts/install_deps_macOS.sh
        npm install  # Ensure npm dependencies are installed
        npm run build
        cd ../..
        ```

## Building Application

1.  **Build problem solver:**

    ```
    cmake --preset release-conan
    cmake --build --preset release
    ```

2.  **Build knowledge base:**

    ```
    ./sc-machine/bin/sc-builder -i repo.path -o kb.bin --clear
    ```

## Running Application

1.  **Start `sc-machine` (in a terminal):**

    ```
    ./sc-machine/bin/sc-machine -s kb.bin -e "sc-machine/lib/extensions;build/Release/extensions"
    ```

2.  **Start `sc-web` interface (in a separate terminal):**

    ```
    cd interface/sc-web
    source .venv/bin/activate && python3 server/app.py
    ```

3.  **Access the interface:** Open `localhost:8000` in your web browser.

    ![Example Screenshot](https://i.imgur.com/6SehI5s.png)

## Documentation

To generate local documentation:

```sh
pip3 install mkdocs mkdocs-material markdown-include
mkdocs serve
```

Then open `http://127.0.0.1:8005/` in your browser.

*Note: The documentation is currently under development and may be incomplete.*

## Project Structure

*   **`knowledge-base`**: Contains the knowledge base source files (`.scs`, `.gwf`).  Remember to rebuild the knowledge base after making changes:

    ```
    ./sc-machine/bin/sc-builder -i repo.path -o kb.bin --clear
    ```

*   **`problem-solver`**: Contains the C++ agents that implement the problem-solving logic.  Rebuild after modifying:

    ```
    cmake --preset release-conan
    cmake --build --preset release
    ```

    For debug mode:

    ```
    conan install . --build=missing -s build_type=Debug
    cmake --preset debug-conan
    cmake --build --preset debug
    ```

    To enable debug logs, configure `ostis-example-app.ini`:

    ```
    log_type = Console
    log_file = sc-memory.log
    log_level = Debug
    ```

## Testing

1.  Enable tests in `CMakeLists.txt` by setting `SC_BUILD_TESTS` to `ON`.
2.  Rebuild the project.
3.  Run tests:

    ```
    cd build/Debug  # or build/Release
    ctest -V
    ```

## Code Style

This project follows the code style guidelines of `sc-machine`, which can be found [here](https://ostis-ai.github.io/sc-machine/dev/codestyle/).

## Author

*  GitHub: [@ostis-apps](https://github.com/ostis-apps), [@ostis-ai](https://github.com/ostis-ai)

## Support

Give us a ⭐️ if you like this project!

## Contributing

Contributions, issues, and feature requests are welcome! Check the [issues page](https://github.com/ostis-apps/ostis-example-app/issues).

## License

This project is licensed under the [MIT License](https://opensource.org/license/mit/)
