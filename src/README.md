# MailScream

Because, sometimes, it's the only way!!

MailScream is Not just for catharsis, but for encouraging healthy emotional boundaries! 🎭✨

## Install Dependencies:

```shell
# Update system
sudo apt update && sudo apt upgrade

# Install build tools and libraries
sudo apt install build-essential git
sudo apt install libcurl4-openssl-dev
sudo apt install libsqlite3-dev
sudo apt install libmicrohttpd-dev
sudo apt install libjson-c-dev
sudo apt install cmake

```

## Compile and Run

```shell
# Run make
make clean 2>/dev/null || echo "Nothing to clean"

# Quick dev:debug
make sanitize

# release
make sanitize-release

# [optional] - view the linkage
ldd mailscream
```

## Add this to ~/.bashrc or ~/.zshrc

Create shortcuts for maximum fun!!

```shell
alias scream="mailscream"
alias boss="mailscream yourbossname"
alias yell='mailscream yourbossname --no-reply'
```

## Now you can:

```shell
scream username "The Jira board just gaslit me again"
boss "Why is everything suddenly in QA?"
yell "Not today, Satan."
```

## Therapeutic Limits - 20 Personas
We've capped your persona list at 20 individuals. 

If you find yourself needing more than 20 different people to regularly scream at, you might want to consider:

1. **Real Therapy** 🛋️
2. **A career change** 💼  
3. **Moving to a deserted island** 🏝️
4. **All of the above** ✅

### Enjoy!!
