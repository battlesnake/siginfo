siginfo
=======

Program for playing with signals.

Also a nice demo of `poll`/`signalfd` i.e. non-blocking IO and uninterruptive signal-handling for anyone who isn't already familiar with these concepts.

    make

    ./siginfo

    # Send some signals e.g. Ctrl+C, Ctrl+Z, Ctrl+\, killall -s USR1 siginfo
    # Press <q> to quit
