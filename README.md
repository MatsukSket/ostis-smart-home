<h1 align="center">OSTIS example app</h1>

[![license](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## About

OSTIS example app is an ostis-system based on [**ostis-web-platform**](https://github.com/ostis-ai/ostis-web-platform) and designed with [OSTIS Technology](https://github.com/ostis-ai).

## Installation

```sh
git clone https://github.com/ostis-apps/ostis-example-app.git
cd ostis-example-app
git checkout release/0.10.0

conan install .
cmake --preset conan-release
cmake --build --preset conan-release

./interface/sc-web/scripts/install_dependencies.sh
./interface/sc-web/scripts/build_sc_web.sh
```

## Build knowledge base

Before first launch or after changes in KB you should build knowledge base. 

```sh
<path/to/sc-builder> -i kb -o kb.bin --clear
```

## Usage

To launch system you should start sc-machine:
```sh
<path/to/sc-machine> -c ostis-example-app.ini -e "<path/to/sc-machine/extensions>;build/Release/extensions"
```

After that launch sc-web interface:

```sh
./interface/sc-web/scripts/run_sc_web.sh
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
