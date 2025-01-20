<h1 align="center">ostis-example-app</h1>

[![license](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## About

ostis-example-app is an ostis-system based on [**ostis-web-platform**](https://github.com/ostis-ai/ostis-web-platform) and designed with [OSTIS Technology](https://github.com/ostis-ai).

## Installation

### Clone repository

```sh
git clone https://github.com/NikitaZotov/ostis-example-app.git
cd ostis-example-app
git checkout release/0.10.0
git submodule update --init --recursive
```

### Install pipx 

To install pipx, use this guide: https://pipx.pypa.io/stable/installation/.

### Install Conan

```sh
# Use pipx to install conan if not already installed
pipx install conan
pipx ensurepath
```

### Relaunch your shell after installation

```sh
exec $SHELL
```

### Install sc-machine libraries

```sh
conan profile detect
conan remote add ostis-ai https://conan.ostis.net/artifactory/api/conan/ostis-ai-sc-machine

conan install . --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
```

### Install sc-machine binaries

#### Ubuntu

```sh
curl -LO https://github.com/NikitaZotov/sc-machine/releases/download/0.10.0/sc-machine-0.10.0-Linux.tar.gz
mkdir sc-machine && tar -xvzf sc-machine-0.10.0-Linux.tar.gz -C sc-machine --strip-components 1
rm -rf sc-machine-0.10.0-Linux.tar.gz
```

#### macOS

```sh
curl -LO https://github.com/NikitaZotov/sc-machine/releases/download/0.10.0/sc-machine-0.10.0-Darwin.tar.gz
mkdir sc-machine && tar -xvzf sc-machine-0.10.0-Darwin.tar.gz -C sc-machine --strip-components 1
rm -rf sc-machine-0.10.0-Darwin.tar.gz
```

### Install sc-web

#### Ubuntu

```sh
cd interface/sc-web
./scripts/install_deps_ubuntu.sh
npm run build
cd ../..
```

#### macOS

```sh
cd interface/sc-web
./scripts/install_deps_macOS.sh
npm run build
cd ../..
```

## Build knowledge base

Before first launch or after changes in knowledge base you should build it. 

```sh
./sc-machine/bin/sc-builder -i kb -o kb.bin --clear
```

## Usage

To launch system you should start sc-machine in the first terminal:
```sh
./sc-machine/bin/sc-machine -s kb.bin -e "sc-machine/lib/extensions;build/Release/extensions"
```

After that launch sc-web interface in the second terminal:

```sh
cd interface/sc-web
source .venv/bin/activate
python3 server/app.py
```

To check that everything is fine open localhost:8000 in your browser.
![](https://i.imgur.com/6SehI5s.png)

## Project Structure

### Knowledge Base

`kb` is the place for the knowledge base source text files of your app. Put your **.scs** and **.gwf** files here.

### Problem Solver

`problem-solver` is the place for the problem solver of your app. Put your agents here. After changes in problem-solver you should rebuild it:

After updating your C++ code you need to rebuild `problem-solver`:  
```sh
cmake --preset conan-release
cmake --build --preset conan-release
```

To enable DEBUG set fields in ostis-example-app.ini:

```sh
log_type = Console
log_file = sc-memory.log
log_level = Debug
```

### Interface

`interface` is the place for your interface modules.

To learn more about creating web components for the new web interface version follow this [link](https://github.com/MikhailSadovsky/sc-machine/tree/example/web/client)

## Author

* GitHub: [@ostis-apps](https://github.com/ostis-apps), [@ostis-ai](https://github.com/ostis-ai)

## Show your support

Give us a ⭐️ if you've liked this project!

## 🤝 Contributing

Contributions, issues and feature requests are welcome!<br />Feel free to check [issues page](https://github.com/ostis-apps/ostis-example-app/issues). 

## 📝 License

This project is [MIT](https://opensource.org/license/mit/) licensed.
