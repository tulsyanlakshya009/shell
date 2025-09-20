# Custom Shell in C

This is a simple Unix-like shell implemented in C. It supports running basic commands and demonstrates how a shell parses input and executes processes.

---

## Build Instructions

Clone the repository and build:

```bash
git clone https://github.com/<your-username>/<repo-name>.git
cd <repo-name>
make
```

This will compile the project and create the executable:

```bash
./shell
```

---

## ▶️ Usage

Start the shell:

```bash
./shell
```

Then you can run commands like:

* `ls` – list files
* `pwd` – print current directory
* `echo hello` – print text
* `cd <dir>` – change directory
* `exit` – quit the shell

If you type a valid system command, the shell will execute it just like Bash or Zsh.

---

## Project Structure

```
.
├── shell.c           # Main shell implementation
├── tokenize.c        # Input tokenizer
├── tokenize_imp.c/h  # Tokenizer implementation & headers
├── Makefile          # Build system
└── README.md         # Project documentation
```

---

## Requirements

* GCC or Clang (C compiler)
* Make
